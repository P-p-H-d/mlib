/*
 * M*LIB - ALGO module
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
#ifndef MSTARLIB_ALGO_H
#define MSTARLIB_ALGO_H

#include "m-core.h"

/* Define different kind of basic algorithms named 'name' over the container
   which oplist is 'contOp', as static inline functions.
   USAGE:
   ALGO_DEF(algogName, containerOplist|type if oplist has been registered) */
#define ALGO_DEF(name, cont_oplist)             \
  ALGOI_DEF_P1(name, M_GLOBAL_OPLIST(cont_oplist))


/* Map a function (or a macro) to all elements of a container.
   USAGE:
   ALGO_FOR_EACH(container, containerOplist, function[, extra arguments of function]) */
#define ALGO_FOR_EACH(container, cont_oplist, ...)                      \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ALGOI_FOR_EACH(container, M_GLOBAL_OPLIST(cont_oplist), __VA_ARGS__), \
   ALGOI_FOR_EACH_ARG(container, M_GLOBAL_OPLIST(cont_oplist), __VA_ARGS__ ))


/* Map a function (or a macro) to all elements of a container
   into another container.
   USAGE:
   ALGO_TRANSFORM(contDst, contDOplist, contSrc, contSrcOplist,
                  function[, extra arguments of function]) */
#define ALGO_TRANSFORM(contD, contDop, contS, contSop, ...)             \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ALGOI_TRANSFORM(contD, M_GLOBAL_OPLIST(contDop), contS, M_GLOBAL_OPLIST(contSop), __VA_ARGS__), \
   ALGOI_TRANSFORM_ARG(contD, M_GLOBAL_OPLIST(contDop), contS, M_GLOBAL_OPLIST(contSop), __VA_ARGS__ ))


/* Extract a subset of a container to copy into another container.
   USAGE:
   ALGO_EXTRACT(contDst, contDstOplist, contSrc, contSrcOplist
               [, function [, extra arguments of function]])  */
#define ALGO_EXTRACT(contD, contDop, contS, ...)			\
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ALGOI_EXTRACT(contD, M_GLOBAL_OPLIST(contDop), M_GLOBAL_OPLIST(contS),  __VA_ARGS__), \
   M_IF_NARGS_EQ2(__VA_ARGS__)						\
   (ALGOI_EXTRACT_FUNC(contD, M_GLOBAL_OPLIST(contDop), M_GLOBAL_OPLIST(contS),  __VA_ARGS__), \
    ALGOI_EXTRACT_ARG(contD, M_GLOBAL_OPLIST(contDop), M_GLOBAL_OPLIST(contS), __VA_ARGS__ )))


/* Perform a Reduce operation over a container.
   USAGE:
   ALGO_REDUCE(dstVar, container, contOplist, reduceFunc
               [, mapFunc[, extraParameters of map function]]) */
#define ALGO_REDUCE(dest, cont, contOp, ...)                            \
  M_IF(M_PARENTHESIS_P(dest))                                           \
  (ALGOI_REDUCE_DISPATCH(M_PAIR_1 dest, M_GLOBAL_OPLIST(M_PAIR_2 dest), M_GLOBAL_TYPE(M_PAIR_2 dest),cont, M_GLOBAL_OPLIST(contOp), __VA_ARGS__), \
   ALGOI_REDUCE_DISPATCH(dest, M_GET_OPLIST M_GLOBAL_OPLIST(contOp), M_GET_SUBTYPE M_GLOBAL_OPLIST(contOp), cont, contOp, __VA_ARGS__)) \
    

/* Initialize & set a container with a variable array list.
   USAGE:
   ALGO_INIT_VA(container, containerOplist, param1[, param2[, ...]]).
   OBSOLETE: WILL BE DELETED.
 */
#define ALGO_INIT_VA(dest, contOp, ...)                         \
  ALGO_INIT_VAI(dest, M_GLOBAL_OPLIST(contOp), __VA_ARGS__)


/* Define, initialize, set & clear a container with a variable array list.
   USAGE:
   ALGO_LET_INIT_VA(container, containerOplist|type, param1[, param2[, ...]]) {
   // Stuff with container
   } 
   OBSOLETE: WILL BE DELETED.
*/
#define ALGO_LET_INIT_VA(dest, contOp, ...)                     \
  ALGO_LET_INIT_VAI(dest, M_GLOBAL_OPLIST(contOp), __VA_ARGS__)


/* Insert into the container 'contDst' at position 'position' all the values
   of container 'contSrc'.
   USAGE:
   ALGO_INSERT_AT(containerDst, containerDstOPLIST|containerDstType, containerDstIterator, containerSrc, containerSrcOPLIST|containerSrcType)
 */
#define ALGO_INSERT_AT(contDst, contDstOp, position, contSrc, contSrcOp) \
  ALGOI_INSERT_AT(contDst, M_GLOBAL_OPLIST(contDstOp), position, contSrc, M_GLOBAL_OPLIST(contSrcOp) )


/********************************** INTERNAL ************************************/

/* Expand the algorithms */
#define ALGOI_DEF_P1(name, cont_oplist)                                 \
  ALGOI_DEF_P2(name, M_GET_TYPE cont_oplist, cont_oplist,               \
               M_GET_SUBTYPE cont_oplist, M_GET_OPLIST cont_oplist,     \
               M_GET_IT_TYPE cont_oplist)

/* First validate the first oplist */
#define ALGOI_DEF_P2(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
  M_IF_OPLIST(cont_oplist)(ALGOI_DEF_P3, ALGOI_DEF_FAILURE)(name, container_t, cont_oplist, type_t, type_oplist, it_t)

/* First validate the second oplist */
#define ALGOI_DEF_P3(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
  M_IF_OPLIST(type_oplist)(ALGOI_DEF_P4, ALGOI_DEF_FAILURE)(name, container_t, cont_oplist, type_t, type_oplist, it_t)

/* Stop processing with a compilation failure */
#define ALGOI_DEF_FAILURE(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(ALGO_DEF): one of the given argument is not a valid oplist: " M_AS_STR(cont_oplist) " / " M_AS_STR(type_oplist) )

