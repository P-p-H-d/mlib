/*
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
#ifndef __M_TUPLE_H
#define __M_TUPLE_H

#include "m-core.h"

/********************** External interface *************************/

/* Define the tuple type and functions.
   USAGE:
     TUPLE_DEF2(name, [(field1, type1, oplist1), (field2, type2, oplist2), ...] ) */
#define TUPLE_DEF2(name, ...)                    \
  TUPLE_DEFINE_TYPE(name, __VA_ARGS__)           \
  TUPLE_DEFINE_ENUM(name, __VA_ARGS__)           \
  TUPLE_DEFINE_INIT(name, __VA_ARGS__)           \
  TUPLE_DEFINE_INIT_SET(name, __VA_ARGS__)       \
  TUPLE_DEFINE_INIT_SET2(name, __VA_ARGS__)      \
  TUPLE_DEFINE_SET(name, __VA_ARGS__)            \
  TUPLE_DEFINE_SET2(name, __VA_ARGS__)           \
  TUPLE_DEFINE_CLEAR(name, __VA_ARGS__)          \
  TUPLE_DEFINE_GETTER(name, __VA_ARGS__)         \
  TUPLE_DEFINE_SETTER(name, __VA_ARGS__)         \
  M_IF(TUPLE_ALL_CMP(__VA_ARGS__))               \
  (TUPLE_DEFINE_CMP(name, __VA_ARGS__),)         \
  M_IF(TUPLE_ALL_CMP(__VA_ARGS__))               \
  (TUPLE_DEFINE_CMP_ORDER(name, __VA_ARGS__),)   \
  TUPLE_DEFINE_CMP_FIELD(name, __VA_ARGS__)      \
  M_IF(TUPLE_ALL_HASH(__VA_ARGS__))              \
  (TUPLE_DEFINE_HASH(name, __VA_ARGS__),)        \
  M_IF(TUPLE_ALL_EQUAL(__VA_ARGS__))             \
  (TUPLE_DEFINE_EQUAL(name, __VA_ARGS__),)       \
  M_IF(TUPLE_ALL_GET_STR(__VA_ARGS__))           \
  (TUPLE_DEFINE_GET_STR(name, __VA_ARGS__),)     \
  M_IF(TUPLE_ALL_OUT_STR(__VA_ARGS__))           \
  (TUPLE_DEFINE_OUT_STR(name, __VA_ARGS__),)     \
  M_IF(TUPLE_ALL_IN_STR(__VA_ARGS__))            \
  (TUPLE_DEFINE_IN_STR(name, __VA_ARGS__),)      \
  M_IF(TUPLE_ALL_PARSE_STR(__VA_ARGS__))         \
  (TUPLE_DEFINE_PARSE_STR(name, __VA_ARGS__),)   \
  M_IF(TUPLE_ALL_INIT_MOVE(__VA_ARGS__))         \
  (TUPLE_DEFINE_INIT_MOVE(name, __VA_ARGS__),)   \
  M_IF(TUPLE_ALL_MOVE(__VA_ARGS__))              \
  (TUPLE_DEFINE_MOVE(name, __VA_ARGS__),)        \
  M_IF(TUPLE_ALL_SWAP(__VA_ARGS__))              \
  (TUPLE_DEFINE_SWAP(name, __VA_ARGS__),)

/* Define the oplist of a tuple.
   USAGE: TUPLE_OPLIST(name[, oplist of the first type, ...]) */
#define TUPLE_OPLIST(...)                                          \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                      \
  (TUPLEI_OPLIST(__VA_ARGS__, () ),                                \
   TUPLEI_OPLIST(__VA_ARGS__ ))

/* Return an array suitable for the _cmp_order function.
   As compound literals are not supported in C++,
   provide a separate definition for C++ using initializer_list
   (shall be constexpr, but only supported in C++14).
*/
#ifndef __cplusplus
#define TUPLE_ORDER(name, ...)                                          \
  ( (const int[]) {M_MAP2_C(TUPLE_ORDER_CONVERT, name, __VA_ARGS__), 0})
