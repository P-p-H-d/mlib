/*
 * M*LIB - TUPLE module
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
#ifndef MSTARLIB_TUPLE_H
#define MSTARLIB_TUPLE_H

#include "m-core.h"

/********************** External interface *************************/

/* Define the tuple type and functions.
   USAGE:
   TUPLE_DEF2(name, [(field1, type1[, oplist1]), (field2, type2[, oplist2]), ...] ) */
#define TUPLE_DEF2(name, ...)                                                 \
  M_BEGIN_PROTECTED_CODE                                                      \
  TUPLEI_DEF2_P1( (name, M_T(name, t) TUPLEI_INJECT_GLOBAL(__VA_ARGS__)) )   \
  M_END_PROTECTED_CODE


/* Define the tuple type and functions
   as the given name.
   USAGE:
   TUPLE_DEF2_AS(name, name_t, [(field1, type1[, oplist1]), (field2, type2[, oplist2]), ...] ) */
#define TUPLE_DEF2_AS(name, name_t, ...)                                      \
  M_BEGIN_PROTECTED_CODE                                                      \
  TUPLEI_DEF2_P1( (name, name_t TUPLEI_INJECT_GLOBAL(__VA_ARGS__)) )          \
  M_END_PROTECTED_CODE


/* Define the oplist of a tuple.
   USAGE: TUPLE_OPLIST(name[, oplist of the first type, ...]) */
#define TUPLE_OPLIST(...)                                                     \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                                 \
  (TUPLEI_OPLIST_P1((__VA_ARGS__, M_DEFAULT_OPLIST )),                        \
   TUPLEI_OPLIST_P1((__VA_ARGS__ )))


/* Return an array suitable for the WIP _cmp_order function.
   As compound literals are not supported in C++,
   provide a separate definition for C++ using initializer_list
   (shall be constexpr, but only supported in C++14).
*/
#ifndef __cplusplus
#define TUPLE_ORDER(name, ...)                                                \
  ( (const int[]) {M_MAP2_C(TUPLEI_ORDER_CONVERT, name, __VA_ARGS__), 0})
#else
#include <initializer_list>
namespace m_tuple {
  template <unsigned int N>
  struct m_int_array {
    int data[N];
    /*constexpr*/ inline m_int_array(std::initializer_list<int> init){
      int j = 0;
      for(auto i:init) {
        data[j++] = i;
      }
    }
  };
}
#define TUPLE_ORDER(name, ...)                                                \
  (m_tuple::m_int_array<M_NARGS(__VA_ARGS__,0)>({M_MAP2_C(TUPLEI_ORDER_CONVERT, name, __VA_ARGS__), 0}).data)
#endif


/********************************** INTERNAL ************************************/

/* Contract of a tuple. Nothing notable */
#define TUPLEI_CONTRACT(tup) do {                                             \
  M_ASSERT(tup != NULL);                                                      \
} while (0)

/* Inject the oplist within the list of arguments */
#define TUPLEI_INJECT_GLOBAL(...)                                             \
  M_MAP(TUPLEI_INJECT_OPLIST_A, __VA_ARGS__)

/* Transform (x, type) into (x, type, oplist) if there is global registered oplist 
   or (x, type, M_DEFAULT_OPLIST) if there is no global one,
   or keep (x, type, oplist) if oplist was already present */
#define TUPLEI_INJECT_OPLIST_A( duo_or_trio )                                 \
  TUPLEI_INJECT_OPLIST_B duo_or_trio

#define TUPLEI_INJECT_OPLIST_B( f, ... )                                      \
  M_DEFERRED_COMMA                                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)( (f, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)()), (f, __VA_ARGS__) )

// Deferred evaluation
#define TUPLEI_DEF2_P1(...)                       TUPLEI_DEF2_P2 __VA_ARGS__

// Test if all third argument of all arguments is an oplist
#define TUPLEI_IF_ALL_OPLIST(...)                                             \
  M_IF(M_REDUCE(TUPLEI_IS_OPLIST_P, M_AND, __VA_ARGS__))

// Test if the third argument of (name, type, oplist) is an oplist
#define TUPLEI_IS_OPLIST_P(a)                                                 \
  M_OPLIST_P(M_RET_ARG3 a)

/* Validate the oplist before going further */
#define TUPLEI_DEF2_P2(name, name_t, ...)                                     \
  TUPLEI_IF_ALL_OPLIST(__VA_ARGS__)(TUPLEI_DEF2_P3, TUPLEI_DEF2_FAILURE)(name, name_t, __VA_ARGS__)

/* Stop processing with a compilation failure */
#define TUPLEI_DEF2_FAILURE(name, name_t, ...)                                \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(TUPLE_DEF2): at least one of the given argument is not a valid oplist: " #__VA_ARGS__)