#define ALGOI_DEF_P4(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
									\
  ALGOI_CALLBACK_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
  M_IF_FUNCOBJ(ALGOI_FUNCOBJ_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t)) \
									\
  M_IF_METHOD(EQUAL, type_oplist)(                                      \
  ALGOI_FIND_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t)    \
  , /* NO EQUAL */)                                                     \
									\
  ALGOI_FIND_IF_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, if, M_C(name, _test_cb_t), M_C(name, _eq_cb_t), M_APPLY, M_APPLY) \
  M_IF_FUNCOBJ(ALGOI_FIND_IF_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, fo, M_C(name, _test_obj_t), M_C(name, _eq_obj_t), M_C(name, _test_obj_call), M_C(name, _eq_obj_call))) \
									\
  ALGOI_MAP_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t)	  \
									\
  ALGOI_ALL_OF_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, _, M_C(name, _test_cb_t), M_APPLY) \
  M_IF_FUNCOBJ(ALGOI_ALL_OF_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, _fo_, M_C(name, _test_obj_t), M_C(name, _test_obj_call)) ) \
									\
  /* If there is a IT_REF method, we consider the container as modifiable through iterator */	\
  M_IF_METHOD(IT_REF, cont_oplist)(                                     \
  ALGOI_FILL_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t)    \
  ALGOI_VECTOR_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t)  \
									\
  M_IF_METHOD(CMP, type_oplist)(                                        \
  ALGOI_MINMAX_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
  ALGOI_SORT_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, +, _sort) \
  ALGOI_SORT_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, -, _sort_dsc) \
  M_IF_METHOD(IT_REMOVE, cont_oplist)(                                  \
  ALGOI_REMOVE_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
  , /* No IT_REMOVE method */)					        \
  , /* No CMP method */)                                                \
									\
  M_IF_FUNCOBJ(ALGOI_SORT_DEF_P4(name, container_t, cont_oplist, type_t, type_oplist, it_t, _sort_fo, ALGOI_SORT_CALL_OBJ_P4, ALGOI_SORT_PARAM_OBJ_P4, ALGOI_SORT_ARG_OBJ_P4) ) \
  , /* No IT_REF method */)						\
									\
  M_IF_METHOD(EXT_ALGO, type_oplist)(					\
  M_GET_EXT_ALGO type_oplist (name, cont_oplist, type_oplist)           \
  , /* No EXT_ALGO method */ )						\


#define ALGOI_CALLBACK_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t)\
									\
  typedef bool (*M_C(name, _test_cb_t))(type_t const);			\
  typedef bool (*M_C(name, _eq_cb_t))(type_t const, type_t const);	\
  typedef int  (*M_C(name, _cmp_cb_t))(type_t const, type_t const);	\
  typedef void (*M_C(name, _transform_cb_t))(type_t *, type_t const);	\
  typedef void (*M_C(name, _apply_cb_t))(type_t);			\

#define ALGOI_FUNCOBJ_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
									\
  FUNC_OBJ_ITF_DEF(M_C(name, _test_obj), bool, type_t const)		\
  FUNC_OBJ_ITF_DEF(M_C(name, _eq_obj), bool, type_t const, type_t const )	\
  FUNC_OBJ_ITF_DEF(M_C(name, _cmp_obj), int, type_t const, type_t const )	\
  FUNC_OBJ_ITF_DEF(M_C(name, _transform_obj), void, type_t *, type_t const )	\
  FUNC_OBJ_ITF_DEF(M_C(name, _apply_obj), void, type_t * )		\
  

#define ALGOI_SORT_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, order, sort_name) \
                                                                        \
  static inline int M_C3(name,sort_name,_cmp)(type_t const*a,type_t const*b) { \
    return order M_CALL_CMP(type_oplist, *a, *b);                       \
      }                                                                 \
									\
  ALGOI_SORT_DEF_P4(name, container_t, cont_oplist, type_t, type_oplist, it_t, sort_name, ALGOI_SORT_CALL_CMP_P4, M_EAT, /*empty*/ )

// For classic CMP order of the type oplist
#define ALGOI_SORT_CALL_CMP_P4(name, sort_name, ref1, ref2) M_C3(name, sort_name,_cmp)(ref1, ref2)

// For function object
#define ALGOI_SORT_CALL_OBJ_P4(name, sort_name, ref1, ref2) M_C(name, _cmp_obj_call)(funcobj, *ref1, *ref2)
#define ALGOI_SORT_PARAM_OBJ_P4(name) M_DEFERRED_COMMA M_C(name, _cmp_obj_t) funcobj
#define ALGOI_SORT_ARG_OBJ_P4 M_DEFERRED_COMMA funcobj

