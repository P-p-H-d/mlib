/*
 * M*LIB - VARIANT module
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
#ifndef __M_VARIANT_H
#define __M_VARIANT_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "m-core.h"

/********************************** INTERNAL ************************************/

#define VARIANTI_GET_FIELD(f,t,o) f
#define VARIANTI_GET_TYPE(f,t,o)  t
#define VARIANTI_GET_INIT(f,t,o)      M_GET_INIT o
#define VARIANTI_GET_INIT_SET(f,t,o)  M_GET_INIT_SET o
#define VARIANTI_GET_INIT_MOVE(f,t,o) M_GET_INIT_MOVE o
#define VARIANTI_GET_MOVE(f,t,o)      M_GET_MOVE o
#define VARIANTI_GET_SET(f,t,o)       M_GET_SET o
#define VARIANTI_GET_CLEAR(f,t,o)     M_GET_CLEAR o
#define VARIANTI_GET_CMP(f,t,o)       M_GET_CMP o
#define VARIANTI_GET_HASH(f,t,o)      M_GET_HASH o
#define VARIANTI_GET_EQUAL(f,t,o)     M_GET_EQUAL o
#define VARIANTI_GET_STR(f,t,o)       M_GET_GET_STR o
#define VARIANTI_GET_OUT_STR(f,t,o)   M_GET_OUT_STR o
#define VARIANTI_GET_IN_STR(f,t,o)    M_GET_IN_STR o

#define VARIANTI_DEFINE_TYPE(name, ...)                                 \
  typedef struct M_C(name, _s) {                                        \
    enum M_C(name, _enum) { M_C(name, _EMPTY)                           \
        M_MAP2(VARIANTI_DEFINE_UNION_ELE, name, __VA_ARGS__)            \
    } type;                                                             \
    union {                                                             \
      M_MAP(VARIANTI_DEFINE_TYPE_ELE , __VA_ARGS__)                     \
    } value;                                                            \
    } M_C(name,_t)[1];
#define VARIANTI_DEFINE_UNION_ELE(name, a)      \
  , M_C3(name, _, VARIANTI_GET_FIELD a)
#define VARIANTI_DEFINE_TYPE_ELE(a)             \
  VARIANTI_GET_TYPE a VARIANTI_GET_FIELD a ;

#define VARIANTI_DEFINE_INIT(name, ...)                           \
  static inline void M_C(name, _init)(M_C(name,_t) my) {          \
    my->type = M_C(name, _EMPTY);                                 \
  }


#define VARIANTI_DEFINE_INIT_SET(name, ...)                                \
  static inline void M_C(name, _init_set)(M_C(name,_t) my , M_C(name,_t) const org) { \
    my->type = org->type;                                               \
    switch (org->type) {                                                \
    case M_C(name, _EMPTY): break;                                      \
      M_MAP2(VARIANTI_DEFINE_INIT_SET_FUNC, name, __VA_ARGS__)          \
    default: assert(false); break;                                      \
    }                                                                   \
  }
#define VARIANTI_DEFINE_INIT_SET_FUNC(name, a)                          \
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  VARIANTI_GET_INIT_SET a (my -> value. VARIANTI_GET_FIELD a ,          \
                           org -> value.VARIANTI_GET_FIELD a );         \
  break;


#define VARIANTI_DEFINE_SET(name, ...)                                  \
  static inline void M_C(name, _set)(M_C(name,_t) my , M_C(name,_t) const org) { \
    if (my->type != org->type) {                                        \
      /* Different types: clear previous, create new */                 \
      M_C(name, _clear)(my);                                            \
      M_C(name, _init_set)(my, org);                                    \
    } else {                                                            \
      /* Same type: optimize the settings */                            \
      switch (org->type) {                                              \
      case M_C(name, _EMPTY): break;                                    \
        M_MAP2(VARIANTI_DEFINE_SET_FUNC, name, __VA_ARGS__)             \
      default: assert(false); break;                                    \
      }                                                                 \
    }                                                                   \
  }
