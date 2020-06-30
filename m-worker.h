/*
 * M*LIB / WORKER - Extra worker interface
 *
 * Copyright (c) 2017-2020, Patrick Pelissier
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
#ifndef MSTARLIB_WORKER_H
#define MSTARLIB_WORKER_H

/* The User Code can define M_USE_WORKER to 0 to disable the use of workers.
   The macros / functions are then defined to only use one core.
   By default, the behavior is to use workers.
*/
#ifndef M_USE_WORKER
# define M_USE_WORKER 1
#endif


#if M_USE_WORKER

#include "m-atomic.h"
#include "m-buffer.h"
#include "m-mutex.h"

/* Include needed system header for detection of how many core on the system */
#if defined(_WIN32)
# include <windows.h>
#elif (defined(__APPLE__) && defined(__MACH__)) \
  || defined(__DragonFly__) || defined(__FreeBSD__) \
  || defined(__NetBSD__) || defined(__OpenBSD__)
# include <sys/param.h>
# include <sys/sysctl.h>
# define M_USE_WORKER_SYSCTL 1
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

   Otherwise go with nested function (GCC) for the MACRO version.

   This behavior can be overriden by User Code by defining to 1 or 0 the
   following macros:
   * M_USE_WORKER_CPP_FUNCTION
   * M_USE_WORKER_CLANG_BLOCK
*/

#if defined(__cplusplus) && !defined(M_USE_WORKER_CPP_FUNCTION)
# define M_USE_WORKER_CPP_FUNCTION 1
# include <functional>
#elif defined(__has_extension) && !defined(M_USE_WORKER_CLANG_BLOCK)
# if __has_extension(blocks)
#  define M_USE_WORKER_CLANG_BLOCK 1
# endif
#endif

#ifndef M_USE_WORKER_CLANG_BLOCK
# define M_USE_WORKER_CLANG_BLOCK 0
#endif
#ifndef M_USE_WORKER_CPP_FUNCTION
# define M_USE_WORKER_CPP_FUNCTION 0
#endif

/* Control that not both options are selected at the same time.
   Note: there are not really incompatible, but if we use C++ we shall go to
   lambda directly (there is no need to support blocks). */
#if M_USE_WORKER_CLANG_BLOCK && M_USE_WORKER_CPP_FUNCTION
# error M_USE_WORKER_CPP_FUNCTION and M_USE_WORKER_CLANG_BLOCK are both defined. This is not supported.
#endif

/* Definition of a work order */
typedef struct work_order_s {
  struct worker_sync_s *block;    // Reference to the shared Synchronization block
  void * data;                    // The work order data
  void (*func) (void *data);      // The work order function (for GCC)
#if M_USE_WORKER_CLANG_BLOCK
  void (^blockFunc)(void *data);  // The work order function (block for clang)
#endif
#if M_USE_WORKER_CPP_FUNCTION
  std::function<void(void*)> function; // The work order function (for C++)
#endif
} worker_order_t;

/* Define the macros needed to initialize an order.
 * * MACRO to be used to send an empty order to stop the thread
 * * MACRO to complete the not-used fields
 */
#if M_USE_WORKER_CLANG_BLOCK || M_USE_WORKER_CPP_FUNCTION
# define WORKERI_EMPTY_ORDER { NULL, NULL, NULL, NULL }
# define WORKERI_EXTRA_ORDER , NULL
#else
# define WORKERI_EMPTY_ORDER { NULL, NULL, NULL }
# define WORKERI_EXTRA_ORDER
#endif

/* As in C++, it uses std::function, M_POD_OPLIST
   is not sufficient for initialization of the structure.
   So let's use C++ constructor, destructor and copy constructor */
#if M_USE_WORKER_CPP_FUNCTION
# define WORKERI_CPP_INIT(x) (new (&(x)) worker_order_t())
# define WORKERI_CPP_INIT_SET(x, y) (new (&(x)) worker_order_t(y))
# define WORKERI_CPP_SET(x, y) ((x) = (y))
# define WORKERI_CPP_CLEAR(x) ((&(x))->~worker_order_t())
# define WORKERI_CPP_INIT_MOVE(x,y) (new (&(x)) worker_order_t(y), ((&(y))->~worker_order_t()))
# define WORKER_OPLIST                                                        \
      (INIT(WORKERI_CPP_INIT), INIT_SET(WORKERI_CPP_INIT_SET),                \
      SET(WORKERI_CPP_SET), CLEAR(WORKERI_CPP_CLEAR), INIT_MOVE(WORKERI_CPP_INIT_MOVE) )
