/*
 * M*LIB - Fixed size BUFFER & STACK interface
 *
 * Copyright (c) 2017, Patrick Pelissier
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
#ifndef __M_BUFFER_H
#define __M_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "m-core.h"
#include "m-mutex.h"

/* Define the different kind of policy a buffer can have. */
typedef enum {
  BUFFER_QUEUE = 0,    BUFFER_STACK = 1,
  BUFFER_BLOCKING = 0, BUFFER_UNBLOCKING = 2,
  BUFFER_THREAD_SAFE = 0, BUFFER_THREAD_UNSAFE = 4,
  BUFFER_PUSH_INIT_POP_MOVE = 8,
  BUFFER_PUSH_OVERWRITE = 16
} buffer_policy_e;

/* Define a buffer.
   USAGE: BUFFER_DEF(name, type, size_of_buffer_or_0, policy[, oplist]) */
#define BUFFER_DEF(name, type, m_size, ... )                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (BUFFERI_DEF2(name, type, m_size,__VA_ARGS__, (), M_C3(buffer_, name,_t)), \
   BUFFERI_DEF2(name, type, m_size,__VA_ARGS__, M_C3(buffer_, name,_t)))


/********************************** INTERNAL ************************************/

#define BUFFERI_IF_CTE_SIZE(m_size)  M_IF(M_BOOL(m_size))
#define BUFFERI_SIZE(m_size)         BUFFERI_IF_CTE_SIZE(m_size) (m_size, v->size)

#define BUFFERI_POLICY_P(policy, val)                                   \
  (((policy) & (val)) != 0)

#define BUFFERI_DEF2(name, type, m_size, policy, oplist, buffer_t)      \
                                                                        \
  typedef struct M_C3(buffer_, name, _s) {                              \
    m_mutex_t mutex;                                                    \
    m_cond_t there_is_data;                                             \
    m_cond_t there_is_room_for_data;                                    \
    BUFFERI_IF_CTE_SIZE(m_size)( ,size_t size;)                         \
    size_t idx_prod, idx_cons, number;                                  \
    type *data;                                                         \
  } buffer_t[1];                                                        \
                                                                        \