#define ALGOI_SORT_DEF_P4(name, container_t, cont_oplist, type_t, type_oplist, it_t, sort_name, cmp_func, cmp_param, cmp_arg) \
                                                                        \
  static inline bool                                                    \
  M_C3(name,sort_name,_p)(const container_t l cmp_param(name))		\
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    M_CALL_IT_FIRST(cont_oplist, it1, l);                               \
    M_CALL_IT_SET(cont_oplist, it2, it1);                               \
    M_CALL_IT_NEXT(cont_oplist, it2);                                   \
    while (!M_CALL_IT_END_P(cont_oplist, it2)) {                        \
      type_t const *ref1 = M_CALL_IT_CREF(cont_oplist, it1);            \
      type_t const *ref2 = M_CALL_IT_CREF(cont_oplist, it2);            \
      if (!(cmp_func(name, sort_name, ref1, ref2) <= 0)) {		\
        return false;                                                   \
      }                                                                 \
      M_CALL_IT_SET(cont_oplist, it1, it2);                             \
      M_CALL_IT_NEXT(cont_oplist, it2);                                 \
    }                                                                   \
    return true;                                                        \
  }                                                                     \
                                                                        \
  /* Sort can be generated from 3 algorithms: */                        \
  /*  - a specialized version defined by the container */               \
  /*  - an unstable merge sort (need 'splice_back' method) */           \
  /*  - an insertion sort (need 'previous' method) */                   \
  /*  - a selection sort */                                             \
  M_IF(M_AND(M_TEST_METHOD_P(SORT, cont_oplist), M_EMPTY_P(cmp_arg)))(	\
    /******** OPTIMIZED SORT FOR CONTAINER *********/			\
  static inline void M_C(name,sort_name)(container_t l)                 \
  {                                                                     \
    M_CALL_SORT(cont_oplist, l, M_C3(name, sort_name,_cmp));		\
  }                                                                     \
  ,                                                                     \
									\
  M_IF_METHOD2(SPLICE_BACK, SPLICE_AT, cont_oplist)(                    \
    /******** MERGE SORT (unstable) ********/				\
    /* NOTE: Only reasonable for lists (To move in m-list.h ?) */	\
  static inline void M_C3(name,sort_name,_split)(container_t l1, container_t l2, container_t l) \
  {                                                                     \
    it_t it;                                                            \
    bool b = false;                                                     \
    /* Split 'l' into 'l1' and 'l2' */					\
    for (M_CALL_IT_FIRST(cont_oplist,it, l);                            \
         !M_CALL_IT_END_P(cont_oplist, it);) {                          \
      M_CALL_SPLICE_BACK(cont_oplist, (b ? l1 : l2), l, it);            \
      b = !b;                                                           \
    }                                                                   \
    /* assert(M_CALL_EMPTY_P (cont_oplistl)); */                        \
  }                                                                     \
                                                                        \
  static inline void M_C3(name,sort_name,_merge)(container_t l, container_t l1, container_t l2 cmp_param(name)) \
  {                                                                     \
    /* Merge into 'l' both sorted containers 'l1' and 'l2'.		\
       'l' is sorted */							\
    /* assert(M_CALL_EMPTY_P (cont_oplist, l)); */                      \
    it_t it;                                                            \
    it_t it1;                                                           \
    it_t it2;                                                           \
    M_CALL_IT_END(cont_oplist, it, l);                                  \
    M_CALL_IT_FIRST(cont_oplist,it1, l1);                               \
    M_CALL_IT_FIRST(cont_oplist,it2, l2);                               \
    while (true) {                                                      \
      /* Compare current elements of the containers l1 and l2 */	\
      int c = cmp_func(name, sort_name, M_CALL_IT_CREF(cont_oplist, it1), \
		       M_CALL_IT_CREF(cont_oplist, it2));		\
      if (c <= 0) {                                                     \
	/* Move the element of l1 in the new container */		\
        M_CALL_SPLICE_AT(cont_oplist, l, it, l1, it1);                  \
        if (M_UNLIKELY (M_CALL_IT_END_P(cont_oplist, it1))) {           \
	  /* Move all remaining elements of l2 in 'l' */		\
          while (!M_CALL_IT_END_P(cont_oplist, it2)) {                  \
            M_CALL_SPLICE_AT(cont_oplist, l, it, l2, it2);              \
          }                                                             \
          return;                                                       \
        }                                                               \
      } else {                                                          \
	/* Move the element of l2 in the new container */		\
        M_CALL_SPLICE_AT(cont_oplist, l, it, l2, it2);                  \
        if (M_UNLIKELY (M_CALL_IT_END_P(cont_oplist, it2))) {           \
	  /* Move all remaining elements of l1 in 'l' */		\
          while (!M_CALL_IT_END_P(cont_oplist, it1)) {                  \
            M_CALL_SPLICE_AT(cont_oplist, l, it, l1, it1);              \
          }                                                             \
          return;                                                       \
        }                                                               \
      }                                                                 \
    }                                                                   \
    /* Cannot occur */							\
  }                                                                     \
                                                                        \
  static inline void M_C(name,sort_name)(container_t l cmp_param(name))	\
  {                                                                     \
    container_t l1;                                                     \
    container_t l2;                                                     \
    it_t it;                                                            \
    it_t it1;                                                           \
    it_t it2;                                                           \
    /* First deal with 0, 1, or 2 size container */                     \
    M_CALL_IT_FIRST(cont_oplist, it, l);                                \
    if (M_UNLIKELY (M_CALL_IT_END_P(cont_oplist, it)))                  \
      return;                                                           \
    M_CALL_IT_SET(cont_oplist, it1, it);                                \
    M_CALL_IT_NEXT(cont_oplist, it);                                    \
    if (M_UNLIKELY (M_CALL_IT_END_P(cont_oplist, it)))                  \
      return;                                                           \
    M_CALL_IT_SET(cont_oplist, it2, it);                                \
    M_CALL_IT_NEXT(cont_oplist, it);                                    \
    if (M_UNLIKELY (M_CALL_IT_END_P(cont_oplist, it))) {                \
      /* Two elements */                                                \
      int c = cmp_func(name, sort_name,					\
		       M_CALL_IT_CREF(cont_oplist, it1),		\
		       M_CALL_IT_CREF(cont_oplist, it2));		\
      if (c > 0) {                                                      \
        /* SWAP */                                                      \
        M_CALL_SPLICE_BACK(cont_oplist, l, l, it2);                     \
      }                                                                 \
      return;                                                           \
    }                                                                   \
    /* Container length is greater than 2: split, sort & merge */       \
    M_CALL_INIT(cont_oplist, l1);                                       \
    M_CALL_INIT(cont_oplist, l2);                                       \
    M_C3(name,sort_name,_split)(l1, l2, l);                             \
    M_C(name,sort_name)(l1 cmp_arg);					\
    M_C(name,sort_name)(l2 cmp_arg);					\
    M_C3(name,sort_name,_merge)(l, l1, l2 cmp_arg);			\
    M_CALL_CLEAR(cont_oplist, l2);                                      \
    M_CALL_CLEAR(cont_oplist, l1);                                      \
  }                                                                     \
                                        ,                               \
  M_IF_METHOD(IT_PREVIOUS, cont_oplist)(                                \
    /******** GENERIC INSERTION SORT *********/				\
  static inline void M_C(name,sort_name)(container_t l cmp_param(name)) \
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    it_t it2p1;                                                         \
    /* NOTE: Do not use SET, this is a MOVE operation */		\
    /* Start from i := 1 */						\
    M_CALL_IT_FIRST(cont_oplist, it1, l);                               \
    M_CALL_IT_NEXT(cont_oplist, it1);                                   \
    while (!M_CALL_IT_END_P(cont_oplist, it1) ) {			\
      type_t x;								\
      /* x := TAB[i] */							\
      memcpy (&x, M_CALL_IT_CREF(cont_oplist, it1), sizeof (type_t));   \
      /* j := i -1 // jp1 := i (= j +1) */				\
      M_CALL_IT_SET(cont_oplist, it2, it1);                             \
      M_CALL_IT_PREVIOUS(cont_oplist, it2);                             \
      M_CALL_IT_SET(cont_oplist, it2p1, it1);                           \
      while (!M_CALL_IT_END_P(cont_oplist, it2)                         \
             && !(cmp_func(name, sort_name,				\
			   M_CALL_IT_CREF(cont_oplist, it2),		\
			   M_CONST_CAST(type_t, &x)) <= 0)) {		\
        /* TAB[jp1=j+1] := TAB[j] */					\
        memcpy(M_CALL_IT_REF(cont_oplist, it2p1),                       \
               M_CALL_IT_CREF(cont_oplist, it2), sizeof (type_t) );     \
        /* jp1 := j (= jp1-1) */					\
        M_CALL_IT_SET(cont_oplist, it2p1, it2);                         \
        M_CALL_IT_PREVIOUS(cont_oplist, it2);                           \
      }                                                                 \
      /* TAB[jp1] := x */						\
      memcpy(M_CALL_IT_REF(cont_oplist, it2p1), &x, sizeof (type_t) );	\
      /* i := i + 1 */							\
      M_CALL_IT_NEXT(cont_oplist, it1);					\
    }                                                                   \
  }                                                                     \
									\
  ,                                                                     \
  /********** GENERIC SELECTION SORT ************/			\
  static inline void M_C(name,sort_name)(container_t l cmp_param(name))	\
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    for(M_CALL_IT_FIRST(cont_oplist, it1, l);                           \
        !M_CALL_IT_LAST_P(cont_oplist, it1);                            \
        M_CALL_IT_NEXT(cont_oplist, it1))  {                            \
      it_t it_min;                                                      \
      M_CALL_IT_SET(cont_oplist, it_min, it1);                          \
      M_CALL_IT_SET(cont_oplist, it2, it1);                             \
      for(M_CALL_IT_NEXT(cont_oplist, it2) ;                            \
          !M_CALL_IT_END_P(cont_oplist, it2);                           \
          M_CALL_IT_NEXT(cont_oplist, it2)) {                           \
        if (cmp_func(name, sort_name, M_CALL_IT_CREF(cont_oplist, it2),	\
		     M_CALL_IT_CREF(cont_oplist, it_min)) < 0) {	\
          M_CALL_IT_SET(cont_oplist, it_min, it2);                      \
        }                                                               \
      }                                                                 \
      if (M_CALL_IT_EQUAL_P(cont_oplist, it_min, it1) == false) {       \
	/* TODO: Use SWAP method of type_oplist if available  */	\
        type_t x; /* Do not use SET, as it is a MOVE operation */       \
        memcpy (&x, M_CALL_IT_CREF(cont_oplist, it1), sizeof (type_t)); \
        memcpy (M_CALL_IT_REF(cont_oplist, it1),                        \
                M_CALL_IT_CREF(cont_oplist, it_min), sizeof (type_t));  \
        memcpy (M_CALL_IT_REF(cont_oplist, it_min), &x, sizeof (type_t)); \
      }                                                                 \
    }                                                                   \
  }                                                                     \
                                                                        ) /* IF IT_PREVIOUS METHOD */ \
                                                                        ) /* SPLICE BACK METHOD */ \
                                                                        ) /* IF SORT METHOD */ \
  /* Compute the union of two ***sorted*** containers  */               \
  M_IF_METHOD(IT_INSERT, cont_oplist)(                                  \
  static inline void                                                    \
  M_C3(name,sort_name,_union)(container_t dst, const container_t src cmp_param(name))   \
  {									\
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    assert(M_C3(name,sort_name,_p)(dst cmp_arg));			\
    assert(M_C3(name,sort_name,_p)(src cmp_arg));			\
    M_CALL_IT_FIRST(cont_oplist, itSrc, src);				\
    M_CALL_IT_FIRST(cont_oplist, itDst, dst);				\
    while (!M_CALL_IT_END_P(cont_oplist, itSrc)				\
           && !M_CALL_IT_END_P(cont_oplist, itDst)) {			\
      type_t const *objSrc = M_CALL_IT_CREF(cont_oplist, itSrc);        \
      type_t const *objDst = M_CALL_IT_CREF(cont_oplist, itDst);        \
      int cmp = cmp_func(name, sort_name, objDst, objSrc);		\
      if (cmp == 0) {							\
	M_CALL_IT_NEXT(cont_oplist, itSrc);				\
	M_CALL_IT_NEXT(cont_oplist, itDst);				\
      } else if (cmp < 0) {						\
	M_CALL_IT_NEXT(cont_oplist, itDst);				\
      } else {								\
	/* insert objSrc before */					\
	/* current implementations insert after... */			\
	M_CALL_IT_INSERT(cont_oplist, dst, itDst, *objSrc);		\
	M_CALL_IT_NEXT(cont_oplist, itSrc);				\
      }									\
    }									\
    while (!M_CALL_IT_END_P(cont_oplist, itSrc)) {			\
      type_t *objSrc = M_CALL_IT_REF(cont_oplist, itSrc);               \
      M_CALL_PUSH(cont_oplist, dst, *objSrc);				\
      M_CALL_IT_NEXT(cont_oplist, itSrc);				\
    }									\
  }									\
  , /* NO IT_INSERT */ )         					\
									\
  /* Compute the intersection of two ***sorted*** containers  */        \
  M_IF_METHOD(IT_REMOVE, cont_oplist)(                                  \
  static inline void                                                    \
  M_C3(name,sort_name,_intersect)(container_t dst, const container_t src cmp_param(name)) \
  {									\
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    assert(M_C3(name,sort_name,_p)(dst cmp_arg));			\
    assert(M_C3(name,sort_name,_p)(src cmp_arg));			\
    M_CALL_IT_FIRST(cont_oplist, itSrc, src);				\
    M_CALL_IT_FIRST(cont_oplist, itDst, dst);				\
    /* TODO: Not optimized at all for array ! O(n^2) */			\
    while (!M_CALL_IT_END_P(cont_oplist, itSrc)				\
           && !M_CALL_IT_END_P(cont_oplist, itDst)) {			\
      type_t const *objSrc = M_CALL_IT_CREF(cont_oplist, itSrc);        \
      type_t const *objDst = M_CALL_IT_CREF(cont_oplist, itDst);        \
      int cmp = cmp_func(name, sort_name, objDst, objSrc);		\
      if (cmp == 0) {							\
	/* Keep it */							\
	M_CALL_IT_NEXT(cont_oplist, itSrc);				\
	M_CALL_IT_NEXT(cont_oplist, itDst);				\
      } else if (cmp < 0) {						\
	M_CALL_IT_REMOVE(cont_oplist, dst, itDst);			\
      } else {								\
	M_CALL_IT_NEXT(cont_oplist, itSrc);				\
      }									\
    }									\
    while (!M_CALL_IT_END_P(cont_oplist, itDst)) {			\
      M_CALL_IT_REMOVE(cont_oplist, dst, itDst);                        \
    }									\
  }									\
  , /* NO IT_REMOVE */ )


