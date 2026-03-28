#include <stdio.h>

#include "m-worker.h"

// Global worker pool used by recursive fib() calls.
// A single pool avoids creating/destroying threads at each recursion level.
worker_t g_workers;

int fib(int n);

// Tiny payload passed to the spawned task.
// n: input value, x: output storage written by the worker.
struct fib2_s {
  int x, n;
};

static void subfunc_1 (void *data) {
  struct fib2_s *f = data;
  // Compute one branch of Fibonacci in the worker thread.
  f->x = fib (f->n );
}

/* Compute Fibonacci using one asynchronous branch + one local branch. */
int fib(int n)
{
  // Base case: no worker needed.
  if (n < 2)
    return n;

  // f lives on the caller stack and is shared with one spawned task.
  // This is safe because worker_sync(b) below waits before returning.
  struct fib2_s f;
  // Synchronization handle for all jobs spawned from this call.
  worker_sync_t b;

  // Start a spawn/sync region on the global worker pool.
  worker_start(b, g_workers);
  f.n = n - 2;
  // Spawn fib(n-2) asynchronously.
  worker_spawn (b, subfunc_1, &f);
  // Compute fib(n-1) in the current thread in parallel with spawned job.
  int y = fib (n-1);
  // Join spawned jobs of this region before reading f.x.
  worker_sync(b);
  return f.x + y;
}

int main(void)
{
  // Create worker pool.
  // (0, 0, NULL) asks mlib to pick default thread count and options.
  worker_init(g_workers, 0, 0, NULL);
  int n = 39;
  int result = fib(n);

  // Display result once all recursive jobs have completed.
  printf("Fibonacci number #%d is %d.\n", n, result);

  // Release worker threads and associated resources.
  worker_clear(g_workers);
  return 0;
}
