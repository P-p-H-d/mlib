/*
 * M*LIB - ALGO module
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
#ifndef MSTARLIB_ALGO_H
#define MSTARLIB_ALGO_H

#include "m-core.h"

/* Define different kind of basic algorithms named 'name' over the container
   which oplist is 'cont_oplist' as static inline functions.
   USAGE:
   ALGO_DEF(algogName, containerOplist|type if oplist has been registered) */
#define ALGO_DEF(name, cont_oplist)             \
  ALGOI_DEF(name, M_GLOBAL_OPLIST(cont_oplist))


/* Map a function or a macro to all elements of a container.
   USAGE:
   ALGO_MAP(container, containerOplist, function[, extra arguments of function]) */
#define ALGO_MAP(container, cont_oplist, ...)                           \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ALGOI_MAP(container, M_GLOBAL_OPLIST(cont_oplist), __VA_ARGS__),     \
   ALGOI_MAP_ARG(container, M_GLOBAL_OPLIST(cont_oplist), __VA_ARGS__ ))


/* Extract a subset of a container to fill in another container.
   USAGE:
   ALGO_EXTRACT(contDst, contDstOplist, contSrc, contSrcOplist, function
               [, extra arguments of function]) */
// TODO: without 'func' parameter, extract all.
#define ALGO_EXTRACT(contD, contDop, contS, contSop, ...)               \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ALGOI_EXTRACT(contD, M_GLOBAL_OPLIST(contDop), M_GLOBAL_OPLIST(contS), contSop, __VA_ARGS__), \
   ALGOI_EXTRACT_ARG(contD, M_GLOBAL_OPLIST(contDop), M_GLOBAL_OPLIST(contS), contSop, __VA_ARGS__ ))


/* Perform a Reduce operation over a container.
   USAGE:
   ALGO_REDUCE(dstVar, container, contOplist, reduceFunc
               [, mapFunc[, extraParameters of map function]]) */
#define ALGO_REDUCE(dest, cont, contOp,  ...)                           \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ALGOI_REDUCE(dest, M_GLOBAL_OPLIST(cont), M_GLOBAL_OPLIST(contOp), __VA_ARGS__), \
   M_IF_NARGS_EQ2(__VA_ARGS__)                                          \
   (ALGOI_REDUCE_MAP(dest, M_GLOBAL_OPLIST(cont), M_GLOBAL_OPLIST(contOp), __VA_ARGS__), \
    ALGOI_REDUCE_MAP_ARG(dest, M_GLOBAL_OPLIST(cont), M_GLOBAL_OPLIST(contOp), __VA_ARGS__) ) )


/* Initialize & set a container with a variable array list.
   USAGE:
   ALGO_INIT_VA(container, containerOplist, param1[, param2[, ...]]) */
#define ALGO_INIT_VA(dest, contOp, ...)                         \
  ALGO_INIT_VAI(dest, M_GLOBAL_OPLIST(contOp), __VA_ARGS__)

/* Define, initialize, set & clear a container with a variable array list.
   USAGE:
   ALGO_LET_INIT_VA(container, containerOplist, param1[, param2[, ...]]) {
        // Stuff with container
   } */
#define ALGO_LET_INIT_VA(dest, contOp, ...)                     \
  ALGO_LET_INIT_VAI(dest, M_GLOBAL_OPLIST(contOp), __VA_ARGS__)


/********************************** INTERNAL ************************************/

#define ALGOI_DEF(name, cont_oplist)                                    \
  ALGOI_DEF2(name, M_GET_TYPE cont_oplist, cont_oplist,                 \
             M_GET_SUBTYPE cont_oplist, M_GET_OPLIST cont_oplist, M_GET_IT_TYPE cont_oplist)