#define ALGOI_FIND_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
  /* It supposes that the container is not sorted */                    \
  static inline void                                                    \
  M_C(name, _find_again) (it_t it, type_t const data)                   \
  {                                                                     \
    for ( /*nothing*/ ; !M_CALL_IT_END_P(cont_oplist, it) ;             \
                      M_CALL_IT_NEXT(cont_oplist, it)) {                \
      if (M_CALL_EQUAL(type_oplist, *M_CALL_IT_CREF(cont_oplist, it), data)) \
        return ;                                                        \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _find) (it_t it, container_t const l, type_t const data)    \
  {                                                                     \
    M_CALL_IT_FIRST(cont_oplist, it, l);                                \
    M_C(name, _find_again)(it, data);                                   \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _contain_p) (container_t const l, type_t const data)        \
  {                                                                     \
    it_t it;                                                            \
    M_C(name,_find)(it, l, data);                                       \
    return !M_CALL_IT_END_P(cont_oplist, it);                           \
  }                                                                     \
                                                                        \
  /* For the definition of _find_last, if the methods                   \
     PREVIOUS & IT_LAST are defined, then search backwards */           \
  M_IF_METHOD2(PREVIOUS, IT_LAST, cont_oplist)                          \
  (                                                                     \
   static inline void                                                   \
   M_C(name, _find_last) (it_t it, container_t const l, type_t const data) \
   {                                                                    \
     for (M_CALL_IT_LAST(cont_oplist, it, l);                           \
          !M_CALL_IT_END_P(cont_oplist, it) ;                           \
          M_CALL_IT_PREVIOUS(cont_oplist, it)) {                        \
       if (M_CALL_EQUAL(type_oplist, *M_CALL_IT_CREF(cont_oplist, it), data)) \
         /* We can stop as soon as a match is found */                  \
         return;                                                        \
     }                                                                  \
   }                                                                    \
   ,                                                                    \
   /* Otherwise search forward, but don't stop on the first occurence */ \
   static inline void                                                   \
   M_C(name, _find_last) (it_t it, container_t const l, type_t const data) \
   {                                                                    \
     M_CALL_IT_END(cont_oplist, it, l);                                 \
     it_t it2;                                                          \
     for (M_CALL_IT_FIRST(cont_oplist, it2, l);                         \
          !M_CALL_IT_END_P(cont_oplist, it2) ;                          \
          M_CALL_IT_NEXT(cont_oplist, it2)) {                           \
       if (M_CALL_EQUAL(type_oplist, *M_CALL_IT_CREF(cont_oplist, it2), data)) \
         /* We cannot stop as soon as a match is found */               \
         M_CALL_IT_SET(cont_oplist, it, it2) ;                          \
     }                                                                  \
   }                                                                    \
                                                                        ) /* End of alternative of _find_last */ \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _count) (container_t const l, type_t const data)            \
  {                                                                     \
    it_t it;                                                            \
    size_t count = 0;                                                   \
    for (M_CALL_IT_FIRST(cont_oplist, it, l);                           \
         !M_CALL_IT_END_P(cont_oplist, it) ;                            \
         M_CALL_IT_NEXT(cont_oplist, it)) {                             \
      if (M_CALL_EQUAL(type_oplist, *M_CALL_IT_CREF(cont_oplist, it), data)) \
        count++ ;                                                       \
    }                                                                   \
    return count;                                                       \
  }                                                                     \
                                                                        \
                                                                        \
  static inline void                                                    \
  M_C(name, _mismatch_again) (it_t it1, it_t it2)                       \
  {                                                                     \
    for (/* nothing */ ; !M_CALL_IT_END_P(cont_oplist, it1) &&          \
                         !M_CALL_IT_END_P(cont_oplist, it2);            \
                       M_CALL_IT_NEXT(cont_oplist, it1),                \
                         M_CALL_IT_NEXT(cont_oplist, it2)) {            \
      if (!M_CALL_EQUAL(type_oplist, *M_CALL_IT_CREF(cont_oplist, it1), \
                        *M_CALL_IT_CREF(cont_oplist, it2)))             \
        break;                                                          \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _mismatch) (it_t it1, it_t it2, container_t const l1, container_t const l2 ) \
  {                                                                     \
    M_CALL_IT_FIRST(cont_oplist, it1, l1);                              \
    M_CALL_IT_FIRST(cont_oplist, it2, l2);                              \
    M_C(name, _mismatch_again)(it1, it2);                               \
  }                                                                     \
                                                                        \



#define ALGOI_FIND_IF_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, suffix, test_t, eq_t, call_test, call_eq) \
									\
  static inline void                                                    \
  M_C3(name, _find_again_, suffix) (it_t it, test_t func)		\
  {                                                                     \
    for (/*nothing */ ; !M_CALL_IT_END_P(cont_oplist, it) ;             \
                      M_CALL_IT_NEXT(cont_oplist, it)) {                \
      if (call_test(func, *M_CALL_IT_CREF(cont_oplist, it)))		\
        return ;                                                        \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(name, _find_, suffix) (it_t it, container_t l, test_t func)	\
  {                                                                     \
    M_CALL_IT_FIRST(cont_oplist, it, l);                                \
    M_C3(name, _find_again_, suffix)(it, func);				\
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C3(name, _count_, suffix) (container_t const l, test_t func)	\
  {                                                                     \
    it_t it;                                                            \
    size_t count = 0;                                                   \
    for (M_CALL_IT_FIRST(cont_oplist, it, l);                           \
         !M_CALL_IT_END_P(cont_oplist, it) ;                            \
         M_CALL_IT_NEXT(cont_oplist, it)) {                             \
      if (call_test(func, *M_CALL_IT_CREF(cont_oplist, it)))		\
        count++ ;                                                       \
    }                                                                   \
    return count;                                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(name, _mismatch_again_, suffix) (it_t it1, it_t it2, eq_t func) \
  {                                                                     \
    for (/*nothing */ ; !M_CALL_IT_END_P(cont_oplist, it1) &&           \
                        !M_CALL_IT_END_P(cont_oplist, it2);             \
                      M_CALL_IT_NEXT(cont_oplist, it1),                 \
                        M_CALL_IT_NEXT(cont_oplist, it2)) {             \
      if (!call_eq(func, *M_CALL_IT_CREF(cont_oplist, it1),		\
		   *M_CALL_IT_CREF(cont_oplist, it2)))			\
        break;                                                          \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(name, _mismatch_, suffix) (it_t it1, it_t it2, container_t const l1, \
                           container_t l2, eq_t func)			\
  {                                                                     \
    M_CALL_IT_FIRST(cont_oplist, it1, l1);                              \
    M_CALL_IT_FIRST(cont_oplist, it2, l2);                              \
    M_C3(name, _mismatch_again_, suffix)(it1, it2, func);		\
  }                                                                     \


#define ALGOI_FILL_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
									\
  static inline void                                                    \
  M_C(name, _fill) (container_t l, type_t const value)                  \
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        M_CALL_SET(type_oplist, *item, value);                          \
    }                                                                   \
  }                                                                     \
                                                                        \
  M_IF_METHOD(PUSH, cont_oplist)(                                       \
  static inline void                                                    \
  M_C(name, _fill_n) (container_t l, size_t n, type_t const value)      \
  {                                                                     \
    M_CALL_CLEAN(cont_oplist, l);                                       \
    for(size_t i = 0; i < n; i++) {                                     \
      M_CALL_PUSH(cont_oplist, l, value);                               \
    }                                                                   \
  }                                                                     \
  , /* PUSH method */ )                                                 \
                                                                        \
  M_IF_METHOD(ADD, type_oplist)(                                        \
  static inline void                                                    \
  M_C(name, _fill_a) (container_t l, type_t const value, type_t const inc) \
  {                                                                     \
    type_t tmp;                                                         \
    M_CALL_INIT_SET(type_oplist, tmp, value);                           \
    for M_EACH(item, l, cont_oplist) {                                  \
        M_CALL_SET(type_oplist, *item, tmp);                            \
        M_CALL_ADD(type_oplist, tmp, tmp, inc);                         \
    }                                                                   \
    M_CALL_CLEAR(type_oplist, tmp);                                     \
  }                                                                     \
                                                                        \
  M_IF_METHOD(PUSH, cont_oplist)(                                       \
  static inline void                                                    \
  M_C(name, _fill_an) (container_t l, size_t n, type_t const value, type_t const inc) \
  {                                                                     \
    type_t tmp;                                                         \
    M_CALL_INIT_SET(type_oplist, tmp, value);                           \
    M_CALL_CLEAN(cont_oplist, l);                                       \
    for(size_t i = 0; i < n; i++) {                                     \
      M_CALL_PUSH(cont_oplist, l, tmp);                                 \
      M_CALL_ADD(type_oplist, tmp, tmp, inc);                           \
    }                                                                   \
    M_CALL_CLEAR(type_oplist, tmp);                                     \
  }                                                                     \
  , /* PUSH method */ )                                                 \
  , /* ADD method */ )                                                  \


#define ALGOI_MAP_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
									\
  static inline void                                                    \
  M_C(name, _for_each) (container_t l, M_C(name, _apply_cb_t) func)	\
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        func(*item);							\
    }                                                                   \
  }                                                                     \
                                                                        \
  M_IF_METHOD(INIT, type_oplist)(                                       \
  M_IF_METHOD(PUSH_MOVE, cont_oplist)(                                  \
									\
  static inline void                                                    \
  M_C(name, _transform) (container_t dst,                               \
                         container_t src,				\
			 M_C(name, _transform_cb_t) func)		\
  {                                                                     \
    assert(dst != src);                                                 \
    M_CALL_CLEAN(cont_oplist, dst);                                     \
    for M_EACH(item, src, cont_oplist) {                                \
        type_t tmp;                                                     \
        M_CALL_INIT(type_oplist, tmp);                                  \
        func(&tmp, *item);						\
        M_CALL_PUSH_MOVE(cont_oplist, dst, &tmp);                       \
    }                                                                   \
    M_IF_METHOD(REVERSE, cont_oplist)(M_CALL_REVERSE(cont_oplist, dst),); \
  }                                                                     \
  , /* END PUSH_MOVE */), /* END INIT */ )                              \
                                                                        \
  M_IF_METHOD(SET, type_oplist)(					\
  static inline void                                                    \
  M_C(name, _reduce) (type_t *dest, container_t const l,                \
                      M_C(name, _transform_cb_t) func)			\
  {                                                                     \
    bool initDone = false;                                              \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (initDone) {                                                 \
          func(dest, *item);						\
        } else {                                                        \
          M_CALL_SET(type_oplist, *dest, *item);                        \
          initDone = true;                                              \
        }                                                               \
      }                                                                 \
  }                                                                     \
  , /* END SET */)							\
                                                                        \
  M_IF_METHOD(INIT, type_oplist)(                                       \
  static inline                                                         \
  void M_C(name, _map_reduce) (type_t *dest,                            \
                               const container_t l,                     \
                               M_C(name, _transform_cb_t) redFunc,	\
                               M_C(name, _transform_cb_t) mapFunc)	\
  {                                                                     \
    bool initDone = false;                                              \
    type_t tmp;                                                         \
    M_CALL_INIT(type_oplist, tmp);                                      \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (initDone) {                                                 \
          mapFunc(&tmp, *item);                                         \
          redFunc(dest, tmp);                                           \
        } else {                                                        \
          mapFunc(dest, *item);                                         \
          initDone = true;                                              \
        }                                                               \
      }                                                                 \
    M_CALL_CLEAR(type_oplist, tmp);                                     \
  }                                                                     \
  , )                                                                   \


#define ALGOI_ALL_OF_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t, suffix, func_t, call) \
									\
  static inline bool                                                    \
  M_C4(name, _any_of, suffix, p) (container_t const l,			\
				  func_t func )				\
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
	if (call(func, *item))						\
          return true;                                                  \
    }                                                                   \
    return false;                                                       \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C4(name, _all_of, suffix, p) (container_t const l,			\
				  func_t func )				\
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (!call(func, *item))						\
          return false;                                                 \
    }                                                                   \
    return true;                                                        \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C4(name, _none_of, suffix, p) (container_t l,			\
				   func_t func )			\
  {                                                                     \
    for M_EACH(item, l, cont_oplist) {                                  \
        if (call(func, *item))						\
          return false;                                                 \
      }                                                                 \
    return true;                                                        \
  }                                                                     \