#else
# define WORKER_OPLIST M_POD_OPLIST
#endif

/* Definition of a worker (implemented by a thread) */
typedef struct worker_thread_s {
  m_thread_t id;
} worker_thread_t;

/* Definition of the queue that will record the work orders */
BUFFER_DEF(worker_queue, worker_order_t, 0, 
           BUFFER_QUEUE|BUFFER_UNBLOCKING_PUSH|BUFFER_BLOCKING_POP|BUFFER_THREAD_SAFE|BUFFER_DEFERRED_POP, WORKER_OPLIST)

/* Definition the global pool of workers */
typedef struct worker_s {
  /* The work order queue */
  worker_queue_t queue_g;

  /* The table of available workers */
  worker_thread_t *worker;

  /* Number of workers in the table */
  unsigned int numWorker_g;

  /* The global reset function */
  void (*resetFunc_g)(void);

  /* The global clear function */
  void (*clearFunc_g)(void);

  m_mutex_t lock;
  m_cond_t  a_thread_ends;        // A worker has ended

} worker_t[1];

/* Definition of the synchronization point for workers */
typedef struct worker_sync_s {
  atomic_int num_spawn;                 // Number of spawned workers accord this synchronization point
  atomic_int num_terminated_spawn;      // Number of terminated spawned workers
  struct worker_s *worker;              // Reference to the pool of workers
} worker_sync_t[1];