#define ALGOI_DEF2(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
                                                                        \
  /* It supposes that the container is not sorted */                    \
  static inline void                                                    \
  M_C(name, _find) (it_t it, container_t l, type_t const data)          \
  {                                                                     \
    for (M_GET_IT_FIRST cont_oplist (it, l);                            \
         !M_GET_IT_END_P cont_oplist (it) ;                             \
         M_GET_IT_NEXT cont_oplist (it)) {                              \
      if (M_GET_EQUAL type_oplist (*M_GET_IT_CREF cont_oplist (it), data)) \
        return ;                                                        \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _contain_p) (container_t l, type_t const data)              \
  {                                                                     \
    it_t it;                                                            \
    M_C(name,_find)(it, l, data);                                       \
    return !M_GET_IT_END_P cont_oplist (it);                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _find_if) (it_t it, container_t l, bool (*func)(type_t const)) \
  {                                                                     \
    for (M_GET_IT_FIRST cont_oplist (it, l);                            \
         !M_GET_IT_END_P cont_oplist (it) ;                             \
         M_GET_IT_NEXT cont_oplist (it)) {                              \
      if (func (*M_GET_IT_CREF cont_oplist (it)))                       \
        return ;                                                        \
    }                                                                   \
  }                                                                     \
                                                                        \
  /* For the definition of _find_last, if the methods                   \
     PREVIOUS & IT_LAST are defined, then search backwards */           \
  M_IF_METHOD2(PREVIOUS, IT_LAST, cont_oplist)                          \
  (                                                                     \
  static inline void                                                    \
  M_C(name, _find_last) (it_t it, container_t l, type_t const data)     \
  {                                                                     \
    for (M_GET_IT_LAST cont_oplist (it, l);                             \
         !M_GET_IT_END_P cont_oplist (it) ;                             \
         M_GET_IT_PREVIOUS cont_oplist (it)) {                          \
      if (M_GET_EQUAL type_oplist (*M_GET_IT_CREF cont_oplist (it), data)) \
        /* We can stop as soon as a match is found */                   \
        return;                                                         \
    }                                                                   \
  }                                                                     \
   ,                                                                    \
  /* Otherwise search forward, but don't stop on the first occurence */ \
  static inline void                                                    \
  M_C(name, _find_last) (it_t it, container_t l, type_t const data)     \
  {                                                                     \
    M_GET_IT_END cont_oplist (it, l);                                   \
    it_t it2;                                                           \
    for (M_GET_IT_FIRST cont_oplist (it2, l);                           \
         !M_GET_IT_END_P cont_oplist (it2) ;                            \
         M_GET_IT_NEXT cont_oplist (it2)) {                             \
      if (M_GET_EQUAL type_oplist (*M_GET_IT_CREF cont_oplist (it2), data)) \
        /* We cannot stop as soon as a match is found */                \
        M_GET_IT_SET cont_oplist (it, it2) ;                            \
    }                                                                   \
  }                                                                     \
  ) /* End of alternative of _find_last */                              \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _count) (container_t l, type_t const data)                  \
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
  static inline void                                                    \
  M_C(name, _map) (container_t l, void (*f)(type_t const) )             \
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        f(*item);                                                       \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _reduce) (type_t *dest, container_t l, void (*f)(type_t *, type_t const) ) \
  {                                                                     \
    bool initDone = false;                                              \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (initDone) {                                                 \
          f(dest, *item);						\
        } else {                                                        \
          M_GET_SET type_oplist (*dest, *item);                         \
          initDone = true;                                              \
        }                                                               \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline                                                         \
  void M_C(name, _map_reduce) (type_t *dest,                            \
                               container_t l,                           \
                               void (*redFunc)(type_t*, type_t const),  \
                               void (*mapFunc)(type_t*, type_t const) ) \
  {                                                                     \
    bool initDone = false;                                              \
    type_t tmp;                                                         \
    M_GET_INIT type_oplist (tmp);                                       \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (initDone) {                                                 \
          mapFunc(&tmp, *item);                                         \
          redFunc(dest, tmp);                                           \
        } else {                                                        \
          mapFunc(dest, *item);                                         \
          initDone = true;                                              \
        }                                                               \
      }                                                                 \
    M_GET_CLEAR type_oplist (tmp);                                      \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _any_of_p) (container_t l, bool (*f)(type_t const) )        \
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (f(*item))                                                   \
          return true;                                                  \
    }                                                                   \
    return false;                                                       \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _all_of_p) (container_t l, bool (*f)(type_t const) )        \
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (!f(*item))                                                  \
          return false;                                                 \
    }                                                                   \
    return true;                                                        \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _none_of_p) (container_t l, bool (*f)(type_t const) )       \
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (f(*item))                                                   \
          return false;                                                 \
      }                                                                 \
    return true;                                                        \
  }                                                                     \
                                                                        \
  M_IF_METHOD(CMP, type_oplist)(                                        \
  static inline type_t *                                                \
  M_C(name, _min) (const container_t l)                                 \
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
  static inline type_t *                                                \
  M_C(name, _max) (const container_t l)                                 \
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
  static inline void                                                    \
  M_C(name, _minmax) (type_t **min_p, type_t **max_p,                   \
                      const container_t l)                              \
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
  ALGOI_SORT_DEF(name, container_t, cont_oplist, type_t, type_oplist, it_t, +, _sort) \
  ALGOI_SORT_DEF(name, container_t, cont_oplist, type_t, type_oplist, it_t, -, _sort_dsc) \
                                                                        \
  M_IF_METHOD(IT_REMOVE, cont_oplist)(                                  \
  static inline void                                                    \
  M_C(name, _uniq)(container_t l)                                       \
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    assert(M_C(name, _sort_p)(l));                                      \
    M_GET_IT_FIRST cont_oplist (it1, l);                                \
    M_GET_IT_SET cont_oplist (it2, it1);                                \
    M_GET_IT_NEXT cont_oplist (it2);                                    \
    /* Not efficient for array! */                                      \
    while (!M_GET_IT_END_P cont_oplist (it2)) {                         \
      const type_t *ref1 = M_GET_IT_CREF cont_oplist (it1);             \
      const type_t *ref2 = M_GET_IT_CREF cont_oplist (it2);             \
      if (M_GET_CMP type_oplist (*ref1, *ref2) == 0) {                  \
        M_GET_IT_REMOVE cont_oplist (l, it2);                           \
      } else {                                                          \
        M_GET_IT_SET cont_oplist (it1, it2);                            \
        M_GET_IT_NEXT cont_oplist (it2);                                \
      }                                                                 \
    }                                                                   \
  }                                                                     \
  , /* No IT_REMOVE method */)                                          \
                                                                        \
  , /* No CMP method */)                                                \
                                                                        \
                                                                        \
  M_IF_METHOD(EXT_ALGO, type_oplist)(                                   \
            M_GET_EXT_ALGO type_oplist (name, cont_oplist, type_oplist) \
            , )                                                         \
                                                                        \
  M_IF_METHOD(ADD, type_oplist)(                                        \
  static inline void M_C(name, _add) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_GET_IT_FIRST cont_oplist (itSrc, src) ,                      \
           M_GET_IT_FIRST cont_oplist (itDst, dst) ;                    \
         !M_GET_IT_END_P cont_oplist (itSrc)                            \
           && !M_GET_IT_END_P cont_oplist (itDst) ;                     \
         M_GET_IT_NEXT cont_oplist (itSrc),                             \
           M_GET_IT_NEXT cont_oplist (itDst) ) {                        \
      type_t *dstItem = M_GET_IT_REF cont_oplist (itDst);               \
      const type_t *srcItem = M_GET_IT_REF cont_oplist (itSrc);         \
      M_GET_ADD type_oplist (*dstItem, *dstItem, *srcItem);             \
    }                                                                   \
  }                                                                     \
  , /* NO_ADD METHOD */ )                                               \
                                                                        \
  M_IF_METHOD(SUB, type_oplist)(                                        \
  static inline void M_C(name, _sub) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_GET_IT_FIRST cont_oplist (itSrc, src) ,                      \
           M_GET_IT_FIRST cont_oplist (itDst, dst) ;                    \
         !M_GET_IT_END_P cont_oplist (itSrc)                            \
           && !M_GET_IT_END_P cont_oplist (itDst) ;                     \
         M_GET_IT_NEXT cont_oplist (itSrc),                             \
           M_GET_IT_NEXT cont_oplist (itDst) ) {                        \
      type_t *dstItem = M_GET_IT_REF cont_oplist (itDst);               \
      const type_t *srcItem = M_GET_IT_REF cont_oplist (itSrc);         \
      M_GET_SUB type_oplist (*dstItem, *dstItem, *srcItem);             \
    }                                                                   \
  }                                                                     \
  , /* NO_SUB METHOD */ )                                               \
                                                                        \
  M_IF_METHOD(MUL, type_oplist)(                                        \
  static inline void M_C(name, _mul) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_GET_IT_FIRST cont_oplist (itSrc, src) ,                      \
           M_GET_IT_FIRST cont_oplist (itDst, dst) ;                    \
         !M_GET_IT_END_P cont_oplist (itSrc)                            \
           && !M_GET_IT_END_P cont_oplist (itDst) ;                     \
         M_GET_IT_NEXT cont_oplist (itSrc),                             \
           M_GET_IT_NEXT cont_oplist (itDst) ) {                        \
      type_t *dstItem = M_GET_IT_REF cont_oplist (itDst);               \
      const type_t *srcItem = M_GET_IT_REF cont_oplist (itSrc);         \
      M_GET_MUL type_oplist (*dstItem, *dstItem, *srcItem);             \
    }                                                                   \
  }                                                                     \
  , /* NO_MUL METHOD */ )                                               \
                                                                        \
  M_IF_METHOD(DIV, type_oplist)(                                        \
  static inline void M_C(name, _div) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_GET_IT_FIRST cont_oplist (itSrc, src) ,                      \
           M_GET_IT_FIRST cont_oplist (itDst, dst) ;                    \
         !M_GET_IT_END_P cont_oplist (itSrc)                            \
           && !M_GET_IT_END_P cont_oplist (itDst) ;                     \
         M_GET_IT_NEXT cont_oplist (itSrc),                             \
           M_GET_IT_NEXT cont_oplist (itDst) ) {                        \
      type_t *dstItem = M_GET_IT_REF cont_oplist (itDst);               \
      const type_t *srcItem = M_GET_IT_REF cont_oplist (itSrc);         \
      M_GET_DIV type_oplist  (*dstItem, *dstItem, *srcItem);            \
    }                                                                   \
  }                                                                     \
  , /* NO_DIV METHOD */ )                                               \
                                                                        \