/* Define the tuple */
#define TUPLEI_DEF2_P3(name, name_t, ...)                                     \
  TUPLEI_DEFINE_TYPE(name, name_t, __VA_ARGS__)                               \
  TUPLEI_DEFINE_ENUM(name, __VA_ARGS__)                                       \
  TUPLEI_CONTROL_ALL_OPLIST(name, __VA_ARGS__)                                \
  TUPLEI_IF_ALL(INIT, __VA_ARGS__)(TUPLEI_DEFINE_INIT(name, __VA_ARGS__),)    \
  TUPLEI_DEFINE_INIT_SET(name, __VA_ARGS__)                                   \
  TUPLEI_DEFINE_INIT_SET2(name, __VA_ARGS__)                                  \
  TUPLEI_DEFINE_SET(name, __VA_ARGS__)                                        \
  TUPLEI_DEFINE_SET2(name, __VA_ARGS__)                                       \
  TUPLEI_DEFINE_CLEAR(name, __VA_ARGS__)                                      \
  TUPLEI_DEFINE_GETTER_FIELD(name, __VA_ARGS__)                               \
  TUPLEI_DEFINE_SETTER_FIELD(name, __VA_ARGS__)                               \
  TUPLEI_IF_ALL(CMP, __VA_ARGS__)(TUPLEI_DEFINE_CMP(name, __VA_ARGS__),)      \
  TUPLEI_IF_ALL(CMP, __VA_ARGS__)(TUPLEI_DEFINE_CMP_ORDER(name, __VA_ARGS__),) \
  TUPLEI_DEFINE_CMP_FIELD(name, __VA_ARGS__)                                  \
  TUPLEI_IF_ALL(HASH, __VA_ARGS__)(TUPLEI_DEFINE_HASH(name, __VA_ARGS__),)    \
  TUPLEI_IF_ALL(EQUAL, __VA_ARGS__)(TUPLEI_DEFINE_EQUAL(name, __VA_ARGS__),)  \
  TUPLEI_IF_ALL(GET_STR, __VA_ARGS__)(TUPLEI_DEFINE_GET_STR(name, __VA_ARGS__),) \
  TUPLEI_IF_ALL(OUT_STR, __VA_ARGS__)(TUPLEI_DEFINE_OUT_STR(name, __VA_ARGS__),) \
  TUPLEI_IF_ALL(IN_STR, __VA_ARGS__)(TUPLEI_DEFINE_IN_STR(name, __VA_ARGS__),) \
  TUPLEI_IF_ALL(PARSE_CSTR, __VA_ARGS__)(TUPLEI_DEFINE_PARSE_CSTR(name, __VA_ARGS__),) \
  TUPLEI_IF_ALL(OUT_SERIAL, __VA_ARGS__)(TUPLEI_DEFINE_OUT_SERIAL(name, __VA_ARGS__),) \
  TUPLEI_IF_ALL(IN_SERIAL, __VA_ARGS__)(TUPLEI_DEFINE_IN_SERIAL(name, __VA_ARGS__),) \
  TUPLEI_IF_ALL(INIT_MOVE, __VA_ARGS__)(TUPLEI_DEFINE_INIT_MOVE(name, __VA_ARGS__),) \
  TUPLEI_IF_ALL(MOVE, __VA_ARGS__)(TUPLEI_DEFINE_MOVE(name, __VA_ARGS__),)    \
  TUPLEI_IF_ALL(SWAP, __VA_ARGS__)(TUPLEI_DEFINE_SWAP(name, __VA_ARGS__),)    \
  TUPLEI_IF_ALL(CLEAN, __VA_ARGS__)(TUPLEI_DEFINE_CLEAN(name, __VA_ARGS__),)

/* Provide order for _cmp_order */
#define TUPLEI_ORDER_CONVERT(name, x) M_C(name, M_C(TUPLEI_ORDER_CONVERT_, x))
#define TUPLEI_ORDER_CONVERT_ASC(x)   M_C(_,x,_value)
#define TUPLEI_ORDER_CONVERT_DSC(x)   M_C(_,x,_value)*-1

/* Get the field name, the type, the oplist or the methods
   based on the tuple (field, type, oplist) */
#define TUPLEI_GET_FIELD(f,t,o)      f
#define TUPLEI_GET_TYPE(f,t,o)       t
#define TUPLEI_GET_OPLIST(f,t,o)     o
#define TUPLEI_GET_INIT(f,t,o)       M_GET_INIT o
#define TUPLEI_GET_INIT_SET(f,t,o)   M_GET_INIT_SET o
#define TUPLEI_GET_INIT_MOVE(f,t,o)  M_GET_INIT_MOVE o
#define TUPLEI_GET_MOVE(f,t,o)       M_GET_MOVE o
#define TUPLEI_GET_SET(f,t,o)        M_GET_SET o
#define TUPLEI_GET_CLEAR(f,t,o)      M_GET_CLEAR o
#define TUPLEI_GET_CMP(f,t,o)        M_GET_CMP o
#define TUPLEI_GET_HASH(f,t,o)       M_GET_HASH o
#define TUPLEI_GET_EQUAL(f,t,o)      M_GET_EQUAL o
#define TUPLEI_GET_STR(f,t,o)        M_GET_GET_STR o
#define TUPLEI_GET_OUT_STR(f,t,o)    M_GET_OUT_STR o
#define TUPLEI_GET_IN_STR(f,t,o)     M_GET_IN_STR o
#define TUPLEI_GET_OUT_SERIAL(f,t,o) M_GET_OUT_SERIAL o
#define TUPLEI_GET_IN_SERIAL(f,t,o)  M_GET_IN_SERIAL o
#define TUPLEI_GET_PARSE_CSTR(f,t,o)  M_GET_PARSE_CSTR o
#define TUPLEI_GET_SWAP(f,t,o)       M_GET_SWAP o
#define TUPLEI_GET_CLEAN(f,t,o)      M_GET_CLEAN o