#define ALGOI_MINMAX_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
                                                                        \
  static inline type_t *                                                \
  M_C(name, _min) (const container_t l)                                 \
  {                                                                     \
    type_t *min = NULL;                                                 \
    for M_EACH(cref, l, cont_oplist) {                                  \
        if (min == NULL ||                                              \
            M_CALL_CMP(type_oplist, *min, *cref) > 0)                   \
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
            M_CALL_CMP(type_oplist, *max, *cref) < 0)                   \
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
            M_CALL_CMP(type_oplist, *min, *cref) > 0)                   \
          min = cref;                                                   \
        if (max == NULL ||                                              \
            M_CALL_CMP(type_oplist, *max, *cref) < 0)                   \
          max = cref;                                                   \
      }                                                                 \
    *min_p = min;                                                       \
    *max_p = max;                                                       \
  }                                                                     \

#define ALGOI_REMOVE_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
                                                                        \
  static inline void                                                    \
  M_C(name, _uniq)(container_t l)                                       \
  {                                                                     \
    it_t it1;                                                           \
    it_t it2;                                                           \
    assert(M_C(name, _sort_p)(l));                                      \
    M_CALL_IT_FIRST(cont_oplist, it1, l);                               \
    M_CALL_IT_SET(cont_oplist, it2, it1);                               \
    M_CALL_IT_NEXT(cont_oplist, it2);                                   \
    /* Not efficient for array! */                                      \
    while (!M_CALL_IT_END_P(cont_oplist, it2)) {                        \
      type_t const *ref1 = M_CALL_IT_CREF(cont_oplist, it1);            \
      type_t const *ref2 = M_CALL_IT_CREF(cont_oplist, it2);            \
      if (M_CALL_CMP(type_oplist, *ref1, *ref2) == 0) {                 \
        M_CALL_IT_REMOVE(cont_oplist, l, it2);                          \
      } else {                                                          \
        M_CALL_IT_SET(cont_oplist, it1, it2);                           \
        M_CALL_IT_NEXT(cont_oplist, it2);                               \
      }                                                                 \
      }                                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _remove_val)(container_t l, type_t const val)               \
  {                                                                     \
    it_t it1;                                                           \
    M_CALL_IT_FIRST(cont_oplist, it1, l);                               \
    while (!M_CALL_IT_END_P(cont_oplist, it1)) {                        \
      type_t const *ref1 = M_CALL_IT_CREF(cont_oplist, it1);            \
      if (M_CALL_EQUAL(type_oplist, *ref1, val)) {                      \
        M_CALL_IT_REMOVE(cont_oplist, l, it1);                          \
      } else {                                                          \
        M_CALL_IT_NEXT(cont_oplist, it1);                               \
      }                                                                 \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _remove_if)(container_t l, M_C(name, _test_cb_t) func)      \
  {                                                                     \
    it_t it1;                                                           \
    M_CALL_IT_FIRST(cont_oplist, it1, l);                               \
    while (!M_CALL_IT_END_P(cont_oplist, it1)) {                        \
      type_t const *ref1 = M_CALL_IT_CREF(cont_oplist, it1);            \
      if (func(*ref1)) {                                                \
        M_CALL_IT_REMOVE(cont_oplist, l, it1);                          \
      } else {                                                          \
        M_CALL_IT_NEXT(cont_oplist, it1);                               \
      }                                                                 \
    }                                                                   \
  }                                                                     \

#define ALGOI_VECTOR_DEF_P5(name, container_t, cont_oplist, type_t, type_oplist, it_t) \
                                                                        \
  M_IF_METHOD(ADD, type_oplist)(                                        \
  static inline void M_C(name, _add) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_CALL_IT_FIRST(cont_oplist, itSrc, src) ,                     \
           M_CALL_IT_FIRST(cont_oplist, itDst, dst) ;                   \
         !M_CALL_IT_END_P(cont_oplist, itSrc)                           \
           && !M_CALL_IT_END_P(cont_oplist, itDst) ;                    \
         M_CALL_IT_NEXT(cont_oplist, itSrc),                            \
           M_CALL_IT_NEXT(cont_oplist, itDst) ) {                       \
      type_t *dstItem = M_CALL_IT_REF(cont_oplist, itDst);              \
      type_t const *srcItem = M_CALL_IT_REF(cont_oplist, itSrc);        \
      M_CALL_ADD(type_oplist, *dstItem, *dstItem, *srcItem);            \
    }                                                                   \
  }                                                                     \
  , /* NO_ADD METHOD */ )                                               \
                                                                        \
  M_IF_METHOD(SUB, type_oplist)(                                        \
  static inline void M_C(name, _sub) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_CALL_IT_FIRST(cont_oplist, itSrc, src) ,                     \
           M_CALL_IT_FIRST(cont_oplist, itDst, dst) ;                   \
         !M_CALL_IT_END_P(cont_oplist, itSrc)                           \
           && !M_CALL_IT_END_P(cont_oplist, itDst) ;                    \
         M_CALL_IT_NEXT(cont_oplist, itSrc),                            \
           M_CALL_IT_NEXT(cont_oplist, itDst) ) {                       \
      type_t *dstItem = M_CALL_IT_REF(cont_oplist, itDst);              \
      type_t const *srcItem = M_CALL_IT_REF(cont_oplist, itSrc);        \
      M_CALL_SUB(type_oplist, *dstItem, *dstItem, *srcItem);            \
    }                                                                   \
  }                                                                     \
  , /* NO_SUB METHOD */ )                                               \
                                                                        \
  M_IF_METHOD(MUL, type_oplist)(                                        \
  static inline void M_C(name, _mul) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_CALL_IT_FIRST(cont_oplist, itSrc, src) ,                     \
           M_CALL_IT_FIRST(cont_oplist, itDst, dst) ;                   \
         !M_CALL_IT_END_P(cont_oplist, itSrc)                           \
           && !M_CALL_IT_END_P(cont_oplist, itDst) ;                    \
         M_CALL_IT_NEXT(cont_oplist, itSrc),                            \
           M_CALL_IT_NEXT(cont_oplist, itDst) ) {                       \
      type_t *dstItem = M_CALL_IT_REF(cont_oplist, itDst);              \
      type_t const *srcItem = M_CALL_IT_REF(cont_oplist, itSrc);        \
      M_CALL_MUL(type_oplist, *dstItem, *dstItem, *srcItem);            \
    }                                                                   \
  }                                                                     \
  , /* NO_MUL METHOD */ )                                               \
                                                                        \
  M_IF_METHOD(DIV, type_oplist)(                                        \
  static inline void M_C(name, _div) (container_t dst, const container_t src) \
  {                                                                     \
    it_t itSrc;                                                         \
    it_t itDst;                                                         \
    for (M_CALL_IT_FIRST(cont_oplist, itSrc, src) ,                     \
           M_CALL_IT_FIRST(cont_oplist, itDst, dst) ;                   \
         !M_CALL_IT_END_P(cont_oplist, itSrc)                           \
           && !M_CALL_IT_END_P(cont_oplist, itDst) ;                    \
         M_CALL_IT_NEXT(cont_oplist, itSrc),                            \
           M_CALL_IT_NEXT(cont_oplist, itDst) ) {                       \
      type_t *dstItem = M_CALL_IT_REF(cont_oplist, itDst);              \
      type_t const *srcItem = M_CALL_IT_REF(cont_oplist, itSrc);        \
      M_CALL_DIV(type_oplist, *dstItem, *dstItem, *srcItem);            \
    }                                                                   \
  }                                                                     \
  , /* NO_DIV METHOD */ )                                               \
                                                                        \