#define ALGOI_SORT_DEF(name, container_t, cont_oplist, type_t, type_oplist, it_t, order, sort_name) \
                                                                        \
  static inline int M_C3(name,sort_name,_cmp)(const type_t*a,const type_t*b) { \
    return order M_GET_CMP type_oplist(*a, *b);                         \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(name,sort_name,_p)(const container_t l)                          \
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    M_GET_IT_FIRST cont_oplist (it1, l);                                \
    M_GET_IT_SET cont_oplist (it2, it1);                                \
    M_GET_IT_NEXT cont_oplist (it2);                                    \
    while (!M_GET_IT_END_P cont_oplist (it2)) {                         \
      const type_t *ref1 = M_GET_IT_CREF cont_oplist (it1);             \
      const type_t *ref2 = M_GET_IT_CREF cont_oplist (it2);             \
      if (!(M_C3(name,sort_name,_cmp)(ref1, ref2) <= 0)) {              \
        return false;                                                   \
      }                                                                 \
      M_GET_IT_SET cont_oplist (it1, it2);                              \
      M_GET_IT_NEXT cont_oplist (it2);                                  \
    }                                                                   \
    return true;                                                        \
  }                                                                     \
                                                                        \
  /* Sort can be generated from 3 algorithms: */                        \
  /*  - a specialized version defined by the container */               \
  /*  - an unstable merge sort (need 'splice_back' method) */           \
  /*  - an insertion sort (need 'previous' method) */                   \
  /*  - a selection sort */                                             \
  M_IF_METHOD(SORT, cont_oplist)(                                       \
  /* optimized sort for container */                                    \
  static inline void M_C(name,sort_name)(container_t l)                 \
  {                                                                     \
    M_GET_SORT cont_oplist(l, M_C3(name,sort_name,_cmp));               \
  }                                                                     \
  ,                                                                     \
  M_IF_METHOD2(SPLICE_BACK, SPLICE_AT, cont_oplist)(                    \
  /* MERGE SORT (unstable) */                                           \
  static inline void M_C3(name,sort_name,_split)(container_t l1, container_t l2, container_t l) \
  {                                                                     \
    it_t it;                                                            \
    bool b = false;                                                     \
    for (M_GET_IT_FIRST cont_oplist(it, l);                             \
         !M_GET_IT_END_P cont_oplist (it);) {                           \
      M_GET_SPLICE_BACK cont_oplist(b ? l1 : l2, l, it);                \
      b = !b;                                                           \
    }                                                                   \
    /* assert(M_GET_EMPTY_P cont_oplist(l)); */                         \
  }                                                                     \
                                                                        \
  static inline void M_C3(name,sort_name,_merge)(container_t l, container_t l1, container_t l2) \
  {                                                                     \
    /* assert(M_GET_EMPTY_P cont_oplist(l)); */                         \
    it_t it;                                                            \
    it_t it1;                                                           \
    it_t it2;                                                           \
    M_GET_IT_END cont_oplist (it, l);                                   \
    M_GET_IT_FIRST cont_oplist(it1, l1);                                \
    M_GET_IT_FIRST cont_oplist(it2, l2);                                \
    while (true) {                                                      \
      int c = M_C3(name,sort_name,_cmp)(M_GET_IT_CREF cont_oplist (it1), \
                                        M_GET_IT_CREF cont_oplist (it2)); \
      if (c <= 0) {                                                     \
        M_GET_SPLICE_AT cont_oplist (l, it, l1, it1);                   \
        if (M_UNLIKELY (M_GET_IT_END_P cont_oplist (it1))) {            \
          while (!M_GET_IT_END_P cont_oplist (it2)) {                   \
            M_GET_SPLICE_AT cont_oplist (l, it, l2, it2);               \
          }                                                             \
          return;                                                       \
        }                                                               \
      } else {                                                          \
        M_GET_SPLICE_AT cont_oplist (l, it, l2, it2);                   \
        if (M_UNLIKELY (M_GET_IT_END_P cont_oplist (it2))) {            \
          while (!M_GET_IT_END_P cont_oplist (it1)) {                   \
            M_GET_SPLICE_AT cont_oplist (l, it, l1, it1);               \
          }                                                             \
          return;                                                       \
        }                                                               \
      }                                                                 \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void M_C(name,sort_name)(container_t l)                 \
  {                                                                     \
    container_t l1;                                                     \
    container_t l2;                                                     \
    it_t it;                                                            \
    it_t it1;                                                           \
    it_t it2;                                                           \
    /* First deal with 0, 1, or 2 size container */                     \
    M_GET_IT_FIRST cont_oplist (it, l);                                 \
    if (M_UNLIKELY (M_GET_IT_END_P cont_oplist(it)))                    \
      return;                                                           \
    M_GET_IT_SET cont_oplist (it1, it);                                 \
    M_GET_IT_NEXT cont_oplist (it);                                     \
    if (M_UNLIKELY (M_GET_IT_END_P cont_oplist(it)))                    \
      return;                                                           \
    M_GET_IT_SET cont_oplist (it2, it);                                 \
    M_GET_IT_NEXT cont_oplist (it);                                     \
    if (M_UNLIKELY (M_GET_IT_END_P cont_oplist(it))) {                  \
      /* Two elements */                                                \
      int c = M_C3(name,sort_name,_cmp)(M_GET_IT_CREF cont_oplist (it1), \
                                        M_GET_IT_CREF cont_oplist (it2)); \
      if (c > 0) {                                                      \
        /* SWAP */                                                      \
        M_GET_SPLICE_BACK cont_oplist (l, l, it2);                      \
      }                                                                 \
      return;                                                           \
    }                                                                   \
    /* Container length is greater than 2: split, sort & merge */       \
    M_GET_INIT cont_oplist(l1);                                         \
    M_GET_INIT cont_oplist(l2);                                         \
    M_C3(name,sort_name,_split)(l1, l2, l);                             \
    M_C(name,sort_name)(l1);                                            \
    M_C(name,sort_name)(l2);                                            \
    M_C3(name,sort_name,_merge)(l, l1, l2);                             \
    M_GET_CLEAR cont_oplist(l2);                                        \
    M_GET_CLEAR cont_oplist(l1);                                        \
  }                                                                     \
                                        ,                               \
  M_IF_METHOD(IT_PREVIOUS, cont_oplist)(                                \
  /* generic insertion sort */                                          \
  static inline void M_C(name,sort_name)(container_t l)                 \
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
             && !(M_C3(name,sort_name,_cmp)(M_GET_IT_CREF cont_oplist (it2), \
                                            M_CONST_CAST(type_t, &x)) <= 0)) { \
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
  static inline void M_C(name,sort_name)(container_t l)                 \
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
        if (M_C3(name,sort_name,_cmp) (M_GET_IT_CREF cont_oplist (it2), \
                                       M_GET_IT_CREF cont_oplist (it_min)) < 0) { \
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
                                                                        ) /* SPLICE BACK METHOD */ \
                                                                        ) /* IF SORT METHOD */ \
  /* Compute the union of two ***sorted*** containers  */               \
  M_IF_METHOD(IT_INSERT, cont_oplist)(                                  \
  static inline void                                                    \
  M_C3(name,sort_name,_union)(container_t dst, const container_t src)   \
  {									\
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    assert(M_C3(name,sort_name,_p)(dst));                               \
    assert(M_C3(name,sort_name,_p)(src));                               \
    M_GET_IT_FIRST cont_oplist (itSrc, src);				\
    M_GET_IT_FIRST cont_oplist (itDst, dst);				\
    while (!M_GET_IT_END_P cont_oplist (itSrc)				\
           && !M_GET_IT_END_P cont_oplist (itDst)) {			\
      const type_t *objSrc = M_GET_IT_CREF cont_oplist (itSrc);		\
      const type_t *objDst = M_GET_IT_CREF cont_oplist (itDst);		\
      int cmp = M_C3(name,sort_name,_cmp)(objDst, objSrc);              \
      if (cmp == 0) {							\
	M_GET_IT_NEXT cont_oplist (itSrc);				\
	M_GET_IT_NEXT cont_oplist (itDst);				\
      } else if (cmp < 0) {						\
	M_GET_IT_NEXT cont_oplist (itDst);				\
      } else {								\
	/* insert objSrc before */					\
	/* current implementations insert after... */			\
	M_GET_IT_INSERT cont_oplist (dst, itDst, *objSrc);		\
	M_GET_IT_NEXT cont_oplist (itSrc);				\
      }									\
    }									\
    while (!M_GET_IT_END_P cont_oplist (itSrc)) {			\
      type *objSrc = M_GET_REF cont_oplist (itSrc);			\
      M_GET_PUSH cont_oplist (dst, *objSrc);				\
      M_GET_IT_NEXT cont_oplist (itSrc);				\
    }									\
  }									\
  , /* NO IT_INSERT */ )         					\
									\
  /* Compute the intersection of two ***sorted*** containers  */        \
  M_IF_METHOD(IT_REMOVE, cont_oplist)(                                  \
  static inline void                                                    \
  M_C3(name,sort_name,_intersect)(container_t dst, const container_t src) \
  {									\
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    assert(M_C3(name,sort_name,_p)(dst));                               \
    assert(M_C3(name,sort_name,_p)(src));                               \
    M_GET_IT_FIRST cont_oplist (itSrc, src);				\
    M_GET_IT_FIRST cont_oplist (itDst, dst);				\
    /* TODO: Not optimized at all for array ! O(n^2) */			\
    while (!M_GET_IT_END_P cont_oplist (itSrc)				\
           && !M_GET_IT_END_P cont_oplist (itDst)) {			\
      const type_t *objSrc = M_GET_IT_CREF cont_oplist (itSrc);		\
      const type_t *objDst = M_GET_IT_CREF cont_oplist (itDst);		\
      int cmp = M_C3(name,sort_name,_cmp)(objDst, objSrc);              \
      if (cmp == 0) {							\
	/* Keep it */							\
	M_GET_IT_NEXT cont_oplist (itSrc);				\
	M_GET_IT_NEXT cont_oplist (itDst);				\
      } else if (cmp < 0) {						\
	M_GET_IT_REMOVE cont_oplist (dst, itDst);			\
      } else {								\
	M_GET_IT_NEXT cont_oplist (itSrc);				\
      }									\
    }									\
    while (!M_GET_IT_END_P cont_oplist (itDst)) {			\
      M_GET_IT_REMOVE cont_oplist (dst, itDst);				\
    }									\
  }									\
  , /* NO IT_REMOVE */ )


//TODO: Algorithm missing
// nth_element ( http://softwareengineering.stackexchange.com/questions/284767/kth-selection-routine-floyd-algorithm-489 )
//, average, find_if, count_if, ...

#define ALGOI_MAP(container, cont_oplist, func) do {                    \
    for M_EACH(item, container, cont_oplist) {                          \
      func(*item);                                                      \
    }                                                                   \
  } while (0)

#define ALGOI_MAP_ARG(container, cont_oplist, func, ...) do {           \
    for M_EACH(item, container, cont_oplist) {                          \
      func(__VA_ARGS__, *item);                                         \
    }                                                                   \
  } while (0)

#define ALGOI_EXTRACT(contDst, contDstOplist,                           \
                      contSrc, contSrcOplist,                           \
                      condFunc) do {                                    \
    M_GET_CLEAN contDstOplist (contDst);                                \
    for M_EACH(item, contSrc, contSrcOplist) {                          \
        if (condFunc (*item))                                           \
          M_GET_PUSH contDstOplist (contDst, *item);                    \
    }                                                                   \
    M_IF_METHOD(REVERSE, contDstOplist) (M_GET_REVERSE contDstOplist (contDstOplist);, ) \
  } while (0)

#define ALGOI_EXTRACT_ARG(contDst, contDstOplist,                       \
                          contSrc, contSrcOplist,                       \
                          condFunc, ...) do {                           \
    M_GET_CLEAN contDstOplist (contDst);                                \
    for M_EACH(item, contSrc, contSrcOplist) {                          \
        if (condFunc (__VA_ARGS__, *item))                              \
          M_GET_PUSH contDstOplist (contDst, *item);                    \
    }                                                                   \
    M_IF_METHOD(REVERSE, contDstOplist) (M_GET_REVERSE contDstOplist (contDstOplist);, ) \
  } while (0)

