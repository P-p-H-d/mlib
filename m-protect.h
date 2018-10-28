/*
 * M*LIB - Basic Protection module over container.
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
#ifndef MSTARLIB_PROTECT_H
#define MSTARLIB_PROTECT_H

#include "m-core.h"
#include "m-mutex.h"

/* Define a dynamic array of the given type and its associated functions.
   USAGE: PROTECT_DEF(name, type [, oplist_of_the_type]) */
#define PROTECT_DEF(name, ...)                                          \
  PROTECTI_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
               ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name,_t), M_C(name,_it_t) ), \
                (name, __VA_ARGS__,                                      M_C(name,_t), M_C(name,_it_t))))



/********************************** INTERNAL ************************************/

// Deferred evaluation for the PROTECT definition.
#define PROTECTI_DEF(arg) PROTECTI_DEF2 arg

// Internal definition.
#define PROTECTI_DEF2(name, type, oplist, protect_t, protect_it_t)      \
                                                                        \
  typedef struct M_C(name, _s) {					\
    m_mutex_t lock;                                                     \
    type data;                                                          \
  } protect_t[1];                                                       \
                                                                        \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
									\
  typedef type M_C(name, _type_t);					\
									\
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _init)(protect_t out)                                       \
  {                                                                     \
    m_mutex_init(out->lock);                                            \
    M_CALL_INIT(oplist, out->data);                                     \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(INIT_SET, oplist)(                                        \
  static inline void                                                    \
  M_C(name, _init_set)(protect_t out, protect_t src)                    \
  {                                                                     \
    m_mutex_init(out->lock);                                            \
    m_mutex_lock (src->lock);                                           \
    M_CALL_INIT_SET(oplist, out->data, src->data);                      \
    m_mutex_unlock(src->lock);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SET, oplist)(                                             \
  static inline void                                                    \
  M_C(name, _set)(protect_t out, protect_t src)                         \
  {                                                                     \
    if (out < src) {                                                    \
      m_mutex_lock (out->lock);                                         \
      m_mutex_lock (src->lock);                                         \
    } else {                                                            \
      m_mutex_lock (src->lock);                                         \
      m_mutex_lock (out->lock);                                         \
    }                                                                   \
    M_CALL_SET(oplist, out->data, src->data);                           \
    if (out < src) {                                                    \
      m_mutex_unlock (src->lock);                                       \
      m_mutex_unlock (out->lock);                                       \
    } else {                                                            \
      m_mutex_unlock (out->lock);                                       \
      m_mutex_unlock (src->lock);                                       \
    }                                                                   \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(CLEAR, oplist)(                                           \
  static inline void                                                    \
  M_C(name, _clear)(protect_t out)                                      \
  {                                                                     \
    /* No need to lock */                                               \
    M_CALL_CLEAR(oplist, out->data);                                    \
    m_mutex_clear (out->lock);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(INIT_MOVE, oplist)(                                       \
  static inline void                                                    \
  M_C(name, _init_move)(protect_t out, protect_t src)                   \
  {                                                                     \
    /* No need to lock 'src' ? */                                       \
    m_mutex_init (out->lock);                                           \
    M_CALL_INIT_MOVE(oplist, out->data, src->data);                     \
    m_mutex_clear (src->lock);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(MOVE, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _move)(protect_t out, protect_t src)                        \
  {                                                                     \
    /* No need to lock 'src' ? */                                       \
    m_mutex_lock (out->lock);                                           \
    M_CALL_MOVE(oplist, out->data, src->data);                          \
    m_mutex_unlock (out->lock);                                         \
    m_mutex_clear (src->lock);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SWAP, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _swap)(protect_t out, protect_t src)                        \
  {                                                                     \
    if (out < src) {                                                    \
      m_mutex_lock (out->lock);                                         \
      m_mutex_lock (src->lock);                                         \
    } else {                                                            \
      m_mutex_lock (src->lock);                                         \
      m_mutex_lock (out->lock);                                         \
    }                                                                   \
    M_CALL_SWAP(oplist, out->data, src->data);                          \
    if (out < src) {                                                    \
      m_mutex_unlock (src->lock);                                       \
      m_mutex_unlock (out->lock);                                       \
    } else {                                                            \
      m_mutex_unlock (out->lock);                                       \
      m_mutex_unlock (src->lock);                                       \
    }                                                                   \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(CLEAN, oplist)(                                           \
  static inline void                                                    \
  M_C(name, _clean)(protect_t out)                                      \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_CLEAN(oplist, out->data);                                    \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SET_KEY, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _set_at)(protect_t out, M_GET_KEY_TYPE oplist key, M_GET_VALUE_TYPE oplist data) \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_SET_KEY(oplist, out->data, key, data);                       \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_KEY, oplist)(                                         \
  static inline bool                                                    \
  M_C(name, _get_copy)(M_GET_VALUE_TYPE oplist *out_data, protect_t out, M_GET_KEY_TYPE oplist key) \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_GET_VALUE_TYPE oplist *p = M_CALL_GET_KEY(oplist, out->data, key);   \
    if (p != NULL) {                                                    \
      M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);             \
    }                                                                   \
    m_mutex_unlock (out->lock);                                         \
    return p != NULL;                                                   \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_SET_KEY, oplist)(                                     \
  static inline void                                                    \
  M_C(name, _get_at_copy)(M_GET_VALUE_TYPE oplist *out_data, protect_t out, M_GET_KEY_TYPE oplist key) \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_GET_VALUE_TYPE oplist *p = M_CALL_GET_SET_KEY(oplist, out->data, key); \
    assert (p != NULL);                                                 \
    M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);               \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(ERASE_KEY, oplist)(                                       \
  static inline bool                                                    \
  M_C(name, _erase)(protect_t out, M_GET_KEY_TYPE oplist key)           \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    bool b = M_CALL_ERASE_KEY(oplist, out->data, key);                  \
    m_mutex_unlock (out->lock);                                         \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PUSH, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _push)(protect_t out, M_GET_SUBTYPE oplist data)            \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_PUSH(oplist, out->data, data);                               \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(POP, oplist)(                                             \
  static inline void                                                    \
  M_C(name, _pop)(M_GET_SUBTYPE oplist *p, protect_t out)               \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_POP(oplist, p, out->data);                                   \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PUSH_MOVE, oplist)(                                       \
  static inline void                                                    \
  M_C(name, _push_move)(protect_t out, M_GET_SUBTYPE oplist *data)      \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_PUSH_MOVE(oplist, out->data, data);                          \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(POP_MOVE, oplist)(                                        \
  static inline void                                                    \
  M_C(name, _pop_move)(M_GET_SUBTYPE oplist *p, protect_t out)          \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_POP_MOVE(oplist, p, out->data);                              \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _get_str)(string_t str, protect_t out, bool a)              \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_GET_STR(oplist, str, out->data, a);                          \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *f, protect_t out)                           \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_OUT_STR(oplist, f, out->data);                               \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PARSE_STR, oplist)(                                       \
  static inline bool                                                    \
  M_C(name, _out_str)(protect_t out, const char str[], const char **e)  \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    bool b = M_CALL_OUT_STR(oplist, out->data, str, e);                 \
    m_mutex_unlock (out->lock);                                         \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C(name, _in_str)(protect_t out, FILE *f)                            \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    bool b = M_CALL_IN_STR(oplist, out->data, f);                       \
    m_mutex_unlock (out->lock);                                         \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  M_C(name, _equal)(protect_t out1, protect_t out2)                     \
  {                                                                     \
    if (out1 < out2) {                                                  \
      m_mutex_lock (out1->lock);                                        \
      m_mutex_lock (out2->lock);                                        \
    } else {                                                            \
      m_mutex_lock (out2->lock);                                        \
      m_mutex_lock (out1->lock);                                        \
    }                                                                   \
    bool b = M_CALL_EQUAL(oplist, out1->data, out2->data);              \
    if (out1 < out2) {                                                  \
      m_mutex_unlock (out2->lock);                                      \
      m_mutex_unlock (out1->lock);                                      \
    } else {                                                            \
      m_mutex_unlock (out1->lock);                                      \
      m_mutex_unlock (out2->lock);                                      \
    }                                                                   \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \

#endif
