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


/* Define a protected concurrent container and its associated functions with Read Preference.
   USAGE: CONCURRENT_RP_DEF(name, type [, oplist_of_the_type]) */
#define CONCURRENT_RP_DEF(name, ...)                                    \
  CONCURRENTI_RP_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
               ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name,_t), M_C(name,_it_t) ), \
                (name, __VA_ARGS__,                                      M_C(name,_t), M_C(name,_it_t))))


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

#define CONCURRENTI_CONTRACT(c) do {            \
    assert ((c) != NULL);                       \
  } while (0)

// Deferred evaluation for the concurrent definition.
#define CONCURRENTI_DEF(arg) CONCURRENTI_DEF2 arg

// Internal definition.
#define CONCURRENTI_DEF2(name, type, oplist, concurrent_t, concurrent_it_t) \
                                                                        \
  typedef struct M_C(name, _s) {					\
    struct M_C(name, _s) *self;                                         \
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
  /* Define the lock strategy (global & shared lock) */                 \
  static inline void                                                    \
  M_C(name, _internal_init)(concurrent_t out)                           \
  {                                                                     \
    m_mutex_init(out->lock);                                            \
    m_cond_init(out->there_is_data);                                    \
    out->self = out;                                                    \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _internal_clear)(concurrent_t out)                          \
  {                                                                     \
    assert (out->self == out);                                          \
    m_mutex_clear(out->lock);                                           \
    m_cond_clear(out->there_is_data);                                   \
    out->self = NULL;                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_lock)(const concurrent_t out)                         \
  {                                                                     \
    assert (out->self == out);                                          \
    m_mutex_lock (out->self->lock);                                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_unlock)(const concurrent_t out)                       \
  {                                                                     \
    assert (out->self == out);                                          \
    m_mutex_unlock (out->self->lock);                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_wait)(int *p, const concurrent_t out)                 \
  {                                                                     \
    assert (out->self == out);                                          \
    (void) p;                                                           \
    m_cond_wait(out->self->there_is_data, out->self->lock);             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_wait_unlock)(int *p, const concurrent_t out)          \
  {                                                                     \
    (void) p;                                                           \
    m_mutex_unlock (out->self->lock);                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_lock)(concurrent_t out)                              \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_unlock)(concurrent_t out)                            \
  {                                                                     \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_wait)(const concurrent_t out)                        \
  {                                                                     \
    m_cond_wait(out->self->there_is_data, out->self->lock);             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_signal)(concurrent_t out)                            \
  {                                                                     \
    m_cond_broadcast(out->there_is_data);                               \
  }                                                                     \
                                                                        \
  CONCURRENTI_DEF_FUNC(name, type, oplist, concurrent_t, concurrent_it_t)