/* The different special patterns which will interpret as special */ 
#define ALGOI_PATTERN_and_and ,
#define ALGOI_PATTERN_or_or ,
#define ALGOI_PATTERN_sum_sum ,
#define ALGOI_PATTERN_sum_add ,
/* Test function for theses patterns */
#define ALGOI_REDUCE_IS_FUNC(refFunc, reduceFunc)       \
  M_COMMA_P(M_C4(ALGOI_PATTERN_, refFunc, _, reduceFunc))

/* The special function handler */
#define ALGOI_REDUCE_AND(a,b) ((a) &= (b))
#define ALGOI_REDUCE_OR(a,b)  ((a) |= (b))
#define ALGOI_REDUCE_SUM(a,b) ((a) += (b))

/* Return the method associated to a reduce operation.
   It returns the special function handler if function is and, or, sum or add.
   Otherwise it returns the original function */
#define ALGOI_REDUCE_FUNC(reduceFunc)            \
  M_IF(ALGOI_REDUCE_IS_FUNC(and, reduceFunc))    \
  (ALGOI_REDUCE_AND,                             \
   M_IF(ALGOI_REDUCE_IS_FUNC(or, reduceFunc))    \
   (ALGOI_REDUCE_OR,                             \
    M_IF(ALGOI_REDUCE_IS_FUNC(sum, reduceFunc))  \
    (ALGOI_REDUCE_SUM, reduceFunc)               \
    )                                            \
   )