//TODO: Algorithm missing
// _nth_element ( http://softwareengineering.stackexchange.com/questions/284767/kth-selection-routine-floyd-algorithm-489 )
// _average
// _sort_uniq (_sort + _uniq)
// fast _uniq for array
// _flatten (takes a set of containers and returns a new container containing all flatten objects)


#define ALGOI_FOR_EACH(container, cont_oplist, func) do {               \
    for M_EACH(_item, container, cont_oplist) {                         \
        func(*_item);                                                   \
      }                                                                 \
  } while (0)

#define ALGOI_FOR_EACH_ARG(container, cont_oplist, func, ...) do {      \
    for M_EACH(_item, container, cont_oplist) {                         \
        func(__VA_ARGS__, *_item);                                      \
      }                                                                 \
  } while (0)


#define ALGOI_TRANSFORM(contD, contDop, contS, contSop, func) do {      \
    M_CALL_CLEAN(contDop, contD);                                       \
    for M_EACH(_item, contS, contSop) {                                 \
        M_GET_SUBTYPE contDop _tmp;                                     \
        M_CALL_INIT(M_GET_OPLIST contDop, _tmp);                        \
        func(_tmp, *_item);                                             \
        M_CALL_PUSH_MOVE(contDop, contD, &_tmp);                        \
      }                                                                 \
    M_IF_METHOD(REVERSE, contDop) (M_CALL_REVERSE(contDop, contD);, ) \
  } while (0)

