/*
 * M*LIB - dynamic ARRAY module
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
#ifndef __M_ARRAY_H
#define __M_ARRAY_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "m-core.h"

/* Define an array of a given type.
   USAGE: ARRAY_DEF(name, type [, oplist_of_the_type]) */
#define ARRAY_DEF(name, ...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ARRAYI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST, M_C3(array_, name,_t), M_C3(array_it_,name,_t) ), \
   ARRAYI_DEF2(name, __VA_ARGS__, M_C3(array_, name,_t), M_C3(array_it_,name,_t)))

/* Define the oplist of an array of type.
   USAGE: ARRAY_OPLIST(name[, oplist of the type]) */
#define ARRAY_OPLIST(...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                        \
  (ARRAYI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),                    \
   ARRAYI_OPLIST(__VA_ARGS__ ))


/********************************** INTERNAL ************************************/

#define ARRAYI_OPLIST(name, oplist)                                     \
  (INIT(M_C3(array_, name, _init))                                      \
   ,INIT_SET(M_C3(array_, name, _init_set))                             \
   ,SET(M_C3(array_, name, _set))                                       \
   ,CLEAR(M_C3(array_, name, _clear))                                   \
   ,INIT_MOVE(M_C3(array_, name, _init_move))                           \
   ,MOVE(M_C3(array_, name, _move))                                     \
   ,SWAP(M_C3(array_, name, _swap))                                     \
   ,TYPE(M_C3(array_,name,_t))                                          \
   ,SUBTYPE(M_C3(array_type_,name,_t))                                  \
   ,IT_TYPE(M_C3(array_it_,name,_t))                                    \
   ,IT_FIRST(M_C3(array_,name,_it))                                     \
   ,IT_LAST(M_C3(array_,name,_it_last))                                 \
   ,IT_END(M_C3(array_,name,_it_end))                                   \
   ,IT_SET(M_C3(array_,name,_it_set))                                   \
   ,IT_END_P(M_C3(array_,name,_end_p))                                  \
   ,IT_LAST_P(M_C3(array_,name,_last_p))                                \
   ,IT_EQUAL_P(M_C3(array_,name,_it_equal_p))                           \
   ,IT_NEXT(M_C3(array_,name,_next))                                    \
   ,IT_PREVIOUS(M_C3(array_,name,_previous))                            \
   ,IT_REF(M_C3(array_,name,_ref))                                      \
   ,IT_CREF(M_C3(array_,name,_cref))                                    \
   ,IT_REMOVE(M_C3(array_,name,_remove))                                \
   ,CLEAN(M_C3(array_,name,_clean))                                     \
   ,PUSH(M_C3(array_,name,_push_back))                                  \
   ,POP(M_C3(array_,name,_pop_back))                                    \
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(CMP, oplist)(SORT(M_C3(array_, name, _sort)),)          \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C3(array_, name, _get_str)),) \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C3(array_, name, _out_str)),) \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C3(array_, name, _in_str)),)   \
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C3(array_, type, _equal_p)),)    \
   ,M_IF_METHOD(HASH, oplist)(HASH(M_C3(array_, type, _hash)),)         \
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

#define ARRAYI_CONTRACT(a) do {                 \
    assert (a != NULL);                         \
    assert (a->size <= a->alloc);               \
    assert (a->size == 0 || a->ptr != NULL);    \
  } while (0)

// Compute alloc from the requested size.
// NOTE: EXPECTED_SIZE can be overloaded by the given oplist.
#define ARRAYI_INC_ALLOC_SIZE(oplist, n)        \
  (M_MAX(M_GET_EXPECTED_SIZE oplist, (n))*2)

