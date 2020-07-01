/*
 * M*LIB - Fixed size (Bounded) QUEUE & STACK interface
 *
 * Copyright 2020 - 2020, SP Vladislav Dmitrievich Turbanov
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
#ifndef MSTARLIB_BUFFER_H
#define MSTARLIB_BUFFER_H

#include "m-core.h"
#include "m-mutex.h"
#include "m-atomic.h"

/* Define the different kind of policy a lock-based buffer can have:
 * - the buffer can be either a queue (policy is FIFO) or a stack (policy is FILO),
 * - if the push method is by default blocking (waiting for the buffer to has some space) or not, *** deprecated ***
 * - if the pop method is by default blocking (waiting for the buffer to has some data) or not, *** deprecated ***
 * - if both methods are blocking, *** deprecated ***
 * - if it shall be thread safe or not (i.e. remove the mutex lock and atomic costs),
 * - if the buffer has to be init with empty elements, or if it shall init an element when it is pushed (and moved when popped),
 * - if the buffer has to overwrite the last element if the buffer is full,
 * - if the pop of an element is not complete until the call to pop_release (preventing push until this call).
 */
typedef enum {
  BUFFER_QUEUE = 0,    BUFFER_STACK = 1,
  BUFFER_BLOCKING_PUSH = 0, BUFFER_UNBLOCKING_PUSH = 2,
  BUFFER_BLOCKING_POP = 0, BUFFER_UNBLOCKING_POP = 4,
  BUFFER_BLOCKING = 0, BUFFER_UNBLOCKING = 6,
  BUFFER_THREAD_SAFE = 0, BUFFER_THREAD_UNSAFE = 8,
  BUFFER_PUSH_INIT_POP_MOVE = 16,
  BUFFER_PUSH_OVERWRITE = 32,
  BUFFER_DEFERRED_POP = 64
} buffer_policy_e;

/* Define a lock based buffer.
   USAGE: BUFFER_DEF(name, type, size_of_buffer_or_0, policy[, oplist]) */
#define BUFFER_DEF(name, type, m_size, ... )                            \
  BUFFERI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                            \
              ((name, type, m_size,__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(type)(), M_C(name,_t)), \
               (name, type, m_size,__VA_ARGS__,                                      M_C(name,_t))))

/* Define the oplist of a lock based buffer given its name and its oplist.
   USAGE: BUFFER_OPLIST(name[, oplist of the type]) */
#define BUFFER_OPLIST(...)                                              \
  BUFFERI_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                         \
                 ((__VA_ARGS__, M_DEFAULT_OPLIST),			\
                  (__VA_ARGS__ )))

/* Define a nearly lock-free queue for Many Producer Many Consummer
   Much faster than queue of BUFFER_DEF in heavy communication scenario
   Size of created queue can only a power of 2.
*/
#define QUEUE_MPMC_DEF(name, type, ...)					\
  QUEUEI_MPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
                  ((name, type, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(type)(), M_C(name,_t)), \
                   (name, type, __VA_ARGS__,                                      M_C(name,_t))))

/* Define a wait-free queue for Single Producer Single Consummer
   Much faster than queue of BUFFER_DEF in heavy communication scenario
   Size of created queue can only a power of 2.
*/
#define QUEUE_SPSC_DEF(name, type, ...)					\
  QUEUEI_SPSC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
                  ((name, type, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(type)(), M_C(name,_t)), \
                   (name, type, __VA_ARGS__,                                      M_C(name,_t))))




/********************************** INTERNAL ************************************/

#define BUFFERI_IF_CTE_SIZE(m_size)  M_IF(M_BOOL(m_size))
#define BUFFERI_SIZE(m_size)         BUFFERI_IF_CTE_SIZE(m_size) (m_size, v->size)

#define BUFFERI_POLICY_P(policy, val)                                   \
  (((policy) & (val)) != 0)

#define BUFFERI_CONTRACT(buffer, size)	do {		\
    assert (buffer != NULL);				\
    assert (buffer->data != NULL);			\
  }while (0)

#define BUFFERI_PROTECTED_CONTRACT(buffer, size) do {                   \
    assert (atomic_load(&buffer->number[0]) <= BUFFERI_SIZE(size));	\
  } while (0)

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define BUFFERI_DEF_P1(arg) BUFFERI_DEF_P2 arg

/* Validate the value oplist before going further */
#define BUFFERI_DEF_P2(name, type, m_size, policy, oplist, buffer_t)    \
  M_IF_OPLIST(oplist)(BUFFERI_DEF_P3, BUFFERI_DEF_FAILURE)(name, type, m_size, policy, oplist, buffer_t)

/* Stop processing with a compilation failure */
#define BUFFERI_DEF_FAILURE(name, type, m_size, policy, oplist, buffer_t) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(BUFFER_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

#define BUFFERI_DEF_P3(name, type, m_size, policy, oplist, buffer_t)    \
                                                                        \
  typedef struct M_C(name, _s) {					\
    m_mutex_t mutexPush;    /* MUTEX used for pushing elements */       \
    size_t    idx_prod;     /* Index of the production threads  */      \
    size_t    overwrite;    /* Number of overwritten values */          \
    m_cond_t there_is_data; /* condition raised when there is data */   \
    m_cond_t there_is_room_for_data; /* Cond. raised when there is room */ \
    m_mutex_t mutexPop;     /* MUTEX used for popping elements */       \
    size_t    idx_cons;     /* Index of the consumption threads */      \
    BUFFERI_IF_CTE_SIZE(m_size)( ,size_t size;) /* Size of the buffer */ \
    /* number[0] := Number of elements in the buffer */                 \
    /* number[1] := [OPTION] Number of elements being deferred in the buffer */ \
    atomic_ulong number[1 + BUFFERI_POLICY_P(policy, BUFFER_DEFERRED_POP)]; \
    /* If fixed size, array of elements, otherwise pointer to element */ \
    BUFFERI_IF_CTE_SIZE(m_size)(type data[m_size], type *data);         \
  } buffer_t[1];                                                        \
                                                                        \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
  typedef union { M_C(name, _srcptr) cptr; M_C(name, _ptr) ptr; } M_C(name, _uptr); \
  typedef type M_C(name, _type_t);                                      \
                                                                        \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                      \
                                                                        \
