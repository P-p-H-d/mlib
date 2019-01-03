/*
 * M*LIB - Concurrent memory pool allocator
 *
 * Copyright (c) 2017-2019, Patrick Pelissier
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
#ifndef MSTARLIB_CONCURRENT_MEMPOOL_H
#define MSTARLIB_CONCURRENT_MEMPOOL_H

#include "m-core.h"
#include "m-atomic.h"
#include "m-genint.h"

/* Minimum number of nodes per group of nodes */
#define C_MEMPOOL_MIN_NODE_PER_GROUP 16

#define C_MEMPOOL_DEF(name, type_t)                     \
  C_MEMPOOL_DEF_SINGLY_LIST(name, type_t)               \
  C_MEMPOOL_DEF_LF_QUEUE(name, type_t)                  \
  C_MEMPOOL_DEF_LFMP_THREAD_MEMPOOL(name, type_t)       \
  C_MEMPOOL_DEF_SYSTEM_ALLOC(name, type_t)              \
  C_MEMPOOL_DEF_LF_MEMPOOL(name, type_t)

/* Classic Singly List without allocation */
#define C_MEMPOOL_DEF_SINGLY_LIST(name, type_t)                         \
                                                                        \
  typedef struct M_C(name, _slist_node_s) {                             \
    struct M_C(name, _slist_node_s) *next;                              \
    type_t               data;                                          \
  } M_C(name, _slist_node_t);                                           \
                                                                        \
  typedef struct M_C(name, _slist_node_s) *M_C(name, _slist_t)[1];      \
                                                                        \
  static inline void                                                    \
  M_C(name, _slist_init)(M_C(name, _slist_t) list)                      \
  {                                                                     \
    *list = NULL;                                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _slist_push)(M_C(name, _slist_t) list,                      \
                        M_C(name, _slist_node_t) *node)                 \
  {                                                                     \
    node->next = *list;                                                 \
    *list = node;                                                       \
  }                                                                     \
                                                                        \
  static inline M_C(name, _slist_node_t) *                              \
  M_C(name, _slist_pop)(M_C(name, _slist_t) list)                       \
  {                                                                     \
    assert (*list != NULL);                                             \
    M_C(name, _slist_node_t) *node = *list;                             \
    *list = node->next;                                                 \
    M_IF_DEBUG(node->next = NULL;)                                      \
    return node;                                                        \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _slist_empty_p)(M_C(name, _slist_t) list)                   \
  {                                                                     \
    return *list == NULL;                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _slist_move)(M_C(name, _slist_t) list,                      \
                         M_C(name, _slist_t) src)                       \
  {                                                                     \
    *list = *src;                                                       \
    *src  = NULL;                                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _slist_clear)(M_C(name, _slist_t) list)                     \
  {                                                                     \
    M_C(name, _slist_node_t) *it = *list, *next;                        \
    while (it) {                                                        \
      next = it->next;                                                  \
      M_MEMORY_DEL(it);                                                 \
      it = next;                                                        \
    }                                                                   \
    *list = NULL;                                                       \
  }                                                                     \


/* Lock Free free queue list (not generic one) of lists without allocation
   Based on Michael & Scott Lock Free Queue List algorithm.
   Each list is considered empty if there is only one node within.
   This LF Queue List doesn't try to prevent the ABA problem. It is up to the 
   caller to avoid recycling the nodes too fast.
   Each list has its own unique NIL ptr in order to avoid issues when 
   migrating a node from a Q to another: in the following scenario,
   - Thread 1 performs a PUSH of N in Q1 with Q1 empty (only node is NA)
   NA.next is NIL.
   - Thread 1 is interrupted just before the CAS on NA.next
   - Thread 2 performs a sucessfull push of NB in Q1. NA.next is set to NB.
   - Thread 2 performs a sucessfull pop of NA in Q1
   - Thread 2 performs a sucessfull push of NA in Q2. NA.next is set to NIL.
   - Thread 1 is restored and will succeed as NA.next is once again NIL.
   In order to prevent the last CAS to succeed, each queue uses its own NIL pointer.
   It is a derived problem of the ABA problem.
 */
/* TODO: Optimize alignement to reduce memory consumption. NIL object can use [] 
   to reduce memory consumption too (non compatible with C++ ...) */
