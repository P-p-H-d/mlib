/*
 * M*LIB - INTRUSIVE SHARED PTR Module
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
#ifndef MSTARLIB_I_SHARED_PTR_H
#define MSTARLIB_I_SHARED_PTR_H

#include "m-core.h"
#include "m-atomic.h"

M_BEGIN_PROTECTED_CODE

/* Define the oplist of a intrusive shared pointer.
   USAGE: ISHARED_OPLIST(name [, oplist_of_the_type]) */
#define ISHARED_PTR_OPLIST(...)                                               \
  ISHAREDI_PTR_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                         ((__VA_ARGS__, M_DEFAULT_OPLIST),                    \
                          (__VA_ARGS__ )))


/* Interface to add to a structure to allow intrusive support.
   name: name of the intrusive shared pointer.
   type: name of the type of the structure (aka. struct test_s) - not used currently.
   NOTE: There can be only one interface of this kind in a type! */
#define ISHARED_PTR_INTERFACE(name, type)                                     \
  atomic_int M_C(name, _cpt)


/* Define the intrusive shared pointer type and its static inline functions.
   USAGE: ISHARED_PTR_DEF(name, type, [, oplist]) */
#define ISHARED_PTR_DEF(name, ...)                                            \
  M_BEGIN_PROTECTED_CODE                                                      \
  ISHAREDI_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                      ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name, _t)  ), \
                       (name, __VA_ARGS__                                       , M_C(name, _t) ))) \
  M_END_PROTECTED_CODE


/* Define the intrusive shared pointer type and its static inline functions
  as the name name_t
   USAGE: ISHARED_PTR_DEF_AS(name, name_t, type, [, oplist]) */
#define ISHARED_PTR_DEF_AS(name, name_t, ...)                                 \
  M_BEGIN_PROTECTED_CODE                                                      \
  ISHAREDI_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                      ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t ), \
                       (name, __VA_ARGS__                                       , name_t ))) \
  M_END_PROTECTED_CODE


/********************************** INTERNAL ************************************/

// Deferred evaluation
#define ISHAREDI_PTR_OPLIST_P1(arg) ISHAREDI_PTR_OPLIST_P2 arg

/* Validation of the given oplist */
#define ISHAREDI_PTR_OPLIST_P2(name, oplist)                                  \
  M_IF_OPLIST(oplist)(ISHAREDI_PTR_OPLIST_P3, ISHAREDI_PTR_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define ISHAREDI_PTR_OPLIST_FAILURE(name, oplist)                             \
  ((M_LIB_ERROR(ARGUMENT_OF_ISHARED_PTR_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

// Define the oplist
#define ISHAREDI_PTR_OPLIST_P3(name, oplist) (                                \
  INIT(M_INIT_DEFAULT),                                                       \
  INIT_SET(API_4(M_C(name, _init_set))),                                      \
  SET(M_C(name, _set) M_IPTR),                                                \
  CLEAR(M_C(name, _clear)),                                                   \
  CLEAN(M_C(name, _clean) M_IPTR),                                            \
  TYPE(M_C(name, _ct)),                                                       \
  OPLIST(oplist),                                                             \
  SUBTYPE(M_C(name, _subtype_ct))                                             \
  ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                           \
  ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)               \
  ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                           \
  )

// Deferred evaluatioin
#define ISHAREDI_PTR_DEF_P1(arg) ISHAREDI_PTR_DEF_P2 arg

/* Validate the oplist before going further */
#define ISHAREDI_PTR_DEF_P2(name, type, oplist, shared_t)                     \
  M_IF_OPLIST(oplist)(ISHAREDI_PTR_DEF_P3, ISHAREDI_PTR_DEF_FAILURE)(name, type, oplist, shared_t)

/* Stop processing with a compilation failure */
#define ISHAREDI_PTR_DEF_FAILURE(name, type, oplist, shared_t)                \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(ISHARED_PTR_DEF): the given argument is not a valid oplist: " #oplist)

#define ISHAREDI_PTR_DEF_P3(name, type, oplist, shared_t)                     \
                                                                              \
  /* The shared pointer is only a pointer to the type */                      \
  typedef type *shared_t;                                                     \
                                                                              \
  /* Define internal types for oplist */                                      \
  typedef shared_t M_C(name, _ct);                                            \
  typedef type     M_C(name, _subtype_ct);                                    \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline shared_t                                                      \
  M_C(name, _init)(type *ptr)                                                 \
  {                                                                           \
    /* Initialize the type referenced by the pointer */                       \
    if (M_LIKELY (ptr != NULL)) {                                             \
      atomic_init(&ptr->M_C(name, _cpt), 1);                                  \
    }                                                                         \
    return ptr;                                                               \
  }                                                                           \
                                                                              \
  static inline shared_t                                                      \
  M_C(name, _init_set)(shared_t shared)                                       \
  {                                                                           \
    if (M_LIKELY (shared != NULL)) {                                          \
      int n = atomic_fetch_add(&(shared->M_C(name, _cpt)), 1);                \
      (void) n;                                                               \
    }                                                                         \
    return shared;                                                            \
  }                                                                           \
                                                                              \
  static inline void M_ATTR_DEPRECATED                                        \
  M_C(name, _init_set2)(shared_t *ptr, shared_t shared)                       \
  {                                                                           \
    assert (ptr != NULL);                                                     \
    *ptr = M_C(name, _init_set)(shared);                                      \
  }                                                                           \
                                                                              \
  M_IF_DISABLED_METHOD(NEW, oplist)                                           \
  ( /* Nothing to do */, M_IF_METHOD(INIT, oplist)                            \
    (                                                                         \
  static inline shared_t                                                      \
  M_C(name, _init_new)(void)                                                  \
  {                                                                           \
    type *ptr = M_CALL_NEW(oplist, type);                                     \
    if (ptr == NULL) {                                                        \
      M_MEMORY_FULL(sizeof(type));                                            \
      return NULL;                                                            \
    }                                                                         \
    M_CALL_INIT(oplist, *ptr);                                                \
    atomic_init (&ptr->M_C(name, _cpt), 1);                                   \
    return ptr;                                                               \
  }                                                                           \
  , /* End of INIT */)                                                        \
    /* End of NEW */)                                                         \
                                                                              \
  static inline void                                                          \
  M_C(name, _clear)(shared_t shared)                                          \
  {                                                                           \
    if (shared != NULL) {                                                     \
      if (atomic_fetch_sub(&(shared->M_C(name, _cpt)), 1) == 1) {             \
        M_CALL_CLEAR(oplist, *shared);                                        \
        M_IF_DISABLED_METHOD(DEL, oplist)(, M_CALL_DEL(oplist, shared);)      \
      }                                                                       \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _clear_ptr)(shared_t *shared)                                     \
  {                                                                           \
    assert(shared != NULL);                                                   \
    M_C(name, _clear)(*shared);                                               \
    *shared = NULL;                                                           \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _clean)(shared_t *shared)                                         \
  {                                                                           \
    M_C(name, _clear)(*shared);                                               \
    *shared = NULL;                                                           \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _set)(shared_t *ptr, shared_t shared)                             \
  {                                                                           \
    assert (ptr != NULL);                                                     \
    if (M_LIKELY (*ptr != shared)) {                                          \
      M_C(name, _clear)(*ptr);                                                \
      *ptr = M_C(name, _init_set)(shared);                                    \
    }                                                                         \
  }                                                                           \
                                                                              \

M_END_PROTECTED_CODE

#endif