/* Call the method associated to the given operator for the given parameter
  of the tuple t=(name, type, oplist) */
#define TUPLEI_CALL_INIT(t, ...)       M_APPLY_API(TUPLEI_GET_INIT t,          \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_INIT_SET(t, ...)   M_APPLY_API(TUPLEI_GET_INIT_SET t,      \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_INIT_MOVE(t, ...)  M_APPLY_API(TUPLEI_GET_INIT_MOVE t,     \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_MOVE(t, ...)       M_APPLY_API(TUPLEI_GET_MOVE t,          \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_SET(t, ...)        M_APPLY_API(TUPLEI_GET_SET t,           \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_CLEAR(t, ...)      M_APPLY_API(TUPLEI_GET_CLEAR t,         \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_CMP(t, ...)        M_APPLY_API(TUPLEI_GET_CMP t,           \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_HASH(t, ...)       M_APPLY_API(TUPLEI_GET_HASH t,          \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_EQUAL(t, ...)      M_APPLY_API(TUPLEI_GET_EQUAL t,         \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_GET_STR(t, ...)    M_APPLY_API(TUPLEI_GET_STR t,           \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_OUT_STR(t, ...)    M_APPLY_API(TUPLEI_GET_OUT_STR t,       \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_IN_STR(t, ...)     M_APPLY_API(TUPLEI_GET_IN_STR t,        \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_PARSE_CSTR(t, ...)  M_APPLY_API(TUPLEI_GET_PARSE_CSTR t,     \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_OUT_SERIAL(t, ...) M_APPLY_API(TUPLEI_GET_OUT_SERIAL t,    \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_IN_SERIAL(t, ...)  M_APPLY_API(TUPLEI_GET_IN_SERIAL t,     \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_SWAP(t, ...)       M_APPLY_API(TUPLEI_GET_SWAP t,          \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)
#define TUPLEI_CALL_CLEAN(t, ...)      M_APPLY_API(TUPLEI_GET_CLEAN t,         \
                                                   TUPLEI_GET_OPLIST t,        \
                                                   __VA_ARGS__)


/* Define the type of a tuple */
#define TUPLEI_DEFINE_TYPE(name, name_t, ...)                                  \
  typedef struct M_T(name, s) {                                                \
    M_MAP(TUPLEI_DEFINE_RECUR_TYPE_ELE , __VA_ARGS__)                          \
  } name_t[1];                                                                 \
                                                                               \
  typedef struct M_T(name, s) *M_T(name, ptr);                                 \
  typedef const struct M_T(name, s) *M_T(name, srcptr);                        \
  /* Define internal type for oplist */                                        \
  typedef name_t M_T(name, ct);

#define TUPLEI_DEFINE_RECUR_TYPE_ELE(a)                                        \
  TUPLEI_GET_TYPE a TUPLEI_GET_FIELD a ;

/* Define the basic enumerate, identifying a parameter */
#define TUPLEI_DEFINE_ENUM(name, ...)                                          \
  typedef enum {                                                               \
    M_I(name, first_one_val),                                                  \
    M_MAP2_C(TUPLEI_DEFINE_ENUM_ELE, name, __VA_ARGS__)                        \
  } M_T3(name, field, e);

#define TUPLEI_DEFINE_ENUM_ELE(name, a)                                        \
  M_C(name, _, TUPLEI_GET_FIELD a, _value)

/* Control that all given op-lists of all parameters are really op-lists */
#define TUPLEI_CONTROL_ALL_OPLIST(name, ...)                                   \
  M_MAP2(TUPLEI_CONTROL_OPLIST, name, __VA_ARGS__)

#define TUPLEI_CONTROL_OPLIST(name, a)                                         \
  M_CHECK_COMPATIBLE_OPLIST(name, TUPLEI_GET_FIELD a,                          \
                            TUPLEI_GET_TYPE a, TUPLEI_GET_OPLIST a)

/* Define the INIT method calling the INIT method for all params */
#define TUPLEI_DEFINE_INIT(name, ...)                                          \
  static inline void M_F(name, M_NAMING_INIT)(M_T(name, ct) my) {              \
    M_MAP(TUPLEI_DEFINE_INIT_FUNC, __VA_ARGS__)                                \
  }

#define TUPLEI_DEFINE_INIT_FUNC(a)                                             \
  TUPLEI_CALL_INIT(a, my -> TUPLEI_GET_FIELD a );

/* Define the INIT_SET method calling the INIT_SET method for all params */
#define TUPLEI_DEFINE_INIT_SET(name, ...)                                      \
  static inline void M_F(name, M_NAMING_INIT_WITH)                              \
        (M_T(name, ct) my , M_T(name, ct) const org) {                         \
    TUPLEI_CONTRACT(org);                                                      \
    M_MAP(TUPLEI_DEFINE_INIT_SET_FUNC, __VA_ARGS__)                            \
  }

