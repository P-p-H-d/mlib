/*
 * M*LIB / WORKER - Extra worker interface
 *
 * Copyright (c) 2017-2018, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __M_WORKER_H
#define __M_WORKER_H

#ifndef WORKER_DISABLE

#include "m-atomic.h"
#include "m-buffer.h"
#include "m-mutex.h"

/* Needed header for detection of how many core on the system */
#if defined(_WIN32)
# include <windows.h>
#elif (defined(__APPLE__) && defined(__MACH__)) \
  || defined(__DragonFly__) || defined(__FreeBSD__) \
  || defined(__NetBSD__) || defined(__OpenBSD__)
# include <sys/param.h>
# include <sys/sysctl.h>
# define WORKER_USE_SYSCTL
#else
# include <unistd.h>
#endif

/* Support for CLANG block since CLANG doesn't support nested function.
   M-WORKER uses its 'blocks' extension instead, but it is not compatible
   with function.
   So you need to compile with "-fblocks" and link with "-lBlocksRuntime"
   if you use clang & want to use the MACRO version.

   if C++, it will use Lambda function (and std::function) instead
   (It doesn't support pre-C++11 compiler).

   Otherwise go with nested function for the MACRO version.
*/
#if defined(__cplusplus) && !defined(WORKER_USE_CPP_FUNCTION)
# define WORKER_USE_CPP_FUNCTION 1
# include <functional>
#elif defined(__has_extension) && !defined(WORKER_USE_CLANG_BLOCK)
# if __has_extension(blocks)
#  define WORKER_USE_CLANG_BLOCK 1
# endif
#endif

#ifndef WORKER_USE_CLANG_BLOCK
# define WORKER_USE_CLANG_BLOCK 0
#endif
#ifndef WORKER_USE_CPP_FUNCTION
# define WORKER_USE_CPP_FUNCTION 0
#endif

/* Control that not both options are selected at the same times.
   Note: there are not really incompatible, but if we use C++ we shall go to
   lambda directly (there is no need to support blocks)! */
#if WORKER_USE_CLANG_BLOCK && WORKER_USE_CPP_FUNCTION
# error WORKER_USE_CPP_FUNCTION and WORKER_USE_CLANG_BLOCK are both defined (not supported).
#endif

/* This type defines a work order */
typedef struct work_order_s {
  struct worker_sync_s *block;
  void * data;
  void (*func) (void *data);
#if WORKER_USE_CLANG_BLOCK
  void (^blockFunc)(void *data);
#endif
#if WORKER_USE_CPP_FUNCTION
  std::function<void(void*)> function;
#endif
} worker_order_t;

/* Define the macros needed to initialize an order.
 * * MACRO to be used to send an empty order to stop the thread
 * * MACRO to complete the not-used fields
 */
#if WORKER_USE_CLANG_BLOCK || WORKER_USE_CPP_FUNCTION
# define WORKER_EMPTY_ORDER { NULL, NULL, NULL, NULL }
# define WORKER_EXTRA_ORDER , NULL
#else
# define WORKER_EMPTY_ORDER { NULL, NULL, NULL }
# define WORKER_EXTRA_ORDER
#endif

/* This type defines a worker represented as a thread */
typedef struct worker_thread_s {
  m_thread_t id;
} worker_thread_t;

/* This type defines a synchronization point for workers */
typedef struct worker_sync_s {
  atomic_int num_spawn;
  atomic_int num_terminated_spawn;
} worker_sync_t[1];

/* Let's define the queue which will manage all work orders */
BUFFER_DEF(worker_queue, worker_order_t, 0, BUFFER_QUEUE|BUFFER_UNBLOCKING_PUSH|BUFFER_BLOCKING_POP|BUFFER_THREAD_SAFE|BUFFER_DEFERRED_POP, M_POD_OPLIST)

/* This type defines the state of the workers */
typedef struct worker_s {
  /* The work order queue */
  worker_queue_t queue_g;

  /* The table of workers */
  worker_thread_t *worker;
  unsigned int numWorker_g;

  /* The global reset function */
  void (*resetFunc_g)(void);
} worker_t[1];

/* Return the number of CPU of the system */
static inline int
workeri_get_cpu_count(void)
{
#if defined(_WIN32)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#elif defined(WORKER_USE_SYSCTL)
  int nm[2];
  int count = 0;
  size_t len = sizeof (count);
  nm[0] = CTL_HW;
  nm[1] = HW_NCPU;
  sysctl(nm, 2, &count, &len, NULL, 0);
  return M_MAX(1, count);
#elif defined (_SC_NPROCESSORS_ONLN)
  return sysconf(_SC_NPROCESSORS_ONLN);
#else
  return 1;
#endif
}