#define C_MEMPOOL_DEF_LF_QUEUE(name, type_t)                            \
                                                                        \
  typedef struct M_C(name, _lf_node_s) {                                \
    M_ATTR_EXTENSION _Atomic(struct M_C(name, _lf_node_s) *) next;      \
    unsigned long                           cpt;                        \
    M_C(name, _slist_t)                     list;                       \
  } M_C(name, _lf_node_t);                                              \
                                                                        \
  typedef struct M_C(name, _lflist_s) {                                 \
    M_ATTR_EXTENSION _Atomic(M_C(name, _lf_node_t) *) head;             \
    char                       align1[M_ALIGN_FOR_CACHELINE_EXCLUSION]; \
    M_ATTR_EXTENSION _Atomic(M_C(name, _lf_node_t) *) tail;             \
    char                       align2[M_ALIGN_FOR_CACHELINE_EXCLUSION]; \
    M_C(name, _lf_node_t)            nil;                               \
  } M_C(name, _lflist_t)[1];                                            \
                                                                        \
  static inline void                                                    \
  M_C(name, _lflist_init)(M_C(name, _lflist_t) list,                    \
                          M_C(name, _lf_node_t) *node)                  \
  {                                                                     \
    atomic_init(&list->head, node);                                     \
    atomic_init(&list->tail, node);                                     \
    atomic_store_explicit(&node->next, &list->nil, memory_order_relaxed); \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _lflist_empty_p)(M_C(name, _lflist_t) list)                 \
  {                                                                     \
    return atomic_load(&list->tail) == atomic_load(&list->head);        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _lflist_push)(M_C(name, _lflist_t) list,                    \
                          M_C(name, _lf_node_t) *node, m_backoff_t bkoff) \
  {                                                                     \
    M_C(name, _lf_node_t) *tail;                                        \
    M_C(name, _lf_node_t) *next;                                        \
                                                                        \
    atomic_store_explicit(&node->next, &list->nil, memory_order_relaxed); \
    m_backoff_reset(bkoff);                                             \
    while (true) {                                                      \
      tail = atomic_load(&list->tail);                                  \
      next = atomic_load_explicit(&tail->next, memory_order_acquire);   \
      if (M_UNLIKELY(next != &list->nil)) {                             \
        /* Tail was not pointing to the last node                       \
           Try to swing Tail to the next node */                        \
        atomic_compare_exchange_weak_explicit(&list->tail,              \
                                              &tail, next,              \
                                              memory_order_release,     \
                                              memory_order_relaxed);    \
      } else {                                                          \
        /* Try to link node at the end of the linked list */            \
        if (atomic_compare_exchange_strong_explicit(&tail->next,        \
                                                    &next, node,        \
                                                    memory_order_release, \
                                                    memory_order_relaxed)) \
          break;                                                        \
        m_backoff_wait(bkoff);                                          \
      }                                                                 \
    }                                                                   \
    /* Enqueue is done.  Try to swing Tail to the inserted node         \
       If it fails, someone else will do it or has already did it. */   \
    atomic_compare_exchange_strong_explicit(&list->tail, &tail, node,   \
                                            memory_order_acq_rel,       \
                                            memory_order_relaxed);      \
  }                                                                     \
                                                                        \
  static inline M_C(name, _lf_node_t) *                                 \
  M_C(name, _lflist_pop)(M_C(name, _lflist_t) list, m_backoff_t bkoff)  \
  {                                                                     \
    M_C(name, _lf_node_t) *head;                                        \
    M_C(name, _lf_node_t) *tail;                                        \
    M_C(name, _lf_node_t) *next;                                        \
                                                                        \
    /* Reinitialize backoff */						\
    m_backoff_reset(bkoff);                                             \
    while (true) {                                                      \
      head = atomic_load(&list->head);                                  \
      tail = atomic_load(&list->tail);                                  \
      next = atomic_load(&head->next);                                  \
      /* Are head, tail, and next consistent?*/                         \
      if (M_LIKELY(head ==                                              \
                   atomic_load_explicit(&list->head, memory_order_relaxed))) \
        {                                                               \
          /* Is queue empty or Tail falling behind? */                  \
          if (head == tail) {                                           \
            /* Is queue empty? */                                       \
            if (next == &list->nil)                                     \
              return NULL;                                              \
            /* Tail is falling behind.  Try to advance it */            \
            atomic_compare_exchange_strong_explicit(&list->tail, &tail, \
                                                    next,               \
                                                    memory_order_release, \
                                                    memory_order_relaxed); \
          } else {                                                      \
            /* Try to swing Head to the next node */                    \
            if (atomic_compare_exchange_strong_explicit(&list->head,    \
                                                        &head, next,    \
                                                        memory_order_release, \
                                                        memory_order_relaxed)) { \
              break;                                                    \
            }                                                           \
	    /* Failure: perform a random exponential backoff */		\
            m_backoff_wait(bkoff);                                      \
          }                                                             \
        }                                                               \
    }                                                                   \
    /* dequeue returns an element that becomes the new dummy element (the new head), \
       and the former dummy element (the former head) is removed:       \
       Since we want a link of free list, and we don't care about the content itsef, \
       provided that the node we return is older than the one we should return, \
       Therefore, we return the previous dummy head.                    \
       As such, it is not the original MSqueue algorithm. */            \
    M_IF_DEBUG(atomic_store(&head->next, (M_C(name, _lf_node_t) *) 0);) \
   return head;                                                         \
  }                                                                     \
                                                                        \
  /* Dequeue a node if the node is old enough */			\
  static inline M_C(name, _lf_node_t) *                                 \
  M_C(name, _lflist_pop_if)(M_C(name, _lflist_t) list,                  \
                            unsigned long age, m_backoff_t bkoff)	\
  {                                                                     \
    M_C(name, _lf_node_t) *head;                                        \
    M_C(name, _lf_node_t) *tail;                                        \
    M_C(name, _lf_node_t) *next;                                        \
                                                                        \
    m_backoff_reset(bkoff);                                             \
    while (true) {                                                      \
      head = atomic_load(&list->head);                                  \
      tail = atomic_load(&list->tail);                                  \
      next = atomic_load(&head->next);                                  \
      if (M_LIKELY(head == atomic_load_explicit(&list->head, memory_order_relaxed))) \
        {                                                               \
          if (head == tail) {                                           \
            if (next == &list->nil)                                     \
              return NULL;                                              \
            atomic_compare_exchange_strong_explicit(&list->tail, &tail, next, \
                                                    memory_order_release, \
                                                    memory_order_relaxed); \
          } else {                                                      \
            /* Test if the node is old enought to be popped */          \
            if (next->cpt >= age)					\
              return NULL;                                              \
            /* Try to swing Head to the next node */                    \
            if (atomic_compare_exchange_strong_explicit(&list->head,    \
                                                        &head, next,    \
                                                        memory_order_release, \
                                                        memory_order_relaxed)) { \
              break;                                                    \
            }                                                           \
            m_backoff_wait(bkoff);                                      \
          }                                                             \
        }                                                               \
    }                                                                   \
    M_IF_DEBUG(atomic_store(&head->next, (M_C(name, _lf_node_t) *) 0);) \
   return head;                                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _lflist_clear)(M_C(name, _lflist_t) list)                   \
  {                                                                     \
    m_backoff_t bkoff;                                                  \
    m_backoff_init(bkoff);                                              \
    while (true) {                                                      \
      M_C(name, _lf_node_t) *node = M_C(name, _lflist_pop)(list, bkoff); \
      if (node == NULL) break;                                          \
      M_C(name, _lf_node_t) *next = atomic_load_explicit(&node->next,   \
                                                         memory_order_relaxed); \
      M_C(name, _slist_clear)(node->list);                              \
      M_MEMORY_DEL(node);                                               \
      node = next;                                                      \
    }                                                                   \
    /* Dummy node to free too */                                        \
    M_C(name, _lf_node_t) *dummy;                                       \
    dummy = atomic_load_explicit(&list->head, memory_order_relaxed);    \
    M_C(name, _slist_clear)(dummy->list);                               \
    M_MEMORY_DEL(dummy);                                                \
  }                                                                     \

