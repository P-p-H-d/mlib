/*
 * M*LIB - dynamic ARRAY module
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
#ifndef MSTARLIB_ARRAY_H
#define MSTARLIB_ARRAY_H

#include "m-core.h"

/* Define a dynamic array of the given type and its associated functions.
   USAGE: ARRAY_DEF(name, type [, oplist_of_the_type]) */
#define ARRAY_DEF(name, ...)                                            \
  ARRAYI_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                                \
  ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), M_C(name,_t), M_C(name,_it_t) ), \
   (name, __VA_ARGS__,                                      M_C(name,_t), M_C(name,_it_t))))

/* Define the oplist of a dynamic array given its name and its oplist.
   USAGE: ARRAY_OPLIST(name[, oplist of the type]) */
#define ARRAY_OPLIST(...)                                               \
  ARRAYI_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ),   \
                 (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

// Deferred evaluation for the oplist definition.
#define ARRAYI_OPLIST(arg) ARRAYI_OPLIST2 arg

/* OPLIST defininition of a dynamic array */
/* FIXME: Do we want to export some methods as they are slow and 
   are not fit to be used for building other methods (like _remove)? */
#define ARRAYI_OPLIST2(name, oplist)					\
  (INIT(M_C(name, _init))						\
   ,INIT_SET(M_C(name, _init_set))					\
   ,SET(M_C(name, _set))						\
   ,CLEAR(M_C(name, _clear))						\
   ,INIT_MOVE(M_C(name, _init_move))					\
   ,MOVE(M_C(name, _move))						\
   ,SWAP(M_C(name, _swap))						\
   ,TYPE(M_C(name,_t))							\
   ,SUBTYPE(M_C(name, _type_t))						\
   ,IT_TYPE(M_C(name,_it_t))						\
   ,IT_FIRST(M_C(name,_it))						\
   ,IT_LAST(M_C(name,_it_last))						\
   ,IT_END(M_C(name,_it_end))						\
   ,IT_SET(M_C(name,_it_set))						\
   ,IT_END_P(M_C(name,_end_p))						\
   ,IT_LAST_P(M_C(name,_last_p))					\
   ,IT_EQUAL_P(M_C(name,_it_equal_p))					\
   ,IT_NEXT(M_C(name,_next))						\
   ,IT_PREVIOUS(M_C(name,_previous))					\
   ,IT_REF(M_C(name,_ref))						\
   ,IT_CREF(M_C(name,_cref))						\
   ,IT_REMOVE(M_C(name,_remove))                                        \
   ,CLEAN(M_C(name,_clean))						\
   ,PUSH(M_C(name,_push_back))						\
   ,POP(M_C(name,_pop_back))						\
   ,PUSH_MOVE(M_C(name,_push_move))                                     \
   ,POP_MOVE(M_C(name,_pop_move))                                       \
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(CMP, oplist)(SORT(M_C(name, _special_sort)),)		\
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

/* Define the contract of an array */
#define ARRAYI_CONTRACT(a) do {                 \
    assert (a != NULL);                         \
    assert (a->size <= a->alloc);               \
    assert (a->size == 0 || a->ptr != NULL);    \
  } while (0)

// Deferred evaluation for the array definition.
#define ARRAYI_DEF(arg) ARRAYI_DEF2 arg

// Internal definition.
#define ARRAYI_DEF2(name, type, oplist, array_t, array_it_t)            \
									\
  typedef struct M_C(name, _s) {					\
    size_t size, alloc;                                                 \
    type *ptr;                                                          \
  } array_t[1];                                                         \
									\
  typedef type M_C(name, _type_t);					\
									\
  typedef struct M_C(name, _it_s) {					\
    size_t index;                                                       \
    const struct M_C(name, _s) *array;					\
  } array_it_t[1];                                                      \
  									\
  static inline void                                                    \
  M_C(name, _init)(array_t v)						\
  {                                                                     \
    assert (v != NULL);                                                 \
    v->size  = 0;                                                       \
    v->alloc = 0;                                                       \
    v->ptr   = NULL;                                                    \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _clean)(array_t v)                                          \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    for(size_t i = 0; i < v->size; i++)                                 \
      M_GET_CLEAR oplist(v->ptr[i]);                                    \
    v->size = 0;                                                        \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
									\
  static inline void                                                    \
  M_C(name, _clear)(array_t v)						\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    M_C(name, _clean)(v);						\
    M_GET_FREE oplist(v->ptr);                                          \
    /* Don't reset size to 0 to keep a field out of range so that it    \
       can be detected by an assertion. */                              \
    v->alloc = 0;                                                       \
    v->ptr = NULL;                                                      \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _set)(array_t d, const array_t s)				\
  {                                                                     \
    ARRAYI_CONTRACT(d);                                                 \
    ARRAYI_CONTRACT(s);                                                 \
    if (M_UNLIKELY (d == s)) return;                                    \
    if (s->size > d->alloc) {                                           \
      const size_t alloc = s->size;					\
      type *ptr = M_GET_REALLOC oplist (type, d->ptr, alloc);           \
      if (M_UNLIKELY (ptr == NULL)) {					\
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return ;                                                        \
      }                                                                 \
      d->ptr = ptr;                                                     \
      d->alloc = alloc;                                                 \
    }                                                                   \
    size_t i, step1 = M_MIN(s->size, d->size);                          \
    for(i = 0; i < step1; i++)                                          \
      M_GET_SET oplist (d->ptr[i], s->ptr[i]);                          \
    for( ; i < s->size; i++)                                            \
      M_GET_INIT_SET oplist (d->ptr[i], s->ptr[i]);                     \
    for( ; i < d->size; i++)                                            \
      M_GET_CLEAR oplist (d->ptr[i]);                                   \
    d->size = s->size;                                                  \
    ARRAYI_CONTRACT(d);                                                 \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _init_set)(array_t d, const array_t s)			\
  {                                                                     \
    assert (d != s);                                                    \
    M_C(name, _init)(d);						\
    M_C(name, _set)(d, s);						\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _init_move)(array_t d, array_t s)				\
  {                                                                     \
    assert (d != s);                                                    \
    ARRAYI_CONTRACT(s);                                                 \
    d->size  = s->size;                                                 \
    d->alloc = s->alloc;                                                \
    d->ptr   = s->ptr;                                                  \
    s->alloc = 0;                                                       \
    s->ptr   = NULL;                                                    \
    ARRAYI_CONTRACT(d);                                                 \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _move)(array_t d, array_t s)				\
  {                                                                     \
    assert (d != s);                                                    \
    M_C(name, _clear)(d);						\
    M_C(name, _init_move)(d, s);					\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _set_at)(array_t v, size_t i, type x)			\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(v->size > 0 && v->ptr != NULL && i < v->size);               \
    M_GET_SET oplist(v->ptr[i], x);                                     \
  }                                                                     \
                                                                        \
  static inline type const *                                            \
  M_C(name, _back)(array_t v)						\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(v->size > 0 && v->ptr != NULL);                              \
    return M_CONST_CAST(type, &v->ptr[v->size-1]);                      \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_raw)(array_t v)					\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    if (M_UNLIKELY (v->size >= v->alloc)) {                             \
      assert(v->size == v->alloc);                                      \
      size_t alloc = M_GET_INC_ALLOC oplist (v->alloc);                 \
      if (M_UNLIKELY (alloc <= v->alloc)) {				\
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return NULL;							\
      }                                                                 \
      assert (alloc > v->size);                                         \
      type *ptr = M_GET_REALLOC oplist (type, v->ptr, alloc);           \
      if (M_UNLIKELY (ptr == NULL) ) {                                  \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return NULL;                                                    \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    assert(v->ptr != NULL);                                             \
    type *ret = &v->ptr[v->size];                                       \
    v->size++;                                                          \
    ARRAYI_CONTRACT(v);                                                 \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _push_back)(array_t v, type const x)			\
  {                                                                     \
    type *data = M_C(name, _push_raw)(v);				\
    if (M_UNLIKELY (data == NULL) )                                     \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                    \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_new)(array_t v)					\
  {                                                                     \
    type *data = M_C(name, _push_raw)(v);				\
    if (M_UNLIKELY (data == NULL) )                                     \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                           \
    return data;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _push_move)(array_t v, type *x)			        \
  {                                                                     \
    assert (x != NULL);                                                 \
    type *data = M_C(name, _push_raw)(v);				\
    if (M_UNLIKELY (data == NULL) )                                     \
      return;                                                           \
    M_DO_INIT_MOVE (oplist, *data, *x);                                 \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_at)(array_t v, size_t key, type const x)		\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (key <= v->size);                                            \
    if (M_UNLIKELY (v->size >= v->alloc) ) {                            \
      assert(v->size == v->alloc);                                      \
      size_t alloc = M_GET_INC_ALLOC oplist (v->alloc);                 \
      if (M_UNLIKELY (alloc <= v->alloc)) {				\
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return ;                                                        \
      }                                                                 \
      assert (alloc > v->size);                                         \
      type *ptr = M_GET_REALLOC oplist (type, v->ptr, alloc);           \
      if (M_UNLIKELY (ptr == NULL) ) {                                  \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    assert(v->ptr != NULL);                                             \
    memmove(&v->ptr[key+1], &v->ptr[key], (v->size-key)*sizeof(type));  \
    v->size++;                                                          \
    M_GET_INIT_SET oplist (v->ptr[key], x);                             \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _resize)(array_t v, size_t size)				\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    if (v->size > size) {                                               \
      /* Decrease size of array */                                      \
      for(size_t i = size ; i < v->size; i++)                           \
        M_GET_CLEAR oplist(v->ptr[i]);                                  \
      v->size = size;                                                   \
    } else if (v->size < size) {                                        \
      /* Increase size of array */                                      \
      if (size > v->alloc) {                                            \
        size_t alloc = size ;                                           \
        type *ptr = M_GET_REALLOC oplist (type, v->ptr, alloc);         \
        if (M_UNLIKELY (ptr == NULL) ) {                                \
          M_MEMORY_FULL(sizeof (type) * alloc);                         \
          return;                                                       \
        }                                                               \
        v->ptr = ptr;                                                   \
        v->alloc = alloc;                                               \
      }                                                                 \
      for(size_t i = v->size ; i < size; i++)                           \
        M_GET_INIT oplist(v->ptr[i]);                                   \
      v->size = size;                                                   \
    }                                                                   \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _reserve)(array_t v, size_t alloc)				\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    /* NOTE: Reserve below needed size, perform a shrink to fit */      \
    if (v->size > alloc) {                                              \
      alloc = v->size;                                                  \
    }                                                                   \
    if (M_UNLIKELY (alloc == 0)) {                                      \
      M_GET_FREE oplist (v->ptr);                                       \
      v->size = v->alloc = 0;                                           \
      v->ptr = NULL;                                                    \
    } else {                                                            \
      type *ptr = M_GET_REALLOC oplist (type, v->ptr, alloc);           \
      if (M_UNLIKELY (ptr == NULL) ) {                                  \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C(name, _get_at)(array_t v, size_t idx)                             \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    const size_t size = idx + 1;                                        \
    /* resize if needed */                                              \
    if (v->size <= size) {                                              \
      /* Increase size of array */                                      \
      if (M_UNLIKELY (size > v->alloc) ) {                              \
        size_t alloc = M_GET_INC_ALLOC oplist (size) ;                  \
        /* In case of overflow of size_t */                             \
	if (M_UNLIKELY (alloc <= v->alloc)) {				\
	  M_MEMORY_FULL(sizeof (type) * alloc);				\
	  return NULL;							\
	}								\
        type *ptr = M_GET_REALLOC oplist (type, v->ptr, alloc);         \
        if (M_UNLIKELY (ptr == NULL) ) {                                \
          M_MEMORY_FULL(sizeof (type) * alloc);                         \
          return NULL;                                                  \
        }                                                               \
        v->ptr = ptr;                                                   \
        v->alloc = alloc;                                               \
      }                                                                 \
      for(size_t i = v->size ; i < size; i++)                           \
        M_GET_INIT oplist(v->ptr[i]);                                   \
      v->size = size;                                                   \
    }                                                                   \
    assert (idx < v->size);                                             \
    ARRAYI_CONTRACT(v);                                                 \
    return &v->ptr[idx];                                                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _pop_back)(type *dest, array_t v)				\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (v->size > 0 && v->ptr != NULL);                             \
    v->size--;                                                          \
    if (dest) {                                                         \
      M_DO_MOVE (oplist, *dest, v->ptr[v->size]);                       \
    } else {                                                            \
      M_GET_CLEAR oplist (v->ptr[v->size]);                             \
    }                                                                   \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _pop_move)(type *dest, array_t v)				\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (v->size > 0 && v->ptr != NULL);                             \
    assert (dest != NULL);                                              \
    v->size--;                                                          \
    M_DO_INIT_MOVE (oplist, *dest, v->ptr[v->size]);                    \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _pop_until)(array_t v, array_it_t pos)                      \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (v == pos->array);                                           \
    M_ASSUME (pos->index <= v->size);                                   \
    M_C(name, _resize)(v, pos->index);                                  \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _empty_p)(const array_t v)					\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    return v->size == 0;                                                \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _size)(const array_t v)					\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    return v->size;                                                     \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _capacity)(const array_t v)					\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    return v->alloc;                                                    \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _pop_at)(type *dest, array_t v, size_t i)			\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (v->size > 0 && v->ptr != NULL);                             \
    if (dest)                                                           \
      M_DO_MOVE (oplist, *dest, v->ptr[i]);                             \
    else                                                                \
      M_GET_CLEAR oplist (v->ptr[i]);                                   \
    memmove(&v->ptr[i], &v->ptr[i+1], sizeof(type)*(v->size - i) );     \
    v->size--;                                                          \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _insert_v)(array_t v, size_t i, size_t num)			\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(i <= v->size);                                               \
    size_t size = v->size + num;                                        \
    /* Avoid overflow of v->size + num */                               \
    if (v->size > v->alloc-num) {                                       \
      size_t alloc = M_GET_INC_ALLOC oplist (size) ;                    \
      if (M_UNLIKELY (size <= v->size || alloc <= v->alloc)) {          \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return ;                                                        \
      }                                                                 \
      type *ptr = M_GET_REALLOC oplist (type, v->ptr, alloc);           \
      if (M_UNLIKELY (ptr == NULL) ) {                                  \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    memmove(&v->ptr[i+num], &v->ptr[i], sizeof(type)*(v->size - i) );   \
    for(size_t k = i ; k < i+num; k++)                                  \
      M_GET_INIT oplist(v->ptr[k]);                                     \
    v->size = size;                                                     \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _remove_v)(array_t v, size_t i, size_t j)			\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(i < v->size && j <= v->size && i < j && v->ptr != NULL);     \
    for(size_t k = i ; k < j; k++)                                      \
      M_GET_CLEAR oplist(v->ptr[k]);                                    \
    memmove(&v->ptr[i], &v->ptr[j], sizeof(type)*(v->size - j) );       \
    v->size -= (j-i);                                                   \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _swap)(array_t v1, array_t v2)                              \
  {                                                                     \
    ARRAYI_CONTRACT(v1);                                                \
    ARRAYI_CONTRACT(v2);                                                \
    size_t tmp = v1->size;                                              \
    v1->size = v2->size;                                                \
    v2->size = tmp;                                                     \
    tmp = v1->alloc;                                                    \
    v1->alloc = v2->alloc;                                              \
    v2->alloc = tmp;                                                    \
    type *p = v1->ptr;                                                  \
    v1->ptr = v2->ptr;                                                  \
    v2->ptr = p;                                                        \
    ARRAYI_CONTRACT(v1);                                                \
    ARRAYI_CONTRACT(v2);                                                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _swap_at)(array_t v, size_t i, size_t j)                    \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(i < v->size && j < v->size && v->ptr != NULL);               \
    type tmp;                                                           \
    M_IF_METHOD(INIT_MOVE, oplist) (                                    \
    M_GET_INIT_MOVE oplist (tmp, v->ptr[i]);                            \
    M_GET_INIT_MOVE oplist (v->ptr[i], v->ptr[j]);                      \
    M_GET_INIT_MOVE oplist (v->ptr[j], tmp);                            \
                                    ,                                   \
    M_GET_INIT_SET oplist (tmp, v->ptr[i]);                             \
    M_GET_SET oplist(v->ptr[i], v->ptr[j]);                             \
    M_GET_SET oplist(v->ptr[j], tmp);                                   \
    M_GET_CLEAR oplist(tmp);                                            \
    ) /* IF INIT_MOVE method */                                         \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C(name, _get)(const array_t v, size_t i)                            \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (i < v->size && v->ptr != NULL);                             \
    return &v->ptr[i];                                                  \
  }                                                                     \
                                                                        \
  static inline type const *                                            \
  M_C(name, _cget)(const array_t v, size_t i)                           \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (i < v->size && v->ptr != NULL);                             \
    return M_CONST_CAST(type, &v->ptr[i]);                              \
  }                                                                     \
                                                                        \
  static inline type const *                                            \
  M_C(name, _front)(const array_t v)                                    \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (v->size > 0);                                               \
    return M_C(name, _cget)(v, 0);                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it)(array_it_t it, const array_t v)			\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (it != NULL);                                                \
    it->index = 0;                                                      \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it_last)(array_it_t it, const array_t v)			\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (it != NULL);                                                \
    it->index = v->size - 1;                                            \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it_end)(array_it_t it, const array_t v)			\
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (it != NULL);                                                \
    it->index = v->size;                                                \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it_set)(array_it_t it, const array_it_t org)		\
  {                                                                     \
    assert (it != NULL && org != NULL);                                 \
    it->index = org->index;                                             \
    it->array = org->array;                                             \
    ARRAYI_CONTRACT(it->array);                                         \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _end_p)(const array_it_t it)				\
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    return it->index >= it->array->size;                                \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _last_p)(const array_it_t it)				\
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    /* NOTE: Can not compute 'size-1' due to potential overflow         \
       if size is 0 */                                                  \
    return it->index + 1 >= it->array->size;                            \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _it_equal_p)(const array_it_t it1,				\
			 const array_it_t it2)				\
  {                                                                     \
    assert(it1 != NULL && it2 != NULL);                                 \
    return it1->array == it2->array && it1->index == it2->index;        \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _next)(array_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    it->index ++;                                                       \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _previous)(array_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    /* NOTE: In the case index=0, it will be set to (unsigned) -1       \
       ==> it will be greater than size ==> end_p will return true */   \
    it->index --;                                                       \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _ref)(const array_it_t it)					\
  {                                                                     \
    assert(it != NULL);                                                 \
    return M_C(name, _get)(it->array, it->index);			\
  }                                                                     \
  									\
  static inline type const *                                            \
  M_C(name, _cref)(const array_it_t it)					\
  {                                                                     \
    assert(it != NULL);                                                 \
    return M_C(name, _cget)(it->array, it->index);			\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _insert)(array_t a, array_it_t it, type const x)		\
  {                                                                     \
    assert (it != NULL && a == it->array);                              \
    M_C(name, _push_at)(a, it->index + 1, x);				\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _remove)(array_t a, array_it_t it)				\
  {                                                                     \
    assert (it != NULL && a == it->array);                              \
    M_C(name, _pop_at)(NULL, a, it->index);				\
    /* NOTE: it->index will naturaly point to the next element */       \
  }                                                                     \
  									\
  M_IF_METHOD(CMP, oplist)                                              \
  (                                                                     \
   static inline void M_C(name, _special_sort)(array_t l,		\
	      int (*func_type) (type const *a, type const *b))	        \
  {                                                                     \
    /* Using qsort is more compact but slower than a full templated     \
       version which can be twice faster */                             \
    int (*func_void)(const void*, const void*);                         \
    /* There is no way (?) to avoid the cast */                         \
    func_void = (int (*)(const void*, const void*))func_type;           \
    qsort (l->ptr, l->size, sizeof(type), func_void);                   \
  }                                                                     \
                                                                        \
  M_IF_METHOD(SWAP, oplist)(                                            \
  static inline void                                                    \
  M_C(name, _special_stable_sort_noalloc) (type tab[], size_t size, type tmp[]) \
  {                                                                     \
    size_t th = 4;                                                      \
    M_IF_DEBUG(type *org_tab = tab;)                                    \
    assert (size > 1);                                                  \
    /* Let's select the threshold of the pass 1 to be sure              \
       the final result is in tab.*/                                    \
    if (m_core_clz64(size-1) & 1)					\
      th += th;                                                         \
                                                                        \
    /* Pass 1: insertion sort (stable) */                               \
    for(size_t k = 0 ; k < size; ) {                                    \
      size_t max = size - k < 2*th ? size - k : th;                     \
      M_ASSUME(max >= th);                                              \
      for(size_t i = 1; i < max; i++) {                                 \
        size_t j = i;                                                   \
        while (j > 0 && M_GET_CMP oplist (tab[k+j-1], tab[k+j]) > 0) {  \
          M_GET_SWAP oplist (tab[k+j-1], tab[k+j]);                     \
          j = j - 1;                                                    \
        }                                                               \
      }                                                                 \
      k += max;                                                         \
    }                                                                   \
                                                                        \
    /* N Pass of merge */                                               \
    while (th < size) {                                                 \
      type *dest = tmp;                                                 \
      /* Pass n: Merge */                                               \
      for(size_t k = 0 ; k < size; ) {                                  \
        type *el1 = &tab[k];                                            \
        type *el2 = &tab[k+th];                                         \
        size_t n1 = th;                                                 \
        size_t n2 = size-k <= 3*th ? size-k-th : th;                    \
        assert (size-k > th);                                           \
        assert (0 < n1 && n1 <= size);                                  \
        assert (0 < n2 && n2 <= size);                                  \
        k += n1+n2;                                                     \
        for (;;) {                                                      \
          if (M_GET_CMP oplist (*el1, *el2) <= 0) {                     \
            M_GET_SET oplist (*dest, *el1);                             \
            dest++;                                                     \
            el1++;                                                      \
            if (-- n1 == 0) {                                           \
              if (n2 > 0) {                                             \
                memcpy (dest, el2, n2 * sizeof (type));                 \
                dest += n2;                                             \
              }                                                         \
              break;                                                    \
            }                                                           \
          } else {                                                      \
            M_GET_SET oplist (*dest, *el2);                             \
            dest++;                                                     \
            el2++;                                                      \
            if (-- n2 == 0) {                                           \
              if (n1 > 0) {                                             \
                memcpy (dest, el1, n1 * sizeof (type));                 \
                dest += n1;                                             \
              }                                                         \
              break;                                                    \
            }                                                           \
          }                                                             \
        }                                                               \
      }                                                                 \
      /* Swap t & tab */                                                \
      M_SWAP(type *, tab, tmp);                                         \
      /* Increase th for next pass */                                   \
      th += th;                                                         \
    }                                                                   \
    assert (org_tab == tab);                                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _special_stable_sort)(array_t l)                            \
  {                                                                     \
    if (M_UNLIKELY (l->size < 2))                                       \
      return;                                                           \
    type *temp = M_GET_REALLOC oplist (type, NULL, l->size);            \
    if (temp == NULL) {                                                 \
      M_MEMORY_FULL(sizeof (type) * l->size);                           \
      return ;                                                          \
    }                                                                   \
    M_C(name, _special_stable_sort_noalloc)(l->ptr, l->size, temp);     \
    M_GET_FREE oplist(temp);                                            \
  }                                                                     \
  ,) /* IF SWAP method */                                               \
                                                                        \
  ,) /* IF CMP oplist */                                                \
  									\
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _get_str)(string_t str, array_t array,			\
		      bool append)					\
  {                                                                     \
    STRINGI_CONTRACT(str);                                              \
    ARRAYI_CONTRACT(array);                                             \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    array_it_t it;                                                      \
    for (M_C(name, _it)(it, array) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      type const *item = M_C(name, _cref)(it);				\
      M_GET_GET_STR oplist (str, *item, true);                          \
      if (!M_C(name, _last_p)(it))					\
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    string_push_back (str, ']');                                        \
    STRINGI_CONTRACT(str);                                              \
  }                                                                     \
  , /* no GET_STR */ )                                                  \
			      						\
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *file, const array_t array)			\
  {                                                                     \
    ARRAYI_CONTRACT(array);                                             \
    assert (file != NULL);                                              \
    fputc ('[', file);                                                  \
    for (size_t i = 0; i < array->size; i++) {                          \
      type const *item = M_C(name, _cget)(array, i);			\
      M_GET_OUT_STR oplist (file, *item);                               \
      if (i != array->size-1)                                           \
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fputc (']', file);                                                  \
  }                                                                     \
  , /* no OUT_STR */ )                                                  \
                                                                        \
  M_IF_METHOD(PARSE_STR, oplist)(                                       \
  static inline bool                                                    \
  M_C(name, _parse_str)(array_t array, const char str[], const char**endp) \
  {                                                                     \
    ARRAYI_CONTRACT(array);                                             \
    assert (str != NULL);                                               \
    M_C(name,_clean)(array);						\
    bool success = false;                                               \
    int c = *str++;                                                     \
    if (M_UNLIKELY (c != '[')) goto exit;                               \
    c = *str++;                                                         \
    if (M_UNLIKELY (c == ']')) { success = true; goto exit; }           \
    if (M_UNLIKELY (c == 0)) goto exit;                                 \
    str--;                                                              \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_PARSE_STR oplist (item, str, &str);                \
      do { c = *str++; } while (isspace(c));                            \
      if (b == false || c == 0) { goto exit; }                          \
      M_C(name, _push_back)(array, item);				\
    } while (c == M_GET_SEPARATOR oplist);				\
    M_GET_CLEAR oplist (item);                                          \
    ARRAYI_CONTRACT(array);                                             \
    success = (c == ']');                                               \
  exit:                                                                 \
    if (endp) *endp = str;                                              \
    return success;                                                     \
  }                                                                     \
  , /* no PARSE_STR */ )                                                \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C(name, _in_str)(array_t array, FILE *file)				\
  {                                                                     \
    ARRAYI_CONTRACT(array);                                             \
    assert (file != NULL);                                              \
    M_C(name,_clean)(array);						\
    int c = fgetc(file);						\
    if (M_UNLIKELY (c != '[')) return false;                            \
    c = fgetc(file);                                                    \
    if (M_UNLIKELY (c == ']')) return true;                             \
    if (M_UNLIKELY (c == EOF)) return false;                            \
    ungetc(c, file);                                                    \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_IN_STR oplist (item, file);                        \
      do { c = fgetc(file); } while (isspace(c));                       \
      if (b == false || c == EOF) { break; }				\
      M_C(name, _push_back)(array, item);				\
    } while (c == M_GET_SEPARATOR oplist);				\
    M_GET_CLEAR oplist (item);                                          \
    ARRAYI_CONTRACT(array);                                             \
    return c == ']';                                                    \
  }                                                                     \
  , /* no IN_STR */ )                                                   \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  M_C(name, _equal_p)(const array_t array1,				\
		      const array_t array2)				\
  {                                                                     \
    ARRAYI_CONTRACT(array1);                                            \
    ARRAYI_CONTRACT(array2);                                            \
    if (array1->size != array2->size) return false;                     \
    size_t i;                                                           \
    for(i = 0; i < array1->size; i++) {                                 \
      type const *item1 = M_C(name, _cget)(array1, i);			\
      type const *item2 = M_C(name, _cget)(array2, i);			\
      bool b = M_GET_EQUAL oplist (*item1, *item2);                     \
      if (!b) return false;                                             \
    }                                                                   \
    return i == array1->size;                                           \
  }                                                                     \
  , /* no EQUAL */ )                                                    \
                                                                        \
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t                                                  \
  M_C(name, _hash)(const array_t array)					\
  {                                                                     \
    ARRAYI_CONTRACT(array);                                             \
    M_HASH_DECL(hash);                                                  \
    for(size_t i = 0 ; i < array->size; i++) {                          \
      size_t hi = M_GET_HASH oplist (array->ptr[i]);                    \
      M_HASH_UP(hash, hi);                                              \
    }                                                                   \
    return M_HASH_FINAL (hash);						\
  }                                                                     \
  , /* no HASH */ )                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _splice)(array_t a1, array_t a2)  			        \
  {                                                                     \
    ARRAYI_CONTRACT(a1);                                                \
    ARRAYI_CONTRACT(a2);                                                \
    if (M_LIKELY (a2->size > 0)) {                                      \
      size_t newSize = a1->size + a2->size;                             \
      if (newSize > a1->alloc) {                                        \
        type *ptr = M_GET_REALLOC oplist (type, a1->ptr, newSize);      \
        if (M_UNLIKELY (ptr == NULL) ) {                                \
          M_MEMORY_FULL(sizeof (type) * newSize);                       \
        }                                                               \
        a1->ptr = ptr;                                                  \
        a1->alloc = newSize;                                            \
      }                                                                 \
      memcpy(&a1->ptr[a1->size], &a2->ptr[0], a2->size * sizeof (type)); \
      a2->size = 0;                                                     \
      a1->size = newSize;                                               \
    }                                                                   \
  }                                                                     \

#endif
