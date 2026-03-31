#include <stdio.h>
#include "m-atomic.h"
#include "m-buffer.h"
#include "m-thread.h"

// Generate a lock-free Multi-Producer/Multi-Consumer queue of int.
// M_BUFFER_QUEUE selects FIFO behavior.
QUEUE_MPMC_DEF(int_shared_queue, int, M_BUFFER_QUEUE)

enum {
    PRODUCER_COUNT = 3,       // number of writer threads
    CONSUMER_COUNT = 2,       // number of reader threads
    ITEMS_PER_PRODUCER = 4000, // integers sent by each producer
    STOP_VALUE = -1           // sentinel that tells a consumer to quit
};

// Each producer thread gets its own id so it pushes a unique integer range,
// ensuring that together they cover exactly PRODUCER_COUNT*ITEMS_PER_PRODUCER values.
typedef struct producer_arg_s {
    int producer_id;
} producer_arg_t;

// Consumers share atomic accumulators through this struct so the main thread
// can verify correctness after all threads have finished.
typedef struct consumer_arg_s {
    atomic_size_t *consumed_count; // total items received across all consumers
    atomic_llong *consumed_sum;    // sum of received values for integrity check
} consumer_arg_t;

// Shared MPMC queue used by all producer and consumer threads.
static int_shared_queue_t g_queue;

static void producer_thread(void *arg)
{
    producer_arg_t *p = arg;
    // Each producer covers a distinct integer range so values don't overlap.
    int start = p->producer_id * ITEMS_PER_PRODUCER;
    int stop = start + ITEMS_PER_PRODUCER;

    for (int value = start; value < stop; value++) {
        // push is non-blocking: spin and yield until the queue has room.
        while (!int_shared_queue_push(g_queue, value)) {
            m_thread_yield();
        }
    }
    // Thread exits naturally; main thread sends STOP tokens after all producers join.
}

static void consumer_thread(void *arg)
{
    consumer_arg_t *c = arg;

    while (true) {
        int value;
        // pop is non-blocking: spin and yield until an item is available.
        if (!int_shared_queue_pop(&value, g_queue)) {
            m_thread_yield();
            continue;
        }
        // A STOP_VALUE sentinel signals this consumer to quit.
        // main() pushes exactly one sentinel per consumer after all producers finish.
        if (value == STOP_VALUE) {
            break;
        }
        // Accumulate results atomically: multiple consumers update the same counters.
        atomic_fetch_add_explicit(c->consumed_count, 1, memory_order_relaxed);
        atomic_fetch_add_explicit(c->consumed_sum, value, memory_order_relaxed);
    }
}

int main(void)
{
    // Queue size must be a power of two.
    int_shared_queue_init(g_queue, 256);

    atomic_size_t consumed_count;
    atomic_llong consumed_sum;
    atomic_init(&consumed_count, 0);
    atomic_init(&consumed_sum, 0);

    m_thread_t producers[PRODUCER_COUNT];
    m_thread_t consumers[CONSUMER_COUNT];
    producer_arg_t producer_args[PRODUCER_COUNT];
    consumer_arg_t consumer_args[CONSUMER_COUNT];

    // Start consumers first so they are already waiting when producers push data.
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        consumer_args[i].consumed_count = &consumed_count;
        consumer_args[i].consumed_sum = &consumed_sum;
        m_thread_create(consumers[i], consumer_thread, &consumer_args[i]);
    }

    // Start all producers; each writes its own integer range into g_queue.
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        producer_args[i].producer_id = i;
        m_thread_create(producers[i], producer_thread, &producer_args[i]);
    }

    // Wait until every producer has finished pushing its range.
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        m_thread_join(producers[i]);
    }

    // One STOP token per consumer so each consumer can exit its loop.
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        while (!int_shared_queue_push(g_queue, STOP_VALUE)) {
            m_thread_yield();
        }
    }

    for (int i = 0; i < CONSUMER_COUNT; i++) {
        m_thread_join(consumers[i]);
    }

    size_t expected_count = (size_t)PRODUCER_COUNT * (size_t)ITEMS_PER_PRODUCER;
    // Producers together push 0, 1, ..., expected_count-1 (no duplicates, no gaps).
    // The closed-form sum of 0..n-1 is n*(n-1)/2.
    long long n = (long long)expected_count;
    long long expected_sum = n * (n - 1) / 2;
    size_t got_count = atomic_load_explicit(&consumed_count, memory_order_relaxed);
    long long got_sum = atomic_load_explicit(&consumed_sum, memory_order_relaxed);

    printf("Produced items: %zu\n", expected_count);
    printf("Consumed items: %zu\n", got_count);
    printf("Count check: %s\n", expected_count == got_count ? "OK" : "FAILED");
    printf("Sum check: %s (expected=%lld got=%lld)\n",
           expected_sum == got_sum ? "OK" : "FAILED",
           expected_sum, got_sum);

    int_shared_queue_clear(g_queue);
    return 0;
}