static inline void                                                      \
M_C(name, M_NAMING_INIT)(buffer_t v, size_t size)                       \
{                                                                       \
  BUFFERI_IF_CTE_SIZE(m_size)(assert(size == m_size), v->size = size);  \
  v->idx_prod = v->idx_cons = v->overwrite = 0;                         \
  atomic_init (&v->number[0], 0UL);                                     \
  if (BUFFERI_POLICY_P(policy, BUFFER_DEFERRED_POP))                    \
    atomic_init (&v->number[1], 0UL);                                   \
  if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {              \
    M_C(m_mutex, M_NAMING_INIT)(v->mutexPush);                          \
    M_C(m_mutex, M_NAMING_INIT)(v->mutexPop);                           \
    M_C(m_cond, M_NAMING_INIT)(v->there_is_data);                       \
    M_C(m_cond, M_NAMING_INIT)(v->there_is_room_for_data);              \
  } else                                                                \
    assert(BUFFERI_POLICY_P((policy), BUFFER_UNBLOCKING));              \
  BUFFERI_IF_CTE_SIZE(m_size)( ,                                        \
    v->data = M_CALL_REALLOC(oplist, type, NULL, BUFFERI_SIZE(m_size)); \
    if (v->data == NULL) {                                              \
      M_MEMORY_FULL (BUFFERI_SIZE(m_size)*sizeof(type));                \
      return;                                                           \
    }                                                                   \
  )                                                                     \
  if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {         \
    for(size_t i = 0; i < size; i++) {					\
      M_CALL_INIT(oplist, v->data[i]);                                  \
    }									\
  }                                                                     \
  BUFFERI_CONTRACT(v,m_size);						\
}									\
                                                                        \
 BUFFERI_IF_CTE_SIZE(m_size)(                                           \
 static inline void                                                     \
 M_C3(name, _int, M_NAMING_INIT)(buffer_t v)                            \
 {                                                                      \
   M_C(name, M_NAMING_INIT)(v, m_size);                                 \
 }                                                                      \
 , )                                                                    \
                                                                        \
 static inline void                                                     \
 M_C(name, _int_clear_obj)(buffer_t v)					\
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {        \
     for(size_t i = 0; i < BUFFERI_SIZE(m_size); i++) {			\
       M_CALL_CLEAR(oplist, v->data[i]);                                \
     }									\
   } else {                                                             \
     size_t i = BUFFERI_POLICY_P((policy), BUFFER_STACK) ? 0 : v->idx_cons; \
     while (i != v->idx_prod) {                                         \
       M_CALL_CLEAR(oplist, v->data[i]);                                \
       i++;                                                             \
       if (!BUFFERI_POLICY_P((policy), BUFFER_STACK) && i >= BUFFERI_SIZE(m_size)) \
         i = 0;                                                         \
     }                                                                  \
   }                                                                    \
   v->idx_prod = v->idx_cons = 0;                                       \
   atomic_store_explicit (&v->number[0], 0UL, memory_order_relaxed);	\
   if (BUFFERI_POLICY_P(policy, BUFFER_DEFERRED_POP))                   \
     atomic_store_explicit(&v->number[1], 0UL, memory_order_relaxed);	\
   BUFFERI_CONTRACT(v,m_size);						\
 }                                                                      \
 									\
 static inline void                                                     \
 M_C(name, M_NAMING_CLEAR)(buffer_t v)						\
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   M_C(name, _int_clear_obj)(v);					\
   BUFFERI_IF_CTE_SIZE(m_size)( ,                                       \
     M_CALL_FREE(oplist, v->data);                                      \
     v->data = NULL;                                                    \
   )                                                                    \
   v->overwrite = 0;                                                    \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     M_C(m_mutex, M_NAMING_CLEAR)(v->mutexPush);                                       \
     M_C(m_mutex, M_NAMING_CLEAR)(v->mutexPop);                                        \
     M_C(m_cond, M_NAMING_CLEAR)(v->there_is_data);                                    \
     M_C(m_cond, M_NAMING_CLEAR)(v->there_is_room_for_data);                           \
   }                                                                    \
 }                                                                      \
 									\
 static inline void                                                     \
 M_C(name, M_NAMING_CLEAN)(buffer_t v)						\
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_lock(v->mutexPush);                                        \
     m_mutex_lock(v->mutexPop);                                         \
   }                                                                    \
   BUFFERI_PROTECTED_CONTRACT(v, m_size);				\
   if (BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE))		\
     M_C(name, _int_clear_obj)(v);					\
   v->idx_prod = v->idx_cons = 0;                                       \
   atomic_store_explicit (&v->number[0], 0UL, memory_order_relaxed);	\
   if (BUFFERI_POLICY_P(policy, BUFFER_DEFERRED_POP))                   \
     atomic_store_explicit(&v->number[1], 0UL, memory_order_relaxed);	\
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_cond_broadcast(v->there_is_room_for_data);                       \
     m_mutex_unlock(v->mutexPop);                                       \
     m_mutex_unlock(v->mutexPush);                                      \
   }                                                                    \
   BUFFERI_CONTRACT(v,m_size);						\
 }                                                                      \
 									\
 static inline void                                                     \
 M_C(name, M_NAMING_INIT_SET)(buffer_t dest, const buffer_t src)                \
 {                                                                      \
   /* unconst 'src', so that we can lock it (semantically it is const) */ \
   M_C(name, _uptr) vu;                                                 \
   vu.cptr = src;                                                       \
   M_C(name, _ptr) v = vu.ptr;                                          \
   assert (dest != v);                                                  \
   BUFFERI_CONTRACT(v,m_size);                                          \
   M_C(name, M_NAMING_INIT)(dest, BUFFERI_SIZE(m_size));                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_lock(v->mutexPush);                                        \
     m_mutex_lock(v->mutexPop);                                         \
   }                                                                    \
                                                                        \
   BUFFERI_PROTECTED_CONTRACT(v, m_size);				\
   if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {        \
     for(size_t i = 0; i < BUFFERI_SIZE(m_size); i++) {			\
       M_CALL_INIT_SET(oplist, dest->data[i], v->data[i]);              \
     }									\
   } else {                                                             \
     size_t i = BUFFERI_POLICY_P((policy), BUFFER_STACK) ? 0 : v->idx_cons; \
     while (i != v->idx_prod) {                                         \
       M_CALL_INIT_SET(oplist, dest->data[i], v->data[i]);              \
       i++;                                                             \
       if (!BUFFERI_POLICY_P((policy), BUFFER_STACK) && i >= BUFFERI_SIZE(m_size)) \
         i = 0;                                                         \
     }                                                                  \
   }                                                                    \
                                                                        \
   dest->idx_prod = v->idx_prod;                                        \
   dest->idx_cons = v->idx_cons;                                        \
   atomic_store_explicit (&dest->number[0], atomic_load(&v->number[0]), memory_order_relaxed); \
   if (BUFFERI_POLICY_P(policy, BUFFER_DEFERRED_POP))                   \
     atomic_store_explicit(&dest->number[1], atomic_load(&v->number[1]), memory_order_relaxed); \
                                                                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_unlock(v->mutexPop);                                       \
     m_mutex_unlock(v->mutexPush);                                      \
   }                                                                    \
   BUFFERI_CONTRACT(v,m_size);                                          \
   BUFFERI_CONTRACT(dest, m_size);                                      \
 }                                                                      \
 									\
 static inline void                                                     \
 M_C(name, _set)(buffer_t dest, const buffer_t src)                     \
 {                                                                      \
   /* unconst 'src', so that we can lock it (semantically it is const) */ \
   M_C(name, _uptr) vu;                                                 \
   vu.cptr = src;                                                       \
   M_C(name, _ptr) v = vu.ptr;                                          \
   BUFFERI_CONTRACT(dest,m_size);                                       \
   BUFFERI_CONTRACT(v,m_size);                                          \
                                                                        \
   if (dest == v) return;                                               \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     /* Case of deadlock: A := B, B:=C, C:=A (all in //)                \
        Solution: order the lock by increasing memory */                \
     if (dest < v) {                                                    \
       m_mutex_lock(dest->mutexPush);                                   \
       m_mutex_lock(dest->mutexPop);                                    \
       m_mutex_lock(v->mutexPush);                                      \
       m_mutex_lock(v->mutexPop);                                       \
     } else {                                                           \
       m_mutex_lock(v->mutexPush);                                      \
       m_mutex_lock(v->mutexPop);                                       \
       m_mutex_lock(dest->mutexPush);                                   \
       m_mutex_lock(dest->mutexPop);                                    \
     }                                                                  \
   }                                                                    \
                                                                        \
   BUFFERI_PROTECTED_CONTRACT(v, m_size);				\
   M_C(name, _int_clear_obj)(dest);					\
                                                                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {        \
     for(size_t i = 0; i < BUFFERI_SIZE(m_size); i++) {			\
       M_CALL_INIT_SET(oplist, dest->data[i], v->data[i]);              \
     }									\
   } else {                                                             \
     size_t i = BUFFERI_POLICY_P((policy), BUFFER_STACK) ? 0 : v->idx_cons; \
     while (i != v->idx_prod) {                                         \
       M_CALL_INIT_SET(oplist, dest->data[i], v->data[i]);              \
       i++;                                                             \
       if (!BUFFERI_POLICY_P((policy), BUFFER_STACK) && i >= BUFFERI_SIZE(m_size)) \
         i = 0;                                                         \
     }                                                                  \
   }                                                                    \
                                                                        \
   dest->idx_prod = v->idx_prod;                                        \
   dest->idx_cons = v->idx_cons;                                        \
   atomic_store_explicit (&dest->number[0], atomic_load(&v->number[0]), memory_order_relaxed); \
   if (BUFFERI_POLICY_P(policy, BUFFER_DEFERRED_POP))                   \
     atomic_store_explicit(&dest->number[1], atomic_load(&v->number[1]), memory_order_relaxed); \
                                                                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     /* It may be false, but it is not wrong! */                        \
     m_cond_broadcast(v->there_is_room_for_data);                       \
     m_cond_broadcast(v->there_is_data);                                \
     if (dest < v) {                                                    \
       m_mutex_unlock(v->mutexPop);                                     \
       m_mutex_unlock(v->mutexPush);                                    \
       m_mutex_unlock(dest->mutexPop);                                  \
       m_mutex_unlock(dest->mutexPush);                                 \
     } else {                                                           \
       m_mutex_unlock(dest->mutexPop);                                  \
       m_mutex_unlock(dest->mutexPush);                                 \
       m_mutex_unlock(v->mutexPop);                                     \
       m_mutex_unlock(v->mutexPush);                                    \
     }                                                                  \
   }                                                                    \
   BUFFERI_CONTRACT(v,m_size);                                          \
   BUFFERI_CONTRACT(dest, m_size);                                      \
 }                                                                      \
 									\
 static inline bool                                                     \
 M_C(name, M_NAMING_TEST_EMPTY)(buffer_t v)					\
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   /* If the buffer has been configured with deferred pop               \
      we considered the queue as empty when the number of               \
      deferred pop has reached 0, not the number of items in the        \
      buffer is 0. */                                                   \
   if (BUFFERI_POLICY_P(policy, BUFFER_DEFERRED_POP))                   \
     return atomic_load_explicit (&v->number[1], memory_order_relaxed) == 0; \
   else                                                                 \
     return atomic_load_explicit (&v->number[0], memory_order_relaxed) == 0; \
 }                                                                      \
 									\
 static inline bool                                                     \
 M_C(name, _full_p)(buffer_t v)                                         \
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   return atomic_load_explicit (&v->number[0], memory_order_relaxed)	\
     == BUFFERI_SIZE(m_size);						\
 }                                                                      \
 									\
 static inline size_t							\
 M_C(name, M_NAMING_SIZE)(buffer_t v)                                           \
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   return atomic_load_explicit (&v->number[0], memory_order_relaxed);	\
 }                                                                      \
 									\
 static inline bool                                                     \
 M_C(name, _push_blocking)(buffer_t v, type const data, bool blocking)  \
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   									\
   /* BUFFER lock */							\
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_lock(v->mutexPush);                                        \
     while (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_OVERWRITE)          \
            && M_C(name, _full_p)(v)) {					\
       if (!blocking) {                                                 \
         m_mutex_unlock(v->mutexPush);                                  \
         return false;                                                  \
       }                                                                \
       m_cond_wait(v->there_is_room_for_data, v->mutexPush);            \
     }                                                                  \
   } else if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_OVERWRITE)        \
              && M_C(name, _full_p)(v))					\
     return false;                                                      \
   BUFFERI_PROTECTED_CONTRACT(v, m_size);				\
   									\
   size_t previousSize, idx = v->idx_prod;                              \
   /* INDEX computation if we have to overwrite the last element */	\
   if (M_UNLIKELY (BUFFERI_POLICY_P((policy), BUFFER_PUSH_OVERWRITE)	\
		   && M_C(name, _full_p)(v))) {                         \
     v->overwrite++;							\
     /* Let's overwrite the last element */                             \
     /* Compute the index of the last push element */                   \
     idx--;                                                             \
     if (!BUFFERI_POLICY_P((policy), BUFFER_STACK)) {                   \
       idx = idx >= BUFFERI_SIZE(m_size) ? BUFFERI_SIZE(m_size)-1 : idx; \
     }                                                                  \
     /* Update data in the buffer */                                    \
     M_CALL_SET(oplist, v->data[idx], data);                           \
     previousSize = BUFFERI_SIZE(m_size);                               \
                                                                        \
   } else {                                                             \
                                                                        \
     /* Add a new item in the buffer */                                 \
     /* PUSH data in the buffer */                                      \
     if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {      \
       M_CALL_SET(oplist, v->data[idx], data);                          \
     } else {                                                           \
       M_CALL_INIT_SET(oplist, v->data[idx], data);                     \
     }                                                                  \
                                                                        \
     /* Increment production INDEX of the buffer */                     \
     idx++;                                                             \
     if (!BUFFERI_POLICY_P((policy), BUFFER_STACK)) {                   \
       idx = (idx == BUFFERI_SIZE(m_size)) ? 0 : idx;                   \
     }                                                                  \
     v->idx_prod = idx;                                                 \
                                                                        \
     /* number[] is the only variable which can be modified by both     \
        the consummer thread which has the pop lock and the producer    \
        thread which has the push lock. As such, it has to be handled   \
        like an atomic variable. */                                     \
     /* Increment number of elements of the buffer */                   \
     previousSize = atomic_fetch_add (&v->number[0], 1UL);		\
     if (BUFFERI_POLICY_P((policy), BUFFER_DEFERRED_POP)) {             \
       previousSize = atomic_fetch_add (&v->number[1], 1UL);		\
     }                                                                  \
   }                                                                    \
                                                                        \
  /* BUFFER unlock */							\
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_unlock(v->mutexPush);                                      \
     /* If the number of items in the buffer was 0, some consummer      \
        may be waiting. Signal to them the availibility of the data     \
        We cannot only signal one thread. */                            \
     if (previousSize == 0) {                                           \
       m_mutex_lock(v->mutexPop);                                       \
       m_cond_broadcast(v->there_is_data);                              \
       m_mutex_unlock(v->mutexPop);                                     \
     }                                                                  \
   }                                                                    \
									\
   BUFFERI_CONTRACT(v,m_size);						\
   return true;                                                         \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C(name, _pop_blocking)(type *data, buffer_t v, bool blocking)        \
 {                                                                      \
   BUFFERI_CONTRACT(v,m_size);						\
   assert (data != NULL);						\
									\
   /* BUFFER lock */							\
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_lock(v->mutexPop);                                         \
     while (M_C(name, M_NAMING_TEST_EMPTY)(v)) {					\
       if (!blocking) {                                                 \
         m_mutex_unlock(v->mutexPop);                                   \
         return false;                                                  \
       }                                                                \
       m_cond_wait(v->there_is_data, v->mutexPop);                      \
     }                                                                  \
   } else if (M_C(name, M_NAMING_TEST_EMPTY)(v))					\
     return false;                                                      \
   BUFFERI_PROTECTED_CONTRACT(v, m_size);				\
   									\
   /* POP data from the buffer and update INDEX */                      \
   if (!BUFFERI_POLICY_P((policy), BUFFER_STACK)) {                     \
     /* FIFO queue */                                                   \
     if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {	\
       M_CALL_SET(oplist, *data, v->data[v->idx_cons]);                 \
     } else {                                                           \
       M_DO_INIT_MOVE (oplist, *data, v->data[v->idx_cons]);            \
     }                                                                  \
     v->idx_cons = (v->idx_cons == BUFFERI_SIZE(m_size)-1) ? 0 : (v->idx_cons + 1); \
   } else {                                                             \
     /* STACK queue */                                                  \
     v->idx_prod --;                                                    \
     if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {      \
       M_CALL_SET(oplist, *data, v->data[v->idx_prod]);                 \
     } else {                                                           \
       M_DO_INIT_MOVE (oplist, *data, v->data[v->idx_prod]);            \
     }                                                                  \
   }                                                                    \
                                                                        \
   /* number[] is the only variable which can be modified by both       \
      the consummer thread which has the pop lock and the producer      \
      thread which has the push lock. As such, it has to be handled     \
      like an atomic variable. */                                       \
   /* Decrement number of elements in the buffer */                     \
   size_t previousSize;                                                 \
   if (!BUFFERI_POLICY_P((policy), BUFFER_DEFERRED_POP)) {              \
     previousSize = atomic_fetch_sub (&v->number[0], 1UL);		\
   } else {                                                             \
     atomic_fetch_sub (&v->number[1], 1UL);				\
   }                                                                    \
                                                                        \
   /* BUFFER unlock */							\
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_unlock(v->mutexPop);                                       \
     /* If the number of items in the buffer was the max, some producer \
        may be waiting. Signal to them the availibility of the free room \
        We cannot only signal one thread. */                            \
     if ((!BUFFERI_POLICY_P((policy), BUFFER_DEFERRED_POP))             \
         && previousSize == BUFFERI_SIZE(m_size)) {                     \
       m_mutex_lock(v->mutexPush);                                      \
       m_cond_broadcast(v->there_is_room_for_data);                     \
       m_mutex_unlock(v->mutexPush);                                    \
     }                                                                  \
   }                                                                    \
									\
   BUFFERI_CONTRACT(v,m_size);						\
   return true;                                                         \
 }                                                                      \
									\
 									\
 static inline bool                                                     \
 M_C(name, _push)(buffer_t v, type const data)				\
 {                                                                      \
   return M_C(name, _push_blocking)(v, data,                            \
                             !BUFFERI_POLICY_P((policy), BUFFER_UNBLOCKING_PUSH)); \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C(name, _pop)(type *data, buffer_t v)				\
 {                                                                      \
   return M_C(name, _pop_blocking)(data, v,                             \
                            !BUFFERI_POLICY_P((policy), BUFFER_UNBLOCKING_POP)); \
 }                                                                      \
                                                                        \
 static inline size_t							\
 M_C(name, _overwrite)(const buffer_t v)				\
 {                                                                      \
   return v->overwrite;							\
 }									\
                                                                        \
 static inline size_t                                                   \
 M_C(name, _capacity)(const buffer_t v)                                 \
 {                                                                      \
   (void) v; /* may be unused */                                        \
   return BUFFERI_SIZE(m_size);                                         \
 }                                                                      \
                                                                        \
 static inline void                                                     \
 M_C(name, _pop_release)(buffer_t v)                                    \
 {                                                                      \
   /* Decrement the effective number of elements in the buffer */       \
   if (BUFFERI_POLICY_P((policy), BUFFER_DEFERRED_POP)) {               \
     size_t previousSize = atomic_fetch_sub (&v->number[0], 1UL);	\
     if (previousSize == BUFFERI_SIZE(m_size)) {                        \
       m_mutex_lock(v->mutexPush);                                      \
       m_cond_broadcast(v->there_is_room_for_data);                     \
       m_mutex_unlock(v->mutexPush);                                    \
     }                                                                  \
   }                                                                    \
 }                                                                      \



/* Definition of a a QUEUE for Many Produccer / Many Consummer
   for high bandwidth scenario:
   * nearly lock-free,
   * quite fast
   * no blocking calls.
   * only queue (no stack)
   * size of queue is always a power of 2
   * no overwriting.
   */

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define QUEUEI_MPMC_DEF_P1(arg) QUEUEI_MPMC_DEF_P2 arg

/* Validate the value oplist before going further */
#define QUEUEI_MPMC_DEF_P2(name, type, policy, oplist, buffer_t)    \
  M_IF_OPLIST(oplist)(QUEUEI_MPMC_DEF_P3, QUEUEI_MPMC_DEF_FAILURE)(name, type, policy, oplist, buffer_t)

/* Stop processing with a compilation failure */
#define QUEUEI_MPMC_DEF_FAILURE(name, type, policy, oplist, buffer_t) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(QUEUE_MPMC_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

#ifdef NDEBUG
# define QUEUEI_MPMC_CONTRACT(v) /* nothing */
#else
# define QUEUEI_MPMC_CONTRACT(v) do {                                   \
    assert (v != 0);                                                    \
    assert (v->Tab != NULL);                                            \
    unsigned int _r = atomic_load(&v->ConsoIdx);                        \
    unsigned int _w = atomic_load(&v->ProdIdx);                         \
    _r = atomic_load(&v->ConsoIdx);                                     \
    assert (_r > _w || _w-_r <= v->size);                               \
    assert (M_POWEROF2_P(v->size));                                     \
  } while (0)