#define TUPLEI_DEFINE_INIT_SET_FUNC(a)                                         \
  TUPLEI_CALL_INIT_SET(a, my -> TUPLEI_GET_FIELD a, org -> TUPLEI_GET_FIELD a);

/* Define the INIT_WITH method calling the INIT_SET method for all params. */
#define TUPLEI_DEFINE_INIT_SET2(name, ...)                                     \
  static inline void M_F(name, init_emplace)(M_T(name, ct) my                  \
                      M_MAP(TUPLEI_DEFINE_INIT_SET2_PROTO, __VA_ARGS__)) {     \
    M_MAP(TUPLEI_DEFINE_INIT_SET2_FUNC, __VA_ARGS__)                           \
  }

#define TUPLEI_DEFINE_INIT_SET2_PROTO(a)                                       \
  , TUPLEI_GET_TYPE a const TUPLEI_GET_FIELD a

#define TUPLEI_DEFINE_INIT_SET2_FUNC(a)                                        \
  TUPLEI_CALL_INIT_SET(a, my -> TUPLEI_GET_FIELD a , TUPLEI_GET_FIELD a );

/* Define the SET method calling the SET method for all params. */
#define TUPLEI_DEFINE_SET(name, ...)                                           \
  static inline void M_F(name, M_NAMING_SET_AS)(M_T(name, ct) my,              \
                                                M_T(name, ct) const org) {     \
    TUPLEI_CONTRACT(my);                                                       \
    TUPLEI_CONTRACT(org);                                                      \
    M_MAP(TUPLEI_DEFINE_SET_FUNC, __VA_ARGS__)                                 \
  }

#define TUPLEI_DEFINE_SET_FUNC(a)                                              \
  TUPLEI_CALL_SET(a, my -> TUPLEI_GET_FIELD a , org -> TUPLEI_GET_FIELD a);


/* Define the SET_WITH method calling the SET method for all params. */
#define TUPLEI_DEFINE_SET2(name, ...)                                          \
  static inline void M_C(name, _emplace)(M_C(name,_ct) my                      \
                      M_MAP(TUPLEI_DEFINE_SET2_PROTO, __VA_ARGS__)             \
                                           ) {                                 \
    TUPLEI_CONTRACT(my);                                                       \
    M_MAP(TUPLEI_DEFINE_SET2_FUNC , __VA_ARGS__)                               \
  }

#define TUPLEI_DEFINE_SET2_PROTO(a)                                            \
  , TUPLEI_GET_TYPE a const TUPLEI_GET_FIELD a

#define TUPLEI_DEFINE_SET2_FUNC(a)                                             \
  TUPLEI_CALL_SET(a, my -> TUPLEI_GET_FIELD a , TUPLEI_GET_FIELD a );

/* Define the CLEAR method calling the CLEAR method for all params. */
#define TUPLEI_DEFINE_CLEAR(name, ...)                                         \
  static inline void M_F(name, M_NAMING_FINALIZE)(M_T(name, ct) my) {             \
    TUPLEI_CONTRACT(my);                                                       \
    M_MAP(TUPLEI_DEFINE_CLEAR_FUNC, __VA_ARGS__)                               \
  }

#define TUPLEI_DEFINE_CLEAR_FUNC(a)                                            \
  TUPLEI_CALL_CLEAR(a, my -> TUPLEI_GET_FIELD a);


/* Define the GET_AT_field & CGET_AT methods for all params. */
#define TUPLEI_DEFINE_GETTER_FIELD(name, ...)                                 \
  M_MAP2(TUPLEI_DEFINE_GETTER_FIELD_PROTO, name, __VA_ARGS__)

#define TUPLEI_DEFINE_GETTER_FIELD_PROTO(name, a)                             \
  static inline TUPLEI_GET_TYPE a * M_F3(name, get_at, TUPLEI_GET_FIELD a)    \
       (M_T(name, ct) my) {                                                   \
    TUPLEI_CONTRACT(my);                                                      \
    return &(my->TUPLEI_GET_FIELD a);                                         \
  }                                                                           \
  static inline TUPLEI_GET_TYPE a const * M_F3(name, cget_at,                 \
                                               TUPLEI_GET_FIELD a)            \
    (M_T(name, ct) const my) {                                                \
    TUPLEI_CONTRACT(my);                                                      \
    return &(my->TUPLEI_GET_FIELD a);                                         \
  }


/* Define the SET_field methods for all params. */
#define TUPLEI_DEFINE_SETTER_FIELD(name, ...)                                 \
  M_MAP2(TUPLEI_DEFINE_SETTER_FIELD_PROTO, name, __VA_ARGS__)

#define TUPLEI_DEFINE_SETTER_FIELD_PROTO(name, a)                             \
  static inline void M_F3(name, set, TUPLEI_GET_FIELD a)                      \
       (M_T(name, ct) my, TUPLEI_GET_TYPE a const TUPLEI_GET_FIELD a) {       \
    TUPLEI_CONTRACT(my);                                                      \
    TUPLEI_CALL_SET(a, my ->TUPLEI_GET_FIELD a, TUPLEI_GET_FIELD a);          \
  }