/* Execute a work order */
static inline void
workeri_exec(worker_order_t *w)
{
  assert (w!= NULL && w->block != NULL);
  //printf ("Starting thread with data %p\n", w->data);
#if WORKER_USE_CLANG_BLOCK
  //printf ("Running %s f=%p b=%p\n", (w->func == NULL) ? "Blocks" : "Function", w->func, w->blockFunc);
  if (w->func == NULL)
    w->blockFunc(w->data);
  else
#endif
#if WORKER_USE_CPP_FUNCTION
    //printf ("Running %s f=%p b=%p\n", (w->function == NULL) ? "Lambda" : "Function", w->func, w->blockFunc);
    if (w->function)
      w->function(w->data);
    else
#endif
      w->func(w->data);
  atomic_fetch_add (&w->block->num_terminated_spawn, 1);
}


/* The worker thread */
static inline void
workeri_thread(void *arg)
{
  struct worker_s *g = M_ASSIGN_CAST(struct worker_s *, arg);
  while (true) {
    worker_order_t w;
    if (g->resetFunc_g != NULL)
      g->resetFunc_g();
    //printf ("Waiting for data (queue: %lu / %lu)\n", worker_queue_size(g->queue_g), worker_queue_capacity(g->queue_g));
    worker_queue_pop(&w, g->queue_g);
    if (w.block == NULL) return;
    workeri_exec(&w);
    worker_queue_pop_release(g->queue_g);
  }
}

/* Initialization of the worker module 
   Input:
   @numWorker: number of worker to create 
   @extraQueue: number of extra work order we can get if all workers are full
   @resetFunc: function to reset the state of a worker between work orders (or NULL if none)
*/
static inline void
worker_init(worker_t g, unsigned int numWorker, unsigned int extraQueue, void (*resetFunc)(void))
{
  if (numWorker == 0)
    numWorker = workeri_get_cpu_count()-1;
  //printf ("Starting queue with: %d\n", numWorker + extraQueue);
  worker_queue_init(g->queue_g, numWorker + extraQueue);
  size_t numWorker_st = numWorker;
  g->worker = M_MEMORY_REALLOC(worker_thread_t, NULL, numWorker_st);
  M_ASSERT_INIT (g->worker != NULL);
  g->numWorker_g = numWorker;
  g->resetFunc_g = resetFunc;
  for(size_t i = 0; i < numWorker; i++) {
    m_thread_create(g->worker[i].id, workeri_thread, M_ASSIGN_CAST(void*, g));
  }
}
// Define function with default values.
#define worker_init(...) worker_init(M_DEFAULT_ARGS(4, (0, 0, NULL), __VA_ARGS__))

/* Clear of the worker module */
static inline void
worker_clear(worker_t g)
{
  for(unsigned int i = 0; i < g->numWorker_g; i++) {
    worker_order_t w = WORKER_EMPTY_ORDER;
    // Normaly all worker threads shall be waiting at this
    // stage, so all push won't block as the queue is empty.
    // But for robustness, let's wait.
    worker_queue_push_blocking (g->queue_g, w, true);
  }
  
  for(unsigned int i = 0; i < g->numWorker_g; i++) {
    m_thread_join(g->worker[i].id);
  }
  M_MEMORY_FREE(g->worker);

  worker_queue_clear(g->queue_g);
}

/* Start a new collaboration between workers
   and define the synchronization point 'block' */
static inline void
worker_start(worker_sync_t block)
{
  atomic_init (&block->num_spawn, 0);
  atomic_init (&block->num_terminated_spawn, 0);
}

/* Spawn or not the given work order to workers,
   or do it ourself if no worker is available */
static inline void
worker_spawn(worker_t g, worker_sync_t block, void (*func)(void *data), void *data)
{
  const worker_order_t w = {  block, data, func WORKER_EXTRA_ORDER };
  if (M_UNLIKELY (!worker_queue_full_p(g->queue_g))
      && worker_queue_push (g->queue_g, w) == true) {
    //printf ("Sending data to thread: %p (block: %d / %d)\n", data, block->num_spawn, block->num_terminated_spawn);
    atomic_fetch_add (&block->num_spawn, 1);
    return;
  }
  //printf ("Running data ourself: %p\n", data);
  /* No thread available. Call the function ourself */
  (*func) (data);
}

