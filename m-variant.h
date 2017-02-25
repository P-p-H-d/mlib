/*
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

// FIXME: enum shall use 'name' too to avoid collision problem§?
#define VARIANTI_DEFINE_TYPE(name, ...)                                 \
  typedef struct M_C(name, _s) {                                        \
    enum { VARIANT_EMPTY,                                               \
           M_MAP(VARIANTI_DEFINE_UNION_ELE, __VA_ARGS__)                \
    } type;                                                             \
    union {                                                             \
      M_MAP(VARIANTI_DEFINE_TYPE_ELE , __VA_ARGS__)                     \
    } value;                                                            \
    } M_C(name,_t)[1];                                                  \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);
#define VARIANTI_DEFINE_UNION_ELE(a)            \
  , M_C(VARIANT_, VARIANTI_GET_FIELD a)
#define VARIANTI_DEFINE_TYPE_ELE(a)             \
  VARIANTI_GET_TYPE a VARIANTI_GET_FIELD a ;

#define VARIANTI_DEFINE_INIT(name, ...)                           \
  static inline void M_C(name, _init)(M_C(name,_t) my) {          \
    my->type = VARIANT_EMPTY;                                     \
  }

#define VARIANTI_DEFINE_INIT_SET(name, ...)                                \
  static inline void M_C(name, _init_set)(M_C(name,_t) my , M_C(name,_t) const org) { \
    my->type = org->type;                                               \
    switch (org->type) {                                                \
    case VARIANT_EMPTY; break;                                          \
    M_MAP(VARIANTI_DEFINE_INIT_SET_FUNC, __VA_ARGS__)                   \
    default: assert(false); break;                                      \
    }                                                                   \
  }
#define VARIANTI_DEFINE_INIT_SET_FUNC(a)                                \
  case M_C(VARIANT_, VARIANTI_GET_FIELD a):                             \
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
      case VARIANT_EMPTY; break;                                        \
      M_MAP(VARIANTI_DEFINE_SET_FUNC, __VA_ARGS__)                      \
      default: assert(false); break;                                    \
      }                                                                 \
    }                                                                   \
  }
#define VARIANTI_DEFINE_SET_FUNC(a)                                     \
  case M_C(VARIANT_, VARIANTI_GET_FIELD a):                             \
  VARIANTI_GET_SET a (my -> value. VARIANTI_GET_FIELD a ,               \
                      org -> value.VARIANTI_GET_FIELD a );              \
  break;

#define VARIANTI_DEFINE_CLEAR(name, ...)                                \
  static inline void M_C(name, _clear)(M_C(name,_t) my) {               \
    switch (my->type) {                                                 \
    case VARIANT_EMPTY; break;                                          \
    M_MAP(VARIANTI_DEFINE_CLEAR_FUNC, __VA_ARGS__)                      \
    default: assert(false); break;                                      \
    }                                                                   \
    my->type = VARIANT_EMPTY;                                           \
  }
#define VARIANTI_DEFINE_CLEAR_FUNC(a)                                   \
  case M_C(VARIANT_, VARIANTI_GET_FIELD a):                             \
  VARIANTI_GET_CLEAR a (my -> value. VARIANTI_GET_FIELD a);             \
  break;

#define VARIANTI_DEFINE_TEST_P(name, ...)                               \
  static inline bool M_C(name, _empty_p)(M_C(name,_t) my) {             \
    return my->type == VARIANT_EMPTY;                                   \
  }                                                                     \
  M_MAP2(VARIANTI_DEFINE_TEST_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_TEST_FUNC(name, a)                              \
  static inline bool M_C4(name, _, VARIANTI_GET_FIELD a, _p)(M_C(name,_t) my) { \
    return my->type == M_C(VARIANT_, VARIANTI_GET_FIELD a);             \
  }

#define VARIANTI_DEFINE_SETTER(name, ...)                               \
  static inline bool M_C(name, _set_empty)(M_C(name,_t) my) {           \
    M_C(name, _clear)(my);                                              \
    M_C(name, _init)(my);                                               \
  }                                                                     \
  M_MAP2(VARIANTI_DEFINE_SETTER_FUNC, name, __VA_ARGS__)
#define VARIANTI_DEFINE_SETTER_FUNC(name, a)                            \
  static inline bool M_C3(name, _set_, VARIANTI_GET_FIELD a)(M_C(name,_t) my,\
                         VARIANTI_GET_TYPE a  VARIANTI_GET_FIELD a  ) { \
    if (my->type == M_C(VARIANT_, VARIANTI_GET_FIELD a) ) {             \
      VARIANTI_GET_SET a (my -> value. VARIANTI_GET_FIELD a,            \
                          VARIANTI_GET_FIELD a);                        \
    } else {                                                            \
      M_C(name, _clear)(my);                                            \
      my->type = M_C(VARIANT_, VARIANTI_GET_FIELD a);                   \
      VARIANTI_GET_INIT_SET a(my -> value. VARIANTI_GET_FIELD a,        \
                              VARIANTI_GET_FIELD a);                    \
    }                                                                   \
  }







/********************** External interface *************************/
#define VARIANT_DEF2(name, ...)                     \
  VARIANTI_DEFINE_TYPE(name, __VA_ARGS__)           \
  VARIANTI_DEFINE_INIT(name, __VA_ARGS__)           \
  VARIANTI_DEFINE_INIT_SET(name, __VA_ARGS__)       \
  VARIANTI_DEFINE_SET(name, __VA_ARGS__)            \
  VARIANTI_DEFINE_CLEAR(name, __VA_ARGS__)          \
  VARIANTI_DEFINE_TEST_P(name, __VA_ARGS__)         \
  VARIANTI_DEFINE_SETTER(name, __VA_ARGS__)         \



  /*
  VARIANTI_DEFINE_GETTER(name, __VA_ARGS__)         \
  */

#endif
