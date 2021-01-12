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
#include "m-mutex.h"

M_BEGIN_PROTECTED_CODE

/* Define the op-list of a intrusive shared pointer.
   USAGE: ISHARED_OPLIST(name [, oplist_of_the_type]) */
#define ISHARED_PTR_OPLIST(...)                                               \
  ISHAREDI_PTR_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                         ((__VA_ARGS__, M_DEFAULT_OPLIST),                    \
                          (__VA_ARGS__ )))

/**
 * @brief Interface to add to a structure to allow intrusive support.
 *
 * @param name The name of the intrusive shared pointer.
 * @param type The name of the type of the structure (aka. struct test_s) - not used currently.
 * NOTE: There can be only one interface of this kind in a type!
 */
#define ISHARED_PTR_INTERFACE(name, type)                                     \
  atomic_int M_I(name, cpt)

/* Value of the interface field for static initialization (Uses C99 designated element). */
#define ISHARED_PTR_STATIC_DESIGNATED_INIT(name, type)                        \
  .M_I(name, cpt) = ATOMIC_VAR_INIT(0)

/* Value of the interface field for static initialization (Uses C89 designated element). */
#define ISHARED_PTR_STATIC_INIT(name, type)                                   \
  ATOMIC_VAR_INIT(0)

/**
 * @brief Define the intrusive shared pointer type and its static inline functions.
 *
 * USAGE: ISHARED_PTR_DEF(name, type, [, oplist])
 *
 * @param name The name of the intrusive shared pointer.
 */
#define ISHARED_PTR_DEF(name, ...)                                            \
  M_BEGIN_PROTECTED_CODE                                                      \
  ISHAREDI_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                      ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_T(name, t)  ), \
                       (name, __VA_ARGS__                                       , M_T(name, t) ))) \
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
  INIT_SET(API_4(M_F(name, M_NAMING_INIT_WITH))),                             \
  SET(M_F(name, M_NAMING_SET_AS) M_IPTR),                                     \
  CLEAR(M_F(name, M_NAMING_FINALIZE)),                                        \
  CLEAN(M_F(name, M_NAMING_CLEAN) M_IPTR),                                    \
  TYPE(M_T(name, ct)),                                                        \
  OPLIST(oplist),                                                             \
  SUBTYPE(M_T(name, subtype_ct)),                                             \
  M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),),                           \
  M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),),               \
  M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                            \
  )

// Deferred evaluation
#define ISHAREDI_PTR_DEF_P1(arg) ISHAREDI_PTR_DEF_P2 arg

/* Validate the oplist before going further */
#define ISHAREDI_PTR_DEF_P2(name, type, oplist, shared_t)                     \
  M_IF_OPLIST(oplist)(ISHAREDI_PTR_DEF_P3, ISHAREDI_PTR_DEF_FAILURE)(name, type, oplist, shared_t)

/* Stop processing with a compilation failure */
#define ISHAREDI_PTR_DEF_FAILURE(name, type, oplist, shared_t)                \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(ISHARED_PTR_DEF): the given argument is not a valid oplist: " #oplist)