// Internal definition.
#define ARRAYI_DEF2(name, type, oplist, array_t, array_it_t)            \
  typedef struct M_C3(array_, name, _s) {                               \
    size_t size, alloc;                                                 \
    type *ptr;                                                          \
  } array_t[1];                                                         \
                                                                        \
  typedef type M_C3(array_type_,name,_t);                               \
                                                                        \
  typedef struct M_C3(array_it_,name,_s) {                              \
    size_t index;                                                       \
    struct M_C3(array_, name, _s) *array;                               \
  } array_it_t[1];                                                      \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _init)(array_t v)                                  \
  {                                                                     \
    assert (v != NULL);                                                 \
    v->size = v->alloc = 0;                                             \
    v->ptr = NULL;                                                      \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _clean)(array_t v)                                 \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    for(size_t i = 0; i < v->size; i++)                                 \
      M_GET_CLEAR oplist(v->ptr[i]);                                    \
    v->size = 0;                                                        \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _clear)(array_t v)                                 \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    M_C3(array_, name, _clean)(v);                                      \
    M_GET_FREE oplist(v->ptr);                                          \
    v->alloc = 0;                                                       \
    v->ptr = NULL;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _set)(array_t d, const array_t s)                  \
  {                                                                     \
    ARRAYI_CONTRACT(d);                                                 \
    ARRAYI_CONTRACT(s);                                                 \
    if (M_UNLIKELY (d == s)) return;                                    \
    if (s->size > d->alloc) {                                           \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE(oplist, s->size);            \
      type *ptr = M_GET_REALLOC oplist (type, d->ptr, alloc);           \
      if (ptr == NULL) {                                                \
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
  M_C3(array_, name, _init_set)(array_t d, const array_t s)             \
  {                                                                     \
    assert (d != s);                                                    \
    M_C3(array_, name, _init)(d);                                       \
    M_C3(array_, name, _set)(d, s);                                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _init_move)(array_t d, array_t s)                  \
  {                                                                     \
    assert (d != s);                                                    \
    ARRAYI_CONTRACT(s);                                                 \
    d->size  = s->size;                                                 \
    d->alloc = s->alloc;                                                \
    d->ptr   = s->ptr;                                                  \
    s->size  = s->alloc = 0;                                            \
    s->ptr   = NULL;                                                    \
    ARRAYI_CONTRACT(d);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _move)(array_t d, array_t s)                       \
  {                                                                     \
    assert (d != s);                                                    \
    M_C3(array_, name, _clear)(d);                                      \
    M_C3(array_, name, _init_move)(d, s);                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _set_at)(array_t v, size_t i, type x)              \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(v->size > 0 && v->ptr != NULL);                              \
    M_GET_SET oplist(v->ptr[i], x);                                     \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(array_, name, _back)(array_t v)                                  \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(v->size > 0 && v->ptr != NULL);                              \
    return M_CONST_CAST(type, &v->ptr[v->size-1]);                      \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(array_, name, _push_raw)(array_t v)                              \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    if (M_UNLIKELY (v->size >= v->alloc)) {                             \
      assert(v->size == v->alloc);                                      \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE(oplist, v->alloc);           \
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
  M_C3(array_, name, _push_back)(array_t v, type const x)               \
  {                                                                     \
    type *data = M_C3(array_, name, _push_raw)(v);                      \
    if (M_UNLIKELY (data == NULL) )                                     \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                    \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(array_, name, _push_new)(array_t v)                              \
  {                                                                     \
    type *data = M_C3(array_, name, _push_raw)(v);                      \
    if (M_UNLIKELY (data == NULL) )                                     \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                           \
    return data;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _push_at)(array_t v, size_t key, type const x)     \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (key <= v->size);                                            \
    if (M_UNLIKELY (v->size >= v->alloc) ) {                            \
      assert(v->size == v->alloc);                                      \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE(oplist, v->alloc);           \
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
  M_C3(array_, name, _resize)(array_t v, size_t size)                   \
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
  M_C3(array_, name, _reserve)(array_t v, size_t alloc)                 \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    /* NOTE: Reserve below needed size, perform a shrink to fit */      \
    if (v->size > alloc) {                                              \
      alloc = v->size;                                                  \
    }                                                                   \
    type *ptr = M_GET_REALLOC oplist (type, v->ptr, alloc);             \
    if (M_UNLIKELY (ptr == NULL) ) {                                    \
      M_MEMORY_FULL(sizeof (type) * alloc);                             \
      return;                                                           \
    }                                                                   \
    v->ptr = ptr;                                                       \
    v->alloc = alloc;                                                   \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _set_at2)(array_t v, size_t idx, type x)           \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    const size_t size = idx + 1;                                        \
    /* NOTE: set at and resize if needed */                             \
    if (v->size <= size) {                                              \
      /* Increase size of array */                                      \
      if (M_UNLIKELY (size > v->alloc) ) {                              \
        size_t alloc = ARRAYI_INC_ALLOC_SIZE(oplist, size) ;            \
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
    assert (idx < v->size);                                             \
    M_GET_SET oplist(v->ptr[idx], x);                                   \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _pop_back)(type *dest, array_t v)                  \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    v->size--;                                                          \
    if (dest)                                                           \
      M_IF_METHOD(MOVE, oplist) (                                       \
      M_GET_MOVE oplist(*dest, v->ptr[v->size]); else                   \
      ,                                                                 \
      M_GET_SET oplist(*dest, v->ptr[v->size]);                         \
      )                                                                 \
    M_GET_CLEAR oplist (v->ptr[v->size]);                               \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(array_, name, _empty_p)(const array_t v)                         \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    return v->size == 0;                                                \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C3(array_, name, _size)(const array_t v)                            \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    return v->size;                                                     \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C3(array_, name, _capacity)(const array_t v)                        \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    return v->alloc;                                                    \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _pop_at)(type *dest, array_t v, size_t i)          \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (v->size > 0 && v->ptr != NULL);                             \
    if (dest)                                                           \
      M_IF_METHOD(MOVE, oplist) (                                       \
      M_GET_MOVE oplist(*dest, v->ptr[i]); else                         \
      ,                                                                 \
      M_GET_SET oplist (*dest, v->ptr[i]);                              \
      )                                                                 \
    M_GET_CLEAR oplist (v->ptr[i]);                                     \
    memmove(&v->ptr[i], &v->ptr[i+1], sizeof(type)*(v->size - i) );     \
    v->size--;                                                          \
    ARRAYI_CONTRACT(v);                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _insert_v)(array_t v, size_t i, size_t num)        \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert(i <= v->size);                                               \
    size_t size = v->size + num;                                        \
    if (size > v->alloc) {                                              \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE(oplist, size) ;              \
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
  M_C3(array_, name, _remove_v)(array_t v, size_t i, size_t j)          \
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
  M_C3(array_, name, _swap)(array_t v1, array_t v2)                     \
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
  M_C3(array_, name, _swap_at)(array_t v, size_t i, size_t j)           \
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
  M_C3(array_, name, _get)(array_t v, size_t i)                         \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (i < v->size && v->ptr != NULL);                             \
    return &v->ptr[i];                                                  \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(array_, name, _cget)(const array_t v, size_t i)                  \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (i < v->size && v->ptr != NULL);                             \
    return M_CONST_CAST(type, &v->ptr[i]);                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _it)(array_it_t it, array_t v)                     \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (it != NULL);                                                \
    it->index = 0;                                                      \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _it_last)(array_it_t it, array_t v)                \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (it != NULL);                                                \
    it->index = v->size - 1;                                            \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _it_end)(array_it_t it, array_t v)                 \
  {                                                                     \
    ARRAYI_CONTRACT(v);                                                 \
    assert (it != NULL);                                                \
    it->index = v->size;                                                \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _it_set)(array_it_t it, const array_it_t org)      \
  {                                                                     \
    assert (it != NULL && org != NULL);                                 \
    it->index = org->index;                                             \
    it->array = org->array;                                             \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(array_, name, _end_p)(const array_it_t it)                       \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    return (it->index) >= it->array->size;                              \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(array_, name, _last_p)(const array_it_t it)                      \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    /* NOTE: Can not compute 'size-1' due to potential overflow         \
       if size was 0 */                                                 \
    return it->index + 1 >= it->array->size;                            \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(array_, name, _it_equal_p)(const array_it_t it1,                 \
                                  const array_it_t it2)                 \
  {                                                                     \
    assert(it1 != NULL && it2 != NULL);                                 \
    return it1->array == it2->array && it1->index == it2->index;        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _next)(array_it_t it)                              \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    it->index ++;                                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _previous)(array_it_t it)                          \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    /* NOTE: In the case index=0, it will be set to (unsigned) -1       \
       ==> it will be greater than size ==> end_p will return true */   \
    it->index --;                                                       \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(array_, name, _ref)(const array_it_t it)                         \
  {                                                                     \
    assert(it != NULL);                                                 \
    return M_C3(array_, name, _get)(it->array, it->index);              \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(array_, name, _cref)(const array_it_t it)                        \
  {                                                                     \
    assert(it != NULL);                                                 \
    return M_C3(array_, name, _cget)(it->array, it->index);             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_, name, _insert)(array_it_t it, type const x)              \
  {                                                                     \
    assert(it != NULL);                                                 \
    M_C3(array_, name, _push_at)(it->array, it->index, x);              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(array_,name, _remove)(array_t a, array_it_t it)                  \
  {                                                                     \
    assert (a == it->array);                                            \
    M_C3(array_, name, _pop_at)(NULL, a, it->index);                    \
    /* NOTE: it->index will naturaly point to the next element */       \
  }                                                                     \
                                                                        \
  M_IF_METHOD(CMP, oplist)                                              \
  (                                                                     \
   static inline void M_C3(array_, name, _sort)(array_t l,              \
                        int (*func_type) (const type *a, const type *b))\
  {                                                                     \
    /* Using qsort is more compact but slower than a full templated     \
       version which can be twice faster */                             \
    int (*func_void)(const void*, const void*);                         \
    /* There is no way (?) to avoid the cast */                         \
    func_void = (int (*)(const void*, const void*))func_type;           \
    qsort (l->ptr, l->size, sizeof(type), func_void);                   \
  }                                                                     \
  ,) /* IF CMP oplist */                                                \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C3(array_, name, _get_str)(string_t str, array_t array,             \
                               bool append)                             \
  {                                                                     \
    STRING_CONTRACT(str);                                               \
    ARRAYI_CONTRACT(array);                                             \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    array_it_t it;                                                      \
    for (M_C3(array_, name, _it)(it, array) ;                           \
         !M_C3(array_, name, _end_p)(it);                               \
         M_C3(array_, name, _next)(it)){                                \
      const type *item = M_C3(array_, name, _cref)(it);                 \
      M_GET_GET_STR oplist (str, *item, true);                          \
      if (!M_C3(array_, name, _last_p)(it))                             \
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    string_push_back (str, ']');                                        \
    STRING_CONTRACT(str);                                               \
  }                                                                     \
  , /* no GET_STR */ )                                                  \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C3(array_, name, _out_str)(FILE *file, const array_t array)         \
  {                                                                     \
    ARRAYI_CONTRACT(array);                                             \
    assert (file != NULL);                                              \
    fprintf (file, "[");                                                \
    for (size_t i = 0; i < array->size; i++) {                          \
      const type *item = M_C3(array_, name, _cget)(array, i);           \
      M_GET_OUT_STR oplist (file, *item);                               \
      if (i != array->size-1)                                           \
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fprintf (file, "]");                                                \
  }                                                                     \
  , /* no OUT_STR */ )                                                  \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C3(array_, name, _in_str)(array_t array, FILE *file)                \
  {                                                                     \
    ARRAYI_CONTRACT(array);                                             \
    assert (file != NULL);                                              \
    M_C3(array_, name,_clean)(array);                                   \
    char c = fgetc(file);                                               \
    if (c != '[') return false;                                         \
    c = fgetc(file);                                                    \
    if (c == ']') return true;                                          \
    ungetc(c, file);                                                    \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_IN_STR oplist (item, file);                        \
      c = fgetc(file);                                                  \
      if (!b) { break; }                                                \
      M_C3(array_, name, _push_back)(array, item);                      \
    } while (c == M_GET_SEPARATOR oplist && !feof(file) && !ferror(file)); \
    M_GET_CLEAR oplist (item);                                          \
    ARRAYI_CONTRACT(array);                                             \
    return c == ']';                                                    \
  }                                                                     \
  , /* no IN_STR */ )                                                   \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  M_C3(array_, name, _equal_p)(const array_t array1,                    \
                               const array_t array2)                    \
  {                                                                     \
    ARRAYI_CONTRACT(array1);                                            \
    ARRAYI_CONTRACT(array2);                                            \
    if (array1->size != array2->size) return false;                     \
    size_t i;                                                           \
    for(i = 0; i < array1->size; i++) {                                 \
      const type *item1 = M_C3(array_, name, _cget)(array1, i);         \
      const type *item2 = M_C3(array_, name, _cget)(array2, i);         \
      bool b = M_GET_EQUAL oplist (*item1, *item2);                     \
      if (!b) return false;                                             \
    }                                                                   \
    return i == array1->size;                                           \
  }                                                                     \
  , /* no EQUAL */ )                                                    \
                                                                        \
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t                                                  \
  M_C3(array_, name, _hash)(const array_t array)                        \
  {                                                                     \
    ARRAYI_CONTRACT(array);                                             \
    M_HASH_DECL(hash);                                                  \
    for(size_t i = 0 ; i < array->size; i++) {                          \
      size_t hi = M_GET_HASH oplist (array->ptr[i]);                    \
      M_HASH_UP(hash, hi);                                              \
    }                                                                   \
    return hash;                                                        \
  }                                                                     \
  , /* no HASH */ )                                                     \

#endif
