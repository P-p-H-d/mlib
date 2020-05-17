/*
 * M*LIB - VARIANT module
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
#ifndef MSTARLIB_VARIANT_H
#define MSTARLIB_VARIANT_H

#include "m-core.h"


/********************** External interface *************************/

/* Define the variant type and functions.
   USAGE:
     VARIANT_DEF2(name, [(field1, type1, oplist1), (field2, type2, oplist2), ...] ) */
#define VARIANT_DEF2(name, ...)                                         \
  VARIANTI_DEF2_P1( (name, VARIANTI_INJECT_GLOBAL(__VA_ARGS__)) )


/* Define the oplist of a variant.
   USAGE: VARIANT_OPLIST(name[, oplist of the first type, ...]) */
#define VARIANT_OPLIST(...)                                        \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                      \
  (VARIANTI_OPLIST_P1((__VA_ARGS__, M_DEFAULT_OPLIST)),		   \
   VARIANTI_OPLIST_P1((__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

/* Inject the oplist within the list of arguments */
#define VARIANTI_INJECT_GLOBAL(...)               \
  M_MAP_C(VARIANTI_INJECT_OPLIST_A, __VA_ARGS__)

/* Transform (x, type) into (x, type, oplist) if there is global registered oplist 
   or (x, type, M_DEFAULT_OPLIST) if there is no global one,
   or keep (x, type, oplist) if oplist was already present */
#define VARIANTI_INJECT_OPLIST_A( duo_or_trio )   \
  VARIANTI_INJECT_OPLIST_B duo_or_trio

#define VARIANTI_INJECT_OPLIST_B( f, ... )                                \
  M_IF_NARGS_EQ1(__VA_ARGS__)( (f, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)()), (f, __VA_ARGS__) )

// Deferred evaluation
#define VARIANTI_DEF2_P1(...)                  VARIANTI_DEF2_P2 __VA_ARGS__

// Test if all third argument of all arguments is an oplist
#define VARIANTI_IF_ALL_OPLIST(...)                               \
  M_IF(M_REDUCE(VARIANTI_IS_OPLIST_P, M_AND, __VA_ARGS__))
// Test if the third argument is an oplist
#define VARIANTI_IS_OPLIST_P(a)                   \
  M_OPLIST_P(M_RET_ARG3 a)

/* Validate the oplist before going further */
#define VARIANTI_DEF2_P2(name, ...)                                        \
  VARIANTI_IF_ALL_OPLIST(__VA_ARGS__)(VARIANTI_DEF2_P3, VARIANTI_DEF2_FAILURE)(name, __VA_ARGS__)

/* Stop processing with a compilation failure */
#define VARIANTI_DEF2_FAILURE(name, ...)                                   \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(VARIANT_DEF2): at least one of the given argument is not a valid oplist: " #__VA_ARGS__)

/* Define the variant */
#define VARIANTI_DEF2_P3(name, ...)                    \
  VARIANTI_DEFINE_TYPE(name, __VA_ARGS__)              \
  VARIANTI_CONTROL_ALL_OPLIST(name, __VA_ARGS__)       \
  VARIANTI_DEFINE_INIT(name, __VA_ARGS__)              \
  VARIANTI_DEFINE_CLEAR(name, __VA_ARGS__)             \
  VARIANTI_DEFINE_INIT_SET(name, __VA_ARGS__)          \
  VARIANTI_DEFINE_SET(name, __VA_ARGS__)               \
  VARIANTI_DEFINE_TEST_P(name, __VA_ARGS__)            \
  VARIANTI_IF_ALL(INIT, __VA_ARGS__)                   \
  (VARIANTI_DEFINE_INIT_FIELD(name, __VA_ARGS__),)     \
  VARIANTI_DEFINE_INIT_SETTER_FIELD(name, __VA_ARGS__) \
  VARIANTI_DEFINE_SETTER_FIELD(name, __VA_ARGS__)      \
  VARIANTI_DEFINE_GETTER_FIELD(name, __VA_ARGS__)      \
  VARIANTI_DEFINE_CLEAN_FUNC(name, __VA_ARGS__)        \
  VARIANTI_IF_ALL(HASH, __VA_ARGS__)                   \
  (VARIANTI_DEFINE_HASH(name, __VA_ARGS__),)           \
  VARIANTI_IF_ALL(EQUAL, __VA_ARGS__)                  \
  (VARIANTI_DEFINE_EQUAL(name, __VA_ARGS__),)          \
  VARIANTI_IF_ALL(GET_STR, __VA_ARGS__)                \
  (VARIANTI_DEFINE_GET_STR(name, __VA_ARGS__),)        \
  VARIANTI_IF_ALL2(PARSE_STR, INIT, __VA_ARGS__)       \
  (VARIANTI_DEFINE_PARSE_STR(name, __VA_ARGS__),)      \
  VARIANTI_IF_ALL(OUT_STR, __VA_ARGS__)                \
  (VARIANTI_DEFINE_OUT_STR(name, __VA_ARGS__),)        \
  VARIANTI_IF_ALL2(IN_STR, INIT, __VA_ARGS__)          \
  (VARIANTI_DEFINE_IN_STR(name, __VA_ARGS__),)         \
  VARIANTI_IF_ALL(OUT_SERIAL, __VA_ARGS__)             \
  (VARIANTI_DEFINE_OUT_SERIAL(name, __VA_ARGS__),)     \
  VARIANTI_IF_ALL2(IN_SERIAL, INIT, __VA_ARGS__)       \
  (VARIANTI_DEFINE_IN_SERIAL(name, __VA_ARGS__),)      \
  VARIANTI_IF_ALL(INIT_MOVE, __VA_ARGS__)              \
  (VARIANTI_DEFINE_INIT_MOVE(name, __VA_ARGS__),)      \
  VARIANTI_IF_ALL(INIT_MOVE, __VA_ARGS__)              \
  (VARIANTI_DEFINE_MOVE(name, __VA_ARGS__),)           \
  VARIANTI_IF_ALL(INIT_MOVE, __VA_ARGS__)              \
  (VARIANTI_DEFINE_MOVER(name, __VA_ARGS__),)          \
  VARIANTI_IF_ALL(SWAP, __VA_ARGS__)                   \
  (VARIANTI_DEFINE_SWAP(name, __VA_ARGS__),)

/* Get the field name, the type, the oplist or the methods
   based on the variant (field, type, oplist) */
#define VARIANTI_GET_FIELD(f,t,o)    f
#define VARIANTI_GET_TYPE(f,t,o)     t
#define VARIANTI_GET_OPLIST(f,t,o)   o
#define VARIANTI_GET_INIT(f,t,o)     M_GET_INIT o
#define VARIANTI_GET_INIT_SET(f,t,o) M_GET_INIT_SET o
#define VARIANTI_GET_INIT_MOVE(f,t,o) M_GET_INIT_MOVE o
#define VARIANTI_GET_MOVE(f,t,o)     M_GET_MOVE o
#define VARIANTI_GET_SET(f,t,o)      M_GET_SET o
#define VARIANTI_GET_CLEAR(f,t,o)    M_GET_CLEAR o
#define VARIANTI_GET_CMP(f,t,o)      M_GET_CMP o
#define VARIANTI_GET_HASH(f,t,o)     M_GET_HASH o
#define VARIANTI_GET_EQUAL(f,t,o)    M_GET_EQUAL o
#define VARIANTI_GET_STR(f,t,o)      M_GET_GET_STR o
#define VARIANTI_GET_PARSE_STR(f,t,o) M_GET_PARSE_STR o
#define VARIANTI_GET_OUT_STR(f,t,o)  M_GET_OUT_STR o
#define VARIANTI_GET_IN_STR(f,t,o)   M_GET_IN_STR o
#define VARIANTI_GET_OUT_SERIAL(f,t,o) M_GET_OUT_SERIAL o
#define VARIANTI_GET_IN_SERIAL(f,t,o)  M_GET_IN_SERIAL o
#define VARIANTI_GET_SWAP(f,t,o)     M_GET_SWAP o

/* Call the methods through API */
#define VARIANTI_CALL_INIT(t, ...)       M_APPLY_API(VARIANTI_GET_INIT t,  VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_INIT_SET(t, ...)   M_APPLY_API(VARIANTI_GET_INIT_SET t,  VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_INIT_MOVE(t, ...)  M_APPLY_API(VARIANTI_GET_INIT_MOVE t,  VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_MOVE(t, ...)       M_APPLY_API(VARIANTI_GET_MOVE t,  VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_SET(t, ...)        M_APPLY_API(VARIANTI_GET_SET t,   VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_CLEAR(t, ...)      M_APPLY_API(VARIANTI_GET_CLEAR t, VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_CMP(t, ...)        M_APPLY_API(VARIANTI_GET_CMP t,   VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_HASH(t, ...)       M_APPLY_API(VARIANTI_GET_HASH t,  VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_EQUAL(t, ...)      M_APPLY_API(VARIANTI_GET_EQUAL t, VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_GET_STR(t, ...)    M_APPLY_API(VARIANTI_GET_STR t,   VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_PARSE_STR(t, ...)  M_APPLY_API(VARIANTI_GET_PARSE_STR t, VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_OUT_STR(t, ...)    M_APPLY_API(VARIANTI_GET_OUT_STR t, VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_IN_STR(t, ...)     M_APPLY_API(VARIANTI_GET_IN_STR t, VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_OUT_SERIAL(t, ...) M_APPLY_API(VARIANTI_GET_OUT_SERIAL t, VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_IN_SERIAL(t, ...)  M_APPLY_API(VARIANTI_GET_IN_SERIAL t, VARIANTI_GET_OPLIST t, __VA_ARGS__)
#define VARIANTI_CALL_SWAP(t, ...)       M_APPLY_API(VARIANTI_GET_SWAP t,  VARIANTI_GET_OPLIST t, __VA_ARGS__)

/* Define the type */
#define VARIANTI_DEFINE_TYPE(name, ...)                                 \
  enum M_C(name, _enum) { M_C(name, _EMPTY)                             \
      M_MAP2(VARIANTI_DEFINE_UNION_ELE, name, __VA_ARGS__)              \
  };                                                                    \
  typedef struct M_C(name, _s) {                                        \
    enum M_C(name, _enum) type;                                         \
    union {                                                             \
      M_MAP(VARIANTI_DEFINE_TYPE_ELE , __VA_ARGS__)                     \
    } value;                                                            \
  } M_C(name,_t)[1];                                                    \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);
#define VARIANTI_DEFINE_UNION_ELE(name, a)      \
  , M_C4(name, _, VARIANTI_GET_FIELD a, _value)
#define VARIANTI_DEFINE_TYPE_ELE(a)             \
  VARIANTI_GET_TYPE a VARIANTI_GET_FIELD a ;


#define VARIANTI_CONTROL_ALL_OPLIST(name, ...)          \
  M_MAP2(VARIANTI_CONTROL_OPLIST, name, __VA_ARGS__)
#define VARIANTI_CONTROL_OPLIST(name, a)                                \
  M_CHECK_COMPATIBLE_OPLIST(name, VARIANTI_GET_FIELD a,                 \
                            VARIANTI_GET_TYPE a, VARIANTI_GET_OPLIST a)


#define VARIANTI_DEFINE_INIT(name, ...)                           \
  static inline void M_C(name, _init)(M_C(name,_t) my) {          \
    my->type = M_C(name, _EMPTY);                                 \
  }


#define VARIANTI_DEFINE_INIT_SET(name, ...)                             \
  static inline void M_C(name, _init_set)(M_C(name,_t) my ,             \
                                          M_C(name,_t) const org) {     \
    my->type = org->type;                                               \
    switch (org->type) {                                                \
    case M_C(name, _EMPTY): break;                                      \
      M_MAP2(VARIANTI_DEFINE_INIT_SET_FUNC, name, __VA_ARGS__)          \
    default: assert(false); break;                                      \
    }                                                                   \
  }
#define VARIANTI_DEFINE_INIT_SET_FUNC(name, a)                          \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  VARIANTI_CALL_INIT_SET(a, my -> value. VARIANTI_GET_FIELD a ,         \
                         org -> value.VARIANTI_GET_FIELD a );           \
  break;