#endif

#define QUEUEI_MPMC_DEF_P3(name, type, policy, oplist, buffer_t)        \
									\
  /* The sequence number of an element will be equal to either		\
     - 2* the index of the production which creates it,			\
     - 1 + 2* the index of the consumption which consummes it		\
     In case of wrapping, as there is no order comparison but only      \
     equal comparison, there is no special issue.                       \
     Each element is put in a separate cache line to avoid false        \
     sharing.                                                           \
  */									\
  typedef struct M_C(name, _el_s) {					\
    atomic_uint  seq;	/* Can only increase until wrapping */          \
    type         x;							\
    M_CACHELINE_ALIGN(align, atomic_uint, type);                        \
  } M_C(name, _el_t);							\
									\
  /* If there is only one producer and one consummer, then they won't   \
     typically use the same cache line, increasing performance. */      \
  typedef struct M_C(name, _s) {					\
    atomic_uint ProdIdx; /* Can only increase until wrapping */         \
    M_CACHELINE_ALIGN(align1, atomic_uint);                             \
    atomic_uint ConsoIdx; /* Can only increase until wrapping */        \
    M_CACHELINE_ALIGN(align2, atomic_uint);                             \
    M_C(name, _el_t) *Tab;                                              \
    unsigned int size;							\
  } buffer_t[1];							\
									\
  static inline bool              					\
  M_C(name, _push)(buffer_t table, type const x)                        \
  {									\
    QUEUEI_MPMC_CONTRACT(table);                                        \
    unsigned int idx = atomic_load_explicit(&table->ProdIdx,            \
                                            memory_order_relaxed);      \
    const unsigned int i = idx & (table->size -1);			\
    const unsigned int seq = atomic_load_explicit(&table->Tab[i].seq,   \
                                                  memory_order_acquire); \
    if (M_UNLIKELY (2*(idx - table->size) + 1 != seq))	{		\
      /* Buffer full (or unlikely preemption). Can not push */          \
      return false;							\
    }									\
    if (M_UNLIKELY (!atomic_compare_exchange_strong_explicit(&table->ProdIdx, \
	   &idx, idx+1, memory_order_relaxed, memory_order_relaxed))) { \
      /* Thread has been preempted by another one. */			\
      return false;							\
    }									\
    /* If it is interrupted here, it may block pop method (not push) */ \
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      M_CALL_SET(oplist, table->Tab[i].x, x);				\
    } else {                                                            \
      M_CALL_INIT_SET(oplist, table->Tab[i].x, x);                      \
    }                                                                   \
    /* Finish transaction */                                            \
    atomic_store_explicit(&table->Tab[i].seq, 2*idx, memory_order_release); \
    QUEUEI_MPMC_CONTRACT(table);                                        \
    return true;                                                        \
  }									\
									\
  static inline bool							\
  M_C(name, _pop)(type *ptr, buffer_t table)				\
  {									\
    QUEUEI_MPMC_CONTRACT(table);                                        \
    assert (ptr != NULL);                                               \
    unsigned int iC = atomic_load_explicit(&table->ConsoIdx,            \
                                           memory_order_relaxed);	\
    const unsigned int i = (iC & (table->size -1));			\
    const unsigned int seq = atomic_load_explicit(&table->Tab[i].seq,   \
                                                  memory_order_acquire); \
    if (seq != 2 * iC) {						\
      /* Nothing in buffer to consumme (or unlikely preemption) */      \
      return false;							\
    }									\
    if (M_UNLIKELY (!atomic_compare_exchange_strong_explicit(&table->ConsoIdx, \
	     &iC, iC+1, memory_order_relaxed, memory_order_relaxed))) { \
      /* Thread has been preempted by another one */			\
      return false;							\
    }									\
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      M_CALL_SET(oplist, *ptr, table->Tab[i].x);                        \
    } else {                                                            \
      M_DO_INIT_MOVE (oplist, *ptr, table->Tab[i].x);                   \
    }                                                                   \
    atomic_store_explicit(&table->Tab[i].seq, 2*iC + 1, memory_order_release); \
    QUEUEI_MPMC_CONTRACT(table);                                        \
    return true;                                                        \
  }									\
									\
  static inline void							\
  M_C(name, M_NAMING_INIT)(buffer_t buffer, size_t size)			\
  {									\
    assert (buffer != NULL);						\
    assert( M_POWEROF2_P(size));					\
    assert (0 < size && size <= UINT_MAX);                              \
    assert(((policy) & (BUFFER_STACK|BUFFER_THREAD_UNSAFE|BUFFER_PUSH_OVERWRITE)) == 0); \
    atomic_init(&buffer->ProdIdx, (unsigned int) size);                 \
    atomic_init(&buffer->ConsoIdx, (unsigned int) size);                \
    buffer->size = (unsigned int) size;                                 \
    buffer->Tab = M_CALL_REALLOC(oplist, M_C(name, _el_t), NULL, size); \
    if (buffer->Tab == NULL) {						\
      M_MEMORY_FULL (size*sizeof(M_C(name, _el_t) ));			\
      return;								\
    }									\
    for(unsigned int j = 0; j < (unsigned int) size; j++) {             \
      atomic_init(&buffer->Tab[j].seq, 2*j+1U);                         \
      if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {     \
        M_CALL_INIT(oplist, buffer->Tab[j].x);				\
      }                                                                 \
    }									\
    QUEUEI_MPMC_CONTRACT(buffer);                                       \
  }									\
									\
  static inline void							\
  M_C(name, M_NAMING_CLEAR)(buffer_t buffer)					\
  {									\
    QUEUEI_MPMC_CONTRACT(buffer);                                       \
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      for(unsigned int j = 0; j < buffer->size; j++) {                  \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                         \
      }									\
    } else {                                                            \
      unsigned int iP = atomic_load_explicit(&buffer->ProdIdx, memory_order_relaxed); \
      unsigned int i  = iP & (buffer->size -1);                         \
      unsigned int iC = atomic_load_explicit(&buffer->ConsoIdx, memory_order_relaxed); \
      unsigned int j  = iC & (buffer->size -1);                         \
      while (i != j) {                                                  \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                         \
        j++;                                                            \
        if (j >= buffer->size)                                          \
          j = 0;                                                        \
      }                                                                 \
    }                                                                   \
    M_CALL_FREE(oplist, buffer->Tab);					\
    buffer->Tab = NULL; /* safer */                                     \
  }									\
									\
  static inline size_t							\
  M_C(name, M_NAMING_SIZE)(buffer_t table)                                      \
  {									\
    QUEUEI_MPMC_CONTRACT(table);                                        \
    const unsigned int iC = atomic_load_explicit(&table->ConsoIdx, memory_order_relaxed); \
    const unsigned int iP = atomic_load_explicit(&table->ProdIdx, memory_order_acquire); \
    /* We return an approximation as we can't read both iC & iP atomically \
       As we read producer index after consummer index,                 \
       and they are atomic variables without reordering			\
       producer index is always greater or equal than consumer index    \
       (or on overflow occurs, in which case as we compute with modulo  \
       arithmetic, the right result is computed).                       \
       We may return a result which is greater than the size of the queue \
       if the function is interrupted a long time between reading iC &  \
       iP. the function is not protected against it.                    \
    */                                                                  \
    return iP-iC;							\
  }									\
                                                                        \
  static inline size_t                                                  \
  M_C(name, _capacity)(buffer_t v)					\
  {                                                                     \
    QUEUEI_MPMC_CONTRACT(v);                                            \
    return v->size;                                                     \
  }                                                                     \
                                                                        \
  static inline bool							\
  M_C(name, M_NAMING_TEST_EMPTY)(buffer_t v)					\
  {									\
    return M_C(name, M_NAMING_SIZE) (v) == 0;					\
  }									\
  									\
  static inline bool							\
  M_C(name, _full_p)(buffer_t v)					\
  {									\
    return M_C(name, M_NAMING_SIZE)(v) >= v->size;                              \
  }									\
  