#else
#include <initializer_list>
namespace m_tuple {
  template <int N>
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
#define TUPLE_ORDER(name, ...)                  \
  (m_tuple::m_int_array<M_NARGS(__VA_ARGS__,0)>({M_MAP2_C(TUPLE_ORDER_CONVERT, name, __VA_ARGS__), 0}).data)
#endif


/********************************** INTERNAL ************************************/

#define TUPLE_ORDER_CONVERT(name, x) M_C(name, M_C(TUPLE_ORDER_CONVERT_, x))
#define TUPLE_ORDER_CONVERT_ASC(x)   M_C3(_,x,_value)
#define TUPLE_ORDER_CONVERT_DSC(x)   M_C3(_,x,_value)*-1

#define TUPLE_GET_FIELD(f,t,o)    f
#define TUPLE_GET_TYPE(f,t,o)     t
#define TUPLE_GET_OPLIST(f,t,o)   o
#define TUPLE_GET_INIT(f,t,o)     M_GET_INIT o
#define TUPLE_GET_INIT_SET(f,t,o) M_GET_INIT_SET o
#define TUPLE_GET_INIT_MOVE(f,t,o) M_GET_INIT_MOVE o
#define TUPLE_GET_MOVE(f,t,o)     M_GET_MOVE o
#define TUPLE_GET_SET(f,t,o)      M_GET_SET o
#define TUPLE_GET_CLEAR(f,t,o)    M_GET_CLEAR o
#define TUPLE_GET_CMP(f,t,o)      M_GET_CMP o
#define TUPLE_GET_HASH(f,t,o)     M_GET_HASH o
#define TUPLE_GET_EQUAL(f,t,o)    M_GET_EQUAL o
#define TUPLE_GET_STR(f,t,o)      M_GET_GET_STR o
#define TUPLE_GET_OUT_STR(f,t,o)  M_GET_OUT_STR o
#define TUPLE_GET_IN_STR(f,t,o)   M_GET_IN_STR o
#define TUPLE_GET_PARSE_STR(f,t,o) M_GET_PARSE_STR o
#define TUPLE_GET_SWAP(f,t,o)     M_GET_SWAP o


#define TUPLE_DEFINE_TYPE(name, ...)                                    \
  typedef struct M_C(name, _s) {                                        \
    M_MAP(TUPLE_DEFINE_RECUR_TYPE_ELE , __VA_ARGS__)                    \
  } M_C(name,_t)[1];                                                    \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);
#define TUPLE_DEFINE_RECUR_TYPE_ELE(a)          \
  TUPLE_GET_TYPE a TUPLE_GET_FIELD a ;


#define TUPLE_DEFINE_ENUM(name, ...)                                    \
  typedef enum {                                                        \
    M_C(name, _first_one_val),                                          \
    M_MAP2_C(TUPLE_DEFINE_ENUM_ELE , name, __VA_ARGS__)                 \
  } M_C(name,_field_e);
#define TUPLE_DEFINE_ENUM_ELE(name, a)          \
  M_C4(name, _, TUPLE_GET_FIELD a, _value)


#define TUPLE_DEFINE_INIT(name, ...)                           \
  static inline void M_C(name, _init)(M_C(name,_t) my) {       \
    M_MAP(TUPLE_DEFINE_INIT_FUNC , __VA_ARGS__)                \
  }
#define TUPLE_DEFINE_INIT_FUNC(a)               \
  TUPLE_GET_INIT a ( my -> TUPLE_GET_FIELD a );


#define TUPLE_DEFINE_INIT_SET(name, ...)                                \
  static inline void M_C(name, _init_set)(M_C(name,_t) my , M_C(name,_t) const org) { \
    M_MAP(TUPLE_DEFINE_INIT_SET_FUNC , __VA_ARGS__)                     \
  }
#define TUPLE_DEFINE_INIT_SET_FUNC(a)                                  \
  TUPLE_GET_INIT_SET a ( my -> TUPLE_GET_FIELD a , org -> TUPLE_GET_FIELD a );