#if WORKER_USE_CLANG_BLOCK
/* Spawn or not the given work order to workers,
   or do it ourself if no worker is available */
static inline void
worker_spawn_block(worker_t g, worker_sync_t block, void (^func)(void *data), void *data)
{
  const worker_order_t w = {  block, data, NULL, func };
  if (M_UNLIKELY (!worker_queue_full_p(g->queue_g))
      && worker_queue_push (g->queue_g, w) == true) {
    //printf ("Sending data to thread as block: %p (block: %d / %d)\n", data, block->num_spawn, block->num_terminated_spawn);
    atomic_fetch_add (&block->num_spawn, 1);
    return;
  }
  //printf ("Running data ourself as block: %p\n", data);
  /* No thread available. Call the function ourself */
  func (data);
}
#endif

#if WORKER_USE_CPP_FUNCTION
/* Spawn or not the given work order to workers,
   or do it ourself if no worker is available */
static inline void
worker_spawn_function(worker_t g, worker_sync_t block, std::function<void(void *data)> func, void *data)
{
  const worker_order_t w = {  block, data, NULL, func };
  if (M_UNLIKELY (!worker_queue_full_p(g->queue_g))
      && worker_queue_push (g->queue_g, w) == true) {
    //printf ("Sending data to thread as block: %p (block: %d / %d)\n", data, block->num_spawn, block->num_terminated_spawn);
    atomic_fetch_add (&block->num_spawn, 1);
    return;
  }
  //printf ("Running data ourself as block: %p\n", data);
  /* No thread available. Call the function ourself */
  func (data);
}
#endif

/* Wait for all work orders to be finished */
static inline void
worker_sync(worker_sync_t block)
{
  //printf ("Waiting for thread terminasion.\n");
  /* If the number of spawns is greated than the number
     of terminated spawns, some spawns are still working.
     So wait for terminaison */
  while (atomic_load(&block->num_spawn) != atomic_load (&block->num_terminated_spawn));
}

/* Flush any work order in the queue if some remains.*/
static inline void
worker_flush(worker_t g)
{
  worker_order_t w;
  while (worker_queue_pop_blocking (&w, g->queue_g, false) == true) {
    workeri_exec(&w);
    worker_queue_pop_release(g->queue_g);
  }
}


/* Return the number of workers */
static inline size_t
worker_count(worker_t g)
{
  return g->numWorker_g + 1;
}

/* Spawn the 'core' block computation into another thread if
   a worker thread is available. Compute it in the current thread otherwise.
   'block' shall be the initialised synchronised block for all threads.
   'input' is the list of input variables of the 'core' block within "( )"
   'output' is the list of output variables of the 'core' block within "( )"
   Output variables are only available after a synchronisation block. */
#if WORKER_USE_CLANG_BLOCK
#define WORKER_SPAWN(_worker, _block, _input, _core, _output)           \
  WORKER_DEF_DATA(_input, _output)                                      \
  WORKER_DEF_SUBBLOCK(_input, _output, _core)                           \
  worker_spawn_block ((_worker), (_block), WORKER_SPAWN_SUBFUNC_NAME,  &WORKER_SPAWN_DATA_NAME)
#elif WORKER_USE_CPP_FUNCTION
// TODO: Explicit pass all arguments by reference.
#define WORKER_SPAWN(_worker, _block, _input, _core, _output)           \
  worker_spawn_function ((_worker), (_block), [&](void *param) {(void)param ; _core } ,  NULL)
#else
#define WORKER_SPAWN(_worker, _block, _input, _core, _output)           \
  WORKER_DEF_DATA(_input, _output)                                      \
  WORKER_DEF_SUBFUNC(_input, _output, _core)                            \
  worker_spawn ((_worker), (_block), WORKER_SPAWN_SUBFUNC_NAME,  &WORKER_SPAWN_DATA_NAME)
#endif

#define WORKER_SPAWN_STRUCT_NAME   M_C(worker_data_s_, __LINE__)
#define WORKER_SPAWN_DATA_NAME     M_C(worker_data_, __LINE__)
#define WORKER_SPAWN_SUBFUNC_NAME  M_C(worker_subfunc_, __LINE__)
#define WORKER_DEF_DATA(_input, _output)                        \
  struct WORKER_SPAWN_STRUCT_NAME {                             \
    WORKER_DEF_DATA_INPUT _input                                \
    M_IF_EMPTY _output ( , WORKER_DEF_DATA_OUTPUT _output)      \
      } WORKER_SPAWN_DATA_NAME = {                              \
    WORKER_INIT_DATA_INPUT _input                               \
    M_IF_EMPTY _output (, WORKER_INIT_DATA_OUTPUT _output)      \
  };