#define VARIANTI_DEFINE_SET_FUNC(name, a)                               \
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  VARIANTI_GET_SET a (my -> value. VARIANTI_GET_FIELD a ,               \
                      org -> value.VARIANTI_GET_FIELD a );              \
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
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  VARIANTI_GET_CLEAR a (my -> value. VARIANTI_GET_FIELD a);             \
  break;


#define VARIANTI_DEFINE_TEST_P(name, ...)                               \
  static inline bool M_C(name, _empty_p)(M_C(name,_t) my) {             \
    return my->type == M_C(name, _EMPTY);                               \
  }                                                                     \
  static inline enum M_C(name, _enum)                                   \
  M_C(name, _type)(M_C(name,_t) my) {                                   \
    return my->type;                                                    \
  }                                                                     \
  M_MAP2(VARIANTI_DEFINE_TEST_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_TEST_FUNC(name, a)                              \
  static inline bool M_C4(name, _, VARIANTI_GET_FIELD a, _p)(M_C(name,_t) my) { \
    return my->type == M_C3(name, _, VARIANTI_GET_FIELD a);             \
  }


#define VARIANTI_DEFINE_SETTER(name, ...)                               \
  static inline void M_C(name, _set_empty)(M_C(name,_t) my) {           \
    M_C(name, _clear)(my);                                              \
    M_C(name, _init)(my);                                               \
  }                                                                     \
  M_MAP2(VARIANTI_DEFINE_SETTER_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_SETTER_FUNC(name, a)                            \
  static inline void M_C3(name, _set_, VARIANTI_GET_FIELD a)(M_C(name,_t) my, \
                         VARIANTI_GET_TYPE a  VARIANTI_GET_FIELD a  ) { \
    if (my->type == M_C3(name, _, VARIANTI_GET_FIELD a) ) {             \
      VARIANTI_GET_SET a (my -> value. VARIANTI_GET_FIELD a,            \
                          VARIANTI_GET_FIELD a);                        \
    } else {                                                            \
      M_C(name, _clear)(my);                                            \
      /* Reinit variable with the given value */                        \
      my->type = M_C3(name, _, VARIANTI_GET_FIELD a);                   \
      VARIANTI_GET_INIT_SET a(my -> value. VARIANTI_GET_FIELD a,        \
                              VARIANTI_GET_FIELD a);                    \
    }                                                                   \
  }


#define VARIANTI_DEFINE_GETTER(name, ...)                       \
  M_MAP2(VARIANTI_DEFINE_GETTER_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_GETTER_FUNC(name, a)                            \
  static inline VARIANTI_GET_TYPE a *                                   \
  M_C3(name, _get_, VARIANTI_GET_FIELD a)(M_C(name,_t) my) {            \
    if (my->type != M_C3(name, _, VARIANTI_GET_FIELD a) ) {             \
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
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  b = VARIANTI_GET_EQUAL a ( e1 -> value . VARIANTI_GET_FIELD a ,       \
                             e2 -> value . VARIANTI_GET_FIELD a );      \
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
    return hash;                                                        \
  }
#define VARIANTI_DEFINE_HASH_FUNC(name, a)                              \
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  M_HASH_UP(hash, VARIANTI_GET_HASH a ( e1 -> value . VARIANTI_GET_FIELD a) ); \
  break;


#define VARIANTI_DEFINE_INIT_MOVE(name, ...)                            \
  static inline void M_C(name, _init_move)(M_C(name,_t) el, M_C(name,_t) org) { \
    el -> type = org -> type;                                           \
    switch (el->type) {                                                 \
    case M_C(name, _EMPTY): break;                                      \
    M_MAP2(VARIANTI_DEFINE_INIT_MOVE_FUNC , name, __VA_ARGS__)          \
    default: assert(false); break;                                      \
    }                                                                   \
    org -> type = M_C(name, _EMPTY);                                    \
  }
#define VARIANTI_DEFINE_INIT_MOVE_FUNC(name, a)                         \
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  VARIANTI_GET_INIT_MOVE a (el -> value . VARIANTI_GET_FIELD a,         \
                            org -> value . VARIANTI_GET_FIELD a);       \
  break;


#define VARIANTI_DEFINE_MOVE(name, ...)                                 \
  static inline void M_C(name, _move)(M_C(name,_t) el, M_C(name,_t) org) { \
    M_C(name, _clear)(el);                                              \
    M_C(name, _init_move)(el , org);                                    \
  }

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
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  func(str, "@" M_APPLY (M_AS_STR, VARIANTI_GET_FIELD a) "@");          \
  VARIANTI_GET_STR a (str, el -> value . VARIANTI_GET_FIELD a, true);   \
  break;


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
  case M_C3(name, _, VARIANTI_GET_FIELD a):                             \
  fprintf(f, "@" M_APPLY(M_AS_STR, VARIANTI_GET_FIELD a) "@");           \
  VARIANTI_GET_OUT_STR a (f, el -> value . VARIANTI_GET_FIELD a);       \
  break;

#define VARIANTI_DEFINE_IN_STR(name, ...)                               \
  static inline bool M_C(name, _in_str)(M_C(name,_t) el,                \
                                        FILE *f) {                      \
    assert (f != NULL && el != NULL);                                   \
    /* A buffer of 400 bytes should be more than enough for all variant names... */ \
    char variantTypeBuf[400];                                           \
    if (fgetc(f) != '@') return false;                                  \
    /* First read the name of the type */                               \
    char c = fgetc(f);                                                  \
    unsigned int i = 0;                                                 \
    while (c != '@' && !feof(f) && !ferror(f) && i < sizeof(variantTypeBuf) - 1) { \
      variantTypeBuf[i++] = c;                                          \
      c = fgetc(f);                                                     \
    }                                                                   \
    variantTypeBuf[i++] = 0;                                            \
    assert(i < sizeof(variantTypeBuf));                                 \
    M_C(name, _clear)(el);                                              \
    /* In function of the type */                                       \
    if (strcmp(variantTypeBuf, "EMPTY") == 0) {                         \
      el->type = M_C(name, _EMPTY);                                     \
    }                                                                   \
    M_MAP2(VARIANTI_DEFINE_IN_STR_FUNC , name, __VA_ARGS__)             \
    else return false;                                                  \
    return fgetc(f) == '@';                                             \
  }
#define VARIANTI_DEFINE_IN_STR_FUNC(name, a)                            \
  else if (strcmp (variantTypeBuf, M_APPLY (M_AS_STR, VARIANTI_GET_FIELD a)) == 0) { \
    el->type = M_C3(name, _, VARIANTI_GET_FIELD a);                     \
    VARIANTI_GET_INIT a (el ->value . VARIANTI_GET_FIELD a );           \
    bool b = VARIANTI_GET_IN_STR a (el -> value . VARIANTI_GET_FIELD a, f); \
    if (!b) return false;                                               \
  }

#define VARIANTI_OPLIST(name, ...)                                      \
  (INIT(M_C(name,_init)),                                               \
   INIT_SET(M_C(name, _init_set)),                                      \
   SET(M_C(name,_set)),                                                 \
   CLEAR(M_C(name, _clear)),                                            \
   TYPE(M_C(name,_t)),                                                  \
   M_IF_METHOD_ALL(HASH, __VA_ARGS__)(HASH(M_C(name, _hash)),),         \
   M_IF_METHOD_ALL(EQUAL, __VA_ARGS__)(EQUAL(M_C(name, _equal_p)),),    \
   M_IF_METHOD_ALL(GET_STR, __VA_ARGS__)(EQUAL(M_C(name, _get_str)),),  \
   M_IF_METHOD_ALL(IN_STR, __VA_ARGS__)(EQUAL(M_C(name, _in_str)),),    \
   M_IF_METHOD_ALL(OUT_STR, __VA_ARGS__)(EQUAL(M_C(name, _out_str)),),  \
   M_IF_METHOD_ALL(INIT_MOVE, __VA_ARGS__)(EQUAL(M_C(name, _init_move)),), \
   M_IF_METHOD_ALL(INIT_MOVE, __VA_ARGS__)(EQUAL(M_C(name, _move)),),   \
   )

/* Macros for testing for method presence */
#define VARIANTI_TEST_METHOD2_P(method, f, t, op)  \
  M_TEST_METHOD_P(method, op)
#define VARIANTI_TEST_METHOD_P(method, trio)               \
  M_APPLY(VARIANTI_TEST_METHOD2_P, method, M_OPLIST_FLAT trio)

#define VARIANTI_ALL_EQUAL(...)                            \
  M_REDUCE2(VARIANTI_TEST_METHOD_P, M_AND, EQUAL, __VA_ARGS__)
#define VARIANTI_ALL_HASH(...)                             \
  M_REDUCE2(VARIANTI_TEST_METHOD_P, M_AND, HASH, __VA_ARGS__)
#define VARIANTI_ALL_GET_STR(...)                                  \
  M_REDUCE2(VARIANTI_TEST_METHOD_P, M_AND, GET_STR, __VA_ARGS__)
#define VARIANTI_ALL_OUT_STR(...)                                  \
  M_REDUCE2(VARIANTI_TEST_METHOD_P, M_AND, OUT_STR, __VA_ARGS__)
#define VARIANTI_ALL_IN_STR(...)                                   \
  M_REDUCE2(VARIANTI_TEST_METHOD_P, M_AND, IN_STR, __VA_ARGS__)
#define VARIANTI_ALL_INIT_MOVE(...)                                \
  M_REDUCE2(VARIANTI_TEST_METHOD_P, M_AND, INIT_MOVE, __VA_ARGS__)


/********************** External interface *************************/
#define VARIANT_DEF2(name, ...)                        \
  VARIANTI_DEFINE_TYPE(name, __VA_ARGS__)              \
  VARIANTI_DEFINE_INIT(name, __VA_ARGS__)              \
  VARIANTI_DEFINE_CLEAR(name, __VA_ARGS__)             \
  VARIANTI_DEFINE_INIT_SET(name, __VA_ARGS__)          \
  VARIANTI_DEFINE_SET(name, __VA_ARGS__)               \
  VARIANTI_DEFINE_TEST_P(name, __VA_ARGS__)            \
  VARIANTI_DEFINE_SETTER(name, __VA_ARGS__)            \
  VARIANTI_DEFINE_GETTER(name, __VA_ARGS__)            \
  M_IF(VARIANTI_ALL_HASH(__VA_ARGS__))                 \
  (VARIANTI_DEFINE_HASH(name, __VA_ARGS__),)           \
  M_IF(VARIANTI_ALL_EQUAL(__VA_ARGS__))                \
  (VARIANTI_DEFINE_EQUAL(name, __VA_ARGS__),)          \
  M_IF(VARIANTI_ALL_GET_STR(__VA_ARGS__))              \
  (VARIANTI_DEFINE_GET_STR(name, __VA_ARGS__),)        \
  M_IF(VARIANTI_ALL_OUT_STR(__VA_ARGS__))              \
  (VARIANTI_DEFINE_OUT_STR(name, __VA_ARGS__),)        \
  M_IF(VARIANTI_ALL_IN_STR(__VA_ARGS__))               \
  (VARIANTI_DEFINE_IN_STR(name, __VA_ARGS__),)         \
  M_IF(VARIANTI_ALL_INIT_MOVE(__VA_ARGS__))            \
  (VARIANTI_DEFINE_INIT_MOVE(name, __VA_ARGS__),)      \
  M_IF(VARIANTI_ALL_INIT_MOVE(__VA_ARGS__))            \
  (VARIANTI_DEFINE_MOVE(name, __VA_ARGS__),)

/* Define the oplist of a tuple.
   VARIANT_OPLIST(name[, oplist of the first type, ...]) */
#define VARIANT_OPLIST(...)                                        \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                      \
  (VARIANTI_OPLIST(__VA_ARGS__, () ),                              \
   VARIANTI_OPLIST(__VA_ARGS__ ))

#endif