#define VARIANTI_DEFINE_SET(name, ...)                                  \
  static inline void M_C(name, _set)(M_C(name,_t) my ,                  \
                                     M_C(name,_t) const org) {          \
    if (my->type != org->type) {                                        \
      /* Different types: clear previous one and create new */          \
      M_C(name, _clear)(my);                                            \
      M_C(name, _init_set)(my, org);                                    \
    } else {                                                            \
      /* Same type: optimize the set */                                 \
      switch (org->type) {                                              \
      case M_C(name, _EMPTY): break;                                    \
        M_MAP2(VARIANTI_DEFINE_SET_FUNC, name, __VA_ARGS__)             \
      default: assert(false); break;                                    \
      }                                                                 \
    }                                                                   \
  }
#define VARIANTI_DEFINE_SET_FUNC(name, a)                               \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  VARIANTI_CALL_SET(a, my -> value. VARIANTI_GET_FIELD a ,              \
                    org -> value.VARIANTI_GET_FIELD a );                \
  break;


#define VARIANTI_DEFINE_CLEAR(name, ...)                                \
  static inline void M_C(name, _clear)(M_C(name,_t) my) {               \
    switch (my->type) {                                                 \
    case M_C(name, _EMPTY): break;                                      \
      M_MAP2(VARIANTI_DEFINE_CLEAR_FUNC, name,  __VA_ARGS__)            \
    default: assert(false); break;                                      \
    }                                                                   \
    my->type = M_C(name, _EMPTY);                                       \
  }