/* Define the CMP method by calling CMP methods for all params. */
#define TUPLEI_DEFINE_CMP(name, ...)                                           \
  static inline int M_F(name, M_NAMING_COMPARE_WITH)                           \
    (M_T(name, ct) const e1,                                                   \
     M_T(name, ct) const e2) {                                                 \
    int i;                                                                     \
    TUPLEI_CONTRACT(e1);                                                       \
    TUPLEI_CONTRACT(e2);                                                       \
    M_MAP(TUPLEI_DEFINE_CMP_FUNC, __VA_ARGS__)                                 \
    return 0;                                                                  \
  }

#define TUPLEI_DEFINE_CMP_FUNC(a)                                              \
  i = TUPLEI_CALL_CMP(a, e1 -> TUPLEI_GET_FIELD a , e2 -> TUPLEI_GET_FIELD a );\
  if (i != 0) return i;


/* Define the CMP_ORDER method by calling CMP methods for all params
   In the right order
   FIXME: _cmp_order is not supported by algorithm yet.
*/
#define TUPLEI_DEFINE_CMP_ORDER(name, ...)                                    \
  static inline int M_F(name, cmp_order)(M_T(name, ct) const e1 ,             \
                                         M_T(name, ct) const e2,              \
                                         const int order[]) {                 \
    int i, r;                                                                 \
    TUPLEI_CONTRACT(e1);                                                      \
    TUPLEI_CONTRACT(e2);                                                      \
    while (true) {                                                            \
      i=*order++;                                                             \
      switch (i) {                                                            \
        case 0: return 0;                                                     \
        M_MAP2(TUPLEI_DEFINE_CMP_ORDER_FUNC, name, __VA_ARGS__)               \
      default: M_ASSERT(0);                                                   \
      }                                                                       \
    }                                                                         \
  }

#define TUPLEI_DEFINE_CMP_ORDER_FUNC(name, a)                                 \
  case M_C(name, _, TUPLEI_GET_FIELD a, _value):                             \
  case -M_C(name, _, TUPLEI_GET_FIELD a, _value):                            \
       r = TUPLEI_CALL_CMP(a, e1 -> TUPLEI_GET_FIELD a,                       \
                           e2 -> TUPLEI_GET_FIELD a);                         \
       if (r != 0) return i < 0 ? -r : r;                                     \
       break;


/* Define a CMP_field method for all given params that export a CMP method */
#define TUPLEI_DEFINE_CMP_FIELD(name, ...)                                    \
  M_MAP2(TUPLEI_MAP_CMP_FIELD, name, __VA_ARGS__)

#define TUPLEI_MAP_CMP_FIELD(name, a)                                         \
  M_IF_METHOD(CMP, TUPLEI_GET_OPLIST a)(                                      \
    TUPLEI_DEFINE_CMP_FIELD_FUNC(name, TUPLEI_GET_FIELD a, TUPLEI_GET_CMP a), \
  )

#define TUPLEI_DEFINE_CMP_FIELD_FUNC(name, field, func_cmp)                   \
  static inline int M_F3(name, cmp, field)(M_T(name, ct) const e1 ,           \
                                           M_T(name, ct) const e2) {          \
    TUPLEI_CONTRACT(e1);                                                      \
    TUPLEI_CONTRACT(e2);                                                      \
    return func_cmp(e1 -> field , e2 -> field);                               \
  }

/* Define an EQUAL method by calling the EQUAL methods  for all params */
#define TUPLEI_DEFINE_EQUAL(name, ...)                                         \
    static inline bool M_F(name, M_NAMING_TEST_EQUAL_TO)(M_T(name, ct) const e1,  \
                                                      M_T(name, ct) const e2)  \
    {                                                                          \
        bool b;                                                                \
        TUPLEI_CONTRACT(e1);                                                   \
        TUPLEI_CONTRACT(e2);                                                   \
        M_MAP(TUPLEI_DEFINE_EQUAL_FUNC, __VA_ARGS__)                           \
        return true;                                                           \
    }

#define TUPLEI_DEFINE_EQUAL_FUNC(a)                                            \
  b = TUPLEI_CALL_EQUAL(a, e1 -> TUPLEI_GET_FIELD a, e2 -> TUPLEI_GET_FIELD a);\
  if (!b) return false;


/* Define a HASH method by calling the HASH methods  for all params */
#define TUPLEI_DEFINE_HASH(name, ...)                                         \
  static inline size_t M_F(name, hash)(M_T(name, ct) const e1) {              \
    TUPLEI_CONTRACT(e1);                                                      \
    M_HASH_DECL(hash);                                                        \
    M_MAP(TUPLEI_DEFINE_HASH_FUNC, __VA_ARGS__)                               \
    return M_HASH_FINAL (hash);                                               \
  }

#define TUPLEI_DEFINE_HASH_FUNC(a)                                            \
  M_HASH_UP(hash, TUPLEI_CALL_HASH(a, e1 -> TUPLEI_GET_FIELD a) );