#define ALGOI_REDUCE(dest, cont, cont_oplist, reduceFunc) do {  \
    bool m_init_done = false;                                   \
    for M_EACH(item, cont, cont_oplist) {                       \
        if (m_init_done) {                                      \
          ALGOI_REDUCE_FUNC(reduceFunc) (dest, *item);          \
        } else {                                                \
          M_GET_SET M_GET_OPLIST cont_oplist (dest, *item);     \
          m_init_done = true;                                   \
        }                                                       \
    }                                                           \
  } while (0)

#define ALGOI_REDUCE_MAP(dest, cont, cont_oplist, reduceFunc, mapFunc) do { \
    bool m_init_done = false;                                           \
    M_GET_SUBTYPE cont_oplist m_tmp;                                    \
    M_GET_INIT M_GET_OPLIST cont_oplist (m_tmp);                        \
    for M_EACH(item, cont, cont_oplist) {                               \
        mapFunc(m_tmp, *item);                                          \
        if (m_init_done) {                                              \
          ALGOI_REDUCE_FUNC(reduceFunc) (dest, m_tmp);                  \
        } else {                                                        \
          M_GET_SET M_GET_OPLIST cont_oplist (dest, m_tmp);             \
          m_init_done = true;                                           \
        }                                                               \
      }                                                                 \
    M_GET_CLEAR M_GET_OPLIST cont_oplist (m_tmp);                       \
  } while (0)