/* Definition of a a QUEUE for Single Producer / Single Consummer
   for high bandwidth scenario:
   * wait-free,
   * quite fast
   * no blocking calls.
   * only queue (no stack)
   * size of queue is always a power of 2
   * no overwriting.
   */

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define QUEUEI_SPSC_DEF_P1(arg) QUEUEI_SPSC_DEF_P2 arg

/* Validate the value oplist before going further */
#define QUEUEI_SPSC_DEF_P2(name, type, policy, oplist, buffer_t)    \
  M_IF_OPLIST(oplist)(QUEUEI_SPSC_DEF_P3, QUEUEI_SPSC_DEF_FAILURE)(name, type, policy, oplist, buffer_t)

/* Stop processing with a compilation failure */
#define QUEUEI_SPSC_DEF_FAILURE(name, type, policy, oplist, buffer_t) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(QUEUE_SPSC_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

#ifdef NDEBUG
#define QUEUEI_SPSC_CONTRACT(table) do { } while (0)
#else
#define QUEUEI_SPSC_CONTRACT(table) do {                                \
    assert (table != NULL);                                             \
    unsigned int _r = atomic_load(&table->consoIdx);                    \
    unsigned int _w = atomic_load(&table->prodIdx);                     \
    /* Due to overflow we don't have assert (_r <= _w); */              \
    _r = atomic_load(&table->consoIdx);                                 \
    assert (_r > _w || _w-_r <= table->size);                           \
    assert (M_POWEROF2_P(table->size));                                 \
  } while (0)