#define ISHAREDI_PTR_DEF_P3(name, type, oplist, shared_t)                     \
  /* The shared pointer is only a pointer to the type */                      \
  typedef type *shared_t;                                                     \
                                                                              \
  /* Define internal types for the op-list */                                 \
  typedef shared_t *M_T(name, ct);                                            \
  typedef type      M_T(name, subtype, ct);                                   \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline shared_t                                                      \
  M_F(name, M_NAMING_INITIALIZE)(type *ptr)                                         \
  {                                                                           \
    /* Initialize the type referenced by the pointer */                       \
    if (M_LIKELY (ptr != NULL)) {                                             \
      atomic_init(&ptr->M_I(name, cpt), 2);                                   \
    }                                                                         \
    return ptr;                                                               \
  }                                                                           \
                                                                              \
                                                                              \
  static inline shared_t                                                      \
  M_F(name, M_NAMING_INIT_WITH)(shared_t shared)                              \
  {                                                                           \
    if (M_LIKELY(shared != NULL)) {                                           \
      int n = atomic_fetch_add(&(shared->M_I(name, cpt)), 2);                 \
      (void) n;                                                               \
    }                                                                         \
    return shared;                                                            \
  }                                                                           \
                                                                              \
  M_IF_METHOD(INIT, oplist)(                                                  \
  M_IF_DISABLED_METHOD(NEW, oplist)                                           \
  (                                                                           \
  /* This function is only for static object */                               \
  static inline shared_t                                                      \
  M_F(name, M_NAMING_INITIALIZE, once)(type *shared)                                \
  {                                                                           \
    if (M_LIKELY(shared != NULL)) {                                           \
      /* Pretty much like atomic_add, except the first one increment by 1,    \
         others by 2 */                                                       \
      int o = atomic_load(&(shared->M_I(name, cpt)));                         \
      int n;                                                                  \
      do {                                                                    \
        n = o + 1 + (o != 0);                                                 \
      } while (!atomic_compare_exchange_strong(&(shared->M_I(name, cpt)),     \
                                               &o, n));                       \
      if (o == 0) {                                                           \
        /* Partial initialization: _cpt is odd */                             \
        /* Call the INIT function once */                                     \
        M_CALL_INIT(oplist, *shared);                                         \
        /* Finish initialization: _cpt is even */                             \
        atomic_fetch_add(&(shared->M_I(name, cpt)), 1);                       \
      } else if ((o & 1) != 0) {                                              \
        /* Not fully initialized yet: wait for initialization */              \
        M_T(m_core, backoff, ct) bkoff;                                       \
        M_F(m_core, backoff, M_NAMING_INITIALIZE)(bkoff);                           \
        /* Wait for _cpt to be _even */                                       \
        while ((atomic_load(&(shared->M_I(name, cpt))) & 1) != 0 ) {          \
            m_core_backoff_wait(bkoff);                                       \
        }                                                                     \
      }                                                                       \
      M_ASSERT((atomic_load(&(shared->M_I(name, cpt))) & 1) == 0);            \
    }                                                                         \
    return shared;                                                            \
  }                                                                           \
  ,                                                                           \
  /* This function is only for dynamic object */                              \
  static inline shared_t                                                      \
  M_F(name, M_NAMING_INITIALIZE_NEW)(void)                                          \
  {                                                                           \
    type *ptr = M_CALL_NEW(oplist, type);                                     \
    if (ptr == NULL) {                                                        \
      M_MEMORY_FULL(sizeof(type));                                            \
      return NULL;                                                            \
    }                                                                         \
    M_CALL_INIT(oplist, *ptr);                                                \
    atomic_init(&ptr->M_I(name, cpt), 2);                                     \
    return ptr;                                                               \
  }                                                                           \
    /* End of NEW */)                                                         \
  , /* End of INIT */)                                                        \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_FINALIZE)(shared_t shared)                               \
  {                                                                           \
    if (shared != NULL) {                                                     \
      if (atomic_fetch_sub(&(shared->M_I(name, cpt)), 2) == 2) {              \
        M_CALL_CLEAR(oplist, *shared);                                        \
        M_IF_DISABLED_METHOD(DEL, oplist)(, M_CALL_DEL(oplist, shared);)      \
      }                                                                       \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_FINALIZE, ptr)(shared_t *shared)                         \
  {                                                                           \
    M_ASSERT(shared != NULL);                                                 \
    M_F(name, M_NAMING_FINALIZE)(*shared);                                    \
    *shared = NULL;                                                           \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_CLEAN)(shared_t *shared)                                 \
  {                                                                           \
    M_F(name, M_NAMING_FINALIZE)(*shared);                                    \
    *shared = NULL;                                                           \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_SET_AS)(shared_t *ptr, shared_t shared)                  \
  {                                                                           \
    M_ASSERT(ptr != NULL);                                                    \
    if (M_LIKELY(*ptr != shared)) {                                           \
      M_F(name, M_NAMING_FINALIZE)(*ptr);                                     \
      *ptr = M_F(name, M_NAMING_INIT_WITH)(shared);                           \
    }                                                                         \
  }

M_END_PROTECTED_CODE

#endif // MSTARLIB_I_SHARED_PTR_H