/* System node allocator: request memory to the system.
   As such it is a non Lock-Free path. */
#define C_MEMPOOL_DEF_SYSTEM_ALLOC(name, type_t)                        \
                                                                        \
  static inline M_C(name, _lf_node_t) *                                 \
       M_C(name, _alloc_node)(unsigned int initial)                     \
  {                                                                     \
    M_C(name, _lf_node_t) * node;                                       \
    node = M_MEMORY_ALLOC(M_C(name, _lf_node_t));                       \
    if (M_UNLIKELY(node == NULL)) {                                     \
      M_MEMORY_FULL(sizeof(M_C(name, _lf_node_t)));                     \
    }                                                                   \
    atomic_init(&node->next, (M_C(name, _lf_node_t) *) 0);              \
    node->cpt = 0UL;                                                    \
    M_C(name, _slist_init)(node->list);                                 \
    for(unsigned i = 0; i < initial; i++) {                             \
      M_C(name, _slist_node_t) *n;                                      \
      n = M_MEMORY_ALLOC(M_C(name, _slist_node_t));                     \
      if (M_UNLIKELY(n == NULL)) {                                      \
        M_MEMORY_FULL(sizeof(M_C(name, _lf_node_t)));                   \
      }                                                                 \
      M_C(name, _slist_push)(node->list, n);                            \
    }                                                                   \
    return node;                                                        \
  }                                                                     \