#define TUPLE_DEFINE_INIT_SET2(name, ...)                              \
  static inline void M_C(name, _init_set2)(M_C(name,_t) my             \
                      M_MAP(TUPLE_DEFINE_INIT_SET2_PROTO, __VA_ARGS__) \
                                           ) {                         \
    M_MAP(TUPLE_DEFINE_INIT_SET2_FUNC , __VA_ARGS__)                   \
  }
#define TUPLE_DEFINE_INIT_SET2_PROTO(a)         \
  , TUPLE_GET_TYPE a const TUPLE_GET_FIELD a
#define TUPLE_DEFINE_INIT_SET2_FUNC(a)                                 \
  TUPLE_GET_INIT_SET a ( my -> TUPLE_GET_FIELD a , TUPLE_GET_FIELD a );


#define TUPLE_DEFINE_SET(name, ...)                                     \
  static inline void M_C(name, _set)(M_C(name,_t) my ,                  \
                                     M_C(name,_t) const org) {          \
    M_MAP(TUPLE_DEFINE_SET_FUNC , __VA_ARGS__)                          \
  }
#define TUPLE_DEFINE_SET_FUNC(a)                                        \
  TUPLE_GET_SET a ( my -> TUPLE_GET_FIELD a , org -> TUPLE_GET_FIELD a );


#define TUPLE_DEFINE_SET2(name, ...)                                   \
  static inline void M_C(name, _set2)(M_C(name,_t) my                  \
                      M_MAP(TUPLE_DEFINE_SET2_PROTO, __VA_ARGS__)      \
                                           ) {                         \
    M_MAP(TUPLE_DEFINE_SET2_FUNC , __VA_ARGS__)                        \
  }
#define TUPLE_DEFINE_SET2_PROTO(a)              \
  , TUPLE_GET_TYPE a const TUPLE_GET_FIELD a
#define TUPLE_DEFINE_SET2_FUNC(a)                                       \
  TUPLE_GET_SET a ( my -> TUPLE_GET_FIELD a , TUPLE_GET_FIELD a );


#define TUPLE_DEFINE_CLEAR(name, ...)                           \
  static inline void M_C(name, _clear)(M_C(name,_t) my) {       \
    M_MAP(TUPLE_DEFINE_CLEAR_FUNC , __VA_ARGS__)                \
  }
#define TUPLE_DEFINE_CLEAR_FUNC(a)                      \
  TUPLE_GET_CLEAR a ( my -> TUPLE_GET_FIELD a );


#define TUPLE_DEFINE_GETTER(name, ...)                  \
  M_MAP2(TUPLE_DEFINE_GETTER_PROTO, name, __VA_ARGS__)
#define TUPLE_DEFINE_GETTER_PROTO(name, a)                              \
  static inline TUPLE_GET_TYPE a const * M_C3(name, _get_, TUPLE_GET_FIELD a) \
       (M_C(name,_t) const my) {                                        \
    return (TUPLE_GET_TYPE a const * ) &(my->TUPLE_GET_FIELD a);}


#define TUPLE_DEFINE_SETTER(name, ...)                                 \
  M_MAP2(TUPLE_DEFINE_SETTER_PROTO, name, __VA_ARGS__)
#define TUPLE_DEFINE_SETTER_PROTO(name, a)                             \
  static inline void M_C3(name, _set_, TUPLE_GET_FIELD a)              \
       (M_C(name,_t) my, TUPLE_GET_TYPE a const TUPLE_GET_FIELD a) {   \
    TUPLE_GET_SET a (my ->TUPLE_GET_FIELD a, TUPLE_GET_FIELD a);}


#define TUPLE_DEFINE_CMP(name, ...)                                     \
  static inline int M_C(name, _cmp)(M_C(name,_t) const e1 ,             \
                                    M_C(name,_t) const e2) {            \
    int i;                                                              \
    M_MAP(TUPLE_DEFINE_CMP_FUNC , __VA_ARGS__)                          \
    return 0;                                                           \
  }
#define TUPLE_DEFINE_CMP_FUNC(a)                                        \
  i = TUPLE_GET_CMP a ( e1 -> TUPLE_GET_FIELD a , e2 -> TUPLE_GET_FIELD a );\
  if (i != 0) return i;