#define ALGOI_TRANSFORM_ARG(contD, contDop, contS, contSop, func, ...) do { \
    M_CALL_CLEAN(contDop, contD);                                       \
    for M_EACH(_item, contS, contSop) {                                 \
        M_GET_SUBTYPE contDop _tmp;                                     \
        M_CALL_INIT(M_GET_OPLIST contDop, _tmp);                        \
        func(_tmp, *_item, __VA_ARGS__);                                \
        M_CALL_PUSH_MOVE(contDop, contD, &_tmp);                        \
      }                                                                 \
    M_IF_METHOD(REVERSE, contDop) (M_CALL_REVERSE(contDop, contD);, )   \
  } while (0)


#define ALGOI_EXTRACT(contDst, contDstOplist,                           \
                      contSrc, contSrcOplist) do {			\
    M_CALL_CLEAN(contDstOplist, contDst);                               \
    for M_EACH(_item, contSrc, contSrcOplist) {                         \
        M_IF_METHOD(PUSH, contDstOplist)(                               \
          M_CALL_PUSH(contDstOplist, contDst, *_item);                  \
        ,                                                               \
          M_CALL_SET_KEY(contDstOplist, contDst, (*_item).key, (*_item).value); \
                                                                        ) \
    }                                                                   \
    M_IF_METHOD(REVERSE, contDstOplist) (M_CALL_REVERSE(contDstOplist, contDst);, ) \
  } while (0)

#define ALGOI_EXTRACT_FUNC(contDst, contDstOplist,			\
                      contSrc, contSrcOplist,                           \
                      condFunc) do {                                    \
    M_CALL_CLEAN(contDstOplist, contDst);                               \
    for M_EACH(_item, contSrc, contSrcOplist) {                         \
        if (condFunc (*_item)) {                                        \
          M_IF_METHOD(PUSH, contDstOplist)(                             \
            M_CALL_PUSH(contDstOplist, contDst, *_item);                \
          ,                                                             \
            M_CALL_SET_KEY(contDstOplist, contDst, (*_item).key, (*_item).value); \
                                                                        ) \
        }                                                               \
    }                                                                   \
    M_IF_METHOD(REVERSE, contDstOplist) (M_CALL_REVERSE(contDstOplist, contDst);, ) \
  } while (0)