/* Concurrent Memory pool
   The data structure is the following.
   Each thread has its own pool of nodes (local) that only it can
   access (it is a singly list). If there is no longer any node in this
   pool, it requests a new pool to the lock free queue of pool (group of
   nodes). If it fails, it requests a new pool to the system allocator
   (and from there it is no longer lock free).
   This memory pool can only be lock free if the initial state is 
   sufficiently dimensionned to avoid calling the system allocator during
   the normal processing.
   Then each thread pushs its deleted node into another pool of nodes,
   where the node is logically deleted (no contain of the node is destroyed
   at this point and the node can be freely accessed by other threads).
   Once the thread mempool is put to sleep, the age of the pool of logical 
   deleted nodes is computed and this pool is move to the Lock Free Queue 
   List of pools to be reclaimed. Then A Garbage Collector is performed
   on this Lock Free Queue list to reclaim all pools thare are sufficiently
   aged (taking into account the grace period of the pool) to be moved back
   to the Lock Free Queue of the free pools.

   Each pool of nodes can be in the following state:
   * FREE state if it is present in the Lock Free Queue of free pools.
   * EMPTY state if it is present in the Lock Free Queue of empty pools
   which means that the nodes present in it has been USED directly by a thread,
   * TO_BE_RECLAIMED state if it is present in the Lock Free Queue of TBR pools

   A pool of nodes will go to the following state:
    FREE --> EMPTY --> TO_BE_RECLAIMED
     ^                      |
     +----------------------+

   The ABA problem is taken into account as a node cannot be reused in the 
   same queue without performing a full cycle of its state. Moreover
   it can only move from TO_BE_RECLAIMED to FREE if and only if a grace 
   period is finished (and then we are sure that no thread references any
   older node).

   Each thread has its own backoff structure (with local pseudo-random
   generator).

   The grace period is detected through a global age counter (ticket)
   that is incremented each time a thread is awaken / sleep.
   Each thread has its own age that is set to the global ticket on sleep/awaken.
   The age of the pool to be reclaimed is also set to this global age counter.

   To ensure that the grace period is finished, it tests if all threads
   are younger than the age of the pool to be reclaimed.

   From a performance point of view, this puts a bottleneck on the global
   age counter that is shared and incremented by all threads. However,
   the sleep/awaken operations are much less frequent than other operations.
   Thus, it shall not have a huge impact on the performance if the user
   code is intelligent with the sleep/awaken operations.

   As such it won't support more than ULONG_MAX sleep for all threads.
   TODO: Compute if sufficient (worst cast ULONG_MAX is 32 bits)
*/
#define C_MEMPOOL_DEF_LFMP_THREAD_MEMPOOL(name, type_t)                 \
                                                                        \
  typedef struct M_C(name, _lfmp_thread_t) {                            \
    atomic_ulong         ticket;                                        \
    m_backoff_t          bkoff;                                         \
    M_C(name, _slist_t)  free;                                          \
    M_C(name, _slist_t)  to_be_reclaimed;                               \
    char                 align1[M_ALIGN_FOR_CACHELINE_EXCLUSION];       \
  } M_C(name, _lfmp_thread_t);                                          \
                                                                        \
  static inline void                                                    \
  M_C(name, _lfmp_thread_init)(M_C(name, _lfmp_thread_t) *t)            \
  {                                                                     \
    atomic_init(&t->ticket, ULONG_MAX);                                 \
    m_backoff_init(t->bkoff);                                           \
    M_C(name, _slist_init)(t->free);                                    \
    M_C(name, _slist_init)(t->to_be_reclaimed);                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _lfmp_thread_clear)(M_C(name, _lfmp_thread_t) *t)           \
  {                                                                     \
    assert(M_C(name, _slist_empty_p)(t->to_be_reclaimed));              \
    M_C(name, _slist_clear)(t->free);                                   \
    M_C(name, _slist_clear)(t->to_be_reclaimed);                        \
    m_backoff_clear(t->bkoff);                                          \
  }                                                                     \