#define TUPLE_DEFINE_CMP_ORDER(name, ...)                               \
  static inline int M_C(name, _cmp_order)(M_C(name,_t) const e1 ,       \
                                          M_C(name,_t) const e2,        \
                                          const int order[]) {          \
    int i, r;                                                           \
    while ((i=*order++) != 0) {                                         \
      switch (i) {                                                      \
        M_MAP2(TUPLE_DEFINE_CMP_ORDER_FUNC , name, __VA_ARGS__)         \
      default: assert(0);                                               \
      }                                                                 \
    }                                                                   \
    return 0;                                                           \
  }
#define TUPLE_DEFINE_CMP_ORDER_FUNC(name, a)                            \
  case M_C4(name, _, TUPLE_GET_FIELD a, _value):                        \
 case -M_C4(name, _, TUPLE_GET_FIELD a, _value):                        \
       r = TUPLE_GET_CMP a ( e1 -> TUPLE_GET_FIELD a , e2 -> TUPLE_GET_FIELD a ); \
       if (r != 0) return i < 0 ? -r : r;                               \
       break;


#define TUPLE_DEFINE_CMP_FIELD(name, ...)               \
  M_MAP2(TUPLE_MAP_CMP_FIELD, name, __VA_ARGS__)
#define TUPLE_MAP_CMP_FIELD(name, a)                                   \
  M_IF_METHOD(CMP, TUPLE_GET_OPLIST a)(                                \
  TUPLE_DEFINE_CMP_FIELD_FUNC(name, TUPLE_GET_FIELD a, TUPLE_GET_CMP a), \
                                                                       )
#define TUPLE_DEFINE_CMP_FIELD_FUNC(name, field, func_cmp)              \
  static inline int M_C3(name, _cmp_, field)(M_C(name,_t) const e1 ,    \
                                             M_C(name,_t) const e2) {   \
    return func_cmp ( e1 -> field , e2 -> field );                      \
  }


#define TUPLE_DEFINE_EQUAL(name, ...)                                   \
  static inline bool M_C(name, _equal_p)(M_C(name,_t) const e1 ,        \
                                         M_C(name,_t) const e2) {       \
    bool b;                                                             \
    M_MAP(TUPLE_DEFINE_EQUAL_FUNC , __VA_ARGS__)                        \
    return true;                                                        \
  }
#define TUPLE_DEFINE_EQUAL_FUNC(a)                                      \
  b = TUPLE_GET_EQUAL a ( e1 -> TUPLE_GET_FIELD a , e2 -> TUPLE_GET_FIELD a ); \
  if (!b) return false;


#define TUPLE_DEFINE_HASH(name, ...)                                    \
  static inline size_t M_C(name, _hash)(M_C(name,_t) const e1) {        \
    M_HASH_DECL(hash);                                                  \
    M_MAP(TUPLE_DEFINE_HASH_FUNC , __VA_ARGS__)                         \
    return M_HASH_FINAL (hash);					        \
  }
#define TUPLE_DEFINE_HASH_FUNC(a)                                       \
  M_HASH_UP(hash, TUPLE_GET_HASH a ( e1 -> TUPLE_GET_FIELD a) );


#define TUPLE_DEFINE_GET_STR(name, ...)                                 \
  static inline void M_C(name, _get_str)(string_t str,                  \
                                         M_C(name,_t) const el,         \
                                         bool append) {                 \
    bool comma = false;                                                 \
    assert (str != NULL && el != NULL);                                 \
    (append ? string_cat_str : string_set_str) (str, "(");              \
    M_MAP(TUPLE_DEFINE_GET_STR_FUNC , __VA_ARGS__)                      \
    string_push_back (str, ')');                                        \
  }
#define TUPLE_DEFINE_GET_STR_FUNC(a)                                    \
  if (comma) string_push_back (str, ',');                               \
  comma = true;                                                         \
  TUPLE_GET_STR a (str, el -> TUPLE_GET_FIELD a, true);                 \


