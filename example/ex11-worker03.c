#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "m-worker.h"

/* Global worker pool, initialized once in main and reused by all recursive calls. */
worker_t g_workers;

static void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Partition using Hoare scheme.
   Returns the split index so that [lo, p] <= pivot and [p+1, hi] >= pivot. */
static size_t partition(int table[], size_t lo, size_t hi)
{
    int pivot = table[lo + (hi - lo) / 2];
    size_t i = lo;
    size_t j = hi;

    while (true) {
        while (table[i] < pivot) {
            i++;
        }
        while (table[j] > pivot) {
            if (j == lo) break;
            j--;
        }
        if (i >= j) {
            return j;
        }
        swap(&table[i], &table[j]);
        i++;
        if (j == 0) {
            return 0;
        }
        j--;
    }
}

/* Generate `m_worker_spawn_sort_args(...)`:
    a typed spawn helper that can queue a call to quicksort(table, lo, hi, block). */
M_WORKER_SPAWN_DEF2(sort_args, (table, int*, M_PTR_OPLIST), (lo, size_t, M_BASIC_OPLIST), (hi, size_t, M_BASIC_OPLIST), (block, struct m_worker_sync_s *, M_PTR_OPLIST) )

/* Below this many elements, stay sequential to avoid worker scheduling overhead. */
#define PARALLEL_THRESHOLD 1024

/* Recursive quicksort over [lo, hi].
    All spawned jobs are attached to the same top-level `block`, so this function
    never waits: only sort_int() performs the final synchronization. */
static void quicksort(int table[], size_t lo, size_t hi, struct m_worker_sync_s *block)
{
    if (lo >= hi) return;
    size_t p = partition(table, lo, hi);

    if (hi - lo >= PARALLEL_THRESHOLD) {
          /* Parallel step:
              - left partition may run on a worker
              - right partition runs in the current thread */
        if (p > lo) {
                /* m-worker may execute immediately if queue/workers are full,
                    preserving correctness while still exposing parallelism. */
            m_worker_spawn_sort_args(block, quicksort, table, lo, p, block);
        }
        quicksort(table, p + 1, hi, block);
    } else {
        if (p > lo) quicksort(table, lo, p, block);
        quicksort(table, p + 1, hi, block);
    }
}

static void sort_int(size_t n, int table[n])
{
    if (n < 2) return;
    /* One synchronization region for the whole sort.
       Every recursive spawn contributes to this same region. */
    worker_sync_t block;
    worker_start(block, g_workers);
    quicksort(table, 0, n - 1, block);
    /* Wait once for all spawned recursive tasks. */
    worker_sync(block);
}

int main(void)
{
    // Autodetect number of worker threads to use and initialize the global pool.
    worker_init(g_workers, 0, 0, NULL);

    /* Small smoke test to quickly inspect output by eye. */
    int table[] = { 5, 2, 9, 1, 5, 6 };
    size_t n = sizeof(table) / sizeof(table[0]);
    sort_int(n, table);
    for (size_t i = 0; i < n; i++) {
        printf("%d ", table[i]);
    }
    printf("\n");

    /* Larger test to exercise parallel recursion on a realistic input size. */
    const size_t BIG = 10000000;
    int *big = malloc(BIG * sizeof *big);
    if (big == NULL) { fprintf(stderr, "malloc failed\n"); return 1; }
    srand(42);
    for (size_t i = 0; i < BIG; i++)
        big[i] = rand();

    struct timespec t0, t1;
    timespec_get(&t0, TIME_UTC);
    sort_int(BIG, big);
    timespec_get(&t1, TIME_UTC);

    double elapsed_s = (double) (t1.tv_sec - t0.tv_sec)
                     + 1e-9 * (double) (t1.tv_nsec - t0.tv_nsec);

    /* Verify non-decreasing order after the sort. */
    bool ok = true;
    for (size_t i = 1; i < BIG; i++) {
        if (big[i] < big[i - 1]) { ok = false; break; }
    }
    printf("Large sort (%zu elements): %s\n", BIG, ok ? "OK" : "FAILED");
    printf("Large sort elapsed time: %.6f s\n", elapsed_s);
    free(big);

    /* Gracefully stop workers and release pool resources. */
    worker_clear(g_workers);
    return 0;
}
