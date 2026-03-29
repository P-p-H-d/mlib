#include <stdio.h>

#include "m-atomic.h"
#include "m-queue.h"
#include "m-shared-ptr.h"
#include "m-thread.h"

/*
 * Demo topology:
 * - 2 producer threads push integer items into one shared queue.
 * - 2 consumer threads pop items and accumulate statistics.
 *
 * The queue itself is NOT thread-safe, but it is wrapped by M_SHARED_PTR_DEF,
 * which adds locking + reference counting around the underlying data.
 */
#define PRODUCER_COUNT 2
#define CONSUMER_COUNT 2
#define ITEMS_PER_PRODUCER 50000
#define QUEUE_CAPACITY 64
#define STOP_TOKEN (-1)

/* Regular queue container. */
M_QUEUE_DEF(int_queue, int, QUEUE_CAPACITY)

/* Shared pointer wrapper with internal lock + ref counter. */
M_SHARED_PTR_DEF(shared_int_queue, int_queue_t, M_QUEUE_OPLIST(int_queue, M_BASIC_OPLIST))

/* Per-producer arguments: each thread gets one shared reference + id. */
typedef struct {
	shared_int_queue_t *queue;
	int producer_id;
} producer_ctx_t;

/* Per-consumer arguments: one shared reference + pointers to global stats. */
typedef struct {
	shared_int_queue_t *queue;
	atomic_uint *consumed_count;
	atomic_ulong *sum;
} consumer_ctx_t;

/*
 * Each producer pushes a distinct range:
 * - producer 0: [1 .. ITEMS_PER_PRODUCER]
 * - producer 1: [ITEMS_PER_PRODUCER+1 .. 2*ITEMS_PER_PRODUCER]
 * This makes expected_sum easy to verify in main().
 */
static void producer_thread(void *arg)
{
	producer_ctx_t *ctx = arg;
	int base = ctx->producer_id * ITEMS_PER_PRODUCER;

	for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
		shared_int_queue_push(ctx->queue, base + i + 1);
	}

	/* Release producer's ownership of the shared pointer. */
	shared_int_queue_release(ctx->queue);
}

/*
 * Consumers run until they receive STOP_TOKEN.
 * STOP_TOKEN values are sent by main once all producers are finished.
 */
static void consumer_thread(void *arg)
{
	consumer_ctx_t *ctx = arg;
	while (true) {
		int value;
		shared_int_queue_pop(&value, ctx->queue);
		if (value == STOP_TOKEN) {
			break;
		}
		atomic_fetch_add(ctx->consumed_count, 1);
		atomic_fetch_add(ctx->sum, (unsigned long) value);
	}

	/* Release consumer's ownership of the shared pointer. */
	shared_int_queue_release(ctx->queue);
}

int main(void)
{
	m_thread_t producer_1, producer_2;
	m_thread_t consumer_1, consumer_2;

	/* Shared statistics updated atomically by consumers. */
	atomic_uint consumed_count;
	atomic_ulong sum;
	atomic_init(&consumed_count, 0);
	atomic_init(&sum, 0);

	/* Create one shared queue instance, initially owned by main. */
	shared_int_queue_t *queue = shared_int_queue_new();

	/*
	 * Acquire one ownership per thread context.
	 * All these pointers refer to the same underlying queue object.
	 */
	producer_ctx_t pctx1 = { shared_int_queue_acquire(queue), 0 };
	producer_ctx_t pctx2 = { shared_int_queue_acquire(queue), 1 };
	consumer_ctx_t cctx1 = { shared_int_queue_acquire(queue), &consumed_count, &sum };
	consumer_ctx_t cctx2 = { shared_int_queue_acquire(queue), &consumed_count, &sum };

	m_thread_create(producer_1, producer_thread, &pctx1);
	m_thread_create(producer_2, producer_thread, &pctx2);
	m_thread_create(consumer_1, consumer_thread, &cctx1);
	m_thread_create(consumer_2, consumer_thread, &cctx2);

	m_thread_join(producer_1);
	m_thread_join(producer_2);

	/* One stop token per consumer, so both can terminate. */
	shared_int_queue_push(queue, STOP_TOKEN);
	shared_int_queue_push(queue, STOP_TOKEN);

	/*
	 * Main thread no longer needs its ownership.
	 * The queue stays alive because consumers still hold references.
	 */
	shared_int_queue_release(queue);

	m_thread_join(consumer_1);
	m_thread_join(consumer_2);

	/*
	 * Sum of 1..N where N = PRODUCER_COUNT * ITEMS_PER_PRODUCER.
	 * Since producers generate disjoint consecutive ranges, this is valid.
	 */
	const unsigned total_items = PRODUCER_COUNT * ITEMS_PER_PRODUCER;
	const unsigned long expected_sum = ((unsigned long) total_items * (total_items + 1)) / 2;

	printf("Produced/consumed: %u / %u\n", total_items, atomic_load(&consumed_count));
	printf("Expected sum:      %lu\n", expected_sum);
	printf("Observed sum:      %lu\n", atomic_load(&sum));

	return 0;
}