#define TUPLE_DEFINE_OUT_STR(name, ...)                                 \
  static inline void M_C(name, _out_str)(FILE *f,                       \
                                         M_C(name,_t) const el) {       \
    bool comma = false;                                                 \
    assert (f != NULL && el != NULL);                                   \
    fputc('(', f);                                                      \
    M_MAP(TUPLE_DEFINE_OUT_STR_FUNC , __VA_ARGS__)                      \
    fputc (')', f);                                                     \
  }
#define TUPLE_DEFINE_OUT_STR_FUNC(a)                                    \
  if (comma) fputc (',', f);                                            \
  comma = true;                                                         \
  TUPLE_GET_OUT_STR a (f, el -> TUPLE_GET_FIELD a);                     \


#define TUPLE_DEFINE_IN_STR(name, ...)                                  \
  static inline bool M_C(name, _in_str)(M_C(name,_t) el, FILE *f) {     \
    bool comma = false;                                                 \
    assert (f != NULL && el != NULL);                                   \
    int c = fgetc(f);							\
    if (c != '(') return false;                                         \
    M_MAP(TUPLE_DEFINE_IN_STR_FUNC , __VA_ARGS__)                       \
    c = fgetc(f);                                                       \
    return (c == ')');                                                  \
  }
#define TUPLE_DEFINE_IN_STR_FUNC(a)                                     \
  if (comma) {                                                          \
    c = fgetc (f);                                                      \
    if (c != ',' || c == EOF) return false;				\
  }                                                                     \
  comma = true;                                                         \
  if (TUPLE_GET_IN_STR a (el -> TUPLE_GET_FIELD a, f) == false)         \
    return false ;                                                      \


#define TUPLE_DEFINE_PARSE_STR(name, ...)                               \
  static inline bool M_C(name, _parse_str)(M_C(name,_t) el,             \
                                        const char str[],               \
                                        const char **endptr) {          \
    assert (str != NULL && el != NULL);                                 \
    bool success = false;                                               \
    bool comma = false;                                                 \
    int c = *str++;							\
    if (c != '(') goto exit;                                            \
    M_MAP(TUPLE_DEFINE_PARSE_STR_FUNC , __VA_ARGS__)                    \
    c = *str++;                                                         \
    success = (c == ')');                                               \
  exit:                                                                 \
    if (endptr) *endptr = str;                                          \
    return success;                                                     \
  }
#define TUPLE_DEFINE_PARSE_STR_FUNC(a)                                  \
  if (comma) {                                                          \
    c = *str++;                                                         \
    if (c != ',' || c == 0) goto exit;                                  \
  }                                                                     \
  comma = true;                                                         \
  if (TUPLE_GET_PARSE_STR a (el -> TUPLE_GET_FIELD a, str, &str) == false) \
    goto exit ;                                                         \


#define TUPLE_DEFINE_INIT_MOVE(name, ...)                               \
  static inline void M_C(name, _init_move)(M_C(name,_t) el, M_C(name,_t) org) { \
    M_MAP(TUPLE_DEFINE_INIT_MOVE_FUNC , __VA_ARGS__)                    \
  }
#define TUPLE_DEFINE_INIT_MOVE_FUNC(a)                                  \
    TUPLE_GET_INIT_MOVE a (el -> TUPLE_GET_FIELD a, org -> TUPLE_GET_FIELD a);


#define TUPLE_DEFINE_MOVE(name, ...)                                    \
 static inline void M_C(name, _move)(M_C(name,_t) el, M_C(name,_t) org) { \
    M_MAP(TUPLE_DEFINE_MOVE_FUNC , __VA_ARGS__)                         \
 }
#define TUPLE_DEFINE_MOVE_FUNC(a)                                  \
    TUPLE_GET_MOVE a (el -> TUPLE_GET_FIELD a, org -> TUPLE_GET_FIELD a);


#define TUPLE_DEFINE_SWAP(name, ...)                                    \
  static inline void M_C(name, _swap)(M_C(name,_t) el1, M_C(name,_t) el2) { \
    M_MAP(TUPLE_DEFINE_SWAP_FUNC , __VA_ARGS__)                         \
  }
