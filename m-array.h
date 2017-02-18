/*
 * MLIB - ARRAY module
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
#define ARRAY_DEF(name, ...)                                     \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                    \
  (ARRAYI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST ),            \
   ARRAYI_DEF2(name, __VA_ARGS__ ))

/* Define the oplist of an array of type.
   USAGE: ARRAY_OPLIST(name[, oplist of the type]) */
#define ARRAY_OPLIST(...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                        \
  (ARRAYI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),                    \
   ARRAYI_OPLIST(__VA_ARGS__ ))


/********************************** INTERNAL ************************************/

#define ARRAYI_OPLIST(name, oplist)                                     \
  (INIT(CAT3(array_, name, _init))                                      \
   ,INIT_SET(CAT3(array_, name, _init_set))                             \
   ,SET(CAT3(array_, name, _set))                                       \
   ,CLEAR(CAT3(array_, name, _clear))                                   \
   ,INIT_MOVE(CAT3(array_, name, _init_move))                           \
   ,MOVE(CAT3(array_, name, _move))                                     \
   ,TYPE(CAT3(array_,name,_t))                                          \
   ,SUBTYPE(CAT3(array_type_,name,_t))                                  \
   ,IT_TYPE(CAT3(array_it_,name,_t))                                    \
   ,IT_FIRST(CAT3(array_,name,_it))                                     \
   ,IT_SET(CAT3(array_,name,_it_set))                                   \
   ,IT_END_P(CAT3(array_,name,_end_p))                                  \
   ,IT_LAST_P(CAT3(array_,name,_last_p))                                \
   ,IT_EQUAL_P(CAT3(array_,name,_it_equal_p))                           \
   ,IT_NEXT(CAT3(array_,name,_next))                                    \
   ,IT_PREVIOUS(CAT3(array_,name,_previous))                            \
   ,IT_REF(CAT3(array_,name,_ref))                                      \
   ,IT_CREF(CAT3(array_,name,_cref))                                    \
   ,CLEAN(CAT3(array_,name,_clean))                                     \
   ,PUSH(CAT3(array_,name,_push_back))                                  \
   ,POP(CAT3(array_,name,_pop_back))                                    \
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(CMP, oplist)(SORT(CAT3(array_, name, _sort)),)          \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(CAT3(array_, name, _get_str)),) \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(CAT3(array_, name, _out_str)),) \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(CAT3(array_, name, _in_str)),)   \
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(CAT3(array_, type, _equal_p)),)    \
   )

// Compute alloc from size.
#define ARRAYI_INC_ALLOC_SIZE(n) ((n) < 8 ? 16 : (n) * 2)

