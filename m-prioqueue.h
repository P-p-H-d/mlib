/*
 * M*LIB - dynamic priority queue module
 *
 * Copyright (c) 2017-2020, Patrick Pelissier
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
#ifndef MSTARLIB_PRIOQUEUE_H
#define MSTARLIB_PRIOQUEUE_H

#include "m-core.h"
#include "m-array.h"            /* Priority queue are built upon array */

/* Priority queue based on binary heap implementation */

/* Define a prioqueue of a given type and its associated functions.
   USAGE: PRIOQUEUE_DEF(name, type [, oplist_of_the_type]) */
#define PRIOQUEUE_DEF(name, ...)                                              \
  M_BEGIN_PROTECTED_CODE                                                      \
  PRIOQUEUEI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                               \
                    ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name,_t), M_C(name,_it_t) ), \
                     (name, __VA_ARGS__,                                        M_C(name,_t), M_C(name,_it_t)))) \
  M_END_PROTECTED_CODE


/* Define a prioqueue of a given type and its associated functions.
  as the name name_t with an iterator named it_t
   USAGE: PRIOQUEUE_DEF_AS(name, name_t, it_t, type [, oplist_of_the_type]) */
#define PRIOQUEUE_DEF_AS(name, name_t, it_t, ...)                             \
  M_BEGIN_PROTECTED_CODE                                                      \
  PRIOQUEUEI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                               \
                    ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
                     (name, __VA_ARGS__,                                        name_t, it_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a prioqueue of type.
   USAGE: PRIOQUEUE_OPLIST(name[, oplist of the type]) */
#define PRIOQUEUE_OPLIST(...)                                                 \
  PRIOQUEUEI_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                            \
                       ((__VA_ARGS__, M_DEFAULT_OPLIST),                      \
                        (__VA_ARGS__ )))



/********************************** INTERNAL ************************************/

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define PRIOQUEUEI_OPLIST_P1(arg) PRIOQUEUEI_OPLIST_P2 arg