#define TUPLE_DEFINE_SWAP_FUNC(a)                                       \
  TUPLE_GET_SWAP a (el1 -> TUPLE_GET_FIELD a, el2 -> TUPLE_GET_FIELD a);

/* Macros for testing for method presence */
#define TUPLE_TEST_METHOD2_P(method, f, t, op)  \
  M_TEST_METHOD_P(method, op)
#define TUPLE_TEST_METHOD_P(method, trio)               \
  M_APPLY(TUPLE_TEST_METHOD2_P, method, M_OPFLAT trio)

#define TUPLE_ALL_CMP(...)                              \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, CMP, __VA_ARGS__)
#define TUPLE_ALL_EQUAL(...)                            \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, EQUAL, __VA_ARGS__)
#define TUPLE_ALL_HASH(...)                             \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, HASH, __VA_ARGS__)
#define TUPLE_ALL_GET_STR(...)                                  \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, GET_STR, __VA_ARGS__)
#define TUPLE_ALL_OUT_STR(...)                                  \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, OUT_STR, __VA_ARGS__)
#define TUPLE_ALL_IN_STR(...)                                   \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, IN_STR, __VA_ARGS__)
#define TUPLE_ALL_PARSE_STR(...)                                \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, PARSE_STR, __VA_ARGS__)
#define TUPLE_ALL_INIT_MOVE(...)                                \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, INIT_MOVE, __VA_ARGS__)
#define TUPLE_ALL_MOVE(...)                                     \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, MOVE, __VA_ARGS__)
#define TUPLE_ALL_SWAP(...)                                     \
  M_REDUCE2(TUPLE_TEST_METHOD_P, M_AND, SWAP, __VA_ARGS__)

#define TUPLEI_OPLIST(name, ...)                                        \
  (INIT(M_C(name,_init)),                                               \
   INIT_SET(M_C(name, _init_set)),                                      \
   SET(M_C(name,_set)),                                                 \
   CLEAR(M_C(name, _clear)),                                            \
   TYPE(M_C(name,_t)),                                                  \
   M_IF_METHOD_ALL(CMP, __VA_ARGS__)(CMP(M_C(name, _cmp)),),            \
   M_IF_METHOD_ALL(HASH, __VA_ARGS__)(HASH(M_C(name, _hash)),),         \
   M_IF_METHOD_ALL(EQUAL, __VA_ARGS__)(EQUAL(M_C(name, _equal_p)),),    \
   M_IF_METHOD_ALL(GET_STR, __VA_ARGS__)(GET_STR(M_C(name, _get_str)),), \
   M_IF_METHOD_ALL(PARSE_STR, __VA_ARGS__)(PARSE_STR(M_C(name, _parse_str)),), \
   M_IF_METHOD_ALL(IN_STR, __VA_ARGS__)(IN_STR(M_C(name, _in_str)),),   \
   M_IF_METHOD_ALL(OUT_STR, __VA_ARGS__)(OUT_STR(M_C(name, _out_str)),), \
   M_IF_METHOD_ALL(INIT_MOVE, __VA_ARGS__)(INIT_MOVE(M_C(name, _init_move)),), \
   M_IF_METHOD_ALL(MOVE, __VA_ARGS__)(MOVE(M_C(name, _move)),),        \
   M_IF_METHOD_ALL(SWAP, __VA_ARGS__)(MOVE(M_C(name, _swap)),),        \
   M_IF_METHOD(NEW, M_RET_ARG1(__VA_ARGS__,))(NEW(M_DELAY2(M_GET_NEW) M_RET_ARG1(__VA_ARGS__,)),), \
   M_IF_METHOD(REALLOC, M_RET_ARG1(__VA_ARGS__,))(REALLOC(M_DELAY2(M_GET_REALLOC) M_RET_ARG1(__VA_ARGS__,)),), \
   M_IF_METHOD(DEL, M_RET_ARG1(__VA_ARGS__,))(DEL(M_DELAY2(M_GET_DEL) M_RET_ARG1(__VA_ARGS__,)),), \
   )


#endif