/* Return the number of CPU of the system */
static inline int
workeri_get_cpu_count(void)
{
#if defined(_WIN32)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  assert(sysinfo.dwNumberOfProcessors <= INT_MAX);
  return (int) sysinfo.dwNumberOfProcessors;
#elif defined(M_USE_WORKER_SYSCTL)
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

// (INTERNAL) Debug support for workers
#if 1
#define WORKERI_DEBUG(...) (void) 0
#else
#define WORKERI_DEBUG(...) printf(__VA_ARGS__)
#endif

/* Execute the registered work order **synchronously** */
static inline void
workeri_exec(worker_order_t *w)
{
  assert (w!= NULL && w->block != NULL);
  WORKERI_DEBUG ("Starting thread with data %p\n", w->data);
#if M_USE_WORKER_CLANG_BLOCK
  WORKERI_DEBUG ("Running %s f=%p b=%p\n", (w->func == NULL) ? "Blocks" : "Function", w->func, w->blockFunc);
  if (w->func == NULL)
    w->blockFunc(w->data);
  else
#endif
#if M_USE_WORKER_CPP_FUNCTION
    WORKERI_DEBUG ("Running %s f=%p b=%p\n", (w->function == NULL) ? "Lambda" : "Function", w->func, w->blockFunc);
    if (w->function)
      w->function(w->data);
    else
#endif
      w->func(w->data);
  /* Increment the number of terminated work order for the synchronous point */
  atomic_fetch_add (&w->block->num_terminated_spawn, 1);
}


/* The worker thread main loop*/
static inline void
workeri_thread(void *arg)
{
  // Get back the given argument
  struct worker_s *g = M_ASSIGN_CAST(struct worker_s *, arg);
  while (true) {
    worker_order_t w;
    // If needed, reset the global state of the worker
    if (g->resetFunc_g != NULL) {
      g->resetFunc_g();
    }
    // Waiting for data
    WORKERI_DEBUG ("Waiting for data (queue: %lu / %lu)\n", worker_queue_size(g->queue_g), worker_queue_capacity(g->queue_g));
    worker_queue_pop(&w, g->queue_g);
    // We received a work order 
    // Note: that the work order is still present in the queue
    // preventing further work order to be pushed in the queue until it finishes doing the work
    // If a stop request is received, terminate the thread 
    if (w.block == NULL) break;
    // Execute the work order
    workeri_exec(&w);
    // Consumme fully the work order in the queue
    worker_queue_pop_release(g->queue_g);
    // Signal that a worker has finished.
    m_mutex_lock(g->lock);
    m_cond_broadcast(g->a_thread_ends);
    m_mutex_unlock(g->lock);
  }
  // If needed, clear global state of the thread
  if (g->clearFunc_g != NULL) {
    g->clearFunc_g();
  }
}

/* Initialization of the worker module (constructor)
   Input:
   @numWorker: number of worker to create (0=autodetect, -1=2*autodetect)
   @extraQueue: number of extra work order we can get if all workers are full
   @resetFunc: function to reset the state of a worker between work orders (or NULL if none)
   @clearFunc: function to clear the state of a worker before terminaning (or NULL if none)
*/
static inline void
worker_init(worker_t g, int numWorker, unsigned int extraQueue, void (*resetFunc)(void), void (*clearFunc)(void))
{
  assert (numWorker >= -1);
  // Auto compute number of workers if the argument is 0
  if (numWorker <= 0)
    numWorker = (1 + (numWorker == -1))*workeri_get_cpu_count()-1;
  WORKERI_DEBUG ("Starting queue with: %d\n", numWorker + extraQueue);
  // Initialization
  assert(numWorker > 0);
  size_t numWorker_st = (size_t) numWorker;
  g->worker = M_MEMORY_REALLOC(worker_thread_t, NULL, numWorker_st);
  if (g->worker == NULL) {
    M_MEMORY_FULL(sizeof (worker_thread_t) * numWorker_st);
    return;
  }
  worker_queue_init(g->queue_g, numWorker_st + extraQueue);
  g->numWorker_g = (unsigned int) numWorker_st;
  g->resetFunc_g = resetFunc;
  g->clearFunc_g = clearFunc;
  m_mutex_init(g->lock);
  m_cond_init(g->a_thread_ends);
  
  // Create & start the workers
  for(size_t i = 0; i < numWorker_st; i++) {
    m_thread_create(g->worker[i].id, workeri_thread, M_ASSIGN_CAST(void*, g));
  }
}
/* Initialization of the worker module (constructor)
   Input:
   @numWorker: number of worker to create (0=autodetect, -1=2*autodetect)
   @extraQueue: number of extra work order we can get if all workers are full
   @resetFunc: function to reset the state of a worker between work orders (optional)
   @clearFunc: function to clear the state of a worker before terminaning (optional)
*/
#define worker_init(...) worker_init(M_DEFAULT_ARGS(5, (0, 0, NULL, NULL), __VA_ARGS__))

/* Clear of the worker module (destructor) */
static inline void
worker_clear(worker_t g)
{
  assert (worker_queue_empty_p (g->queue_g));
  // Push the terminate order on the queue
  for(unsigned int i = 0; i < g->numWorker_g; i++) {
    worker_order_t w = WORKERI_EMPTY_ORDER;
    // Normaly all worker threads shall be waiting at this
    // stage, so all push won't block as the queue is empty.
    // But for robustness, let's wait.
    worker_queue_push_blocking (g->queue_g, w, true);
  }
  // Wait for thread terminanison
  for(unsigned int i = 0; i < g->numWorker_g; i++) {
    m_thread_join(g->worker[i].id);
  }
  // Clear memory
  M_MEMORY_FREE(g->worker);
  m_mutex_clear(g->lock);
  m_cond_clear(g->a_thread_ends);
  worker_queue_clear(g->queue_g);
}

/* Start a new collaboration between workers of pool 'g'
   by defining the synchronization point 'block' */
static inline void
worker_start(worker_sync_t block, worker_t g)
{
  atomic_init (&block->num_spawn, 0);
  atomic_init (&block->num_terminated_spawn, 0);
  block->worker = g;
}

/* Spawn the given work order to workers if possible,
   or do it ourself if no worker is available.
   The synchronization point is defined a 'block'
   The work order if composed of the function 'func' and its 'data'
*/
static inline void
worker_spawn(worker_sync_t block, void (*func)(void *data), void *data)
{
  const worker_order_t w = {  block, data, func WORKERI_EXTRA_ORDER };
  if (M_UNLIKELY (!worker_queue_full_p(block->worker->queue_g))
      && worker_queue_push (block->worker->queue_g, w) == true) {
    WORKERI_DEBUG ("Sending data to thread: %p (block: %d / %d)\n", data, block->num_spawn, block->num_terminated_spawn);
    atomic_fetch_add (&block->num_spawn, 1);
    return;
  }
  WORKERI_DEBUG ("Running data ourself: %p\n", data);
  /* No worker available. Call the function ourself */
  (*func) (data);
}

#if M_USE_WORKER_CLANG_BLOCK
/* Spawn or not the given work order to workers,
   or do it ourself if no worker is available */
static inline void
worker_spawn_block(worker_sync_t block, void (^func)(void *data), void *data)
{
  const worker_order_t w = {  block, data, NULL, func };
  if (M_UNLIKELY (!worker_queue_full_p(block->worker->queue_g))
      && worker_queue_push (block->worker->queue_g, w) == true) {
    WORKERI_DEBUG ("Sending data to thread as block: %p (block: %d / %d)\n", data, block->num_spawn, block->num_terminated_spawn);
    atomic_fetch_add (&block->num_spawn, 1);
    return;
  }
  WORKERI_DEBUG ("Running data ourself as block: %p\n", data);
  /* No worker available. Call the function ourself */
  func (data);
}
#endif

#if M_USE_WORKER_CPP_FUNCTION
/* Spawn or not the given work order to workers,
   or do it ourself if no worker is available */
static inline void
worker_spawn_function(worker_sync_t block, std::function<void(void *data)> func, void *data)
{
  const worker_order_t w = {  block, data, NULL, func };
  if (M_UNLIKELY (!worker_queue_full_p(block->worker->queue_g))
      && worker_queue_push (block->worker->queue_g, w) == true) {
    WORKERI_DEBUG ("Sending data to thread as block: %p (block: %d / %d)\n", data, block->num_spawn, block->num_terminated_spawn);
    atomic_fetch_add (&block->num_spawn, 1);
    return;
  }
  WORKERI_DEBUG ("Running data ourself as block: %p\n", data);
  /* No worker available. Call the function ourself */
  func (data);
}
#endif

/* Test if all work orders of the given synchronization point are finished */
static inline bool
worker_sync_p(worker_sync_t block)
{
  /* If the number of spawns is greated than the number
     of terminated spawns, some spawns are still working.
     So wait for terminaison */
  return (atomic_load(&block->num_spawn) == atomic_load (&block->num_terminated_spawn));
}

/* Wait for all work orders of the given synchronization point to be finished */
static inline void
worker_sync(worker_sync_t block)
{
  WORKERI_DEBUG ("Waiting for thread terminasion.\n");
  // Fast case: all workers have finished
  if (worker_sync_p(block)) return;
  // Slow case: perform a locked wait to put this thread to waiting state
  m_mutex_lock(block->worker->lock);
  while (!worker_sync_p(block)) {
    m_cond_wait(block->worker->a_thread_ends, block->worker->lock);
  }
  m_mutex_unlock(block->worker->lock);
}

/* Flush any work order in the queue ourself if some remains.*/
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
#if M_USE_WORKER_CLANG_BLOCK
#define WORKER_SPAWN(_block, _input, _core, _output)           \
  WORKER_DEF_DATA(_input, _output)                                      \
  WORKER_DEF_SUBBLOCK(_input, _output, _core)                           \
  worker_spawn_block ((_block), WORKER_SPAWN_SUBFUNC_NAME,  &WORKER_SPAWN_DATA_NAME)
#elif M_USE_WORKER_CPP_FUNCTION
// TODO: Explicit pass all arguments by reference.
#define WORKER_SPAWN(_block, _input, _core, _output)           \
  worker_spawn_function ((_block), [&](void *param) {(void)param ; _core } ,  NULL)
#else
#define WORKER_SPAWN(_block, _input, _core, _output)           \
  WORKER_DEF_DATA(_input, _output)                                      \
  WORKER_DEF_SUBFUNC(_input, _output, _core)                            \
  worker_spawn ((_block), WORKER_SPAWN_SUBFUNC_NAME,  &WORKER_SPAWN_DATA_NAME)
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
  __typeof__(var) *M_C(var, _ptr);
#define WORKER_DEF_DATA_OUTPUT(...)                 \
  M_MAP(WORKER_DEF_SINGLE_OUTPUT, __VA_ARGS__)
#define WORKER_INIT_SINGLE_INPUT(var)              \
  .var = var,
#define WORKER_INIT_DATA_INPUT(...)                \
  M_MAP(WORKER_INIT_SINGLE_INPUT, __VA_ARGS__)
#define WORKER_INIT_SINGLE_OUTPUT(var)              \
  .M_C(var, _ptr) = &var,
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
  *(_s_data->M_C(var, _ptr)) = var;
#define WORKER_PROPAGATE_LOCAL_OUTPUT(...)              \
  M_MAP(WORKER_PROPAGATE_SINGLE_OUTPUT, __VA_ARGS__)


#else /* M_USE_WORKER */

/*   Define empty types and empty functions to not use any worker */

typedef struct worker_block_s {
  int x;
} worker_sync_t[1];

typedef struct worker_s {
  int x;
} worker_t[1];

#define worker_init(g, numWorker, extraQueue, resetFunc) do { (void) g; } while (0)
#define worker_clear(g) do { (void) g; } while (0)
#define worker_start(b, w) do { (void) b; } while (0)
#define worker_spawn(b, f, d) do { f(d); } while (0)
#define worker_sync(b) do { (void) b; } while (0)
#define worker_count(w) 1
#define WORKER_SPAWN(b, i, c, o) do { c } while (0)

#endif /* M_USE_WORKER */

#endif
