#include <stdio.h>

#include "m-buffer.h"
#include "m-thread.h"

// Generate a lock-free Single-Producer/Single-Consumer queue of int.
// M_BUFFER_QUEUE selects FIFO behavior.
QUEUE_SPSC_DEF(int_shared_queue, int, M_BUFFER_QUEUE)

typedef struct spsc_ctx_s {
    // Shared queue accessed by exactly one producer thread and one consumer thread.
    int_shared_queue_t queue;
    // Number of integers to transfer from producer to consumer.
    size_t item_count;
    // Number of FIFO ordering mismatches detected by consumer.
    size_t consumer_errors;
    // Sum of received values, used as a quick integrity check.
    long long consumer_sum;
} spsc_ctx_t;

static void producer_thread(void *arg)
{
    spsc_ctx_t *ctx = arg;
    for (size_t i = 0; i < ctx->item_count; i++) {
        // push is non-blocking: spin/yield until there is room.
        while (!int_shared_queue_push(ctx->queue, (int)i)) {
            m_thread_yield();
        }
    }
}

static void consumer_thread(void *arg)
{
    spsc_ctx_t *ctx = arg;
    size_t errors = 0;
    long long sum = 0;

    for (size_t expected = 0; expected < ctx->item_count; ) {
        int value;
        // pop is non-blocking: spin/yield until data is available.
        if (!int_shared_queue_pop(&value, ctx->queue)) {
            m_thread_yield();
            continue;
        }
        // In this demo the producer emits 0,1,2,... so order should match expected.
        if (value != (int) expected) {
            errors++;
        }
        sum += value;
        expected++;
    }

    ctx->consumer_errors = errors;
    ctx->consumer_sum = sum;
}

int main(void)
{
    spsc_ctx_t ctx;
    // Configure one test run.
    ctx.item_count = 10000;
    ctx.consumer_errors = 0;
    ctx.consumer_sum = 0;

    // Capacity must be a power of two for this SPSC queue implementation.
    int_shared_queue_init(ctx.queue, 64);

    m_thread_t producer;
    m_thread_t consumer;
    // Start one producer thread and one consumer thread on the same queue.
    m_thread_create(producer, producer_thread, &ctx);
    m_thread_create(consumer, consumer_thread, &ctx);

    // Wait until both threads complete before reading final stats.
    m_thread_join(producer);
    m_thread_join(consumer);

    // Sum of arithmetic sequence 0 + 1 + ... + (n-1).
    long long expected_sum = (long long)ctx.item_count * (long long)(ctx.item_count - 1) / 2;
    printf("Transferred: %zu items\n", ctx.item_count);
    printf("FIFO errors: %zu\n", ctx.consumer_errors);
    printf("Sum check: %s (expected=%lld got=%lld)\n",
           expected_sum == ctx.consumer_sum ? "OK" : "FAILED",
           expected_sum, ctx.consumer_sum);

        // Release queue resources.
    int_shared_queue_clear(ctx.queue);

    return 0;
}
