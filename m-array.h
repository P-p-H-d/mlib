/**
 * @file m-array.h
 * @authors Patrick Pelissier,
 *          Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * @brief General linear dynamic-buffer array.
 * @version 0.2
 *
 * @copyright Copyright (c) 2017-2020, Patrick Pelissier
 * Copyright (c) 2020, Vladislav Dmitrievich Turbanov
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
   USAGE: M_ARRAY_DEF(name, type [, oplist_of_the_type]) */
#define M_ARRAY_DEF(name, ...)                                                \
  M_BEGIN_PROTECTED_CODE                                                      \
  _M_ARRAY_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
             ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),      \
               M_T(name, t), M_T(name, it_t) ),                         \
              (name, __VA_ARGS__,                                        \
               M_T(name, t), M_T(name, it_t))))                         \
  M_END_PROTECTED_CODE


/* Define a dynamic array of the given type and its associated functions
   as the provided type name_t with the iterator named it_t
   USAGE: M_ARRAY_DEF_AS(name, name_t, it_t, type [, oplist_of_the_type]) */
#define M_ARRAY_DEF_AS(name, name_t, it_t, ...)                               \
  M_BEGIN_PROTECTED_CODE                                                      \
  _M_ARRAY_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
             ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), \
               name_t, it_t), \
              (name, __VA_ARGS__,                                        \
               name_t, it_t))) \
  M_END_PROTECTED_CODE


/* Define the op-list of a dynamic array given its name and its oplist.
   If no op-list is given it is assumed to be M_DEFAULT_OPLIST
   USAGE: M_ARRAY_OPLIST(name[, oplist of the type]) */
#define M_ARRAY_OPLIST(...)                                               \
  _M_M_ARRAY_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                     ((__VA_ARGS__, M_DEFAULT_OPLIST),                    \
                      (__VA_ARGS__ )))



/********************************************************************************/
/********************************** INTERNAL ************************************/
/********************************************************************************/

/* Deferred evaluation for the oplist definition,
   so that all arguments are evaluated before further expansion */
#define _M_M_ARRAY_OPLIST_P1(arg) _M_M_ARRAY_OPLIST_P2 arg

/* Validation of the given oplist */
#define _M_M_ARRAY_OPLIST_P2(name, oplist)                                        \
  M_IF_OPLIST(oplist)(_M_M_ARRAY_OPLIST_P3, _M_M_ARRAY_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define _M_M_ARRAY_OPLIST_FAILURE(name, oplist)                                   \
  ((M_LIB_ERROR(ARGUMENT_OF_M_ARRAY_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* OPLIST definition of a dynamic array */
/* FIXME: Do we want to export some methods as they are slow and 
   are not fit to be used for building other methods (like _it_remove)? */
#define _M_M_ARRAY_OPLIST_P3(name, oplist)                                      \
    (INIT(M_F(name, M_NAMING_INIT)),                                          \
     M_IF_METHOD2(INIT_SET, SET,                                              \
                  oplist)(INIT_SET(M_F(name, M_NAMING_INIT_FROM)), ),          \
     M_IF_METHOD(INIT_SET, oplist)(INIT_WITH(API_1(M_INIT_VAI)), ),           \
     M_IF_METHOD2(INIT_SET, SET, oplist)(SET(M_F(name, M_NAMING_SET_AS)), ),     \
     CLEAR(M_F(name, M_NAMING_CLEAR)),                                        \
     INIT_MOVE(M_F(name, init_move)),                                         \
     MOVE(M_F(name, move)),                                                   \
     SWAP(M_F(name, swap)),                                                   \
     TYPE(M_T(name, ct)),                                                     \
     SUBTYPE(M_T3(name, subtype, ct)),                                        \
     TEST_EMPTY(M_F(name, M_NAMING_TEST_EMPTY)),                              \
     IT_TYPE(M_T3(name, it, ct)),                                               \
     IT_FIRST(M_F(name, M_NAMING_IT_FIRST)),                                  \
     IT_LAST(M_F(name, M_NAMING_IT_LAST)),                                    \
     IT_END(M_F(name, M_NAMING_IT_END)),                                      \
     IT_SET(M_F(name, M_NAMING_IT_SET)),                                      \
     IT_END_P(M_F(name, M_NAMING_IT_TEST_END)),                               \
     IT_LAST_P(M_F(name, M_NAMING_IT_TEST_LAST)),                             \
     IT_EQUAL_P(M_F(name, M_NAMING_IT_TEST_EQUAL)),                           \
     IT_NEXT(M_F(name, next)),                                                \
     IT_PREVIOUS(M_F(name, previous)),                                        \
     IT_REF(M_F(name, ref)),                                                  \
     IT_CREF(M_F(name, cref)),                                                \
     M_IF_METHOD(INIT_SET, oplist)(IT_INSERT(M_F(name, insert)), ),           \
     M_IF_AT_LEAST_METHOD(SET, INIT_MOVE,                                     \
                          oplist)(IT_REMOVE(M_F(name, remove)), ),            \
     CLEAN(M_F(name, M_NAMING_CLEAN)),                                        \
     KEY_TYPE(size_t),                                                        \
     VALUE_TYPE(M_T3(name, subtype, ct)),                                       \
     KEY_OPLIST(M_DEFAULT_OPLIST),                                            \
     VALUE_OPLIST(oplist),                                                    \
     M_IF_METHOD(SET, oplist)(SET_KEY(M_F(name, M_NAMING_SET_AT)), ),         \
     GET_KEY(M_F(name, M_NAMING_GET)),                                        \
     M_IF_METHOD(INIT, oplist)(GET_SET_KEY(M_F(name, M_NAMING_GET_AT)), ),    \
     M_IF_AT_LEAST_METHOD(SET, INIT_MOVE,                                     \
                          oplist)(ERASE_KEY(M_F(name, erase)), ),             \
     GET_SIZE(M_F(name, M_NAMING_SIZE)),                                      \
     M_IF_METHOD(INIT_SET, oplist)(PUSH(M_F(name, push_back)), ),             \
     M_IF_AT_LEAST_METHOD(SET, INIT_MOVE,                                     \
                          oplist)(POP(M_F(name, pop_back)), ),                \
     M_IF_AT_LEAST_METHOD(INIT_SET, INIT_MOVE,                                \
                          oplist)(PUSH_MOVE(M_F(name, push_move)), ),         \
     M_IF_AT_LEAST_METHOD(INIT_SET, INIT_MOVE,                                \
                          oplist)(POP_MOVE(M_F(name, pop_move)), ),           \
     OPLIST(oplist),                                                          \
     M_IF_METHOD(CMP, oplist)(SORT(M_F(name, special_sort)), ),               \
     M_IF_METHOD(GET_STR, oplist)(GET_STR(M_F(name, get_str)), ),             \
     M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_F(name, parse_cstr)), ),       \
     M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_F(name, out_str)), ),             \
     M_IF_METHOD(IN_STR, oplist)(IN_STR(M_F(name, in_str)), ),                \
     M_IF_METHOD(OUT_SERIAL, oplist)(OUT_SERIAL(M_F(name, out_serial)), ),    \
     M_IF_METHOD(IN_SERIAL, oplist)(IN_SERIAL(M_F(name, in_serial)), ),       \
     M_IF_METHOD(EQUAL, oplist)(EQUAL(M_F(name, M_NAMING_TEST_EQUAL)), ),     \
     M_IF_METHOD(HASH, oplist)(HASH(M_F(name, hash)), ),                      \
     M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist), ),                       \
     M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist), ),           \
     M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist), ))