#define ALGOI_EXTRACT_ARG(contDst, contDstOplist,                       \
                          contSrc, contSrcOplist,                       \
                          condFunc, ...) do {                           \
    M_CALL_CLEAN(contDstOplist, contDst);                               \
    for M_EACH(_item, contSrc, contSrcOplist) {                         \
        if (condFunc (__VA_ARGS__, *_item)) {                           \
          M_IF_METHOD(PUSH, contDstOplist)(                             \
            M_CALL_PUSH(contDstOplist, contDst, *_item);                \
            ,                                                           \
            M_CALL_SET_KEY(contDstOplist, contDst, (*_item).key, (*_item).value); \
                                                                        ) \
        }                                                               \
    }                                                                   \
    M_IF_METHOD(REVERSE, contDstOplist) (M_CALL_REVERSE(contDstOplist, contDstOplist);, ) \
  } while (0)


#define ALGOI_REDUCE_DISPATCH(dest, destOp, dest_t, cont, contOp,  ...) \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ALGOI_REDUCE_BASIC(dest, dest_t, destOp, cont, contOp, __VA_ARGS__), \
   M_IF_NARGS_EQ2(__VA_ARGS__)                                          \
   (ALGOI_REDUCE_FOR_EACH(dest, dest_t, destOp, cont, contOp, __VA_ARGS__), \
    ALGOI_REDUCE_FOR_EACH_ARG(dest, dest_t, destOp, cont, contOp, __VA_ARGS__) ) )

/* The special function handler */
#define ALGOI_REDUCE_AND(a,b) ((a) &= (b))
#define ALGOI_REDUCE_OR(a,b)  ((a) |= (b))
#define ALGOI_REDUCE_SUM(a,b) ((a) += (b))
#define ALGOI_REDUCE_PRODUCT(a,b) ((a) *= (b))

/* Return the method associated to a reduce operation.
   It returns the special function handler if function is and, or, sum or add.
   Otherwise it returns the original function */
#define ALGOI_REDUCE_FUNC(reduceFunc)            \
  M_IF(M_KEYWORD_P(and, reduceFunc))             \
  (ALGOI_REDUCE_AND,                             \
   M_IF(M_KEYWORD_P(or, reduceFunc))             \
   (ALGOI_REDUCE_OR,                             \
    M_IF(M_KEYWORD_P(sum, reduceFunc))           \
    (ALGOI_REDUCE_SUM,                           \
     M_IF(M_KEYWORD_P(product, reduceFunc))      \
     (ALGOI_REDUCE_PRODUCT,                      \
      reduceFunc)                                \
     )                                           \
    )                                            \
   )

#define ALGOI_REDUCE_BASIC(dest, dest_t, destOp, cont, cont_oplist, reduceFunc) do { \
    bool _init_done = false;                                            \
    for M_EACH(_item, cont, cont_oplist) {                              \
        if (_init_done) {                                               \
          ALGOI_REDUCE_FUNC(reduceFunc) (dest, *_item);                 \
        } else {                                                        \
          M_CALL_SET(destOp, dest, *_item);                             \
          _init_done = true;                                            \
        }                                                               \
      }                                                                 \
  } while (0)

#define ALGOI_REDUCE_FOR_EACH(dest, dest_t, destOp, cont, cont_oplist, reduceFunc, mapFunc) do { \
    bool _init_done = false;                                            \
    dest_t _tmp;                                                         \
    M_CALL_INIT(destOp, _tmp);                                          \
    for M_EACH(_item, cont, cont_oplist) {                              \
        mapFunc(_tmp, *_item);                                          \
        if (_init_done) {                                               \
          ALGOI_REDUCE_FUNC(reduceFunc) (dest, _tmp);                   \
        } else {                                                        \
          M_CALL_SET(destOp, dest, _tmp);                               \
          _init_done = true;                                            \
        }                                                               \
      }                                                                 \
    M_CALL_CLEAR(destOp, _tmp);                                         \
  } while (0)

#define ALGOI_REDUCE_FOR_EACH_ARG(dest, dest_t, destOp, cont, cont_oplist, reduceFunc, mapFunc, ...) do { \
    bool _init_done = false;                                            \
    dest_t _tmp;                                                        \
    M_CALL_INIT(destOp, _tmp);                                          \
    for M_EACH(_item, cont, cont_oplist) {                              \
        mapFunc(_tmp, __VA_ARGS__, *_item);                             \
        if (_init_done) {                                               \
          ALGOI_REDUCE_FUNC(reduceFunc) (dest, _tmp);                   \
        } else {                                                        \
          M_CALL_SET(destOp, dest, _tmp);                               \
          _init_done = true;                                            \
        }                                                               \
      }                                                                 \
    M_CALL_CLEAR(destOp, _tmp);                                         \
  } while (0)


#define ALGOI_INIT_VA_FUNC(d, a)                        \
  M_RET_ARG2 d (M_RET_ARG1 d, a) M_DEFERRED_COMMA

#define ALGO_INIT_VAI(dest, contOp, ...)                                \
  (void)(M_CALL_INIT(contOp, dest) M_DEFERRED_COMMA                     \
         M_MAP2(ALGOI_INIT_VA_FUNC, (dest, M_GET_PUSH contOp, ) , __VA_ARGS__) \
         true)


#define ALGO_LET_INIT_VAI(dest,  ...)           \
  ALGOI_LET_INIT_VAI(M_C3(m_cont_, __LINE__, dest), dest, __VA_ARGS__)

#define ALGOI_LET_INIT_VAI(cont, dest, contOp, ...)                     \
  for(bool cont = true; cont ; /* unused */)                            \
    for(M_GET_TYPE contOp dest;                                         \
        cont && (ALGO_INIT_VAI (dest, contOp, __VA_ARGS__), true);      \
        (M_CALL_CLEAR(contOp, dest), cont = false))


#define ALGOI_INSERT_AT(contDst, contDstOp, position, contSrc, contSrcOp) do { \
    M_GET_IT_TYPE contSrcOp _itSrc;                                     \
    M_GET_IT_TYPE contDstOp _itDst;                                     \
    M_CALL_IT_SET(contDstOp, _itDst, position);                         \
    for (M_CALL_IT_FIRST(contSrcOp, _itSrc, contSrc) ;                  \
         !M_CALL_IT_END_P(contSrcOp, _itSrc) ;                          \
         M_CALL_IT_NEXT(contSrcOp, _itSrc) ) {                          \
      M_CALL_IT_INSERT(contDstOp, contDst, _itDst,                      \
                       *M_CALL_IT_CREF(contSrcOp, _itSrc));             \
      M_CALL_IT_NEXT(contDstOp, _itDst);                                \
    }                                                                   \
  } while (0)

#endif