#endif

#define QUEUEI_SPSC_DEF_P3(name, type, policy, oplist, buffer_t)        \
                                                                        \
  typedef struct M_C(name, _el_s) {					\
    type         x;							\
  } M_C(name, _el_t);							\
									\
  typedef struct M_C(name, _s) {                                        \
    atomic_uint  consoIdx; /* Can only increase until overflow */       \
    unsigned int size;                                                  \
    M_C(name, _el_t) *Tab;                                              \
    M_CACHELINE_ALIGN(align, atomic_uint, size_t, M_C(name, _el_t) *);  \
    atomic_uint prodIdx;  /* Can only increase until overflow */        \
  } buffer_t[1];                                                        \
                                                                        \
  static inline bool              					\
  M_C(name, _push)(buffer_t table, type const x)                        \
  {									\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    unsigned int r = atomic_load_explicit(&table->consoIdx,             \
                                          memory_order_relaxed);        \
    unsigned int w = atomic_load_explicit(&table->prodIdx,              \
                                          memory_order_acquire);        \
    if (w-r >= table->size)                                             \
      return false;                                                     \
    unsigned int i = w & (table->size -1);                              \
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      M_CALL_SET(oplist, table->Tab[i].x, x);				\
    } else {                                                            \
      M_CALL_INIT_SET(oplist, table->Tab[i].x, x);                      \
    }                                                                   \
    atomic_store_explicit(&table->prodIdx, w+1, memory_order_release);	\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    return true;                                                        \
  }                                                                     \
                                                                        \
  static inline bool              					\
  M_C(name, _push_move)(buffer_t table, type x)                         \
  {									\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    unsigned int r = atomic_load_explicit(&table->consoIdx,             \
                                          memory_order_relaxed);        \
    unsigned int w = atomic_load_explicit(&table->prodIdx,              \
                                          memory_order_acquire);        \
    if (w-r >= table->size)                                             \
      return false;                                                     \
    unsigned int i = w & (table->size -1);                              \
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      M_DO_MOVE(oplist, table->Tab[i].x, x);				\
    } else {                                                            \
      M_DO_INIT_MOVE(oplist, table->Tab[i].x, x);                       \
    }                                                                   \
    atomic_store_explicit(&table->prodIdx, w+1, memory_order_release);	\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    return true;                                                        \
  }                                                                     \
                                                                        \
  static inline bool              					\
  M_C(name, _pop)(type *ptr, buffer_t table)                            \
  {									\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    assert (ptr != NULL);                                               \
    unsigned int w = atomic_load_explicit(&table->prodIdx,              \
                                          memory_order_relaxed);	\
    unsigned int r = atomic_load_explicit(&table->consoIdx,             \
                                          memory_order_acquire);	\
    if (w-r == 0)                                                       \
      return false;                                                     \
    unsigned int i = r & (table->size -1);                              \
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      M_CALL_SET(oplist, *ptr , table->Tab[i].x);                       \
    } else {                                                            \
      M_DO_INIT_MOVE (oplist, *ptr, table->Tab[i].x);                   \
    }                                                                   \
    atomic_store_explicit(&table->consoIdx, r+1, memory_order_release);	\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    return true;                                                        \
  }                                                                     \
                                                                        \
  static inline unsigned              					\
  M_C(name, _push_bulk)(buffer_t table, unsigned n, type const x[])     \
  {									\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    assert (x != NULL);                                                 \
    assert (n <= table->size);                                          \
    unsigned int r = atomic_load_explicit(&table->consoIdx,             \
                                          memory_order_relaxed);        \
    unsigned int w = atomic_load_explicit(&table->prodIdx,              \
                                          memory_order_acquire);        \
    unsigned int max = M_MIN(n, table->size - (w-r) );                  \
    if (max == 0)                                                       \
      return 0;                                                         \
    for(unsigned int k = 0; k < max; k++) {                             \
      unsigned int i = (w+k) & (table->size -1);                        \
      if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {     \
        M_CALL_SET(oplist, table->Tab[i].x, x[k]);                      \
      } else {                                                          \
        M_CALL_INIT_SET(oplist, table->Tab[i].x, x[k]);                 \
      }                                                                 \
    }                                                                   \
    atomic_store_explicit(&table->prodIdx, w+max, memory_order_release); \
    QUEUEI_SPSC_CONTRACT(table);                                        \
    return max;                                                         \
  }                                                                     \
                                                                        \
  static inline unsigned              					\
  M_C(name, _pop_bulk)(unsigned int n, type ptr[], buffer_t table)      \
  {									\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    assert (ptr != NULL);                                               \
    assert (n <= table->size);                                          \
    unsigned int w = atomic_load_explicit(&table->prodIdx,              \
                                          memory_order_relaxed);	\
    unsigned int r = atomic_load_explicit(&table->consoIdx,             \
                                          memory_order_acquire);	\
    if (w-r == 0)                                                       \
      return 0;                                                         \
    unsigned int max = M_MIN(w-r, n);                                   \
    for(unsigned int k = 0; k < max; k++) {                             \
      unsigned int i = (r+k) & (table->size -1);                        \
      if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {     \
        M_CALL_SET(oplist, ptr[k], table->Tab[i].x);                    \
      } else {                                                          \
        M_DO_INIT_MOVE (oplist, ptr[k], table->Tab[i].x);               \
      }                                                                 \
    }                                                                   \
    atomic_store_explicit(&table->consoIdx, r+max, memory_order_release); \
    QUEUEI_SPSC_CONTRACT(table);                                        \
    return max;                                                         \
  }                                                                     \
                                                                        \
  static inline void              					\
  M_C(name, _push_force)(buffer_t table, type const x)                  \
  {									\
    QUEUEI_SPSC_CONTRACT(table);                                        \
    unsigned int r = atomic_load_explicit(&table->consoIdx,             \
                                          memory_order_relaxed);        \
    unsigned int w = atomic_load_explicit(&table->prodIdx,              \
                                          memory_order_acquire);        \
    /* If no place in queue, try to skip the last one */                \
    while (w-r >= table->size) {                                        \
      bool b = atomic_compare_exchange_strong(&table->consoIdx, &r, r+1); \
      r += b;                                                           \
    }                                                                   \
    unsigned int i = w & (table->size -1);                              \
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      M_CALL_SET(oplist, table->Tab[i].x, x);				\
    } else {                                                            \
      M_CALL_INIT_SET(oplist, table->Tab[i].x, x);                      \
    }                                                                   \
    atomic_store_explicit(&table->prodIdx, w+1, memory_order_release);	\
    QUEUEI_SPSC_CONTRACT(table);                                        \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, M_NAMING_SIZE)(buffer_t table)                                      \
  {                                                                     \
    QUEUEI_SPSC_CONTRACT(table);                                        \
    unsigned int r = atomic_load_explicit(&table->consoIdx,             \
                                          memory_order_relaxed);	\
    unsigned int w = atomic_load_explicit(&table->prodIdx,              \
                                          memory_order_acquire);	\
    /* We return an approximation as we can't read both r & w atomically \
       As we read producer index after consummer index,                 \
       and they are atomic variables without reordering			\
       producer index is always greater or equal than consumer index    \
       (or on overflow occurs, in which case as we compute with modulo  \
       arithmetic, the right result is computed).                       \
       We may return a result which is greater than the size of the queue \
       if the function is interrupted a long time between reading the   \
       indexs. The function is not protected against it.                \
    */                                                                  \
    return w-r;                                                         \
  }                                                                     \
                                                                        \
 static inline size_t                                                   \
 M_C(name, _capacity)(buffer_t v)					\
 {                                                                      \
   return v->size;                                                      \
 }                                                                      \
                                                                        \
  static inline bool							\
  M_C(name, M_NAMING_TEST_EMPTY)(buffer_t v)					\
  {									\
    return M_C(name, M_NAMING_SIZE) (v) == 0;					\
  }									\
  									\
  static inline bool							\
  M_C(name, _full_p)(buffer_t v)					\
  {									\
    return M_C(name, M_NAMING_SIZE)(v) >= v->size;                              \
  }									\
                                                                        \
  static inline void							\
  M_C(name, M_NAMING_INIT)(buffer_t buffer, size_t size)			\
  {									\
    assert (buffer != NULL);						\
    assert( M_POWEROF2_P(size));					\
    assert (0 < size && size <= UINT_MAX);                              \
    assert(((policy) & (BUFFER_STACK|BUFFER_THREAD_UNSAFE|BUFFER_PUSH_OVERWRITE)) == 0); \
    atomic_init(&buffer->prodIdx, (unsigned int) size);                 \
    atomic_init(&buffer->consoIdx, (unsigned int) size);                \
    buffer->size = (unsigned int) size;                                 \
    buffer->Tab = M_CALL_REALLOC(oplist, M_C(name, _el_t), NULL, size); \
    if (buffer->Tab == NULL) {						\
      M_MEMORY_FULL (size*sizeof(M_C(name, _el_t) ));			\
      return;								\
    }									\
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      for(unsigned int j = 0; j < (unsigned int) size; j++) {           \
        M_CALL_INIT(oplist, buffer->Tab[j].x);				\
      }                                                                 \
    }									\
    QUEUEI_SPSC_CONTRACT(buffer);                                       \
  }									\
									\
  static inline void							\
  M_C(name, M_NAMING_CLEAR)(buffer_t buffer)					\
  {									\
    QUEUEI_SPSC_CONTRACT(buffer);                                       \
    if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
      for(unsigned int j = 0; j < buffer->size; j++) {                  \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                         \
      }									\
    } else {                                                            \
      unsigned int iP = atomic_load_explicit(&buffer->prodIdx, memory_order_relaxed); \
      unsigned int i  = iP & (buffer->size -1);                         \
      unsigned int iC = atomic_load_explicit(&buffer->consoIdx, memory_order_relaxed); \
      unsigned int j  = iC & (buffer->size -1);                         \
      while (i != j) {                                                  \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                         \
        j++;                                                            \
        if (j >= buffer->size)                                          \
          j = 0;                                                        \
      }                                                                 \
    }                                                                   \
    M_CALL_FREE(oplist, buffer->Tab);					\
    buffer->Tab = NULL; /* safer */                                     \
  }									\
									\

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define BUFFERI_OPLIST_P1(arg) BUFFERI_OPLIST_P2 arg