/* Define the internal contract of an array */
#define _M_ARRAY_CONTRACT(a) do {                                             \
    M_ASSERT(a != NULL);                                                      \
    M_ASSERT(a->size <= a->alloc);                                            \
    M_ASSERT(a->size == 0 || a->ptr != NULL);                                 \
  } while (0)

/* Deferred evaluation for the array definition,
   so that all arguments are evaluated before further expansion */
#define _M_ARRAY_DEF_P1(arg) _M_ARRAY_DEF_P2 arg

/* Validate the oplist before going further */
#define _M_ARRAY_DEF_P2(name, type, oplist, array_t, it_t)                \
  M_IF_OPLIST(oplist)(_M_ARRAY_DEF_P3, _M_ARRAY_DEF_FAILURE)(name, type, oplist, array_t, it_t)

/* Stop processing with a compilation failure */
#define _M_ARRAY_DEF_FAILURE(name, type, oplist, array_t, it_t)           \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(M_ARRAY_DEF): the given argument is not a valid oplist: " #oplist)

/* Internal definition:
   - name: prefix to be used
   - type: type of the elements of the array
   - oplist: the op-list for the element type
   - array_t: alias for the type of the array
   - it_t: alias for the iterator of the array
*/
#define _M_ARRAY_DEF_P3(name, type, oplist, array_t, it_t)              \
                                                                        \
  /* Define a dynamic array */                                          \
  typedef struct M_T(name, s) {                                         \
    size_t size;            /* Number of elements in the array */       \
    size_t alloc;           /* Allocated size for the array base */     \
    type *ptr;              /* Pointer to the array base */             \
  } array_t[1];                                                         \
                                                                        \
  /* Define an iterator over an array */                                \
  typedef struct M_T(name, it_s) {                                      \
    size_t index;                      /* Index of the element */       \
    const struct M_T(name, s) *array;  /* Reference of the array */     \
  } it_t[1];                                                            \
                                                                        \
  /* Definition of the synonyms of the type */                          \
  typedef struct M_T(name, s) *M_T(name, ptr);                          \
  typedef const struct M_T(name, s) *M_T(name, srcptr);                 \
  /* Constant, unchanging types */                                      \
  typedef array_t M_T(name, ct);                                        \
  typedef it_t M_T3(name, it, ct);                                        \
  typedef type M_T3(name, subtype, ct);                                   \
                                                                        \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                      \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_INIT)(array_t v)                                   \
  {                                                                     \
    M_ASSERT(v != NULL);                                                \
    /* Initially, the array is empty with nothing allocated */          \
    v->size  = 0;                                                       \
    v->alloc = 0;                                                       \
    v->ptr   = NULL;                                                    \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_CLEAN)(array_t v)                                  \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    for(size_t i = 0; i < v->size; i++)                                 \
      M_CALL_CLEAR(oplist, v->ptr[i]);                                  \
    v->size = 0;                                                        \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_CLEAR)(array_t v)                                  \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_F(name, M_NAMING_CLEAN)(v);                                       \
    M_CALL_FREE(oplist, v->ptr);                                        \
    v->alloc = 0;                                                       \
    v->ptr   = NULL;                                                    \
  }                                                                     \
                                                                        \
  M_IF_METHOD2(INIT_SET, SET, oplist)(                                  \
  static inline void                                                    \
  M_F(name, M_NAMING_SET_AS)(array_t d, const array_t s)                   \
  {                                                                     \
    _M_ARRAY_CONTRACT(d);                                               \
    _M_ARRAY_CONTRACT(s);                                               \
    if (M_UNLIKELY (d == s)) return;                                    \
    if (s->size > d->alloc) {                                           \
      const size_t alloc = s->size;                                     \
      type *ptr = M_CALL_REALLOC(oplist, type, d->ptr, alloc);          \
      if (M_UNLIKELY (ptr == NULL)) {                                   \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return ;                                                        \
      }                                                                 \
      d->ptr = ptr;                                                     \
      d->alloc = alloc;                                                 \
    }                                                                   \
    size_t i;                                                           \
    size_t step1 = M_MIN(s->size, d->size);                             \
    for(i = 0; i < step1; i++)                                          \
      M_CALL_SET(oplist, d->ptr[i], s->ptr[i]);                         \
    for( ; i < s->size; i++)                                            \
      M_CALL_INIT_SET(oplist, d->ptr[i], s->ptr[i]);                    \
    for( ; i < d->size; i++)                                            \
      M_CALL_CLEAR(oplist, d->ptr[i]);                                  \
    d->size = s->size;                                                  \
    _M_ARRAY_CONTRACT(d);                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_INIT_FROM)(array_t d, const array_t s)              \
  {                                                                     \
    M_ASSERT(d != s);                                                   \
    M_F(name, M_NAMING_INIT)(d);                                        \
    M_F(name, M_NAMING_SET_AS)(d, s);                                      \
  }                                                                     \
  , /* No SET & INIT_SET */)                                            \
                                                                        \
  static inline void                                                    \
  M_F(name, init_move)(array_t d, array_t s)                            \
  {                                                                     \
    M_ASSERT(d != s);                                                    \
    _M_ARRAY_CONTRACT(s);                                               \
    d->size  = s->size;                                                 \
    d->alloc = s->alloc;                                                \
    d->ptr   = s->ptr;                                                  \
    s->alloc = 0;                                                       \
    s->ptr   = NULL;                                                    \
    _M_ARRAY_CONTRACT(d);                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, move)(array_t d, array_t s)                                 \
  {                                                                     \
    M_ASSERT(d != s);                                                   \
    M_F(name, M_NAMING_CLEAR)(d);                                       \
    M_F(name, init_move)(d, s);                                         \
  }                                                                     \
                                                                        \
  M_IF_METHOD(SET, oplist)(                                             \
  static inline void                                                    \
  M_F(name, M_NAMING_SET_AT)(array_t v, size_t i, type const x)         \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->size > 0 && v->ptr != NULL);                            \
    M_ASSERT_INDEX(i, v->size);                                         \
    M_CALL_SET(oplist, v->ptr[i], x);                                   \
  }                                                                     \
  , /* No SET */)                                                       \
                                                                        \
  static inline type const *                                            \
  M_F(name, back)(array_t v)                                            \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->ptr != NULL);                                           \
    M_ASSERT_INDEX(0, v->size);                                         \
    return M_CONST_CAST(type, &v->ptr[v->size-1]);                      \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_F(name, push_raw)(array_t v)                                        \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    if (M_UNLIKELY (v->size >= v->alloc)) {                             \
      M_ASSERT(v->size == v->alloc);                                    \
      size_t alloc = M_CALL_INC_ALLOC(oplist, v->alloc);                \
      if (M_UNLIKELY(alloc <= v->alloc)) {                              \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return NULL;                                                    \
      }                                                                 \
      M_ASSERT(alloc > v->size);                                        \
      type *ptr = M_CALL_REALLOC(oplist, type, v->ptr, alloc);          \
      if (M_UNLIKELY(ptr == NULL)) {                                    \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return NULL;                                                    \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    M_ASSERT(v->ptr != NULL);                                           \
    type *ret = &v->ptr[v->size];                                       \
    v->size++;                                                          \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSUME(ret != NULL);                                              \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  M_IF_METHOD(INIT_SET, oplist)(                                        \
  static inline void                                                    \
  M_F(name, push_back)(array_t v, type const x)                         \
  {                                                                     \
    type *data = M_F(name, push_raw)(v);                                \
    if (M_UNLIKELY (data == NULL) )                                     \
      return;                                                           \
    M_CALL_INIT_SET(oplist, *data, x);                                  \
  }                                                                     \
  , /* No INIT_SET */ )                                                 \
                                                                        \
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline type *                                                  \
  M_F(name, push_new)(array_t v)                                        \
  {                                                                     \
    type *data = M_F(name, push_raw)(v);                                \
    if (M_UNLIKELY (data == NULL) )                                     \
      return NULL;                                                      \
    M_CALL_INIT(oplist, *data);                                         \
    return data;                                                        \
  }                                                                     \
  , /* No INIT */ )                                                     \
                                                                        \
  M_IF_AT_LEAST_METHOD(INIT_SET, INIT_MOVE, oplist)(                    \
  static inline void                                                    \
  M_F(name, push_move)(array_t v, type *x)                              \
  {                                                                     \
    M_ASSERT(x != NULL);                                                \
    type *data = M_F(name, push_raw)(v);                                \
    if (M_UNLIKELY(data == NULL) )                                      \
      return;                                                           \
    M_DO_INIT_MOVE(oplist, *data, *x);                                  \
  }                                                                     \
  , /* INIT_SET | INIT_MOVE */ )                                        \
                                                                        \
  M_IF_METHOD(INIT_SET, oplist)(                                        \
  static inline void                                                    \
  M_F(name, push_at)(array_t v, size_t key, type const x)               \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT_INDEX(key, v->size+1);                                     \
    if (M_UNLIKELY (v->size >= v->alloc) ) {                            \
      M_ASSERT(v->size == v->alloc);                                    \
      size_t alloc = M_CALL_INC_ALLOC(oplist, v->alloc);                \
      if (M_UNLIKELY (alloc <= v->alloc)) {                             \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      M_ASSERT(alloc > v->size);                                        \
      type *ptr = M_CALL_REALLOC(oplist, type, v->ptr, alloc);          \
      if (M_UNLIKELY (ptr == NULL) ) {                                  \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    M_ASSERT(v->ptr != NULL);                                           \
    memmove(&v->ptr[key+1], &v->ptr[key], (v->size-key)*sizeof(type));  \
    v->size++;                                                          \
    M_CALL_INIT_SET(oplist, v->ptr[key], x);                            \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
  , /* No INIT_SET */ )                                                 \
                                                                        \
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void                                                    \
  M_F(name, resize)(array_t v, size_t size)                             \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    if (v->size > size) {                                               \
      /* Decrease size of array */                                      \
      for(size_t i = size ; i < v->size; i++)                           \
        M_CALL_CLEAR(oplist, v->ptr[i]);                                \
      v->size = size;                                                   \
    } else if (v->size < size) {                                        \
      /* Increase size of array */                                      \
      if (size > v->alloc) {                                            \
        size_t alloc = size ;                                           \
        type *ptr = M_CALL_REALLOC(oplist, type, v->ptr, alloc);        \
        if (M_UNLIKELY (ptr == NULL) ) {                                \
          M_MEMORY_FULL(sizeof (type) * alloc);                         \
          return;                                                       \
        }                                                               \
        v->ptr = ptr;                                                   \
        v->alloc = alloc;                                               \
      }                                                                 \
      for(size_t i = v->size ; i < size; i++)                           \
        M_CALL_INIT(oplist, v->ptr[i]);                                 \
      v->size = size;                                                   \
    }                                                                   \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
  , /* No INIT */ )                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, reserve)(array_t v, size_t alloc)                           \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    /* NOTE: Reserve below needed size to perform a shrink to fit */    \
    if (v->size > alloc) {                                              \
      alloc = v->size;                                                  \
    }                                                                   \
    if (M_UNLIKELY (alloc == 0)) {                                      \
      M_CALL_FREE(oplist, v->ptr);                                      \
      v->size = v->alloc = 0;                                           \
      v->ptr = NULL;                                                    \
    } else {                                                            \
      type *ptr = M_CALL_REALLOC(oplist, type, v->ptr, alloc);          \
      if (M_UNLIKELY(ptr == NULL) ) {                                  \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
                                                                        \
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline type *                                                  \
  M_F(name, M_NAMING_GET_AT)(array_t v, size_t idx)                     \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    const size_t size = idx + 1;                                        \
    /* resize if needed */                                              \
    if (v->size <= size) {                                              \
      /* Increase size of array */                                      \
      if (M_UNLIKELY(size > v->alloc) ) {                               \
        size_t alloc = M_CALL_INC_ALLOC(oplist, size) ;                 \
        /* In case of overflow of size_t */                             \
        if (M_UNLIKELY(alloc <= v->alloc)) {                            \
          M_MEMORY_FULL(sizeof (type) * alloc);                         \
          return NULL;                                                  \
        }                                                               \
        type *ptr = M_CALL_REALLOC(oplist, type, v->ptr, alloc);        \
        if (M_UNLIKELY(ptr == NULL) ) {                                 \
          M_MEMORY_FULL(sizeof (type) * alloc);                         \
          return NULL;                                                  \
        }                                                               \
        v->ptr = ptr;                                                   \
        v->alloc = alloc;                                               \
      }                                                                 \
      for(size_t i = v->size ; i < size; i++)                           \
        M_CALL_INIT(oplist, v->ptr[i]);                                 \
      v->size = size;                                                   \
    }                                                                   \
    M_ASSERT(idx < v->size);                                            \
    _M_ARRAY_CONTRACT(v);                                               \
    return &v->ptr[idx];                                                \
  }                                                                     \
  , /* No INIT */)                                                      \
                                                                        \
  M_IF_AT_LEAST_METHOD(SET, INIT_MOVE, oplist)(                         \
  static inline void                                                    \
  M_F(name, pop_back)(type *dest, array_t v)                            \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->ptr != NULL);                                           \
    M_ASSERT_INDEX(0, v->size);                                         \
    v->size--;                                                          \
    if (dest) {                                                         \
      M_DO_MOVE (oplist, *dest, v->ptr[v->size]);                       \
    } else {                                                            \
      M_CALL_CLEAR(oplist, v->ptr[v->size]);                            \
    }                                                                   \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
  , /* SET | INIT_MOVE */ )                                             \
                                                                        \
  M_IF_AT_LEAST_METHOD(INIT_SET, INIT_MOVE, oplist)(                    \
  static inline void                                                    \
  M_F(name, pop_move)(type *dest, array_t v)                            \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->ptr != NULL);                                           \
    M_ASSERT_INDEX(0, v->size);                                         \
    M_ASSERT(dest != NULL);                                             \
    v->size--;                                                          \
    M_DO_INIT_MOVE(oplist, *dest, v->ptr[v->size]);                     \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
  , /* INIT_SET | INIT_MOVE */ )                                        \
                                                                        \
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void                                                    \
  M_F(name, pop_until)(array_t v, it_t pos)                             \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v == pos->array);                                          \
    M_ASSERT_INDEX(pos->index, v->size + 1);                              \
    M_F(name, resize)(v, pos->index);                                   \
  }                                                                     \
  , /* No INIT */ )                                                     \
                                                                        \
  static inline bool                                                    \
  M_F(name, M_NAMING_TEST_EMPTY)(const array_t v)                       \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    return v->size == 0;                                                \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_F(name, M_NAMING_SIZE)(const array_t v)                             \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    return v->size;                                                     \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_F(name, capacity)(const array_t v)                                  \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    return v->alloc;                                                    \
  }                                                                     \
                                                                        \
  M_IF_AT_LEAST_METHOD(SET, INIT_MOVE, oplist)(                         \
  static inline void                                                    \
  M_F(name, pop_at)(type *dest, array_t v, size_t i)                    \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->size > 0 && v->ptr != NULL);                            \
    M_ASSERT_INDEX(i, v->size);                                         \
    if (dest)                                                           \
      M_DO_MOVE (oplist, *dest, v->ptr[i]);                             \
    else                                                                \
      M_CALL_CLEAR(oplist, v->ptr[i]);                                  \
    memmove(&v->ptr[i], &v->ptr[i+1], sizeof(type)*(v->size - i) );     \
    v->size--;                                                          \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_F(name, erase)(array_t a, size_t i)                                 \
  {                                                                     \
    _M_ARRAY_CONTRACT(a);                                               \
    if (i >= a->size) return false;                                     \
    M_F(name, pop_at)(NULL, a, i);                                      \
    return true;                                                        \
  }                                                                     \
  , /* SET | INIT_MOVE */ )                                             \
                                                                        \
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void                                                    \
  M_F(name, insert_v)(array_t v, size_t i, size_t num)                  \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT_INDEX(i, v->size+1);                                       \
    size_t size = v->size + num;                                        \
    /* Test for overflow of variable size */                            \
    if (M_UNLIKELY (size <= v->size)) {                                 \
      /* Unlikely case of nothing to do */                              \
      if (num == 0) return;                                             \
      M_MEMORY_FULL(sizeof (type) * v->size);                           \
      return ;                                                          \
    }                                                                   \
    /* Test if alloc array is sufficient */                             \
    if (size > v->alloc) {                                              \
      size_t alloc = M_CALL_INC_ALLOC(oplist, size) ;                   \
      if (M_UNLIKELY (alloc <= v->alloc)) {                             \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return ;                                                        \
      }                                                                 \
      type *ptr = M_CALL_REALLOC(oplist, type, v->ptr, alloc);          \
      if (M_UNLIKELY (ptr == NULL) ) {                                  \
        M_MEMORY_FULL(sizeof (type) * alloc);                           \
        return;                                                         \
      }                                                                 \
      v->ptr = ptr;                                                     \
      v->alloc = alloc;                                                 \
    }                                                                   \
    memmove(&v->ptr[i+num], &v->ptr[i], sizeof(type)*(v->size - i) );   \
    for(size_t k = i ; k < i+num; k++)                                  \
      M_CALL_INIT(oplist, v->ptr[k]);                                   \
    v->size = size;                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
  , /* No INIT */)                                                      \
                                                                        \
  static inline void                                                    \
  M_F(name, remove_v)(array_t v, size_t i, size_t j)                    \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(i < j && v->ptr != NULL);                                  \
    M_ASSERT_INDEX(i, v->size);                                         \
    M_ASSERT_INDEX(j, v->size+1);                                       \
    for(size_t k = i ; k < j; k++)                                      \
      M_CALL_CLEAR(oplist, v->ptr[k]);                                  \
    memmove(&v->ptr[i], &v->ptr[j], sizeof(type)*(v->size - j) );       \
    v->size -= (j-i);                                                   \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, swap)(array_t v1, array_t v2)                               \
  {                                                                     \
    _M_ARRAY_CONTRACT(v1);                                              \
    _M_ARRAY_CONTRACT(v2);                                              \
    size_t tmp = v1->size;                                              \
    v1->size = v2->size;                                                \
    v2->size = tmp;                                                     \
    tmp = v1->alloc;                                                    \
    v1->alloc = v2->alloc;                                              \
    v2->alloc = tmp;                                                    \
    type *p = v1->ptr;                                                  \
    v1->ptr = v2->ptr;                                                  \
    v2->ptr = p;                                                        \
    _M_ARRAY_CONTRACT(v1);                                              \
    _M_ARRAY_CONTRACT(v2);                                              \
  }                                                                     \
                                                                        \
  M_IF_AT_LEAST_METHOD(INIT_SET,INIT_MOVE,oplist) (                     \
  static inline void                                                    \
  M_F(name, swap_at)(array_t v, size_t i, size_t j)                     \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->ptr != NULL);                                           \
    M_ASSERT_INDEX(i, v->size);                                         \
    M_ASSERT_INDEX(j, v->size);                                         \
    type tmp;                                                           \
    M_DO_INIT_MOVE(oplist, tmp, v->ptr[i]);                             \
    M_DO_INIT_MOVE(oplist, v->ptr[i], v->ptr[j]);                       \
    M_DO_INIT_MOVE(oplist, v->ptr[j], tmp);                             \
    _M_ARRAY_CONTRACT(v);                                               \
  }                                                                     \
  , /* INIT_SET | INIT_MOVE */ )                                        \
                                                                        \
  static inline type *                                                  \
  M_F(name, M_NAMING_GET)(const array_t v, size_t i)                    \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->ptr != NULL);                                           \
    M_ASSERT_INDEX(i, v->size);                                         \
    return &v->ptr[i];                                                  \
  }                                                                     \
                                                                        \
  static inline type const *                                            \
  M_F(name, cget)(const array_t v, size_t i)                            \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(v->ptr != NULL);                                           \
    M_ASSERT_INDEX(i, v->size);                                         \
    return M_CONST_CAST(type, &v->ptr[i]);                              \
  }                                                                     \
                                                                        \
  static inline type const *                                            \
  M_F(name, front)(const array_t v)                                     \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT_INDEX(0, v->size);                                         \
    return M_F(name, cget)(v, 0);                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_IT_FIRST)(it_t it, const array_t v)                \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(it != NULL);                                               \
    it->index = 0;                                                      \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_IT_LAST)(it_t it, const array_t v)                 \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(it != NULL);                                               \
    /* If the size is 0, the index is an unsigned -1, */                \
    /* so it is greater than the end. */                                \
    it->index = v->size - 1;                                            \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_IT_END)(it_t it, const array_t v)                  \
  {                                                                     \
    _M_ARRAY_CONTRACT(v);                                               \
    M_ASSERT(it != NULL);                                               \
    it->index = v->size;                                                \
    it->array = v;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, M_NAMING_IT_SET)(it_t it, const it_t org)                   \
  {                                                                     \
    M_ASSERT(it != NULL && org != NULL);                                \
    it->index = org->index;                                             \
    it->array = org->array;                                             \
    _M_ARRAY_CONTRACT(it->array);                                       \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_F(name, M_NAMING_IT_TEST_END)(const it_t it)                        \
  {                                                                     \
    M_ASSERT(it != NULL && it->array != NULL);                          \
    return it->index >= it->array->size;                                \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_F(name, M_NAMING_IT_TEST_LAST)(const it_t it)                       \
  {                                                                     \
    M_ASSERT(it != NULL && it->array != NULL);                          \
    /* NOTE: Can not compute 'size-1' due to potential overflow         \
       if size is 0 */                                                  \
    return it->index + 1 >= it->array->size;                            \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_F(name, M_NAMING_IT_TEST_EQUAL)(const it_t it1,                     \
                                    const it_t it2)                     \
  {                                                                     \
    M_ASSERT(it1 != NULL && it2 != NULL);                               \
    return it1->array == it2->array && it1->index == it2->index;        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, next)(it_t it)                                              \
  {                                                                     \
    M_ASSERT(it != NULL && it->array != NULL);                          \
    it->index++;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, previous)(it_t it)                                          \
  {                                                                     \
    M_ASSERT(it != NULL && it->array != NULL);                          \
    /* NOTE: In the case index=0, it will be set to (unsigned) -1       \
       ==> it will be greater than size ==> end_p will return true */   \
    it->index--;                                                        \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_F(name, ref)(const it_t it)                                         \
  {                                                                     \
    M_ASSERT(it != NULL);                                               \
    return M_F(name, M_NAMING_GET)(it->array, it->index);               \
  }                                                                     \
                                                                        \
  static inline type const *                                            \
  M_F(name, cref)(const it_t it)                                        \
  {                                                                     \
    M_ASSERT(it != NULL);                                               \
    return M_F(name, cget)(it->array, it->index);                       \
  }                                                                     \
                                                                        \
  M_IF_METHOD(INIT_SET, oplist)(                                        \
  static inline void                                                    \
  M_F(name, insert)(array_t a, it_t it, type const x)                   \
  {                                                                     \
    M_ASSERT(it != NULL && a == it->array);                             \
    M_F(name, push_at)(a, it->index + 1, x);                            \
    it->index++;                                                        \
  }                                                                     \
  , /* End of INIT_SET */ )                                             \
                                                                        \
  M_IF_AT_LEAST_METHOD(SET, INIT_MOVE, oplist)(                         \
  static inline void                                                    \
  M_F(name, remove)(array_t a, it_t it)                                 \
  {                                                                     \
    M_ASSERT(it != NULL && a == it->array);                             \
    M_F(name, pop_at)(NULL, a, it->index);                              \
    /* NOTE: it->index will naturally point to the next element */      \
  }                                                                     \
  , /* End of SET | INIT_SET */ )                                       \
                                                                        \
  M_IF_METHOD(CMP, oplist)                                              \
  (                                                                     \
   static inline void M_F(name, special_sort)(array_t l,                \
              int (*func_type) (type const *a, type const *b))          \
  {                                                                     \
    /* Using qsort is more compact but slower than a full templated     \
       version which can be twice faster */                             \
    int (*func_void)(const void*, const void*);                         \
    /* There is no way (?) to avoid the cast */                         \
    func_void = (int (*)(const void*, const void*))func_type;           \
    qsort(l->ptr, l->size, sizeof(type), func_void);                    \
  }                                                                     \
                                                                        \
  M_IF_METHOD2(SWAP, SET, oplist)(                                      \
  static inline void                                                    \
  M_F(name, special_stable_sort_noalloc)                                \
    (type tab[], size_t size, type tmp[])                               \
  {                                                                     \
    size_t th = 4;                                                      \
    M_IF_DEBUG(type *org_tab = tab;)                                    \
    M_ASSERT(size > 1);                                                 \
    /* Let's select the threshold of the pass 1 to be sure              \
       the final result is in tab.*/                                    \
    if (m_core_clz64(size-1) & 1)                                       \
      th += th;                                                         \
                                                                        \
    /* Pass 1: insertion sort (stable) */                               \
    for(size_t k = 0 ; k < size; ) {                                    \
      size_t max = size - k < 2*th ? size - k : th;                     \
      M_ASSUME(max >= th);                                              \
      for(size_t i = 1; i < max; i++) {                                 \
        size_t j = i;                                                   \
        while (j > 0 && M_CALL_CMP(oplist, tab[k+j-1], tab[k+j]) > 0) { \
          M_CALL_SWAP(oplist, tab[k+j-1], tab[k+j]);                    \
          j = j - 1;                                                    \
        }                                                               \
      }                                                                 \
      k += max;                                                         \
    }                                                                   \
                                                                        \
    /* N Pass of merge sort (stable) */                                 \
    while (th < size) {                                                 \
      type *dest = tmp;                                                 \
      /* Pass n: Merge */                                               \
      for(size_t k = 0 ; k < size; ) {                                  \
        type *el1 = &tab[k];                                            \
        type *el2 = &tab[k+th];                                         \
        size_t n1 = th;                                                 \
        size_t n2 = size-k <= 3*th ? size-k-th : th;                    \
        M_ASSERT(size-k > th);                                          \
        M_ASSERT(0 < n1 && n1 <= size);                                 \
        M_ASSERT(0 < n2 && n2 <= size);                                 \
        k += n1+n2;                                                     \
        for (;;) {                                                      \
          if (M_CALL_CMP(oplist, *el1, *el2) <= 0) {                    \
            M_CALL_SET(oplist, *dest, *el1);                            \
            dest++;                                                     \
            el1++;                                                      \
            if (-- n1 == 0) {                                           \
              if (n2 > 0) {                                             \
                memcpy(dest, el2, n2 * sizeof (type));                  \
                dest += n2;                                             \
              }                                                         \
              break;                                                    \
            }                                                           \
          } else {                                                      \
            M_CALL_SET(oplist, *dest, *el2);                            \
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
    M_ASSERT(org_tab == tab);                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, special_stable_sort)(array_t l)                             \
  {                                                                     \
    if (M_UNLIKELY(l->size < 2))                                        \
      return;                                                           \
    /* NOTE: if size is <= 4, no need to perform an allocation */       \
    type *temp = M_CALL_REALLOC(oplist, type, NULL, l->size);           \
    if (temp == NULL) {                                                 \
      M_MEMORY_FULL(sizeof (type) * l->size);                           \
      return;                                                           \
    }                                                                   \
    M_F(name, special_stable_sort_noalloc)(l->ptr, l->size, temp);      \
    M_CALL_FREE(oplist, temp);                                          \
  }                                                                     \
  ,) /* IF SWAP & SET methods */                                        \
                                                                        \
  ,) /* IF CMP oplist */                                                \
                                                                        \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_F(name, get_str)(string_t str, array_t const array,                 \
                     bool append)                                       \
  {                                                                     \
    STRINGI_CONTRACT(str);                                              \
    _M_ARRAY_CONTRACT(array);                                           \
    (append ? M_F(string, cat_cstr) : M_F3(string, M_NAMING_SET_AS, cstr)) (str, "[");              \
    it_t it;                                                            \
    for (M_F(name, M_NAMING_IT_FIRST)(it, array);                       \
         !M_F(name, M_NAMING_IT_TEST_END)(it);                          \
         M_F(name, next)(it)){                                          \
      type const *item = M_F(name, cref)(it);                           \
      M_CALL_GET_STR(oplist, str, *item, true);                         \
      if (!M_F(name, M_NAMING_IT_TEST_LAST)(it))                        \
        M_F(string, push_back)(str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    M_F(string, push_back) (str, ']');                                        \
    STRINGI_CONTRACT(str);                                              \
  }                                                                     \
  , /* no GET_STR */ )                                                  \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_F(name, out_str)(FILE *file, const array_t array)                   \
  {                                                                     \
    _M_ARRAY_CONTRACT(array);                                           \
    M_ASSERT(file != NULL);                                             \
    fputc ('[', file);                                                  \
    for (size_t i = 0; i < array->size; i++) {                          \
      type const *item = M_F(name, cget)(array, i);                     \
      M_CALL_OUT_STR(oplist, file, *item);                              \
      if (i != array->size-1)                                           \
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fputc (']', file);                                                  \
  }                                                                     \
  , /* no OUT_STR */ )                                                  \
                                                                        \
  M_IF_METHOD2(PARSE_STR, INIT, oplist)(                                \
  static inline bool                                                    \
  M_F(name, parse_cstr)(array_t array, const char str[],                 \
                       const char**endp)                                \
  {                                                                     \
    _M_ARRAY_CONTRACT(array);                                           \
    M_ASSERT(str != NULL);                                              \
    M_F(name, M_NAMING_CLEAN)(array);                                   \
    bool success = false;                                               \
    int c = *str++;                                                     \
    if (M_UNLIKELY(c != '[')) goto exit;                                \
    c = *str++;                                                         \
    if (M_UNLIKELY(c == ']')) { success = true; goto exit; }            \
    if (M_UNLIKELY(c == 0)) goto exit;                                  \
    str--;                                                              \
    type item;                                                          \
    M_CALL_INIT(oplist, item);                                          \
    do {                                                                \
      bool b = M_CALL_PARSE_STR(oplist, item, str, &str);               \
      do { c = *str++; } while (isspace(c));                            \
      if (b == false || c == 0) { goto exit_clear; }                    \
      M_F(name, push_back)(array, item);                                \
    } while (c == M_GET_SEPARATOR oplist);                              \
    _M_ARRAY_CONTRACT(array);                                           \
    success = (c == ']');                                               \
  exit_clear:                                                           \
    M_CALL_CLEAR(oplist, item);                                         \
  exit:                                                                 \
    if (endp) *endp = str;                                              \
    return success;                                                     \
  }                                                                     \
  , /* no PARSE_STR & INIT */ )                                         \
                                                                        \
  M_IF_METHOD2(IN_STR, INIT, oplist)(                                   \
  static inline bool                                                    \
  M_F(name, in_str)(array_t array, FILE *file)                          \
  {                                                                     \
    _M_ARRAY_CONTRACT(array);                                           \
    M_ASSERT(file != NULL);                                             \
    M_F(name, M_NAMING_CLEAN)(array);                                   \
    int c = fgetc(file);                                                \
    if (M_UNLIKELY(c != '[')) return false;                             \
    c = fgetc(file);                                                    \
    if (M_UNLIKELY(c == ']')) return true;                              \
    if (M_UNLIKELY(c == EOF)) return false;                             \
    ungetc(c, file);                                                    \
    type item;                                                          \
    M_CALL_INIT(oplist, item);                                          \
    do {                                                                \
      bool b = M_CALL_IN_STR(oplist, item, file);                       \
      do { c = fgetc(file); } while (isspace(c));                       \
      if (b == false || c == EOF) { break; }                            \
      M_F(name, push_back)(array, item);                                \
    } while (c == M_GET_SEPARATOR oplist);                              \
    M_CALL_CLEAR(oplist, item);                                         \
    _M_ARRAY_CONTRACT(array);                                           \
    return c == ']';                                                    \
  }                                                                     \
  , /* no IN_STR & INIT */ )                                            \
                                                                        \
  M_IF_METHOD(OUT_SERIAL, oplist)(                                      \
  static inline m_serial_return_code_t                                  \
  M_F(name, out_serial)(m_serial_write_t f, const array_t array)        \
  {                                                                     \
    _M_ARRAY_CONTRACT(array);                                           \
    M_ASSERT(f != NULL && f->m_interface != NULL);                      \
    m_serial_return_code_t ret;                                         \
    m_serial_local_t local;                                             \
    ret = f->m_interface->write_array_start(local, f, array->size);     \
    for (size_t i = 0; i < array->size; i++) {                          \
      type const *item = M_F(name, cget)(array, i);                     \
      if (i != 0) {                                                     \
        ret |= f->m_interface->write_array_next(local, f);              \
      }                                                                 \
      ret |= M_CALL_OUT_SERIAL(oplist, f, *item);                       \
    }                                                                   \
    ret |= f->m_interface->write_array_end(local, f);                   \
    return ret & M_SERIAL_FAIL;                                         \
  }                                                                     \
  , /* no OUT_SERIAL */ )                                               \
                                                                        \
  M_IF_METHOD2(IN_SERIAL, INIT, oplist)(                                \
  static inline m_serial_return_code_t                                  \
  M_F(name, in_serial)(array_t array, m_serial_read_t f)                \
  {                                                                     \
    _M_ARRAY_CONTRACT(array);                                           \
    M_ASSERT(f != NULL && f->m_interface != NULL);                      \
    m_serial_return_code_t ret;                                         \
    m_serial_local_t local;                                             \
    size_t estimated_size = 0;                                          \
    M_F(name, M_NAMING_CLEAN)(array);                                   \
    ret = f->m_interface->read_array_start(local, f, &estimated_size);  \
    if (M_UNLIKELY (ret != M_SERIAL_OK_CONTINUE)) {                     \
       return ret;                                                      \
    }                                                                   \
    if (estimated_size != 0) {                                          \
      /* The format has given an estimation of the array size */        \
      M_F(name, reserve)(array, estimated_size);                        \
    }                                                                   \
    type item;                                                          \
    M_CALL_INIT(oplist, item);                                          \
    do {                                                                \
      ret = M_CALL_IN_SERIAL(oplist, item, f);                          \
      if (ret != M_SERIAL_OK_DONE) { break; }                           \
      M_F(name, push_back)(array, item);                                \
      ret = f->m_interface->read_array_next(local, f);                  \
    } while (ret == M_SERIAL_OK_CONTINUE);                              \
    M_CALL_CLEAR(oplist, item);                                         \
    _M_ARRAY_CONTRACT(array);                                           \
    return ret;                                                         \
  }                                                                     \
  , /* no IN_SERIAL & INIT */ )                                         \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  M_F(name, M_NAMING_TEST_EQUAL)(const array_t array1,                  \
                                 const array_t array2)                  \
  {                                                                     \
    _M_ARRAY_CONTRACT(array1);                                          \
    _M_ARRAY_CONTRACT(array2);                                          \
    if (array1->size != array2->size) return false;                     \
    size_t i;                                                           \
    for(i = 0; i < array1->size; i++) {                                 \
      type const *item1 = M_F(name, cget)(array1, i);                   \
      type const *item2 = M_F(name, cget)(array2, i);                   \
      bool b = M_CALL_EQUAL(oplist, *item1, *item2);                    \
      if (!b) return false;                                             \
    }                                                                   \
    return i == array1->size;                                           \
  }                                                                     \
  , /* no EQUAL */ )                                                    \
                                                                        \
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t                                                  \
  M_F(name, hash)(const array_t array)                                  \
  {                                                                     \
    _M_ARRAY_CONTRACT(array);                                           \
    M_HASH_DECL(hash);                                                  \
    for(size_t i = 0 ; i < array->size; i++) {                          \
      size_t hi = M_CALL_HASH(oplist, array->ptr[i]);                   \
      M_HASH_UP(hash, hi);                                              \
    }                                                                   \
    return M_HASH_FINAL (hash);                                         \
  }                                                                     \
  , /* no HASH */ )                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, splice)(array_t a1, array_t a2)                             \
  {                                                                     \
    _M_ARRAY_CONTRACT(a1);                                              \
    _M_ARRAY_CONTRACT(a2);                                              \
    if (M_LIKELY(a2->size > 0)) {                                       \
      size_t newSize = a1->size + a2->size;                             \
      if (newSize > a1->alloc) {                                        \
        type *ptr = M_CALL_REALLOC(oplist, type, a1->ptr, newSize);     \
        if (M_UNLIKELY (ptr == NULL) ) {                                \
          M_MEMORY_FULL(sizeof (type) * newSize);                       \
        }                                                               \
        a1->ptr = ptr;                                                  \
        a1->alloc = newSize;                                            \
      }                                                                 \
      memcpy(&a1->ptr[a1->size], &a2->ptr[0], a2->size * sizeof (type));\
      /* a2 is now empty */                                             \
      a2->size = 0;                                                     \
      /* a1 has been expanded with the items of a2 */                   \
      a1->size = newSize;                                               \
    }                                                                   \
  }                                                                     \

#endif // MSTARLIB_ARRAY_H