/* Validation of the given oplist */
#define PRIOQUEUEI_OPLIST_P2(name, oplist)                                    \
  M_IF_OPLIST(oplist)(PRIOQUEUEI_OPLIST_P3, PRIOQUEUEI_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define PRIOQUEUEI_OPLIST_FAILURE(name, oplist)                               \
  ((M_LIB_ERROR(ARGUMENT_OF_PRIOQUEUE_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* Define oplist of a priority queue */
#define PRIOQUEUEI_OPLIST_P3(name, oplist)                                    \
  (INIT(M_F(name, M_NAMING_INIT)),						                      \
   INIT_SET(M_F(name, M_NAMING_INIT_WITH)),					                  \
   INIT_WITH(API_1(M_INIT_VAI)),                                              \
   SET(M_F(name, M_NAMING_SET_AS)),						                      \
   CLEAR(M_F(name, M_NAMING_FINALIZE)),						                  \
   INIT_MOVE(M_F(name, init_move)),					                          \
   MOVE(M_F(name, move)),						                              \
   SWAP(M_F(name, swap)),						                              \
   TYPE(M_T(name, ct)),                                                       \
   SUBTYPE(M_T(name, subtype_ct)),                                            \
   CLEAN(M_F(name, M_NAMING_CLEAN)),						                  \
   PUSH(M_F(name, push)),						                              \
   POP(M_F(name,pop)),                                                        \
   OPLIST(oplist),                                                            \
   TEST_EMPTY(M_F(name, M_NAMING_TEST_EMPTY)),                                \
   GET_SIZE(M_F(name, M_NAMING_GET_SIZE))                                         \
   IT_TYPE(M_C(name, _it_ct)),                                                \
   IT_FIRST(M_C(name,_it)),                                                   \
   IT_END(M_C(name,_it_end)),                                                 \
   IT_SET(M_C(name,_it_set)),                                                 \
   IT_END_P(M_C(name,_end_p)),                                                \
   IT_EQUAL_P(M_C(name,_it_equal_p)),                                         \
   IT_LAST_P(M_C(name,_last_p)),                                              \
   IT_NEXT(M_C(name,_next)),                                                  \
   IT_CREF(M_C(name,_cref)),                                                  \
   )


/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define PRIOQUEUEI_DEF_P1(arg)    PRIOQUEUEI_DEF_P2 arg

/* Validate the oplist before going further */
#define PRIOQUEUEI_DEF_P2(name, type, oplist, prioqueue_t, it_t)              \
  M_IF_OPLIST(oplist)(PRIOQUEUEI_DEF_P3, PRIOQUEUEI_DEF_FAILURE)(name, type, oplist, prioqueue_t, it_t)

/* Stop processing with a compilation failure */
#define PRIOQUEUEI_DEF_FAILURE(name, type, oplist, prioqueue_t, it_t)         \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(PRIOQUEUE_DEF): the given argument is not a valid oplist: " #oplist)

/* Define the priority queue:
   - name: prefix to use,
   - type: type of the contained objects,
   - oplist: oplist of the contained objects,
   - prioqueue_t: type of the container,
   - it_t: iterator of the container
*/
#define PRIOQUEUEI_DEF_P3(name, type, oplist, prioqueue_t, it_t)              \
                                                                              \
  /* Definition of the internal array used to construct the priority queue */ \
  M_ARRAY_DEF(M_I(name, array), type, oplist)                                   \
                                                                              \
  /* Define the priority queue over the defined array */                      \
  typedef struct M_T(name, s) {                                               \
    M_T3(name, array, t) array;                                               \
  } prioqueue_t[1];                                                           \
  /* Define the pointer references to the priority queue */                   \
  typedef struct M_T(name, s) *M_T(name, ptr);                                \
  typedef const struct M_T(name, s) *M_T(name, srcptr);                       \
                                                                              \
  /* The iterator is the same one as the one of the internal array */         \
  typedef M_T3(name, array, it_t) it_t;                                       \
                                                                              \
  /* Definition of the internal types used by the oplist */                   \
  typedef prioqueue_t M_T(name, ct);                                          \
  typedef type        M_T(name, subtype_ct);                                  \
  typedef it_t        M_T(name, it_ct);                                       \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_INIT)(prioqueue_t p)                                     \
  {                                                                           \
    M_F3(name, array, M_NAMING_INIT)(p->array);                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_INIT_WITH)(prioqueue_t p, prioqueue_t const o)            \
  {                                                                           \
    M_F3(name, array, M_NAMING_INIT_WITH)(p->array, o->array);                 \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_SET_AS)(prioqueue_t p, prioqueue_t const o)                   \
  {                                                                           \
    M_F3(name, array, M_NAMING_SET_AS)(p->array, o->array);                                \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_FINALIZE)(prioqueue_t p)                              \
  {                                                                           \
    M_F3(name, array, M_NAMING_FINALIZE)(p->array);                       \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, init_move)(prioqueue_t p, prioqueue_t o)                   \
  {                                                                           \
    M_F3(name, array, init_move)(p->array, o->array);                          \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, move)(prioqueue_t p, prioqueue_t o)                        \
  {                                                                           \
    M_F3(name, array, move)(p->array, o->array);                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, swap)(prioqueue_t p, prioqueue_t o)                               \
  {                                                                           \
    M_F3(name, array, swap)(p->array, o->array);                              \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_CLEAN)(prioqueue_t p)                                    \
  {                                                                           \
    M_F3(name, array, M_NAMING_CLEAN)(p->array);                              \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_F3(name, i, parent)(size_t i)                                             \
  {                                                                           \
    M_ASSERT(i > 0);                                                          \
    return (i - 1) / 2;                                                       \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_F3(name, i, lchild)(size_t i)                                             \
  {                                                                           \
    M_ASSERT(i <= ((SIZE_MAX)-2)/2);                                          \
    return 2 * i + 1;                                                         \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_F3(name, i, rchild)(size_t i)                                             \
  {                                                                           \
    M_ASSERT(i <= ((SIZE_MAX)-2)/2);                                          \
    return 2 * i + 2;                                                         \
  }                                                                           \
                                                                              \
  static inline int                                                           \
  M_F3(name, i, cmp)(const prioqueue_t p, size_t i, size_t j)                 \
  {                                                                           \
    return M_CALL_CMP(oplist, *M_F3(name, array, cget)(p->array, i),          \
                              *M_F3(name, array, cget)(p->array, j));         \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, M_NAMING_TEST_EMPTY)(prioqueue_t const p)                         \
  {                                                                           \
    return M_F3(name, array, M_NAMING_TEST_EMPTY)(p->array);                  \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_F(name, M_NAMING_GET_SIZE)(prioqueue_t const p)                               \
  {                                                                           \
    return M_F3(name, array, M_NAMING_GET_SIZE)(p->array);                        \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, push)(prioqueue_t p, type const x)                                \
  {                                                                           \
    /* Push back the new element at the end of the array */                   \
    M_F3(name, array, push_back)(p->array, x);                                \
                                                                              \
    /* Reorder the array by swapping with its parent                          \
     * until it reaches the right position */                                 \
    size_t i = M_F3(name, array, M_NAMING_GET_SIZE)(p->array) - 1;                \
    while (i > 0) {                                                           \
      size_t j = M_F3(name, i, parent)(i);                                    \
      if (M_F3(name, i, cmp)(p, j, i) <= 0)                                   \
        break;                                                                \
      M_F3(name, array, swap_at) (p->array, i, j);                            \
      i = j;                                                                  \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, front)(prioqueue_t const p)                                       \
  {                                                                           \
    return M_F3(name, array, cget)(p->array, 0);                              \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, pop)(type *x, prioqueue_t p)                                      \
  {                                                                           \
    /* Swap the front element with the last element */                        \
    size_t size = M_F3(name, array, M_NAMING_GET_SIZE)(p->array) - 1;             \
    M_F3(name, array, swap_at)(p->array, 0, size);                            \
    /* Swap the new last element  */                                          \
    M_F3(name, array, pop_back)(x, p->array);                                 \
                                                                              \
    /* Reorder the heap */                                                    \
    size_t i = 0;                                                             \
    while (true) {                                                            \
      size_t child = M_F3(name, i, lchild)(i);                                \
      if (child >= size)                                                      \
        break;                                                                \
      size_t otherChild = M_F3(name, i, rchild)(i);                           \
      if (otherChild < size                                                   \
          && M_F3(name, i, cmp)(p, otherChild, child) < 0 ) {                 \
        child = otherChild;                                                   \
      }                                                                       \
      if (M_F3(name, i, cmp)(p, i, child) <= 0)                               \
        break;                                                                \
      M_F3(name, array, swap_at) (p->array, i, child);                        \
      i = child;                                                              \
    }                                                                         \
  }                                                                           \
                                                                              \
  /* Define iterators over the array iterator */                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_FIRST)(it_t it, prioqueue_t const v)                  \
  {                                                                           \
    M_F3(name, array, M_NAMING_IT_FIRST)(it, v->array);                       \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_LAST)(it_t it, prioqueue_t const v)			          \
  {                                                                           \
    M_F3(name, array, M_NAMING_IT_LAST)(it, v->array);                        \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_END)(it_t it, prioqueue_t const v)		              \
  {                                                                           \
    M_F3(name, array, M_NAMING_IT_END)(it, v->array);                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_SET)(it_t it, const it_t org)                         \
  {                                                                           \
    M_F3(name, array, M_NAMING_IT_SET)(it, org);                              \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, M_NAMING_IT_TEST_END)(const it_t it)                              \
  {                                                                           \
    return M_F3(name, array, M_NAMING_IT_TEST_END)(it);                       \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, M_NAMING_IT_TEST_LAST)(const it_t it)                             \
  {                                                                           \
    return M_F3(name, array, M_NAMING_IT_TEST_LAST)(it);                      \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, M_NAMING_IT_TEST_EQUAL)(const it_t it1,				              \
			                        const it_t it2)				              \
  {                                                                           \
    return M_F3(name, array, M_NAMING_IT_TEST_EQUAL)(it1, it2);               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, next)(it_t it)                                             \
  {                                                                           \
    M_F3(name, array, next)(it);                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, previous)(it_t it)                                                \
  {                                                                           \
    M_F3(name, array, previous)(it);                                          \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, cref)(const it_t it)					                          \
  {                                                                           \
    return M_F3(name, array, cref)(it);                                       \
  }                                                                           \
                                                                              \
  M_IF_METHOD(EQUAL, oplist)                                                  \
  (                                                                           \
    /* EQUAL & CMP may be uncorrelated */                                     \
    static inline bool                                                        \
    M_F(name, M_NAMING_TEST_EQUAL_TO)(prioqueue_t const p, prioqueue_t const q)  \
    {                                                                         \
      return M_F3(name, array, M_NAMING_TEST_EQUAL_TO)(p->array, q->array);      \
    }                                                                         \
                                                                              \
   static inline bool                                                         \
   M_F(name, erase)(prioqueue_t p, type x)                                    \
   {                                                                          \
     /* First pass: search for an item EQUAL to x */                          \
     /* NOTE: An HASHMAP may be a good idea to optimize this pass. */         \
     size_t size = M_F3(name, array, M_NAMING_GET_SIZE)(p->array);                \
     size_t i = 0;                                                            \
     for(i = 0; i < size; i++) {                                              \
       if (M_CALL_EQUAL(oplist, *M_F3(name, array, cget)(p->array, i), x))    \
         break;                                                               \
     }                                                                        \
     /* If x is not found, then stop */                                       \
     if (i >= size)                                                           \
       return false;                                                          \
     /* Swap the found item and the last element */                           \
     size--;                                                                  \
     M_F3(name, array, swap_at) (p->array, i, size);                          \
     M_F3(name, array, pop_back)(NULL, p->array);                             \
     /* Move back the last swapped element to its right position in the heap */ \
     while (true) {                                                           \
       size_t child = M_F3(name, i, lchild)(i);                               \
       if (child >= size) break;                                              \
       size_t otherChild = M_F3(name, i, rchild)(i);                          \
       if (otherChild < size                                                  \
           && M_F3(name, i, cmp)(p, otherChild, child) < 0 ) {                \
         child = otherChild;                                                  \
       }                                                                      \
       if (M_F3(name, i, cmp)(p, i, child) <= 0) break;                       \
       M_F3(name, array, swap_at) (p->array, i, child);                       \
       i = child;                                                             \
     }                                                                        \
     return true;                                                             \
   }                                                                          \
                                                                              \
   static inline void                                                         \
   M_F(name, update)(prioqueue_t p, type const xold, type const xnew)         \
   {                                                                          \
     /* NOTE: xold can be the same pointer than xnew */                       \
     /* First pass: search for an item EQUAL to x */                          \
     size_t size = M_F3(name, array, M_NAMING_GET_SIZE)(p->array);                \
     size_t i = 0;                                                            \
     for(i = 0; i < size; i++) {                                              \
       if (M_CALL_EQUAL(oplist, *M_F3(name, array, cget)(p->array, i), xold)) \
         break;                                                               \
     }                                                                        \
     /* We shall have found the item */                                       \
     M_ASSERT(i < size);                                                      \
     /* Test if the position of the old data is further or nearer than the new */ \
     int cmp = M_CALL_CMP(oplist, *M_F3(name, array, cget)(p->array, i), xnew); \
     /* Set the found item to the new element */                              \
     M_F3(name, array, set_at) (p->array, i, xnew);                           \
     if (cmp < 0) {                                                           \
      /* Move back the updated element to its new position, further in the heap */ \
      while (true) {                                                          \
        size_t child = M_F3(name, i, lchild)(i);                              \
        if (child >= size) break;                                             \
        size_t otherChild = M_F3(name, i, rchild)(i);                         \
        if (otherChild < size                                                 \
            && M_F3(name, i, cmp)(p, otherChild, child) < 0 ) {               \
          child = otherChild;                                                 \
        }                                                                     \
        if (M_F3(name, i, cmp)(p, i, child) <= 0) break;                      \
        M_F3(name, array, swap_at)(p->array, i, child);                       \
        i = child;                                                            \
      }                                                                       \
     } else {                                                                 \
      /* Move back the updated element to its new position, nearest in the heap */ \
      while (i > 0) {                                                         \
        size_t parent = M_F3(name, i, parent)(i);                             \
        if (M_F3(name, i, cmp)(p, parent, i) <= 0) break;                     \
        M_F3(name, array, swap_at) (p->array, i, parent);                     \
        i = parent;                                                           \
      }                                                                       \
     }                                                                        \
   }                                                                          \
   , /* No EQUAL */ )                                                         \


// TODO: set all & remove all function

#endif // MSTARLIB_PRIOQUEUE_H