/* Define a GET_STR method by calling the GET_STR methods for all params */
#define TUPLEI_DEFINE_GET_STR(name, ...)                                      \
  static inline void M_F(name, get_str)(string_t str,                         \
                                        M_T(name, ct) const el,               \
                                        bool append) {                        \
    bool comma = false;                                                       \
    TUPLEI_CONTRACT(el);                                                      \
    M_ASSERT(str != NULL);                                                    \
    (append ? M_F3(string, M_NAMING_CONCATENATE_WITH, cstr):                  \
              M_F3(string, M_NAMING_INIT_WITH, cstr)) (str, "(");             \
    M_MAP(TUPLEI_DEFINE_GET_STR_FUNC, __VA_ARGS__)                            \
    M_F(string, push_back)(str, ')');                                         \
  }

#define TUPLEI_DEFINE_GET_STR_FUNC(a)                                         \
  if (comma) M_F(string, push_back) (str, ',');                               \
  comma = true;                                                               \
  TUPLEI_CALL_GET_STR(a, str, el -> TUPLEI_GET_FIELD a, true);                \


/* Define a OUT_STR method by calling the OUT_STR methods for all params */
#define TUPLEI_DEFINE_OUT_STR(name, ...)                                      \
  static inline void M_F(name, out_str)(FILE *f, M_T(name, ct) const el) {    \
    bool comma = false;                                                       \
    TUPLEI_CONTRACT(el);                                                      \
    M_ASSERT(f != NULL);                                                      \
    fputc('(', f);                                                            \
    M_MAP(TUPLEI_DEFINE_OUT_STR_FUNC , __VA_ARGS__)                           \
    fputc (')', f);                                                           \
  }

#define TUPLEI_DEFINE_OUT_STR_FUNC(a)                                         \
  if (comma) fputc (',', f);                                                  \
  comma = true;                                                               \
  TUPLEI_CALL_OUT_STR(a, f, el -> TUPLEI_GET_FIELD a);                        \


/* Define a IN_STR method by calling the IN_STR methods for all params */
#define TUPLEI_DEFINE_IN_STR(name, ...)                                       \
  static inline bool M_F(name, in_str)(M_T(name, ct) el, FILE *f) {           \
    bool comma = false;                                                       \
    TUPLEI_CONTRACT(el);                                                      \
    M_ASSERT(f != NULL);                                                      \
    int c = fgetc(f);                                                         \
    if (c != '(') return false;                                               \
    M_MAP(TUPLEI_DEFINE_IN_STR_FUNC , __VA_ARGS__)                            \
    c = fgetc(f);                                                             \
    return (c == ')');                                                        \
  }

#define TUPLEI_DEFINE_IN_STR_FUNC(a)                                          \
  if (comma) {                                                                \
    c = fgetc (f);                                                            \
    if (c != ',' || c == EOF) return false;                                   \
  }                                                                           \
  comma = true;                                                               \
  if (TUPLEI_CALL_IN_STR(a, el -> TUPLEI_GET_FIELD a, f) == false)            \
    return false ;                                                            \


/* Define a PARSE_CSTR method by calling the PARSE_CSTR methods for all params */
#define TUPLEI_DEFINE_PARSE_CSTR(name, ...)                                     \
  static inline bool M_F(name, parse_cstr)(M_T(name, ct) el,                   \
                                          const char str[],                   \
                                          const char **endptr) {              \
    TUPLEI_CONTRACT(el);                                                      \
    M_ASSERT(str != NULL);                                                    \
    bool success = false;                                                     \
    bool comma = false;                                                       \
    int c = *str++;                                                           \
    if (c != '(') goto exit;                                                  \
    M_MAP(TUPLEI_DEFINE_PARSE_CSTR_FUNC, __VA_ARGS__)                          \
    c = *str++;                                                               \
    success = (c == ')');                                                     \
  exit:                                                                       \
    if (endptr) *endptr = str;                                                \
    return success;                                                           \
  }

#define TUPLEI_DEFINE_PARSE_CSTR_FUNC(a)                                       \
  if (comma) {                                                                \
    c = *str++;                                                               \
    if (c != ',' || c == 0) goto exit;                                        \
  }                                                                           \
  comma = true;                                                               \
  if (TUPLEI_CALL_PARSE_CSTR(a, el -> TUPLEI_GET_FIELD a, str, &str) == false) \
    goto exit ;                                                               \


/* Return the parameter name as a C string */
#define TUPLEI_STRINGIFY_NAME(a)                                              \
  M_APPLY(M_AS_STR, TUPLEI_GET_FIELD a)


/* Define a OUT_SERIAL method by calling the OUT_SERIAL methods for all params */
#define TUPLEI_DEFINE_OUT_SERIAL(name, ...)                                   \
  static inline m_serial_return_code_t                                        \
  M_F(name, out_serial)(m_serial_write_t f,                                   \
                        M_T(name, ct) const el) {                             \
    TUPLEI_CONTRACT(el);                                                      \
    M_ASSERT(f != NULL && f->m_interface != NULL);                            \
    const int field_max = M_NARGS(__VA_ARGS__);                               \
    /* Define a constant static table of all fields names */                  \
    static const char *const field_name[] =                                   \
      { M_REDUCE(TUPLEI_STRINGIFY_NAME, M_ID, __VA_ARGS__) };                 \
    int index = 0;                                                            \
    m_serial_local_t local;                                                   \
    m_serial_return_code_t ret;                                               \
    ret = f->m_interface->write_tuple_start(local, f);                        \
    M_MAP(TUPLEI_DEFINE_OUT_SERIAL_FUNC , __VA_ARGS__)                        \
    M_ASSERT(index == field_max);                                             \
    ret |= f->m_interface->write_tuple_end(local, f);                         \
    return ret & M_SERIAL_FAIL;                                               \
  }

