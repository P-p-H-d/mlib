/*
 * M*LIB - ALGO module
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
#ifndef __M_ALGO_H
#define __M_ALGO_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "m-core.h"

#define ALGO_DEF(name, cont_oplist)                                     \
  ALGOI_DEF(name, M_GET_TYPE cont_oplist, cont_oplist,                  \
            M_GET_SUBTYPE cont_oplist, M_GET_OPLIST cont_oplist, M_GET_IT_TYPE cont_oplist)

#define ALGO_MAP(container, cont_oplist, ...)                  \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                  \
  (ALGOI_MAP(container, cont_oplist, __VA_ARGS__),             \
   ALGOI_MAP_ARG(name, cont_oplist, __VA_ARGS__ ))

/********************************** INTERNAL ************************************/

#define ALGOI_DEF(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
                                                                        \
  static inline void M_C(name, _find) (it_t it, container_t l, const type_t data) \
  {                                                                     \
    for (M_GET_IT_FIRST cont_oplist (it, l);                            \
         !M_GET_IT_END_P cont_oplist (it) ;                             \
         M_GET_IT_NEXT cont_oplist (it)) {                              \
      if (M_GET_EQUAL type_oplist (*M_GET_IT_CREF cont_oplist (it), data)) \
        return ;                                                        \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline bool M_C(name, _contains) (container_t l, const type_t data) \
  {                                                                     \
    it_t it;                                                            \
    M_C(name,_find)(it, l, data);                                       \
    return !M_GET_IT_END_P cont_oplist (it);                            \
  }                                                                     \
                                                                        \
  static inline size_t M_C(name, _count) (container_t l, const type_t data) \
  {                                                                     \
    it_t it;                                                            \
    size_t count = 0;                                                   \
    for (M_GET_IT_FIRST cont_oplist (it, l);                            \
         !M_GET_IT_END_P cont_oplist (it) ;                             \
         M_GET_IT_NEXT cont_oplist (it)) {                              \
      if (M_GET_EQUAL type_oplist (*M_GET_IT_CREF cont_oplist (it), data)) \
        count++ ;                                                       \
    }                                                                   \
    return count;                                                       \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _map) (container_t l,                    \
                                      void (*f)(type_t const) )         \
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        f(*item);                                                       \
      }                                                                 \
  }                                                                     \
                                                                        \
  M_IF_METHOD(CMP, type_oplist)(                                        \
  static inline type_t *M_C(name, _min) (/*const*/ container_t l)       \
  {                                                                     \
    type_t *min = NULL;                                                 \
    for M_EACH(cref, l, cont_oplist) {                                  \
        if (min == NULL ||                                              \
            M_GET_CMP type_oplist (*min, *cref) > 0)                    \
          min = cref;                                                   \
      }                                                                 \
    return min;                                                         \
  }                                                                     \
                                                                        \
  static inline type_t *M_C(name, _max) (/*const*/ container_t l)       \
  {                                                                     \
    type_t *max = NULL;                                                 \
    for M_EACH(cref, l, cont_oplist) {                                  \
        if (max == NULL ||                                              \
            M_GET_CMP type_oplist (*max, *cref) < 0)                    \
          max = cref;                                                   \
      }                                                                 \
    return max;                                                         \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _minmax) (type_t **min_p, type_t **max_p, \
                                         /*const*/ container_t l)       \
  {                                                                     \
    type_t *min = NULL;                                                 \
    type_t *max = NULL;                                                 \
    for M_EACH(cref, l, cont_oplist) {                                  \
        if (min == NULL ||                                              \
            M_GET_CMP type_oplist (*min, *cref) > 0)                    \
          min = cref;                                                   \
        if (max == NULL ||                                              \
            M_GET_CMP type_oplist (*max, *cref) < 0)                    \
          max = cref;                                                   \
      }                                                                 \
    *min_p = min;                                                       \
    *max_p = max;                                                       \
  }                                                                     \
                                                                        \
  /* Sort can be generated from 3 algorithms: */                        \
  /*  - a specialized version defined by the container */               \
  /*  - an insertion sort (need 'previous' method) */                   \
  /*  - a selection sort */                                             \
  M_IF_METHOD(SORT, cont_oplist)(                                       \
  /* optimized sort for container */                                    \
  static inline int M_C(name,_sort_cmp)(const type_t*a,const type_t*b) {\
    return M_GET_CMP type_oplist(*a, *b);                               \
  }                                                                     \
  static inline void M_C(name, _sort)(container_t l)                    \
  {                                                                     \
    M_GET_SORT cont_oplist(l, M_C(name, _sort_cmp));                    \
  }                                                                     \
  ,                                                                     \
  M_IF_METHOD(IT_PREVIOUS, cont_oplist)(                                \
  /* generic insertion sort */                                          \
  static inline void M_C(name, _sort)(container_t l)                    \
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    it_t it21;                                                          \
    for(M_GET_IT_FIRST cont_oplist (it1, l);                            \
        !M_GET_IT_LAST_P cont_oplist (it1);                             \
        M_GET_IT_NEXT cont_oplist (it1))  {                             \
      type_t x; /* Do not use SET, as it is a MOVE operation */         \
      memcpy (&x, M_GET_IT_CREF cont_oplist (it1), sizeof (type_t));    \
      M_GET_IT_SET cont_oplist (it2, it1);                              \
      M_GET_IT_SET cont_oplist (it21, it1);                             \
      M_GET_IT_PREVIOUS cont_oplist (it2);                              \
      while (!M_GET_IT_END_P cont_oplist (it2)                          \
             && M_GET_CMP type_oplist (*M_GET_IT_CREF cont_oplist (it2), \
                                       x) > 0) {                        \
        memcpy(M_GET_IT_REF cont_oplist (it21),                         \
               M_GET_IT_CREF cont_oplist (it2), sizeof (type_t) );      \
        M_GET_IT_SET cont_oplist (it21, it2);                           \
        M_GET_IT_PREVIOUS cont_oplist (it2);                            \
      }                                                                 \
      memcpy(M_GET_IT_REF cont_oplist (it21), &x, sizeof (type_t) );    \
    }                                                                   \
  }                                                                     \
  ,                                                                     \
  /* generic selection sort */                                          \
  static inline void M_C(name, _sort)(container_t l)                    \
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    for(M_GET_IT_FIRST cont_oplist (it1, l);                            \
        !M_GET_IT_LAST_P cont_oplist (it1);                             \
        M_GET_IT_NEXT cont_oplist (it1))  {                             \
      it_t it_min;                                                      \
      M_GET_IT_SET cont_oplist (it_min, it1);                           \
      M_GET_IT_SET cont_oplist (it2, it1);                              \
      for(M_GET_IT_NEXT cont_oplist (it2) ;                             \
          !M_GET_IT_END_P cont_oplist (it2);                            \
          M_GET_IT_NEXT cont_oplist (it2)) {                            \
        if (M_GET_CMP type_oplist (*M_GET_IT_CREF cont_oplist (it2),    \
                                   *M_GET_IT_CREF cont_oplist (it_min)) < 0) { \
          M_GET_IT_SET cont_oplist (it_min, it2);                       \
        }                                                               \
      }                                                                 \
      if (M_GET_IT_EQUAL_P cont_oplist (it_min, it1) == false) {        \
        type_t x; /* Do not use SET, as it is a MOVE operation */       \
        memcpy (&x, M_GET_IT_CREF cont_oplist (it1), sizeof (type_t));  \
        memcpy (M_GET_IT_REF cont_oplist (it1),                         \
                M_GET_IT_CREF cont_oplist (it_min), sizeof (type_t));   \
        memcpy (M_GET_IT_REF cont_oplist (it_min), &x, sizeof (type_t)); \
      }                                                                 \
    }                                                                   \
  }                                                                     \
                                                                        ) /* IF IT_PREVIOUS METHOD */ \
                                                                        ) /* IF SORT METHOD */ \
  ,)  /* IF CMP METHOD */                                               \
                                                                        \
  M_IF_METHOD(EXT_ALGO, type_oplist)(                                   \
            M_GET_EXT_ALGO type_oplist (name, cont_oplist, type_oplist) \
            , )                                                         \

//TODO: const_iterator & CM_EACH missing...
//TODO: Algorithm missing
// nth_element, ...

#define ALGOI_MAP(container, cont_oplist, func)                         \
  for M_EACH(item, l, cont_oplist) {                                    \
    func(*item);                                                        \
  }                                                                     \
  
#define ALGOI_MAP_ARG(container, cont_oplist, func, ...)                \
  for M_EACH(item, l, cont_oplist) {                                    \
    func(__VA_ARGS, *item);                                             \
  }                                                                     \

#endif
