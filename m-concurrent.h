/*
 * M*LIB - Basic Protected Concurrent module over container.
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
#ifndef MSTARLIB_CONCURRENT_H
#define MSTARLIB_CONCURRENT_H

#include "m-core.h"
#include "m-mutex.h"

/* Define a protected concurrent container and its associated functions.
   USAGE: CONCURRENT_DEF(name, type [, oplist_of_the_type]) */
#define CONCURRENT_DEF(name, ...)                                          \
  CONCURRENTI_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
               ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name,_t), M_C(name,_it_t) ), \
                (name, __VA_ARGS__,                                      M_C(name,_t), M_C(name,_it_t))))


/* Define the oplist of a protected concurrent container given its name and its oplist.
   USAGE: CONCURRENT_OPLIST(name[, oplist of the type]) */
#define CONCURRENT_OPLIST(...)                                          \
  CONCURRENTI_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
                     ((__VA_ARGS__, M_DEFAULT_OPLIST),			\
                      (__VA_ARGS__ )))



/********************************** INTERNAL ************************************/

// Deferred evaluation for the oplist definition.
#define CONCURRENTI_OPLIST(arg) CONCURRENTI_OPLIST2 arg

/* OPLIST definition
   GET_KEY is not present as its interface is not compatible with a concurrent
   container (_get returns a pointer to an internal data, data that may be 
   destroyed by another thread).
*/
#define CONCURRENTI_OPLIST2(name, oplist)					\
  (M_IF_METHOD(INIT, oplist)(INIT(M_C(name, _init)),)                   \
   ,M_IF_METHOD(INIT_SET, oplist)(INIT_SET(M_C(name, _init_set)),)      \
   ,M_IF_METHOD(SET, oplist)(SET(M_C(name, _set)),)                     \
   ,M_IF_METHOD(CLEAR, oplist)(CLEAR(M_C(name, _clear)),)               \
   ,M_IF_METHOD(INIT_MOVE, oplist)(INIT_MOVE(M_C(name, _init_move)),)   \
   ,M_IF_METHOD(MOVE, oplist)(MOVE(M_C(name, _move)),)                  \
   ,M_IF_METHOD(SWAP,oplist)(SWAP(M_C(name, _swap)),)                   \
   ,TYPE(M_C(name,_t))							\
   ,SUBTYPE(M_C(name, _type_t))						\
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(TEST_EMPTY, oplist)(TEST_EMPTY(M_C(name,_empty_p)),)    \
   ,M_IF_METHOD(CLEAN, oplist)(CLEAN(M_C(name,_clean)),)                \
   ,M_IF_METHOD(KEY_TYPE, oplist)(KEY_TYPE(M_GET_KEY_TYPE oplist),)     \
   ,M_IF_METHOD(VALUE_TYPE, oplist)(VALUE_TYPE(M_GET_VALUE_TYPE oplist),) \
   ,M_IF_METHOD(KEY_TYPE, oplist)(KEY_OPLIST(M_GET_KEY_OPLIST oplist),) \
   ,M_IF_METHOD(VALUE_TYPE, oplist)(VALUE_OPLIST(M_GET_VALUE_OPLIST oplist), ) \
   ,M_IF_METHOD(SET_KEY, oplist)(SET_KEY(M_C(name, _set_at)),)          \
   ,M_IF_METHOD(ERASE_KEY, oplist)(ERASE_KEY(M_C(name, _erase)),)       \
   ,M_IF_METHOD(PUSH, oplist)(PUSH(M_C(name,_push)),)                   \
   ,M_IF_METHOD(POP, oplist)(POP(M_C(name,_pop)),)                      \
   ,M_IF_METHOD(PUSH_MOVE, oplist)(PUSH_MOVE(M_C(name,_push_move)),)    \
   ,M_IF_METHOD(POP_MOVE, oplist)(POP_MOVE(M_C(name,_pop_move)),)       \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C(name, _get_str)),)		\
   ,M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_C(name, _parse_str)),)   \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C(name, _out_str)),)		\
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C(name, _in_str)),)		\
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C(name, _equal_p)),)		\
   ,M_IF_METHOD(HASH, oplist)(HASH(M_C(name, _hash)),)			\
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )


// Deferred evaluation for the concurrent definition.
#define CONCURRENTI_DEF(arg) CONCURRENTI_DEF2 arg