#define TUPLEI_DEFINE_OUT_SERIAL_FUNC(a)                                      \
  f->m_interface->write_tuple_id(local, f, field_name, field_max, index);     \
  TUPLEI_CALL_OUT_SERIAL(a, f, el -> TUPLEI_GET_FIELD a);                     \
  index++;                                                                    \


/* Define a IN_SERIAL method by calling the IN_SERIAL methods for all params */
#define TUPLEI_DEFINE_IN_SERIAL(name, ...)                                     \
  static inline m_serial_return_code_t                                         \
  M_F(name, in_serial)(M_T(name, ct) el, m_serial_read_t f) {                  \
    TUPLEI_CONTRACT(el);                                                       \
    M_ASSERT(f != NULL && f->m_interface != NULL);                             \
    int index = -1;                                                            \
    const int field_max = M_NARGS(__VA_ARGS__);                                \
    static const char *const field_name[] =                                    \
      { M_REDUCE(TUPLEI_STRINGIFY_NAME, M_ID, __VA_ARGS__) };                  \
    m_serial_local_t local;                                                    \
    m_serial_return_code_t ret;                                                \
    ret = f->m_interface->read_tuple_start(local, f);                          \
    while (ret == M_SERIAL_OK_CONTINUE) {                                      \
      ret = f->m_interface->read_tuple_id(local, f, field_name,                \
                                          field_max, &index);                  \
      if (ret == M_SERIAL_OK_CONTINUE) {                                       \
        M_ASSERT(index >= 0 && index < field_max);                             \
        switch (1 + index) {                                                   \
          M_MAP2(TUPLEI_DEFINE_IN_SERIAL_FUNC , name, __VA_ARGS__)             \
        default: M_ASSERT(0);                                                  \
        }                                                                      \
        ret = (ret == M_SERIAL_OK_DONE) ?                                      \
              M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;                            \
      }                                                                        \
    }                                                                          \
    return ret;                                                                \
  }

#define TUPLEI_DEFINE_IN_SERIAL_FUNC(name, a)                                  \
  case M_C(name, _, TUPLEI_GET_FIELD a, _value):                              \
  ret = TUPLEI_CALL_IN_SERIAL(a, el -> TUPLEI_GET_FIELD a, f);                 \
  break;                                                                       \

/* Define a INIT_MOVE method by calling the INIT_MOVE methods for all params */
#define TUPLEI_DEFINE_INIT_MOVE(name, ...)                                     \
  static inline void M_F(name, init_move)                                      \
                                     (M_T(name, ct) el, M_T(name, ct) org) {   \
    TUPLEI_CONTRACT(el);                                                       \
    M_MAP(TUPLEI_DEFINE_INIT_MOVE_FUNC, __VA_ARGS__)                           \
  }

#define TUPLEI_DEFINE_INIT_MOVE_FUNC(a)                                        \
  TUPLEI_CALL_INIT_MOVE(a, el -> TUPLEI_GET_FIELD a, org -> TUPLEI_GET_FIELD a);

/* Define a MOVE method by calling the MOVE methods for all params */
#define TUPLEI_DEFINE_MOVE(name, ...)                                          \
 static inline void M_F(name, move)(M_T(name, ct) el, M_T(name, ct) org) {     \
    TUPLEI_CONTRACT(el);                                                       \
    M_MAP(TUPLEI_DEFINE_MOVE_FUNC, __VA_ARGS__)                                \
 }

#define TUPLEI_DEFINE_MOVE_FUNC(a)                                             \
  TUPLEI_CALL_MOVE(a, el -> TUPLEI_GET_FIELD a, org -> TUPLEI_GET_FIELD a);

/* Define a SWAP method by calling the SWAP methods for all params */
#define TUPLEI_DEFINE_SWAP(name, ...)                                          \
  static inline void M_F(name, swap)(M_T(name, ct) el1, M_T(name, ct) el2) {   \
    TUPLEI_CONTRACT(el1);                                                      \
    TUPLEI_CONTRACT(el2);                                                      \
    M_MAP(TUPLEI_DEFINE_SWAP_FUNC , __VA_ARGS__)                               \
  }

#define TUPLEI_DEFINE_SWAP_FUNC(a)                                             \
  TUPLEI_CALL_SWAP(a, el1 -> TUPLEI_GET_FIELD a, el2 -> TUPLEI_GET_FIELD a);

/* Define a CLEAN method by calling the CLEAN methods for all params */
#define TUPLEI_DEFINE_CLEAN(name, ...)                                         \
  static inline void M_F(name, M_NAMING_CLEAN)(M_T(name, ct) el1) {            \
    TUPLEI_CONTRACT(el1);                                                      \
    M_MAP(TUPLEI_DEFINE_CLEAN_FUNC, __VA_ARGS__)                               \
  }