// Internal definition of the functions.
#define CONCURRENTI_DEF_FUNC(name, type, oplist, concurrent_t, concurrent_it_t) \
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _init)(concurrent_t out)                                    \
  {                                                                     \
    M_C(name, _internal_init)(out);                                     \
    M_CALL_INIT(oplist, out->data);                                     \
    CONCURRENTI_CONTRACT(out);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(INIT_SET, oplist)(                                        \
  static inline void                                                    \
  M_C(name, _init_set)(concurrent_t out, concurrent_t const src)        \
  {                                                                     \
    CONCURRENTI_CONTRACT(src);                                          \
    assert (out != src);                                                \
    M_C(name, _internal_init)(out);                                     \
    M_C(name, _read_lock)(src);                                         \
    M_CALL_INIT_SET(oplist, out->data, src->data);                      \
    M_C(name, _read_unlock)(src);                                       \
    CONCURRENTI_CONTRACT(out);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SET, oplist)(                                             \
  static inline void                                                    \
  M_C(name, _set)(concurrent_t out, concurrent_t const src)             \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    if (out == src) return;                                             \
    if (out < src) {                                                    \
      M_C(name, _write_lock)(out);                                      \
      M_C(name, _read_lock)(src);                                       \
    } else {                                                            \
      M_C(name, _read_lock)(src);                                       \
      M_C(name, _write_lock)(out);                                      \
    }                                                                   \
    M_CALL_SET(oplist, out->data, src->data);                           \
    if (out < src) {                                                    \
      M_C(name, _read_lock)(src);                                       \
      M_C(name, _write_unlock)(out);                                    \
    } else {                                                            \
      M_C(name, _write_unlock)(out);                                    \
      M_C(name, _read_unlock)(src);                                     \
    }                                                                   \
    CONCURRENTI_CONTRACT(out);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(CLEAR, oplist)(                                           \
  static inline void                                                    \
  M_C(name, _clear)(concurrent_t out)                                   \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    /* No need to lock */                                               \
    M_CALL_CLEAR(oplist, out->data);                                    \
    M_C(name, _internal_clear)(out);                                    \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(INIT_MOVE, oplist)(                                       \
  static inline void                                                    \
  M_C(name, _init_move)(concurrent_t out, concurrent_t src)             \
  {                                                                     \
    CONCURRENTI_CONTRACT(src);                                          \
    assert (out != src);                                                \
    /* No need to lock 'src' ? */                                       \
    M_C(name, _internal_init)(out);                                     \
    M_CALL_INIT_MOVE(oplist, out->data, src->data);                     \
    M_C(name, _internal_clear)(src);                                    \
    CONCURRENTI_CONTRACT(out);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(MOVE, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _move)(concurrent_t out, concurrent_t src)                  \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    CONCURRENTI_CONTRACT(src);                                          \
    /* No need to lock 'src' ? */                                       \
    M_C(name, _write_lock)(out);                                        \
    M_CALL_MOVE(oplist, out->data, src->data);                          \
    M_C(name, _write_unlock)(out);                                      \
    M_C(name, _internal_clear)(src);                                    \
    CONCURRENTI_CONTRACT(out);                                          \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SWAP, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _swap)(concurrent_t out, concurrent_t src)                  \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    CONCURRENTI_CONTRACT(src);                                          \
    if (out < src) {                                                    \
      M_C(name, _write_lock)(out);                                      \
      M_C(name, _write_lock)(src);                                      \
    } else {                                                            \
      M_C(name, _write_lock)(src);                                      \
      M_C(name, _write_lock)(out);                                      \
    }                                                                   \
    M_CALL_SWAP(oplist, out->data, src->data);                          \
    if (out < src) {                                                    \
      M_C(name, _write_unlock)(src);                                    \
      M_C(name, _write_unlock)(out);                                    \
    } else {                                                            \
      M_C(name, _write_unlock)(out);                                    \
      M_C(name, _write_unlock)(src);                                    \
    }                                                                   \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(CLEAN, oplist)(                                           \
  static inline void                                                    \
  M_C(name, _clean)(concurrent_t out)                                   \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    M_CALL_CLEAN(oplist, out->data);                                    \
    M_C(name, _write_unlock)(out);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(TEST_EMPTY, oplist)(                                      \
  static inline bool                                                    \
  M_C(name, _empty_p)(concurrent_t const out)                           \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _read_lock)(out);                                         \
    bool b = M_CALL_TEST_EMPTY(oplist, out->data);                      \
    M_C(name, _read_unlock)(out);                                       \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(SET_KEY, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _set_at)(concurrent_t out, M_GET_KEY_TYPE oplist const key, M_GET_VALUE_TYPE oplist const data) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    M_CALL_SET_KEY(oplist, out->data, key, data);                       \
    M_C(name, _write_signal)(out);                                      \
    M_C(name, _write_unlock)(out);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_KEY, oplist)(                                         \
  static inline bool                                                    \
  M_C(name, _get_copy)(M_GET_VALUE_TYPE oplist *out_data, const concurrent_t out, M_GET_KEY_TYPE oplist const key) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    assert (out_data != NULL);                                          \
    M_C(name, _read_lock)(out);                                         \
    M_GET_VALUE_TYPE oplist *p = M_CALL_GET_KEY(oplist, out->data, key); \
    if (p != NULL) {                                                    \
      M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);             \
    }                                                                   \
    M_C(name, _read_unlock)(out);                                       \
    return p != NULL;                                                   \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_SET_KEY, oplist)(                                     \
  static inline void                                                    \
  M_C(name, _get_at_copy)(M_GET_VALUE_TYPE oplist *out_data, concurrent_t out, M_GET_KEY_TYPE oplist const key) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    assert (out_data != NULL);                                          \
    M_C(name, _write_lock)(out);                                        \
    M_GET_VALUE_TYPE oplist *p = M_CALL_GET_SET_KEY(oplist, out->data, key); \
    assert (p != NULL);                                                 \
    M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);               \
    M_C(name, _write_unlock)(out);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(ERASE_KEY, oplist)(                                       \
  static inline bool                                                    \
  M_C(name, _erase)(concurrent_t out, M_GET_KEY_TYPE oplist const key)  \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    bool b = M_CALL_ERASE_KEY(oplist, out->data, key);                  \
    M_C(name, _write_unlock)(out);                                      \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PUSH, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _push)(concurrent_t out, M_GET_SUBTYPE oplist const data)   \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    M_CALL_PUSH(oplist, out->data, data);                               \
    M_C(name, _write_signal)(out);                                      \
    M_C(name, _write_unlock)(out);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(POP, oplist)(                                             \
  static inline void                                                    \
  M_C(name, _pop)(M_GET_SUBTYPE oplist *p, concurrent_t out)            \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    M_CALL_POP(oplist, p, out->data);                                   \
    M_C(name, _write_unlock)(out);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PUSH_MOVE, oplist)(                                       \
  static inline void                                                    \
  M_C(name, _push_move)(concurrent_t out, M_GET_SUBTYPE oplist *data)   \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    M_CALL_PUSH_MOVE(oplist, out->data, data);                          \
    M_C(name, _write_signal)(out);                                      \
    M_C(name, _write_unlock)(out);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(POP_MOVE, oplist)(                                        \
  static inline void                                                    \
  M_C(name, _pop_move)(M_GET_SUBTYPE oplist *p, concurrent_t out)       \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    M_CALL_POP_MOVE(oplist, p, out->data);                              \
    M_C(name, _write_unlock)(out);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _get_str)(string_t str, concurrent_t const out, bool a)     \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _read_lock)(out);                                         \
    M_CALL_GET_STR(oplist, str, out->data, a);                          \
    M_C(name, _read_unlock)(out);                                       \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *f, concurrent_t const out)                  \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _read_lock)(out);                                         \
    M_CALL_OUT_STR(oplist, f, out->data);                               \
    M_C(name, _read_unlock)(out);                                       \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(PARSE_STR, oplist)(                                       \
  static inline bool                                                    \
  M_C(name, _parse_str)(concurrent_t out, const char str[], const char **e) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    bool b = M_CALL_PARSE_STR(oplist, out->data, str, e);               \
    M_C(name, _write_signal)(out);                                      \
    M_C(name, _write_unlock)(out);                                      \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C(name, _in_str)(concurrent_t out, FILE *f)                         \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _write_lock)(out);                                        \
    bool b = M_CALL_IN_STR(oplist, out->data, f);                       \
    M_C(name, _write_signal)(out);                                      \
    M_C(name, _write_unlock)(out);                                      \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  M_C(name, _equal_p)(concurrent_t const out1, concurrent_t const out2) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out1);                                         \
    CONCURRENTI_CONTRACT(out2);                                         \
    if (out1 == out2) return true;                                      \
    if (out1 < out2) {                                                  \
      M_C(name, _read_lock)(out1);                                      \
      M_C(name, _read_lock)(out2);                                      \
    } else {                                                            \
      M_C(name, _read_lock)(out2);                                      \
      M_C(name, _read_lock)(out1);                                      \
    }                                                                   \
    bool b = M_CALL_EQUAL(oplist, out1->data, out2->data);              \
    if (out1 < out2) {                                                  \
      M_C(name, _read_unlock)(out2);                                    \
      M_C(name, _read_unlock)(out1);                                    \
    } else {                                                            \
      M_C(name, _read_unlock)(out1);                                    \
      M_C(name, _read_unlock)(out2);                                    \
    }                                                                   \
    return b;                                                           \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(GET_KEY, oplist)(                                         \
  static inline bool                                                    \
  M_C(name, _get_blocking)(M_GET_VALUE_TYPE oplist *out_data, const concurrent_t out, M_GET_KEY_TYPE oplist const key, bool blocking) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    assert (out_data != NULL);                                          \
    bool ret = false;                                                   \
    int  count = 0;                                                     \
    M_C(name, _read_lock)(out);                                         \
    while (true) {                                                      \
      M_GET_VALUE_TYPE oplist *p = M_CALL_GET_KEY(oplist, out->data, key); \
      if (p != NULL) {                                                  \
        M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);           \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
      if (blocking == false) break;                                     \
      M_C(name, _read_wait)(&count, out);                               \
    }                                                                   \
    M_C(name, _read_wait_unlock)(&count, out);                          \
    return ret;                                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD2(POP, TEST_EMPTY, oplist)(                                \
  static inline bool                                                    \
  M_C(name, _pop_blocking)(M_GET_SUBTYPE oplist *p, concurrent_t out, bool blocking) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    assert (p != NULL);                                                 \
    bool ret = false;                                                   \
    M_C(name, _write_lock)(out);                                        \
    while (true) {                                                      \
      if (!M_CALL_TEST_EMPTY(oplist, out->data)) {                      \
        M_CALL_POP(oplist, p, out->data);                               \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
      if (blocking == false) break;                                     \
      M_C(name, _write_wait)(out);                                      \
    }                                                                   \
    M_C(name, _write_unlock)(out);                                      \
    return ret;                                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD2(POP_MOVE, TEST_EMPTY, oplist)(                           \
  static inline bool                                                    \
  M_C(name, _pop_move_blocking)(M_GET_SUBTYPE oplist *p, concurrent_t out, bool blocking) \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    assert (p != NULL);                                                 \
    bool ret = false;                                                   \
    M_C(name, _write_lock)(out);                                        \
    while (true) {                                                      \
      if (!M_CALL_TEST_EMPTY(oplist, out->data)) {                      \
        M_CALL_POP_MOVE(oplist, p, out->data);                          \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
      if (blocking == false) break;                                     \
      M_C(name, _write_wait)(out);                                      \
    }                                                                   \
    M_C(name, _write_unlock)(out);                                      \
    return ret;                                                         \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t                                                  \
  M_C(name, _hash)(concurrent_t const out)                              \
  {                                                                     \
    CONCURRENTI_CONTRACT(out);                                          \
    M_C(name, _read_lock)(out);                                         \
    size_t h = M_CALL_HASH(oplist, out->data);                          \
    M_C(name, _read_unlock)(out);                                       \
    return h;                                                           \
  }                                                                     \
  ,)                                                                    \



// Deferred evaluation for the concurrent definition.
#define CONCURRENTI_RP_DEF(arg) CONCURRENTI_RP_DEF2 arg

// Internal definition.
#define CONCURRENTI_RP_DEF2(name, type, oplist, concurrent_t, concurrent_it_t) \
                                                                        \
  typedef struct M_C(name, _s) {					\
    struct M_C(name, _s) *self;                                         \
    m_mutex_t lock;                                                     \
    m_mutex_t read_lock;                                                \
    size_t    read_count;                                               \
    m_cond_t  there_is_data; /* condition raised when there is data */  \
    type      data;                                                     \
  } concurrent_t[1];                                                    \
                                                                        \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
									\
  typedef type M_C(name, _type_t);					\
                                                                        \
  /* Define the lock strategy (global & shared lock) */                 \
  static inline void                                                    \
  M_C(name, _internal_init)(concurrent_t out)                           \
  {                                                                     \
    m_mutex_init(out->lock);                                            \
    m_mutex_init(out->read_lock);                                       \
    m_cond_init(out->there_is_data);                                    \
    out->self = out;                                                    \
    out->read_count = 0;                                                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _internal_clear)(concurrent_t out)                          \
  {                                                                     \
    assert (out->self == out);                                          \
    m_mutex_clear(out->lock);                                           \
    m_mutex_clear(out->read_lock);                                      \
    m_cond_clear(out->there_is_data);                                   \
    out->self = NULL;                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_lock)(const concurrent_t out)                         \
  {                                                                     \
    struct M_C(name, _s) *self = out->self;                             \
    assert (self == out);                                               \
    m_mutex_lock (self->read_lock);                                     \
    self->read_count ++;                                                \
    if (self->read_count == 1) {                                        \
      m_mutex_lock (self->lock);                                        \
    }                                                                   \
    m_mutex_unlock (self->read_lock);                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_unlock)(const concurrent_t out)                       \
  {                                                                     \
    struct M_C(name, _s) *self = out->self;                             \
    assert (self == out);                                               \
    m_mutex_lock (self->read_lock);                                     \
    self->read_count --;                                                \
    if (self->read_count == 0) {                                        \
      m_mutex_unlock (self->lock);                                      \
    }                                                                   \
    m_mutex_unlock (self->read_lock);                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_lock)(concurrent_t out)                              \
  {                                                                     \
    m_mutex_lock (out->lock);                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_unlock)(concurrent_t out)                            \
  {                                                                     \
    m_mutex_unlock (out->lock);                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_wait)(int *p, const concurrent_t out)                 \
  {                                                                     \
    struct M_C(name, _s) *self = out->self;                             \
    assert (self == out);                                               \
    if (*p == 0) {                                                      \
      m_mutex_unlock (self->read_lock);                                 \
      m_mutex_lock (self->lock);                                        \
      *p = 1;                                                           \
    } else {                                                            \
      m_cond_wait(self->there_is_data, self->lock);                     \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _read_wait_unlock)(int *p, const concurrent_t out)          \
  {                                                                     \
    m_mutex_unlock (*p == 0 ? out->self->read_lock : out->self->lock);  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_wait)(const concurrent_t out)                        \
  {                                                                     \
    m_cond_wait(out->self->there_is_data, out->self->lock);             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _write_signal)(concurrent_t out)                            \
  {                                                                     \
    m_cond_broadcast(out->there_is_data);                               \
  }                                                                     \
                                                                        \
  CONCURRENTI_DEF_FUNC(name, type, oplist, concurrent_t, concurrent_it_t)

#endif