/* NOTE: once a node is deleted, its data are kept readable until the future GC */
#define C_MEMPOOL_DEF_LF_MEMPOOL(name, type_t)                          \
                                                                        \
  typedef struct M_C(name, _s) {                                        \
    atomic_ulong              ticket;                                   \
    unsigned                  initial;                                  \
    unsigned                  max_thread;                               \
    genint_t                  thread_alloc;                             \
    M_C(name, _lfmp_thread_t) *thread_data;                             \
    M_C(name, _lflist_t)      free;                                     \
    M_C(name, _lflist_t)      to_be_reclaimed;                          \
    M_C(name, _lflist_t)      empty;                                    \
  } M_C(name, _t)[1];                                                   \
                                                                        \
  typedef int M_C(name, _tid_t);                                        \
                                                                        \
  static inline void                                                    \
  M_C(name, _init)(M_C(name, _t) mem, unsigned long max_thread,         \
                   unsigned init_node_count, unsigned init_group_count) \
  {                                                                     \
    atomic_init(&mem->ticket, 0UL);                                     \
    genint_init(mem->thread_alloc, max_thread);                         \
    mem->initial = M_MAX(C_MEMPOOL_MIN_NODE_PER_GROUP, init_node_count); \
    mem->thread_data = M_MEMORY_REALLOC(M_C(name, _lfmp_thread_t), NULL, max_thread); \
    if (mem->thread_data == NULL) {                                     \
      M_MEMORY_FULL(max_thread * sizeof(M_C(name, _lfmp_thread_t)));    \
    }                                                                   \
    for(unsigned i = 0; i < max_thread;i++) {                           \
      M_C(name, _lfmp_thread_init)(&mem->thread_data[i]);               \
    }                                                                   \
    M_C(name, _lflist_init)(mem->free, M_C(name, _alloc_node)(init_node_count)); \
    M_C(name, _lflist_init)(mem->to_be_reclaimed, M_C(name, _alloc_node)(init_node_count)); \
    M_C(name, _lflist_init)(mem->empty, M_C(name, _alloc_node)(0));     \
    for(unsigned i = 1; i < init_group_count; i++) {                    \
      M_C(name, _lflist_push)(mem->free, M_C(name, _alloc_node)(init_node_count), \
                              mem->thread_data[0].bkoff);               \
      M_C(name, _lflist_push)(mem->empty, M_C(name, _alloc_node)(0),    \
                              mem->thread_data[0].bkoff);               \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _clear)(M_C(name, _t) mem)                                  \
  {                                                                     \
    for(unsigned i = 0; i < mem->max_thread;i++) {                      \
      M_C(name, _lfmp_thread_clear)(&mem->thread_data[i]);              \
    }                                                                   \
    M_C(name, _lflist_clear)(mem->empty);                               \
    M_C(name, _lflist_clear)(mem->free);                                \
    assert(M_C(name, _lflist_empty_p)(mem->to_be_reclaimed));           \
    M_C(name, _lflist_clear)(mem->to_be_reclaimed);                     \
    genint_clear(mem->thread_alloc);                                    \
  }                                                                     \
                                                                        \
  static inline M_C(name, _tid_t)                                       \
  M_C(name, _attach_thread)(M_C(name, _t) mem)                          \
  {                                                                     \
    return genint_pop(mem->thread_alloc);                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _detach_thread)(M_C(name, _t) mem, M_C(name, _tid_t) id)    \
  {                                                                     \
    assert(M_C(name, _slist_empty_p)(mem->thread_data[id].to_be_reclaimed)); \
    assert(atomic_load(&mem->thread_data[id].ticket) == ULONG_MAX);     \
    genint_push(mem->thread_alloc, id);                                 \
  }                                                                     \
                                                                        \
  static inline type_t *                                                \
  M_C(name, _new)(M_C(name, _t) mem, M_C(name, _tid_t) id)              \
  {                                                                     \
    M_C(name, _slist_node_t) *snode;                                    \
    M_C(name, _lf_node_t) *node;                                        \
    while (true) {                                                      \
      /* Fast & likely path where we access the thread pool of nodes */ \
      if (M_LIKELY(!M_C(name, _slist_empty_p)(mem->thread_data[id].free))) { \
        snode = M_C(name, _slist_pop)(mem->thread_data[id].free);       \
        return &snode->data;                                            \
      }                                                                 \
      /* Request a group node to the freelist of groups */              \
      node = M_C(name, _lflist_pop)(mem->free, mem->thread_data[id].bkoff); \
      if (M_UNLIKELY (node == NULL)) {                                  \
        /* Request a new group to the system. Non Lock Free path */     \
        assert(mem->initial > 0);                                       \
        node = M_C(name, _alloc_node)(mem->initial);                    \
        assert(!M_C(name, _slist_empty_p)(node->list));                 \
      }                                                                 \
      M_C(name, _slist_move)(mem->thread_data[id].free, node->list);    \
      /* Push back the empty group */                                   \
      assert (M_C(name, _slist_empty_p)(node->list));                   \
      M_C(name, _lflist_push)(mem->empty, node, mem->thread_data[id].bkoff); \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _del)(M_C(name, _t) mem, type_t *d, M_C(name, _tid_t) id)   \
  {                                                                     \
    M_C(name, _slist_node_t) *snode;                                    \
    assert( d != NULL);                                                 \
    snode = M_TYPE_FROM_FIELD(M_C(name, _slist_node_t), d, type_t, data); \
    M_C(name, _slist_push)(mem->thread_data[id].to_be_reclaimed, snode); \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _awake)(M_C(name, _t) mem, M_C(name, _tid_t) id)            \
  {                                                                     \
    unsigned long t = atomic_fetch_add(&mem->ticket, 1UL) + 1;          \
    atomic_store(&mem->thread_data[id].ticket, t);                      \
    assert(M_C(name, _slist_empty_p)(mem->thread_data[id].to_be_reclaimed)); \
  }                                                                     \
                                                                        \
  static inline unsigned long                                           \
  M_C(name, _int_min_ticket)(M_C(name, _t) mem)                         \
  {                                                                     \
    unsigned long min = atomic_load(&mem->thread_data[0].ticket);       \
    for(unsigned i = 1; i < mem->max_thread; i++) {                     \
      unsigned long t = atomic_load(&mem->thread_data[i].ticket);       \
      min = M_MIN(t, min);                                              \
    }                                                                   \
    return min;                                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _int_gc_on_sleep)(M_C(name, _t) mem, M_C(name, _tid_t) id)  \
  {                                                                     \
    const unsigned long min_ticket = M_C(name, _int_min_ticket)(mem);   \
    /* Perform a GC of the freelist of nodes */                         \
    while (true) {                                                      \
      M_C(name, _lf_node_t) *node;                                      \
      node = M_C(name, _lflist_pop_if)(mem->to_be_reclaimed,            \
                                       min_ticket, mem->thread_data[id].bkoff); \
      if (node == NULL) break;                                          \
      M_C(name, _lflist_push)(mem->free, node, mem->thread_data[id].bkoff); \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _sleep)(M_C(name, _t) mem, M_C(name, _tid_t) id)            \
  {                                                                     \
    /* Move the local nodes to be reclaimed to the thread into the global pool */ \
    if (!M_C(name, _slist_empty_p)(mem->thread_data[id].to_be_reclaimed)) { \
      M_C(name, _lf_node_t) *node;                                      \
      node = M_C(name, _lflist_pop)(mem->empty, mem->thread_data[id].bkoff); \
      if (M_UNLIKELY (node == NULL)) {                                  \
        node = M_C(name, _alloc_node)(0);                               \
      }                                                                 \
      assert(M_C(name, _slist_empty_p)(node->list));                    \
      M_C(name, _slist_move)(node->list, mem->thread_data[id].to_be_reclaimed); \
      node->cpt = atomic_load(&mem->ticket);                            \
      M_C(name, _lflist_push)(mem->to_be_reclaimed, node, mem->thread_data[id].bkoff); \
    }                                                                   \
    /* Increase life time of the thread */                              \
    unsigned long t = atomic_fetch_add(&mem->ticket, 1UL) + 1;          \
    atomic_store(&mem->thread_data[id].ticket, t);                      \
    /* Perform a garbage collect */                                     \
    M_C(name, _int_gc_on_sleep)(mem, id);                               \
    /* Sleep the thread */                                              \
    atomic_store(&mem->thread_data[id].ticket, ULONG_MAX);              \
  }                                                                     \

#endif