static inline void                                                      \
M_C3(buffer_, name, _init)(buffer_t v, size_t size)                     \
{                                                                       \
  assert( size > 0);                                                    \
  BUFFERI_IF_CTE_SIZE(m_size)(assert(size == m_size), v->size = size);  \
  v->idx_prod = v->idx_cons = v->number = 0;                            \
  if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {              \
    m_mutex_init(v->mutex);                                             \
    m_cond_init(v->there_is_data);                                      \
    m_cond_init(v->there_is_room_for_data);                             \
  } else                                                                \
    assert(BUFFERI_POLICY_P((policy), BUFFER_UNBLOCKING));              \
  v->data = M_GET_REALLOC oplist (type, NULL, BUFFERI_SIZE(m_size));    \
  if (v->data == NULL) {                                                \
    M_MEMORY_FULL (BUFFERI_SIZE(m_size)*sizeof(type));                  \
    return;                                                             \
  }                                                                     \
  if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {         \
    for(size_t i = 0; i < size; i++)                                    \
      M_GET_INIT oplist(v->data[i]);                                    \
  }                                                                     \
}                                                                       \
                                                                        \
 static inline void                                                     \
 M_C3(bufferi_, name, _clear_obj)(buffer_t v)                           \
 {                                                                      \
   if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {        \
     for(size_t i = 0; i < BUFFERI_SIZE(m_size); i++)                   \
       M_GET_CLEAR oplist(v->data[i]);                                  \
   } else {                                                             \
     size_t i = BUFFERI_POLICY_P((policy), BUFFER_STACK) ? 0 : v->idx_cons; \
     while (i != v->idx_prod) {                                         \
       M_GET_CLEAR oplist(v->data[i]);                                  \
       i++;                                                             \
       if (!BUFFERI_POLICY_P((policy), BUFFER_STACK) && i >= BUFFERI_SIZE(m_size)) \
         i = 0;                                                         \
     }                                                                  \
   }                                                                    \
   v->idx_prod = v->idx_cons = v->number = 0;                           \
 }                                                                      \
                                                                        \
 static inline void                                                     \
 M_C3(buffer_, name, _clear)(buffer_t v)                                \
 {                                                                      \
   M_C3(bufferi_, name, _clear_obj)(v);                                 \
   M_GET_FREE oplist (v->data);                                         \
   v->data = NULL;                                                      \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_clear(v->mutex);                                           \
     m_cond_clear(v->there_is_data);                                    \
     m_cond_clear(v->there_is_room_for_data);                           \
   }                                                                    \
 }                                                                      \
                                                                        \
 static inline void                                                     \
 M_C3(buffer_, name, _clean)(buffer_t v)                                \
 {                                                                      \
   assert (v->number <=  BUFFERI_SIZE(m_size));                         \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE))               \
     m_mutex_lock(v->mutex);                                            \
   if (BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE))           \
     M_C3(bufferi_, name, _clear_obj)(v);                               \
   else                                                                 \
     v->idx_prod = v->idx_cons = v->number = 0;                         \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_cond_signal(v->there_is_room_for_data);                          \
     m_mutex_unlock(v->mutex);                                          \
   }                                                                    \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C3(buffer_, name, _empty_p)(buffer_t v)                              \
 {                                                                      \
   assert(v->number <=  BUFFERI_SIZE(m_size));                          \
   return v->number == 0;                                               \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C3(buffer_, name, _full_p)(buffer_t v)                               \
 {                                                                      \
   assert(v->number <=  BUFFERI_SIZE(m_size));                          \
   return v->number == BUFFERI_SIZE(m_size);                            \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C3(buffer_, name, _push)(buffer_t v, type const data)                \
 {                                                                      \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_lock(v->mutex);                                            \
     while (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_OVERWRITE)          \
            && M_C3(buffer_, name, _full_p)(v)) {                       \
       if (BUFFERI_POLICY_P((policy), BUFFER_UNBLOCKING)) {             \
         m_mutex_unlock(v->mutex);                                      \
         return false;                                                  \
       }                                                                \
       m_cond_wait(v->there_is_room_for_data, v->mutex);                \
     }                                                                  \
   } else if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_OVERWRITE)        \
              && M_C3(buffer_, name, _full_p)(v))                       \
     return false;                                                      \
                                                                        \
   if (BUFFERI_POLICY_P((policy), BUFFER_PUSH_OVERWRITE)                \
       && v->number == BUFFERI_SIZE(m_size) ) {                         \
     /* Let's clear the last element to overwrite it */                 \
     if (!BUFFERI_POLICY_P((policy), BUFFER_STACK)) {                   \
       v->idx_prod = (v->idx_prod - 1) % BUFFERI_SIZE(m_size);          \
     } else {                                                           \
       (v->idx_prod) --;                                                \
     }                                                                  \
     v->number --;                                                      \
     if (BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {       \
       M_GET_CLEAR oplist(v->data[v->idx_prod]);                        \
     }                                                                  \
   }                                                                    \
                                                                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {        \
     M_GET_SET oplist(v->data[v->idx_prod], data);                      \
   } else {                                                             \
     M_GET_INIT_SET oplist(v->data[v->idx_prod], data);                 \
   }                                                                    \
   if (!BUFFERI_POLICY_P((policy), BUFFER_STACK)) {                     \
     v->idx_prod = (v->idx_prod +1) % BUFFERI_SIZE(m_size);             \
   } else {                                                             \
     (v->idx_prod) ++;                                                  \
   }                                                                    \
   v->number ++;                                                        \
                                                                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_cond_signal(v->there_is_data);                                   \
     m_mutex_unlock(v->mutex);                                          \
   }                                                                    \
   return true;                                                         \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C3(buffer_, name, _pop)(type *data, buffer_t v)                      \
 {                                                                      \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_mutex_lock(v->mutex);                                            \
     while (M_C3(buffer_, name, _empty_p)(v)) {                         \
       if (BUFFERI_POLICY_P((policy), BUFFER_UNBLOCKING)) {             \
         m_mutex_unlock(v->mutex);                                      \
         return false;                                                  \
       }                                                                \
       m_cond_wait(v->there_is_data, v->mutex);                         \
     }                                                                  \
   } else if (M_C3(buffer_, name, _empty_p)(v))                         \
     return false;                                                      \
                                                                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_STACK)) {                     \
     if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {     \
       M_GET_SET oplist (*data, v->data[v->idx_cons]);                  \
     } else {                                                           \
       M_DO_INIT_MOVE (oplist, *data, v->data[v->idx_cons]);            \
     }                                                                  \
     v->idx_cons = (v->idx_cons +1) % BUFFERI_SIZE(m_size);             \
   } else {                                                             \
     v->idx_prod --;                                                    \
     if (!BUFFERI_POLICY_P((policy), BUFFER_PUSH_INIT_POP_MOVE)) {      \
       M_GET_SET oplist (*data, v->data[v->idx_prod]);                  \
     } else {                                                           \
       M_DO_INIT_MOVE (oplist, *data, v->data[v->idx_prod]);            \
     }                                                                  \
   }                                                                    \
   v->number --;                                                        \
                                                                        \
   if (!BUFFERI_POLICY_P((policy), BUFFER_THREAD_UNSAFE)) {             \
     m_cond_signal(v->there_is_room_for_data);                          \
     m_mutex_unlock(v->mutex);                                          \
   }                                                                    \
   return true;                                                         \
 }                                                                      \

// NOTE: SET & INIT_SET are deliberatly missing. TBC if it the right way.
// NOTE: There is no oplist defined for this container.

// TODO: priority queue when it is the one with the huge priority which is taken out
// TODO: merge queue waiting for all threads to finish producing one object to make it available to consummer (allow partial production by different threads)

#endif