// Internal definition.
#define CONCURRENTI_DEF2(name, type, oplist, concurrent_t, concurrent_it_t) \
                                                                        \
  typedef struct M_C(name, _s) {					\
    m_mutex_t lock;                                                     \
    m_cond_t  there_is_data; /* condition raised when there is data */  \
    type      data;                                                     \
  } concurrent_t[1];                                                    \
                                                                        \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
									\
  typedef type M_C(name, _type_t);					\
                                                                        \
									\
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _init)(concurrent_t out)                                    \
  {                                                                     \
    m_mutex_init(out->lock);                                            \
    m_cond_init(out->there_is_data);                                    \
    M_CALL_INIT(oplist, out->data);                                     \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(INIT_SET, oplist)(                                        \
  static inline void                                                    \
  M_C(name, _init_set)(concurrent_t out, concurrent_t src)              \
  {                                                                     \
    assert (out != src);                                                \
    m_mutex_init(out->lock);                                            \
    m_cond_init(out->there_is_data);                                    \
    m_mutex_lock (src->lock);                                           \
    M_CALL_INIT_SET(oplist, out->data, src->data);                      \
    m_mutex_unlock(src->lock);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SET, oplist)(                                             \
  static inline void                                                    \
  M_C(name, _set)(concurrent_t out, concurrent_t src)                   \
  {                                                                     \
    if (out == src) return;                                             \
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
  M_C(name, _clear)(concurrent_t out)                                   \
  {                                                                     \
    /* No need to lock */                                               \
    M_CALL_CLEAR(oplist, out->data);                                    \
    m_mutex_clear (out->lock);                                          \
    m_cond_clear(out->there_is_data);                                   \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(INIT_MOVE, oplist)(                                       \
  static inline void                                                    \
  M_C(name, _init_move)(concurrent_t out, concurrent_t src)             \
  {                                                                     \
    assert (out != src);                                                \
    /* No need to lock 'src' ? */                                       \
    m_mutex_init (out->lock);                                           \
    m_cond_init (out->there_is_data);                                   \
    M_CALL_INIT_MOVE(oplist, out->data, src->data);                     \
    m_mutex_clear (src->lock);                                          \
    m_cond_clear (src->there_is_data);                                  \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(MOVE, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _move)(concurrent_t out, concurrent_t src)                  \
  {                                                                     \
    /* No need to lock 'src' ? */                                       \
    m_mutex_lock (out->lock);                                           \
    M_CALL_MOVE(oplist, out->data, src->data);                          \
    m_mutex_unlock (out->lock);                                         \
    m_mutex_clear (src->lock);                                          \
    m_cond_clear (src->there_is_data);                                  \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SWAP, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _swap)(concurrent_t out, concurrent_t src)                  \
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
  M_C(name, _clean)(concurrent_t out)                                   \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_CLEAN(oplist, out->data);                                    \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(TEST_EMPTY, oplist)(                                      \
  static inline bool                                                    \
  M_C(name, _empty_p)(concurrent_t out)                                 \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    bool b = M_CALL_TEST_EMPTY(oplist, out->data);                      \
    m_mutex_unlock (out->lock);                                         \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SET_KEY, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _set_at)(concurrent_t out, M_GET_KEY_TYPE oplist const key, M_GET_VALUE_TYPE oplist const data) \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_SET_KEY(oplist, out->data, key, data);                       \
    m_cond_broadcast(out->there_is_data);                               \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_KEY, oplist)(                                         \
  static inline bool                                                    \
  M_C(name, _get_copy)(M_GET_VALUE_TYPE oplist *out_data, concurrent_t out, M_GET_KEY_TYPE oplist const key) \
  {                                                                     \
    assert (out_data != NULL);                                          \
    m_mutex_lock (out->lock);                                           \
    M_GET_VALUE_TYPE oplist *p = M_CALL_GET_KEY(oplist, out->data, key); \
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
  M_C(name, _get_at_copy)(M_GET_VALUE_TYPE oplist *out_data, concurrent_t out, M_GET_KEY_TYPE oplist const key) \
  {                                                                     \
    assert (out_data != NULL);                                          \
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
  M_C(name, _erase)(concurrent_t out, M_GET_KEY_TYPE oplist const key)  \
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
  M_C(name, _push)(concurrent_t out, M_GET_SUBTYPE oplist const data)   \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_PUSH(oplist, out->data, data);                               \
    m_cond_broadcast(out->there_is_data);                               \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(POP, oplist)(                                             \
  static inline void                                                    \
  M_C(name, _pop)(M_GET_SUBTYPE oplist *p, concurrent_t out)            \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_POP(oplist, p, out->data);                                   \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PUSH_MOVE, oplist)(                                       \
  static inline void                                                    \
  M_C(name, _push_move)(concurrent_t out, M_GET_SUBTYPE oplist *data)   \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_PUSH_MOVE(oplist, out->data, data);                          \
    m_cond_broadcast(out->there_is_data);                               \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(POP_MOVE, oplist)(                                        \
  static inline void                                                    \
  M_C(name, _pop_move)(M_GET_SUBTYPE oplist *p, concurrent_t out)       \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_POP_MOVE(oplist, p, out->data);                              \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _get_str)(string_t str, concurrent_t out, bool a)           \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_GET_STR(oplist, str, out->data, a);                          \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *f, concurrent_t out)                        \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    M_CALL_OUT_STR(oplist, f, out->data);                               \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PARSE_STR, oplist)(                                       \
  static inline bool                                                    \
  M_C(name, _parse_str)(concurrent_t out, const char str[], const char **e) \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    bool b = M_CALL_PARSE_STR(oplist, out->data, str, e);               \
    m_mutex_unlock (out->lock);                                         \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C(name, _in_str)(concurrent_t out, FILE *f)                         \
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
  M_C(name, _equal)(concurrent_t out1, concurrent_t out2)               \
  {                                                                     \
    if (out1 == out2) return true;                                      \
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
                                                                        \
  M_IF_METHOD(GET_KEY, oplist)(                                         \
  static inline bool                                                    \
  M_C(name, _get_blocking)(M_GET_VALUE_TYPE oplist *out_data, concurrent_t out, M_GET_KEY_TYPE oplist const key, bool blocking) \
  {                                                                     \
    assert (out_data != NULL);                                          \
    bool ret = false;                                                   \
    m_mutex_lock (out->lock);                                           \
    while (true) {                                                      \
      M_GET_VALUE_TYPE oplist *p = M_CALL_GET_KEY(oplist, out->data, key); \
      if (p != NULL) {                                                  \
        M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);           \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
      if (blocking == false) break;                                     \
      m_cond_wait(out->there_is_data, out->lock);                       \
    }                                                                   \
    m_mutex_unlock (out->lock);                                         \
    return ret;                                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD2(POP, TEST_EMPTY, oplist)(                                \
  static inline bool                                                    \
  M_C(name, _pop_blocking)(M_GET_SUBTYPE oplist *p, concurrent_t out, bool blocking) \
  {                                                                     \
    assert (p != NULL);                                                 \
    bool ret = false;                                                   \
    m_mutex_lock (out->lock);                                           \
    while (true) {                                                      \
      if (!M_CALL_TEST_EMPTY(oplist, out->data)) {                      \
        M_CALL_POP(oplist, p, out->data);                               \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
      if (blocking == false) break;                                     \
      m_cond_wait(out->there_is_data, out->lock);                       \
    }                                                                   \
    m_mutex_unlock (out->lock);                                         \
    return ret;                                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD2(POP_MOVE, TEST_EMPTY, oplist)(                           \
  static inline bool                                                    \
  M_C(name, _pop_move_blocking)(M_GET_SUBTYPE oplist *p, concurrent_t out, bool blocking) \
  {                                                                     \
    assert (p != NULL);                                                 \
    bool ret = false;                                                   \
    m_mutex_lock (out->lock);                                           \
    while (true) {                                                      \
      if (!M_CALL_TEST_EMPTY(oplist, out->data)) {                      \
        M_CALL_POP_MOVE(oplist, p, out->data);                          \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
      if (blocking == false) break;                                     \
      m_cond_wait(out->there_is_data, out->lock);                       \
    }                                                                   \
    m_mutex_unlock (out->lock);                                         \
    return ret;                                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t                                                  \
  M_C(name, _hash)(concurrent_t out)                                    \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
    size_t h = M_CALL_HASH(oplist, out->data);                          \
    m_mutex_unlock (out->lock);                                         \
    return h;                                                           \
  }                                                                     \
  ,)                                                                    \

#endif
