// Detect whether the compiler supports the block syntax used by WORKER_SPAWN.
// This example needs GCC (nested-function extension path) or Clang with blocks.
#if ( defined(__GNUC__) && !defined(__clang__) )
# define DO_EXAMPLE 1
#elif defined(__clang__) && defined(__has_extension) 
# if  __has_extension(blocks)
#  define DO_EXAMPLE 1
#endif
#endif 

#ifdef DO_EXAMPLE
#include <stdio.h>

#include "m-worker.h"

// Global worker pool reused by all recursive calls.
worker_t g_workers;

/* Compute Fibonacci by running one branch asynchronously in the worker pool. */
static int fib(int n)
{
  // Base case of the recursion.
  if (n < 2)
    return n;

  // Synchronization token for jobs launched in this call.
  worker_sync_t b;
  // x is produced by the spawned branch, y by the local branch.
  int x, y;

  // Open a spawn/sync region bound to g_workers.
  worker_start(b, g_workers);
  // Launch fib(n-2) in parallel.
  // WORKER_SPAWN captures variables explicitly:
  // - first tuple: values copied into the task (here: n)
  // - last tuple: variables written by the task and made visible after sync (here: x)
  WORKER_SPAWN(b, (n), { x = fib(n-2); } , (x));
  // Compute fib(n-1) locally while worker computes fib(n-2).
  y = fib (n-1);
  // Wait for spawned jobs before reading x.
  worker_sync(b);
  return x + y;
}

int main(void)
{
  // Initialize worker pool with default settings.
  worker_init(g_workers, 0, 0, NULL);
  int n = 39;
  int result = fib(n);

  // Display result once all recursive tasks are completed.
  printf("Fibonacci number #%d is %d.\n", n, result);

  // Release worker resources.
  worker_clear(g_workers);
  return 0;
}

#else
#include <stdio.h>

int main(void)
{
  // Fallback path when required compiler extensions are unavailable.
  printf("This example requires either GCC or Clang with blocks support.\n");
  return 0;
}
#endif