#define WORKER_DEF_SINGLE_INPUT(var) __typeof__(var) var;
#define WORKER_DEF_DATA_INPUT(...)                 \
  M_MAP(WORKER_DEF_SINGLE_INPUT, __VA_ARGS__)
#define WORKER_DEF_SINGLE_OUTPUT(var)              \
  __typeof__(var) *M_C(var, __ptr);
#define WORKER_DEF_DATA_OUTPUT(...)                 \
  M_MAP(WORKER_DEF_SINGLE_OUTPUT, __VA_ARGS__)
#define WORKER_INIT_SINGLE_INPUT(var)              \
  .var = var,
#define WORKER_INIT_DATA_INPUT(...)                \
  M_MAP(WORKER_INIT_SINGLE_INPUT, __VA_ARGS__)
#define WORKER_INIT_SINGLE_OUTPUT(var)              \
  .M_C(var, __ptr) = &var,
#define WORKER_INIT_DATA_OUTPUT(...)                \
  M_MAP(WORKER_INIT_SINGLE_OUTPUT, __VA_ARGS__)
#define WORKER_DEF_SUBFUNC(_input, _output, _core)                      \
  __extension__ auto void WORKER_SPAWN_SUBFUNC_NAME(void *) ;           \
  __extension__ void WORKER_SPAWN_SUBFUNC_NAME(void *_data)             \
  {                                                                     \
    struct WORKER_SPAWN_STRUCT_NAME *_s_data = _data ;                  \
    WORKER_INIT_LOCAL_INPUT _input                                      \
      M_IF_EMPTY _output ( , WORKER_INIT_LOCAL_OUTPUT _output)          \
      do { _core } while (0);                                           \
    M_IF_EMPTY _output ( , WORKER_PROPAGATE_LOCAL_OUTPUT _output)       \
  };
#define WORKER_DEF_SUBBLOCK(_input, _output, _core)                     \
  void (^WORKER_SPAWN_SUBFUNC_NAME) (void *) = ^ void (void * _data)    \
  {                                                                     \
    struct WORKER_SPAWN_STRUCT_NAME *_s_data = _data ;                  \
    WORKER_INIT_LOCAL_INPUT _input                                      \
      M_IF_EMPTY _output ( , WORKER_INIT_LOCAL_OUTPUT _output)          \
      do { _core } while (0);                                           \
    M_IF_EMPTY _output ( , WORKER_PROPAGATE_LOCAL_OUTPUT _output)       \
  };
#define WORKER_INIT_SINGLE_LOCAL_INPUT(var)     \
  __typeof__(var) var = _s_data->var;
#define WORKER_INIT_LOCAL_INPUT(...)                    \
  M_MAP(WORKER_INIT_SINGLE_LOCAL_INPUT, __VA_ARGS__)
#define WORKER_INIT_SINGLE_LOCAL_OUTPUT(var)    \
  __typeof__(var) var;
#define WORKER_INIT_LOCAL_OUTPUT(...)                   \
  M_MAP(WORKER_INIT_SINGLE_LOCAL_OUTPUT, __VA_ARGS__)
#define WORKER_PROPAGATE_SINGLE_OUTPUT(var)     \
  *(_s_data->M_C(var, __ptr)) = var;
#define WORKER_PROPAGATE_LOCAL_OUTPUT(...)              \
  M_MAP(WORKER_PROPAGATE_SINGLE_OUTPUT, __VA_ARGS__)


#else /* WORKER_DISABLE */


/* User has defined WORKER_DISABLE to disable the use of workers.
   Define empty types and empty functions
*/

typedef struct worker_block_s {
  int x;
} worker_sync_t[1];

typedef struct worker_s {
  int x;
} worker_t[1];

#define worker_init(g, numWorker, extraQueue, resetFunc) do { (void) g; } while (0)
#define worker_clear(g) do { (void) g; } while (0)
#define worker_start(b) do { (void) b; } while (0)
#define worker_spawn(w, b, f, d) do { f(d); } while (0)
#define worker_sync(b) do { (void) b; } while (0)
#define worker_count(w) 1
#define WORKER_SPAWN(w, b, i, c, o) do { c } while (0)

#endif /* WORKER_DISABLE */

#endif