#define VARIANTI_DEFINE_CLEAR_FUNC(name, a)                             \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  VARIANTI_CALL_CLEAR(a, my -> value. VARIANTI_GET_FIELD a);            \
  break;


#define VARIANTI_DEFINE_TEST_P(name, ...)                               \
  static inline bool M_C(name, _empty_p)(M_C(name,_t) const my) {       \
    return my->type == M_C(name, _EMPTY);                               \
  }                                                                     \
  static inline enum M_C(name, _enum)                                   \
  M_C(name, _type)(M_C(name,_t) my) {                                   \
    return my->type;                                                    \
  }                                                                     \
  M_MAP2(VARIANTI_DEFINE_TEST_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_TEST_FUNC(name, a)                              \
  static inline bool                                                    \
  M_C4(name, _, VARIANTI_GET_FIELD a, _p)(M_C(name,_t) const my) {      \
    return my->type == M_C4(name, _, VARIANTI_GET_FIELD a, _value);     \
  }


#define VARIANTI_DEFINE_INIT_FIELD(name, ...)                   \
  M_MAP2(VARIANTI_DEFINE_INIT_FIELD_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_INIT_FIELD_FUNC(name, a)                        \
  static inline void                                                    \
  M_C3(name, _init_, VARIANTI_GET_FIELD a)(M_C(name,_t) my) {           \
    /* Reinit variable with the given value */                          \
    my->type = M_C4(name, _, VARIANTI_GET_FIELD a, _value);             \
    VARIANTI_CALL_INIT(a, my -> value. VARIANTI_GET_FIELD a);           \
  }


#define VARIANTI_DEFINE_INIT_SETTER_FIELD(name, ...)                    \
  M_MAP2(VARIANTI_DEFINE_INIT_SETTER_FIELD_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_INIT_SETTER_FIELD_FUNC(name, a)                 \
  static inline void                                                    \
  M_C3(name, _init_set_, VARIANTI_GET_FIELD a)(M_C(name,_t) my,         \
                                               VARIANTI_GET_TYPE a const VARIANTI_GET_FIELD a  ) { \
    my->type = M_C4(name, _, VARIANTI_GET_FIELD a, _value);             \
    VARIANTI_CALL_INIT_SET(a, my -> value. VARIANTI_GET_FIELD a,        \
                           VARIANTI_GET_FIELD a);                       \
  }


#define VARIANTI_DEFINE_SETTER_FIELD(name, ...)                 \
  M_MAP2(VARIANTI_DEFINE_SETTER_FIELD_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_SETTER_FIELD_FUNC(name, a)                      \
  static inline void                                                    \
  M_C3(name, _set_, VARIANTI_GET_FIELD a)(M_C(name,_t) my,              \
                                          VARIANTI_GET_TYPE a const VARIANTI_GET_FIELD a  ) { \
    if (my->type == M_C4(name, _, VARIANTI_GET_FIELD a, _value) ) {     \
      VARIANTI_CALL_SET(a, my -> value. VARIANTI_GET_FIELD a,           \
                        VARIANTI_GET_FIELD a);                          \
    } else {                                                            \
      M_C(name, _clear)(my);                                            \
      /* Reinit variable with the given value */                        \
      my->type = M_C4(name, _, VARIANTI_GET_FIELD a, _value);           \
      VARIANTI_CALL_INIT_SET(a, my -> value. VARIANTI_GET_FIELD a,      \
                             VARIANTI_GET_FIELD a);                     \
    }                                                                   \
  }


#define VARIANTI_DEFINE_GETTER_FIELD(name, ...)                 \
  M_MAP2(VARIANTI_DEFINE_GETTER_FIELD_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_GETTER_FIELD_FUNC(name, a)                      \
  static inline VARIANTI_GET_TYPE a *                                   \
  M_C3(name, _get_, VARIANTI_GET_FIELD a)(M_C(name,_t) my) {            \
    if (my->type != M_C4(name, _, VARIANTI_GET_FIELD a, _value) ) {     \
      return NULL;                                                      \
    }                                                                   \
    return &my -> value . VARIANTI_GET_FIELD a;                         \
  }                                                                     \
                                                                        \
  static inline VARIANTI_GET_TYPE a const *                             \
  M_C3(name, _cget_, VARIANTI_GET_FIELD a)(M_C(name,_t) const my) {     \
    if (my->type != M_C4(name, _, VARIANTI_GET_FIELD a, _value) ) {     \
      return NULL;                                                      \
    }                                                                   \
    return &my -> value . VARIANTI_GET_FIELD a;                         \
  }


#define VARIANTI_DEFINE_EQUAL(name, ...)                                \
  static inline bool M_C(name, _equal_p)(M_C(name,_t) const e1 ,        \
                                         M_C(name,_t) const e2) {       \
    bool b;                                                             \
    if (e1->type != e2->type) return false;                             \
    switch (e1->type) {                                                 \
    case M_C(name, _EMPTY): break;                                      \
      M_MAP2(VARIANTI_DEFINE_EQUAL_FUNC , name, __VA_ARGS__)            \
    default: assert(false); break;                                      \
    }                                                                   \
    return true;                                                        \
  }
#define VARIANTI_DEFINE_EQUAL_FUNC(name, a)                             \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  b = VARIANTI_CALL_EQUAL(a, e1 -> value . VARIANTI_GET_FIELD a ,       \
                          e2 -> value . VARIANTI_GET_FIELD a );         \
  if (!b) return false;                                                 \
  break;


#define VARIANTI_DEFINE_HASH(name, ...)                                 \
  static inline size_t M_C(name, _hash)(M_C(name,_t) const e1) {        \
    M_HASH_DECL(hash);                                                  \
    M_HASH_UP (hash, e1 -> type);                                       \
    switch (e1->type) {                                                 \
    case M_C(name, _EMPTY): break;                                      \
      M_MAP2(VARIANTI_DEFINE_HASH_FUNC , name, __VA_ARGS__)             \
    default: assert(false); break;                                      \
    }                                                                   \
    return M_HASH_FINAL (hash);						\
  }
#define VARIANTI_DEFINE_HASH_FUNC(name, a)                              \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  M_HASH_UP(hash, VARIANTI_CALL_HASH(a, e1 -> value . VARIANTI_GET_FIELD a) ); \
  break;


#define VARIANTI_DEFINE_INIT_MOVE(name, ...)                            \
  static inline void                                                    \
  M_C(name, _init_move)(M_C(name,_t) el, M_C(name,_t) org) {            \
    el -> type = org -> type;                                           \
    switch (el->type) {                                                 \
    case M_C(name, _EMPTY): break;                                      \
    M_MAP2(VARIANTI_DEFINE_INIT_MOVE_FUNC , name, __VA_ARGS__)          \
    default: assert(false); break;                                      \
    }                                                                   \
    org -> type = M_C(name, _EMPTY);                                    \
  }
#define VARIANTI_DEFINE_INIT_MOVE_FUNC(name, a)                         \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  VARIANTI_CALL_INIT_MOVE(a, el -> value . VARIANTI_GET_FIELD a,        \
                          org -> value . VARIANTI_GET_FIELD a);         \
  break;


#define VARIANTI_DEFINE_MOVE(name, ...)                                 \
  static inline void                                                    \
  M_C(name, _move)(M_C(name,_t) el, M_C(name,_t) org) {                 \
    M_C(name, _clear)(el);                                              \
    M_C(name, _init_move)(el , org);                                    \
  }


#define VARIANTI_DEFINE_MOVER(name, ...)                                \
  M_MAP2(VARIANTI_DEFINE_MOVER_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_MOVER_FUNC(name, a)                             \
  static inline void                                                    \
  M_C3(name, _move_, VARIANTI_GET_FIELD a)(M_C(name,_t) my,             \
                                           VARIANTI_GET_TYPE a  VARIANTI_GET_FIELD a  ) { \
    M_C(name, _clear)(my);                                              \
    /* Reinit variable with the given value */                          \
    my->type = M_C4(name, _, VARIANTI_GET_FIELD a, _value);             \
    VARIANTI_CALL_INIT_MOVE(a, my -> value. VARIANTI_GET_FIELD a,       \
                            VARIANTI_GET_FIELD a);                      \
  }


#define VARIANTI_DEFINE_SWAP(name, ...)                                 \
  static inline void                                                    \
  M_C(name, _swap)(M_C(name,_t) el1, M_C(name,_t) el2) {                \
    if (el1->type == el2->type) {                                       \
      switch (el1->type) {                                              \
      case M_C(name, _EMPTY): break;                                    \
        M_MAP2(VARIANTI_DEFINE_INIT_SWAP_FUNC , name, __VA_ARGS__)      \
      default: assert(false); break;                                    \
      }                                                                 \
    } else {                                                            \
      M_C(name,_t) tmp;                                                 \
      VARIANTI_IF_ALL(INIT_MOVE, __VA_ARGS__)                           \
        (      /* NOTE: Slow implementation */                          \
         M_C(name, _init_move)(tmp, el1);                               \
         M_C(name, _init_move)(el1, el2);                               \
         M_C(name, _init_move)(el2, tmp);                               \
         ,                                                              \
         /* NOTE: Very slow implementation */                           \
         M_C(name, _init_set)(tmp, el1);                                \
         M_C(name, _set)(el1, el2);                                     \
         M_C(name, _set)(el2, tmp);                                     \
         M_C(name, _clear)(tmp);                                        \
               )                                                        \
    }                                                                   \
  }
#define VARIANTI_DEFINE_INIT_SWAP_FUNC(name, a)                         \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  VARIANTI_CALL_SWAP(a, el1 -> value . VARIANTI_GET_FIELD a,            \
                     el2 -> value . VARIANTI_GET_FIELD a);              \
  break;


#define VARIANTI_DEFINE_GET_STR(name, ...)                              \
  static inline void M_C(name, _get_str)(string_t str,                  \
                                         M_C(name,_t) const el,         \
                                         bool append) {                 \
    assert (str != NULL && el != NULL);                                 \
    void (*func)(string_t, const char *);                               \
    func = append ? string_cat_str : string_set_str;                    \
    switch (el->type) {                                                 \
    case M_C(name, _EMPTY): func(str, "@EMPTY@"); break;                \
      M_MAP2(VARIANTI_DEFINE_GET_STR_FUNC , name, __VA_ARGS__)          \
    default: assert(false); break;                                      \
    }                                                                   \
    string_push_back (str, '@');                                        \
  }
#define VARIANTI_DEFINE_GET_STR_FUNC(name, a)                           \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  func(str, "@" M_APPLY (M_AS_STR, VARIANTI_GET_FIELD a) "@");          \
  VARIANTI_CALL_GET_STR(a, str, el -> value . VARIANTI_GET_FIELD a, true); \
  break;


#define VARIANTI_DEFINE_PARSE_STR(name, ...)                            \
  static inline bool M_C(name, _parse_str)(M_C(name,_t) el,             \
                                           const char str[],            \
                                           const char **endp) {         \
    assert (str != NULL && el != NULL);                                 \
    bool success = false;                                               \
    char variantTypeBuf[M_MAX_IDENTIFIER_LENGTH+1];                     \
    int  c = *str++;                                                    \
    unsigned int i = 0;                                                 \
    M_C(name, _clean)(el);                                              \
    if (c != '@') goto exit;                                            \
    /* First read the name of the type */                               \
    c = *str++;                                                         \
    while (c != '@' && c != 0 && i < sizeof(variantTypeBuf) - 1) {	\
      variantTypeBuf[i++] = c;                                          \
      c = *str++;                                                       \
    }                                                                   \
    if (c != '@') goto exit;                                            \
    variantTypeBuf[i++] = 0;                                            \
    assert(i < sizeof(variantTypeBuf));                                 \
    /* In function of the type */                                       \
    if (strcmp(variantTypeBuf, "EMPTY") == 0) {                         \
      el->type = M_C(name, _EMPTY);                                     \
    }                                                                   \
    M_MAP2(VARIANTI_DEFINE_PARSE_STR_FUNC , name, __VA_ARGS__)          \
    else goto exit;                                                     \
    success = (*str++ == '@');                                          \
  exit:                                                                 \
    if (endp) *endp = str;                                              \
    return success;                                                     \
  }
#define VARIANTI_DEFINE_PARSE_STR_FUNC(name, a)                         \
  else if (strcmp (variantTypeBuf, M_APPLY (M_AS_STR, VARIANTI_GET_FIELD a)) == 0) { \
    el->type = M_C4(name, _, VARIANTI_GET_FIELD a, _value);             \
    VARIANTI_CALL_INIT(a, el ->value . VARIANTI_GET_FIELD a );          \
    bool b = VARIANTI_CALL_PARSE_STR(a, el -> value . VARIANTI_GET_FIELD a, str, &str); \
    if (!b) goto exit;                                                  \
  }


#define VARIANTI_DEFINE_OUT_STR(name, ...)                              \
  static inline void M_C(name, _out_str)(FILE *f,                       \
                                         M_C(name,_t) const el) {       \
    assert (f != NULL && el != NULL);                                   \
    switch (el->type) {                                                 \
    case M_C(name, _EMPTY): fprintf(f, "@EMPTY@"); break;               \
      M_MAP2(VARIANTI_DEFINE_OUT_STR_FUNC , name, __VA_ARGS__)          \
    default: assert(false); break;                                      \
    }                                                                   \
    fputc ('@', f);                                                     \
  }
#define VARIANTI_DEFINE_OUT_STR_FUNC(name, a)                           \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  fprintf(f, "@" M_APPLY(M_AS_STR, VARIANTI_GET_FIELD a) "@");          \
  VARIANTI_CALL_OUT_STR(a, f, el -> value . VARIANTI_GET_FIELD a);      \
  break;


#define VARIANTI_DEFINE_IN_STR(name, ...)                               \
  static inline bool M_C(name, _in_str)(M_C(name,_t) el,                \
                                        FILE *f) {                      \
    assert (f != NULL && el != NULL);                                   \
    char variantTypeBuf[M_MAX_IDENTIFIER_LENGTH+1];                     \
    M_C(name, _clean)(el);                                              \
    if (fgetc(f) != '@') return false;                                  \
    /* First read the name of the type */                               \
    bool b = true;                                                      \
    int c = fgetc(f);							\
    unsigned int i = 0;                                                 \
    while (c != '@' && c != EOF && i < sizeof(variantTypeBuf) - 1) {	\
      variantTypeBuf[i++] = c;                                          \
      c = fgetc(f);                                                     \
    }                                                                   \
    if (c != '@') return false;                                         \
    variantTypeBuf[i++] = 0;                                            \
    assert(i < sizeof(variantTypeBuf));                                 \
    /* In function of the type */                                       \
    if (strcmp(variantTypeBuf, "EMPTY") == 0) {                         \
      el->type = M_C(name, _EMPTY);                                     \
    }                                                                   \
    M_MAP2(VARIANTI_DEFINE_IN_STR_FUNC , name, __VA_ARGS__)             \
    else { b = false; }                                                 \
    return b && (fgetc(f) == '@');                                      \
  }
#define VARIANTI_DEFINE_IN_STR_FUNC(name, a)                            \
  else if (strcmp (variantTypeBuf, M_APPLY (M_AS_STR, VARIANTI_GET_FIELD a)) == 0) { \
    el->type = M_C4(name, _, VARIANTI_GET_FIELD a, _value);             \
    VARIANTI_CALL_INIT(a, el ->value . VARIANTI_GET_FIELD a );          \
    b = VARIANTI_CALL_IN_STR(a, el -> value . VARIANTI_GET_FIELD a, f); \
  }


#define VARIANTI_STRINGIFY_NAME(a)              \
  M_APPLY(M_AS_STR, VARIANTI_GET_FIELD a)
#define VARIANTI_DEFINE_OUT_SERIAL(name, ...)                           \
  static inline m_serial_return_code_t                                  \
  M_C(name, _out_serial)(m_serial_write_t f,                            \
                         M_C(name,_t) const el) {                       \
    const int field_max = M_NARGS(__VA_ARGS__);                         \
    static const char *const field_name[] =                             \
      { M_REDUCE(VARIANTI_STRINGIFY_NAME, M_ID, __VA_ARGS__) };         \
    assert (f != NULL && f->m_interface != NULL && el != NULL);           \
    m_serial_local_t local;                                             \
    m_serial_return_code_t ret;                                         \
    switch (el->type) {                                                 \
    case M_C(name, _EMPTY):                                             \
      return f->m_interface->write_variant_start(local, f, field_name, field_max, -1); \
      break;                                                            \
    M_MAP2(VARIANTI_DEFINE_OUT_SERIAL_FUNC , name, __VA_ARGS__)         \
    default: assert(false); break;                                      \
    }                                                                   \
    ret |= f->m_interface->write_variant_end(local, f);                   \
    return ret & M_SERIAL_FAIL;                                         \
  }
#define VARIANTI_DEFINE_OUT_SERIAL_FUNC(name, a)                        \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
  ret = f->m_interface->write_variant_start(local, f, field_name, field_max, \
                       M_C4(name, _, VARIANTI_GET_FIELD a, _value) -1); \
  VARIANTI_CALL_OUT_SERIAL(a, f, el -> value . VARIANTI_GET_FIELD a);   \
  break;


#define VARIANTI_DEFINE_IN_SERIAL(name, ...)                            \
  static inline m_serial_return_code_t                                  \
  M_C(name, _in_serial)(M_C(name,_t) el,                                \
                        m_serial_read_t f) {                            \
    const int field_max = M_NARGS(__VA_ARGS__);                         \
    static const char *const field_name[] =                             \
      { M_REDUCE(VARIANTI_STRINGIFY_NAME, M_ID, __VA_ARGS__) };         \
    assert (f != NULL && f->m_interface != NULL && el != NULL);           \
    m_serial_local_t local;                                             \
    m_serial_return_code_t ret;                                         \
    int id = -1;                                                        \
    M_C(name, _clean)(el);                                              \
    ret = f->m_interface->read_variant_start(local, f, field_name, field_max, &id); \
    if (ret != M_SERIAL_OK_CONTINUE) return ret;                        \
    assert (id >= 0 && id < field_max);                                 \
    el->type = (enum M_C(name, _enum))(id+1);                           \
    switch (id+1) {                                                     \
      M_MAP2(VARIANTI_DEFINE_IN_SERIAL_FUNC , name, __VA_ARGS__)        \
    default: assert(false); break;                                      \
    }                                                                   \
    if (ret == M_SERIAL_OK_DONE)                                        \
      ret = f->m_interface->read_variant_end(local, f);                   \
    return ret;                                                         \
  }
#define VARIANTI_DEFINE_IN_SERIAL_FUNC(name, a)                         \
  case M_C4(name, _, VARIANTI_GET_FIELD a, _value):                     \
    VARIANTI_CALL_INIT(a, el ->value . VARIANTI_GET_FIELD a );          \
    ret = VARIANTI_CALL_IN_SERIAL(a, el -> value . VARIANTI_GET_FIELD a, f); \
    break;                                                              \


#define VARIANTI_DEFINE_CLEAN_FUNC(name, ...)                           \
  static inline void M_C(name, _clean)(M_C(name,_t) my)                 \
  {                                                                     \
    M_C(name, _clear)(my);                                              \
    M_C(name, _init)(my);                                               \
  }                                                                     \


// deferred
#define VARIANTI_OPLIST_P1(arg) VARIANTI_OPLIST_P2 arg

/* Validate the oplist before going further */
#define VARIANTI_OPLIST_P2(name, ...)					\
  M_IF(M_REDUCE(M_OPLIST_P, M_AND, __VA_ARGS__))(VARIANTI_OPLIST_P3, VARIANTI_OPLIST_FAILURE)(name, __VA_ARGS__)

/* Prepare a clean compilation failure */
#define VARIANTI_OPLIST_FAILURE(name, ...)				\
  M_LIB_ERROR(ONE_ARGUMENT_OF_VARIANT_OPLIST_IS_NOT_AN_OPLIST, name, __VA_ARGS__)

#define VARIANTI_OPLIST_P3(name, ...)                                   \
  (INIT(M_C(name,_init)),                                               \
   INIT_SET(M_C(name, _init_set)),                                      \
   SET(M_C(name,_set)),                                                 \
   CLEAR(M_C(name, _clear)),                                            \
   CLEAN(M_C(name, _clean)),                                            \
   TYPE(M_C(name,_t)),                                                  \
   TEST_EMPTY(M_C(name,_empty_p)),                                      \
   M_IF_METHOD_ALL(HASH, __VA_ARGS__)(HASH(M_C(name, _hash)),),         \
   M_IF_METHOD_ALL(EQUAL, __VA_ARGS__)(EQUAL(M_C(name, _equal_p)),),    \
   M_IF_METHOD_ALL(GET_STR, __VA_ARGS__)(GET_STR(M_C(name, _get_str)),), \
   M_IF_METHOD2_ALL(PARSE_STR, INIT, __VA_ARGS__)(PARSE_STR(M_C(name, _parse_str)),), \
   M_IF_METHOD2_ALL(IN_STR, INIT, __VA_ARGS__)(IN_STR(M_C(name, _in_str)),), \
   M_IF_METHOD_ALL(OUT_STR, __VA_ARGS__)(OUT_STR(M_C(name, _out_str)),), \
   M_IF_METHOD2_ALL(IN_SERIAL, INIT, __VA_ARGS__)(IN_SERIAL(M_C(name, _in_serial)),), \
   M_IF_METHOD_ALL(OUT_SERIAL, __VA_ARGS__)(OUT_SERIAL(M_C(name, _out_serial)),), \
   M_IF_METHOD_ALL(INIT_MOVE, __VA_ARGS__)(INIT_MOVE(M_C(name, _init_move)),), \
   M_IF_METHOD_ALL(INIT_MOVE, __VA_ARGS__)(MOVE(M_C(name, _move)),),    \
   M_IF_METHOD_ALL(SWAP, __VA_ARGS__)(SWAP(M_C(name, _swap)),),         \
   M_IF_METHOD(NEW, M_RET_ARG1(__VA_ARGS__,))(NEW(M_DELAY2(M_GET_NEW) M_RET_ARG1(__VA_ARGS__,)),), \
   M_IF_METHOD(REALLOC, M_RET_ARG1(__VA_ARGS__,))(REALLOC(M_DELAY2(M_GET_REALLOC) M_RET_ARG1(__VA_ARGS__,)),), \
   M_IF_METHOD(DEL, M_RET_ARG1(__VA_ARGS__,))(DEL(M_DELAY2(M_GET_DEL) M_RET_ARG1(__VA_ARGS__,)),), \
   )

/* Macros for testing for method presence */
#define VARIANTI_TEST_METHOD_P2(method, f, t, op)  \
  M_TEST_METHOD_P(method, op)
#define VARIANTI_TEST_METHOD_P(method, trio)               \
  M_APPLY(VARIANTI_TEST_METHOD_P2, method, M_OPFLAT trio)
#define VARIANTI_IF_ALL(method, ...)                                    \
  M_IF(M_REDUCE2(VARIANTI_TEST_METHOD_P, M_AND, method, __VA_ARGS__))

#define VARIANTI_TEST_METHOD2_P2(method1, method2, f, t, op)    \
  M_AND(M_TEST_METHOD_P(method1, op), M_TEST_METHOD_P(method2, op))
#define VARIANTI_TEST_METHOD2_P(method, trio)               \
  M_APPLY(VARIANTI_TEST_METHOD2_P2, M_PAIR_1 method, M_PAIR_2 method, M_OPFLAT trio)
#define VARIANTI_IF_ALL2(method1, method2,  ...)                        \
  M_IF(M_REDUCE2(VARIANTI_TEST_METHOD2_P, M_AND, (method1, method2), __VA_ARGS__))

#endif