// Internal
#define ARRAYI_DEF2(name, type, oplist)                                 \
  typedef struct CAT3(array_, name, _s) {                               \
    size_t size, alloc;                                                 \
    type *ptr;                                                          \
  } CAT3(array_, name,_t)[1];                                           \
                                                                        \
  typedef union {                                                       \
    type *ptr;                                                          \
    const type *cptr;                                                   \
  } CAT3(array_union_, name,_t);                                        \
                                                                        \
  typedef type CAT3(array_type_,name,_t);                               \
                                                                        \
  typedef struct CAT3(array_it_,name,_s) {                              \
    size_t index;                                                       \
    struct CAT3(array_, name, _s) *array;                               \
  } CAT3(array_it_,name,_t)[1];                                         \
                                                                        \
  static inline const type *                                            \
  CAT3(array_, name, _const_cast)(type *ptr)                            \
  {                                                                     \
    CAT3(array_union_, name,_t) u;                                      \
    u.ptr = ptr;                                                        \
    return u.cptr;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _init)(CAT3(array_, name,_t) v)                    \
  {                                                                     \
    assert (v != NULL);                                                 \
    v->size = v->alloc = 0;                                             \
    v->ptr = NULL;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _clean)(CAT3(array_, name,_t) v)                   \
  {                                                                     \
    assert (v != NULL && v->size <= v->alloc);                          \
    for(size_t i = 0; i < v->size; i++)                                 \
      M_GET_CLEAR oplist(v->ptr[i]);                                      \
    v->size = 0;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _clear)(CAT3(array_, name,_t) v)                   \
  {                                                                     \
    assert (v != NULL && v->size <= v->alloc);                          \
    CAT3(array_, name, _clean)(v);                                      \
    M_MEMORY_FREE(v->ptr);                                              \
    v->alloc = 0;                                                       \
    v->ptr = NULL;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _set)(CAT3(array_, name,_t) d,                     \
                           const CAT3(array_, name,_t) s)               \
  {                                                                     \
    assert (d != NULL && s != NULL                                      \
            && d->size <= d->alloc &&  s->size <= s->alloc);            \
    if (d == s) return;                                                 \
    if (s->size > d->alloc) {                                           \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE(s->size);                    \
      type *ptr = M_MEMORY_REALLOC (type, d->ptr, alloc);               \
      if (ptr == NULL) {                                                \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return ;                                                        \
      }                                                                 \
      d->ptr = ptr;                                                     \
      d->alloc = alloc;                                                 \
    }                                                                   \
    size_t i, step1 = M_MIN(s->size, d->size);                          \
    for(i = 0; i < step1; i++)                                          \
      M_GET_SET oplist (d->ptr[i], s->ptr[i]);                            \
    for( ; i < s->size; i++)                                            \
      M_GET_INIT_SET oplist (d->ptr[i], s->ptr[i]);                       \
    for( ; i < d->size; i++)                                            \
      M_GET_CLEAR oplist (d->ptr[i]);                                     \
    d->size = s->size;                                                  \
    assert (d->size <= d->alloc);                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _init_set)(CAT3(array_, name,_t) d,                \
                                const CAT3(array_, name,_t) s)          \
  {                                                                     \
    assert (d != s);                                                    \
    CAT3(array_, name, _init)(d);                                       \
    CAT3(array_, name, _set)(d, s);                                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _init_move)(CAT3(array_, name,_t) d,               \
                                 CAT3(array_, name,_t) s)               \
  {                                                                     \
    assert (d != s);                                                    \
    d->size  = s->size;                                                 \
    d->alloc = s->alloc;                                                \
    d->ptr   = s->ptr;                                                  \
    s->size = s->alloc = 0;                                             \
    s->ptr = NULL;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _move)(CAT3(array_, name,_t) d,                    \
                            CAT3(array_, name,_t) s)                    \
  {                                                                     \
    assert (d != s);                                                    \
    CAT3(array_, name, _clear)(d);                                      \
    CAT3(array_, name, _init_move)(d, s);                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _set_at)(CAT3(array_, name,_t) v, size_t i, type x) \
  {                                                                     \
    assert(v != NULL && i < v->size && v->ptr != NULL);                 \
    M_GET_SET oplist(v->ptr[i], x);                                       \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  CAT3(array_, name, _back)(CAT3(array_, name,_t) v)                    \
  {                                                                     \
    assert (v != NULL && v->ptr != NULL);                               \
    return CAT3(array_, name, _const_cast)(&v->ptr[v->size-1]);         \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(array_, name, _push_raw)(CAT3(array_, name,_t) v)                \
  {                                                                     \
    assert (v != NULL && v->size <= v->alloc);                          \
    if (v->size >= v->alloc) {                                          \
      assert(v->size == v->alloc);                                      \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE(v->alloc);                   \
      assert (alloc > v->size);                                         \
      type *ptr = M_MEMORY_REALLOC (type, v->ptr, alloc);               \
      if (ptr == NULL) {                                                \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return NULL;                                                    \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    assert(v->ptr != NULL);                                             \
    type *ret = &v->ptr[v->size];                                       \
    v->size++;                                                          \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _push_back)(CAT3(array_, name,_t) v, type const x) \
  {                                                                     \
    assert (v != NULL);                                                 \
    type *data = CAT3(array_, name, _push_raw)(v);                      \
    if (data == NULL)                                                   \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                      \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(array_, name, _push_new)(CAT3(array_, name,_t) v)                \
  {                                                                     \
    assert (v != NULL);                                                 \
    type *data = CAT3(array_, name, _push_raw)(v);                      \
    if (data == NULL)                                                   \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                             \
    return data;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _push_at)(CAT3(array_, name,_t) v, size_t key, type const x) \
  {                                                                     \
    assert (v != NULL && key <= v->size && v->size <= v->alloc);        \
    if (v->size >= v->alloc) {                                          \
      assert(v->size == v->alloc);                                      \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE(v->alloc);                   \
      assert (alloc > v->size);                                         \
      type *ptr = M_MEMORY_REALLOC (type, v->ptr, alloc);               \
      if (ptr == NULL) {                                                \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    assert(v->ptr != NULL);                                             \
    memmove(&v->ptr[key+1], &v->ptr[key], (v->size-key)*sizeof(type));  \
    v->size++;                                                          \
    M_GET_INIT_SET oplist (v->ptr[key], x);                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _resize)(CAT3(array_, name, _t) v, size_t size)    \
  {                                                                     \
    assert (v != NULL && v->size <= v->alloc);                          \
    if (v->size > size) {                                               \
      /* Decrease size of array */                                      \
      for(size_t i = size ; i < v->size; i++)                           \
        M_GET_CLEAR oplist(v->ptr[i]);                                    \
      v->size = size;                                                   \
    } else if (v->size < size) {                                        \
      /* Increase size of array */                                      \
      if (size > v->alloc) {                                            \
        size_t alloc = size ;                                           \
        type *ptr = M_MEMORY_REALLOC (type, v->ptr, alloc);             \
        if (ptr == NULL) {                                              \
          M_MEMORY_FULL(sizeof (type) * alloc);                         \
          return;                                                       \
        }                                                               \
        v->ptr = ptr;                                                   \
        v->alloc = alloc;                                               \
      }                                                                 \
      for(size_t i = v->size ; i < size; i++)                           \
        M_GET_INIT oplist(v->ptr[i]);                                     \
      v->size = size;                                                   \
    }                                                                   \
    assert (v != NULL && v->size <= v->alloc);                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _set_at2)(CAT3(array_, name,_t) v, size_t idx, type x) \
  {                                                                     \
    assert (v != NULL && v->size <= v->alloc);                          \
    size_t size = idx + 1;                                              \
    if (v->size <= size) {                                              \
      /* Increase size of array */                                      \
      if (size > v->alloc) {                                            \
        size_t alloc = ARRAYI_INC_ALLOC_SIZE (size) ;                   \
        type *ptr = M_MEMORY_REALLOC (type, v->ptr, alloc);             \
        if (ptr == NULL) {                                              \
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
    M_GET_SET oplist(v->ptr[idx], x);                                   \
    assert (v != NULL && v->size <= v->alloc);                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _pop_back)(type *dest, CAT3(array_, name,_t) v)    \
  {                                                                     \
    assert (v != NULL && v->size > 0 && v->ptr != NULL);                \
    v->size--;                                                          \
    if (dest)                                                           \
      M_IF_METHOD(MOVE, oplist) (                                       \
      M_GET_MOVE oplist(*dest, v->ptr[v->size]); else                   \
      ,                                                                 \
      M_GET_SET oplist(*dest, v->ptr[v->size]);                         \
      )                                                                 \
    M_GET_CLEAR oplist (v->ptr[v->size]);                               \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(array_, name, _empty_p)(const CAT3(array_, name,_t) v)           \
  {                                                                     \
    assert (v != NULL);                                                 \
    return v->size == 0;                                                \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  CAT3(array_, name, _size)(const CAT3(array_, name,_t) v)              \
  {                                                                     \
    assert (v != NULL);                                                 \
    return v->size;                                                     \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  CAT3(array_, name, _capacity)(const CAT3(array_, name,_t) v)          \
  {                                                                     \
    assert (v != NULL);                                                 \
    return v->alloc;                                                    \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _pop_at)(type *dest, CAT3(array_, name,_t) v, size_t i) \
  {                                                                     \
    assert(v != NULL && v->size > 0 && i < v->size && v->ptr != NULL);  \
    if (dest)                                                           \
      M_IF_METHOD(MOVE, oplist) (                                       \
      M_GET_MOVE oplist(*dest, v->ptr[i]); else                         \
      ,                                                                 \
      M_GET_SET oplist (*dest, v->ptr[i]);                              \
      )                                                                 \
    M_GET_CLEAR oplist (v->ptr[i]);                                     \
    memmove(&v->ptr[i], &v->ptr[i+1], sizeof(type)*(v->size - i) );     \
    v->size--;                                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _insert_v)(CAT3(array_, name,_t) v, size_t i, size_t j) \
  {                                                                     \
    assert (v != NULL);                                                 \
    assert(i < v->size && j <= v->size && i < j);                       \
    size_t size = v->size + (j-i);                                      \
    if (size > v->alloc) {                                              \
      size_t alloc = ARRAYI_INC_ALLOC_SIZE (size) ;                     \
      type *ptr = M_MEMORY_REALLOC (type, v->ptr, alloc);               \
      if (ptr == NULL) {                                                \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    memmove(&v->ptr[j], &v->ptr[i], sizeof(type)*(v->size - j) );       \
    for(size_t k = i ; k < j; k++)                                      \
      M_GET_INIT oplist(v->ptr[i]);                                     \
    v->size = size;                                                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _remove_v)(CAT3(array_, name,_t) v, size_t i, size_t j) \
  {                                                                     \
    assert (v != NULL);                                                 \
    assert(i < v->size && j <= v->size && i < j && v->ptr != NULL);     \
    for(size_t k = i ; k < j; k++)                                      \
      M_GET_CLEAR oplist(v->ptr[k]);                                    \
    memmove(&v->ptr[i], &v->ptr[j], sizeof(type)*(v->size - j) );       \
    v->size -= (j-i);                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_,name, _remove)(CAT3(array_, name,_t) v, size_t i)         \
  {                                                                     \
    CAT3(array_, name, _pop_at)(NULL, v, i);                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _swap)(CAT3(array_, name,_t) v, size_t i, size_t j) \
  {                                                                     \
    assert (v != NULL);                                                 \
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
    }                                                                   \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _shrink_to_fit)(CAT3(array_, name,_t) v)           \
  {                                                                     \
    assert (v != NULL && v->size <= v->alloc);                          \
    v->ptr = M_MEMORY_REALLOC (type, v->ptr, v->size);                  \
    assert(v->ptr != NULL || v->size == 0);                             \
    v->alloc = v->size;                                                 \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(array_, name, _get)(CAT3(array_, name,_t) v, size_t i)           \
  {                                                                     \
    assert (v != NULL && i < v->size && v->ptr != NULL);                \
    return &v->ptr[i];                                                  \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  CAT3(array_, name, _cget)(const CAT3(array_, name,_t) v, size_t i)    \
  {                                                                     \
    assert(v != NULL && i < v->size && v->ptr != NULL);                 \
    return CAT3(array_, name, _const_cast)(&v->ptr[i]);                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _it)(CAT3(array_it_,name,_t) it,                   \
                          CAT3(array_, name, _t) v)                     \
  {                                                                     \
    assert (it != NULL && v != NULL);                                   \
    it->index = 0;                                                      \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _it_set)(CAT3(array_it_,name,_t) it,               \
                              const CAT3(array_it_, name, _t) org)      \
  {                                                                     \
    assert (it != NULL && org != NULL);                                 \
    it->index = org->index;                                             \
    it->array = org->array;                                             \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(array_, name, _end_p)(const CAT3(array_it_, name, _t) it)        \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    return (it->index) >= it->array->size;                              \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(array_, name, _last_p)(const CAT3(array_it_, name, _t) it)       \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    /* Case i=0 & s=0 ==> true since unsigned type */                   \
    return (it->index) >= it->array->size-1;                            \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(array_, name, _it_equal_p)(const CAT3(array_it_, name,_t) it1,   \
                                  const CAT3(array_it_, name,_t) it2)   \
  {                                                                     \
    assert(it1 != NULL && it2 != NULL);                                 \
    return it1->array == it2->array && it1->index == it2->index;        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _next)(CAT3(array_it_,name,_t) it)                 \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    it->index ++;                                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _previous)(CAT3(array_it_,name,_t) it)             \
  {                                                                     \
    assert(it != NULL && it->array != NULL);                            \
    /* In the case index=0, it will be set to (unsigned) -1             \
       ==> it will be greater than size ==> end_p will return true */   \
    it->index --;                                                       \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(array_, name, _ref)(CAT3(array_it_, name,_t) it)                 \
  {                                                                     \
    assert(it != NULL);                                                 \
    return CAT3(array_, name, _get)(it->array, it->index);              \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  CAT3(array_, name, _cref)(const CAT3(array_it_, name,_t) it)          \
  {                                                                     \
    assert(it != NULL);                                                 \
    return CAT3(array_, name, _cget)(it->array, it->index);             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(array_, name, _insert)(CAT3(array_it_, name,_t) it, type const x) \
  {                                                                     \
    assert(it != NULL);                                                 \
    CAT3(array_, name, _push_at)(it->array, it->index, x);              \
  }                                                                     \
                                                                        \
  M_IF_METHOD(CMP, oplist)                                              \
  (                                                                     \
  static inline void CAT3(array_, name, _sort)(CAT3(array_, name,_t) l, \
                        int (*func_type) (const type *a, const type *b))\
  {                                                                     \
    /* Using qsort is more compact but slower than a full templated     \
       version which can be twice faster */                             \
    int (*func_void)(const void*, const void*);                         \
    /* There is no way to avoid the cast */                             \
    func_void = (int (*)(const void*, const void*))func_type;           \
    qsort (l->ptr, l->size, sizeof(type), func_void);                   \
  }                                                                     \
  ,) /* IF CMP oplist */                                                \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  CAT3(array_, name, _get_str)(string_t str, CAT3(array_, name,_t) array, \
                               bool append)                             \
  {                                                                     \
    assert (str != NULL && array != NULL);                              \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    CAT3(array_it_, name, _t) it;                                       \
    for (CAT3(array_, name, _it)(it, array) ;                           \
         !CAT3(array_, name, _end_p)(it);                               \
         CAT3(array_, name, _next)(it)){                                \
      const type *item = CAT3(array_, name, _cref)(it);                 \
      M_GET_GET_STR oplist (str, *item, true);                          \
      if (!CAT3(array_, name, _last_p)(it))                             \
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    string_push_back (str, ']');                                        \
  }                                                                     \
  , /* no str */ )                                                      \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  CAT3(array_, name, _out_str)(FILE *file, CAT3(array_, name,_t) array) \
  {                                                                     \
    assert (file != NULL && array != NULL);                             \
    fprintf (file, "[");                                                \
    CAT3(array_it_, name, _t) it;                                       \
    for (CAT3(array_, name, _it)(it, array) ;                           \
         !CAT3(array_, name, _end_p)(it);                               \
         CAT3(array_, name, _next)(it)){                                \
      const type *item = CAT3(array_, name, _cref)(it);                 \
      M_GET_OUT_STR oplist (file, *item);                               \
      if (!CAT3(array_, name, _last_p)(it))                             \
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fprintf (file, "]");                                                \
  }                                                                     \
  , /* no out_str */ )                                                  \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  CAT3(array_, name, _in_str)(CAT3(array_, name,_t) array, FILE *file)  \
  {                                                                     \
    assert (file != NULL && array != NULL);                             \
    CAT3(array_, name,_clean)(array);                                   \
    char c = fgetc(file);                                               \
    if (c != '[') return false;                                         \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_IN_STR oplist (item, file);                        \
      if (!b) { M_GET_CLEAR oplist (item); return false; }              \
      CAT3(array_, name, _push_back)(array, item);                      \
      c = fgetc(file);                                                  \
    } while (c == M_GET_SEPARATOR oplist && !feof(file) && !ferror(file)); \
    M_GET_CLEAR oplist (item);                                          \
    return c == ']';                                                    \
  }                                                                     \
  , /* no in_str */ )                                                   \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  CAT3(array_, name, _equal_p)(CAT3(array_, name,_t) array1,            \
                              CAT3(array_, name,_t) array2)             \
  {                                                                     \
    assert (array1 != NULL && array2 != NULL);                          \
    CAT3(array_it_, name, _t) it1;                                      \
    CAT3(array_it_, name, _t) it2;                                      \
    CAT3(array_, name, _it)(it1, array1);                               \
    CAT3(array_, name, _it)(it2, array2);                               \
    while (!CAT3(array_, name, _end_p)(it1)                             \
           &&!CAT3(array_, name, _end_p)(it2)) {                        \
      const type *item1 = CAT3(array_, name, _cref)(it1);               \
      const type *item2 = CAT3(array_, name, _cref)(it2);               \
      bool b = M_GET_EQUAL oplist (*item1, *item2);                     \
      if (!b) return false;                                             \
      CAT3(array_, name, _next)(it1);                                   \
      CAT3(array_, name, _next)(it2);                                   \
    }                                                                   \
    return CAT3(array_, name, _end_p)(it1)                              \
      && CAT3(array_, name, _end_p)(it2);                               \
  }                                                                     \
  , /* no equal */ )                                                    \
                                                                        \
  
// TODO: Conditional HASH if HASH is present
// TODO: 'remove' interface is different in list & array ==> erase for iterator & pop for size_t

#endif