/* Validation of the given oplist */
#define BUFFERI_OPLIST_P2(name, oplist)					\
  M_IF_OPLIST(oplist)(BUFFERI_OPLIST_P3, BUFFERI_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define BUFFERI_OPLIST_FAILURE(name, oplist)		\
  ((M_LIB_ERROR(ARGUMENT_OF_BUFFER_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* OPLIST defininition of a buffer */
#define BUFFERI_OPLIST_P3(name, oplist)			                        		\
  (INIT(M_C3(name, _int, M_NAMING_INIT))                                \
   ,INIT_SET(M_C(name, M_NAMING_INIT_SET))					                    \
   ,SET(M_C(name, M_NAMING_SET))						                            \
   ,CLEAR(M_C(name, M_NAMING_CLEAR))						                        \
   ,TYPE(M_C(name, _t))							                                    \
   ,SUBTYPE(M_C(name, _type_t))						                              \
   ,CLEAN(M_C(name, M_NAMING_CLEAN))						                        \
   ,PUSH(M_C(name, _push))						                                  \
   ,POP(M_C(name, _pop))                                                \
   ,OPLIST(oplist)                                                      \
   ,TEST_EMPTY(M_C(name, M_NAMING_TEST_EMPTY)),                            \
   ,GET_SIZE(M_C(name, M_NAMING_SIZE))                                  \
   )


#endif