#define TUPLEI_DEFINE_CLEAN_FUNC(a)                                            \
  TUPLEI_CALL_CLEAN(a, el1 -> TUPLEI_GET_FIELD a);


/* Macros for testing for the presence of a method in the parameter (name, type, oplist) */
#define TUPLEI_TEST_METHOD_P(method, trio)                                     \
  M_APPLY(TUPLEI_TEST_METHOD2_P, method, M_OPFLAT trio)

#define TUPLEI_TEST_METHOD2_P(method, f, t, op)                                \
  M_TEST_METHOD_P(method, op)


/* Macros for testing for the presence of a method in all the params */
#define TUPLEI_IF_ALL(method, ...)                                            \
  M_IF(M_REDUCE2(TUPLEI_TEST_METHOD_P, M_AND, method, __VA_ARGS__))

// deferred evaluation
#define TUPLEI_OPLIST_P1(arg) TUPLEI_OPLIST_P2 arg

/* Validate the oplist before going further */
#define TUPLEI_OPLIST_P2(name, ...)                                           \
  M_IF(M_REDUCE(M_OPLIST_P, M_AND, __VA_ARGS__))                              \
    (TUPLEI_OPLIST_P3, TUPLEI_OPLIST_FAILURE)(name, __VA_ARGS__)

/* Prepare a clean compilation failure */
#define TUPLEI_OPLIST_FAILURE(name, ...)                                      \
  ((M_LIB_ERROR(ONE_ARGUMENT_OF_TUPLEI_OPLIST_IS_NOT_AN_OPLIST,               \
                name, __VA_ARGS__)))

/* Define the TUPLE op-list */
#define TUPLEI_OPLIST_P3(name, ...)                                            \
    (M_IF_METHOD_ALL(INIT, __VA_ARGS__)(INIT(M_F(name, M_NAMING_INIT)), ),     \
     INIT_SET(M_F(name, M_NAMING_INIT_WITH)),                                   \
     INIT_WITH(M_F(name, init_emplace)),                                       \
     SET(M_F(name, M_NAMING_SET_AS)),                                             \
     CLEAR(M_F(name, M_NAMING_FINALIZE)),                                         \
     TYPE(M_T(name, ct)),                                                      \
     M_IF_METHOD_ALL(CMP, __VA_ARGS__)(CMP(M_F(string, M_NAMING_COMPARE_WITH)), ),                 \
     M_IF_METHOD_ALL(HASH, __VA_ARGS__)(HASH(M_F(name, hash)), ),              \
     M_IF_METHOD_ALL(EQUAL, __VA_ARGS__)                                       \
         (EQUAL(M_F(name, M_NAMING_TEST_EQUAL_TO)), ),                            \
     M_IF_METHOD_ALL(GET_STR, __VA_ARGS__)(GET_STR(M_F(name, get_str)), ),     \
     M_IF_METHOD_ALL(PARSE_CSTR, __VA_ARGS__)                                   \
         (PARSE_CSTR(M_F(name, parse_cstr)), ),                                  \
     M_IF_METHOD_ALL(IN_STR, __VA_ARGS__)(IN_STR(M_F(name, in_str)), ),        \
     M_IF_METHOD_ALL(OUT_STR, __VA_ARGS__)(OUT_STR(M_F(name, out_str)), ),     \
     M_IF_METHOD_ALL(IN_SERIAL, __VA_ARGS__)                                   \
         (IN_SERIAL(M_F(name, in_serial)), ),                                  \
     M_IF_METHOD_ALL(OUT_SERIAL, __VA_ARGS__)                                  \
         (OUT_SERIAL(M_F(name, out_serial)), ),                                \
     M_IF_METHOD_ALL(INIT_MOVE, __VA_ARGS__)                                   \
         (INIT_MOVE(M_F(name, init_move)), ),                                  \
     M_IF_METHOD_ALL(MOVE, __VA_ARGS__)(MOVE(M_F(name, move)), ),              \
     M_IF_METHOD_ALL(SWAP, __VA_ARGS__)(SWAP(M_F(name, swap)), ),              \
     M_IF_METHOD_ALL(CLEAN, __VA_ARGS__)(CLEAN(M_F(name, M_NAMING_CLEAN)), ),  \
     M_IF_METHOD(NEW, M_RET_ARG1(__VA_ARGS__, ))(                              \
         NEW(M_DELAY2(M_GET_NEW) M_RET_ARG1(__VA_ARGS__, )), ),                \
     M_IF_METHOD(REALLOC, M_RET_ARG1(__VA_ARGS__, ))(                          \
         REALLOC(M_DELAY2(M_GET_REALLOC) M_RET_ARG1(__VA_ARGS__, )), ),        \
     M_IF_METHOD(DEL, M_RET_ARG1(__VA_ARGS__, ))(                              \
         DEL(M_DELAY2(M_GET_DEL) M_RET_ARG1(__VA_ARGS__, )), ), )

#endif // MSTARLIB_TUPLE_H