#define ALGOI_REDUCE_MAP_ARG(dest, cont, cont_oplist, reduceFunc, mapFunc, ...) do { \
    bool m_init_done = false;                                           \
    M_GET_SUBTYPE cont_oplist m_tmp;                                    \
    M_GET_INIT M_GET_OPLIST cont_oplist (m_tmp);                        \
    for M_EACH(item, cont, cont_oplist) {                               \
        mapFunc(m_tmp, __VA_ARGS__, *item);                             \
        if (m_init_done) {                                              \
          ALGOI_REDUCE_FUNC(reduceFunc) (dest, m_tmp);                  \
        } else {                                                        \
          M_GET_SET M_GET_OPLIST cont_oplist (dest, m_tmp);             \
          m_init_done = true;                                           \
        }                                                               \
      }                                                                 \
    M_GET_CLEAR M_GET_OPLIST cont_oplist (m_tmp);                       \
  } while (0)

#define ALGOI_INIT_VA_FUNC(d, a)                        \
  M_RET_ARG2 d (M_RET_ARG1 d, a) M_DEFERRED_COMMA

#define ALGO_INIT_VAI(dest, contOp, ...)                                \
  (void)(M_GET_INIT contOp (dest) M_DEFERRED_COMMA                      \
         M_MAP2(ALGOI_INIT_VA_FUNC, (dest, M_GET_PUSH contOp, ) , __VA_ARGS__) \
         true)

#define ALGO_LET_INIT_VAI(dest, contOp, ...)                            \
  for(bool cont = true; cont ; /* unused */)                            \
    for(M_GET_TYPE contOp dest;                                         \
        cont && (ALGO_INIT_VAI (dest, contOp, __VA_ARGS__), true); \
        (M_GET_CLEAR contOp (dest), cont = false))

#endif
