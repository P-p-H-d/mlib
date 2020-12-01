/**
 * @file m-module.h
 * @authors Patrick Pelissier, Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * @brief Global module functionality.
 * @version 0.1
 * @date 2020-11-30
 * 
 * @copyright Copyright (c) 2017-2020, Patrick Pelissier
 * Copyright (c) 2020, Vladislav Dmitrievich Turbanov
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
#ifndef MSTARLIB_MODULE_H
#define MSTARLIB_MODULE_H

#include "m-core.h"
#include "m-atomic.h"
#include "m-mutex.h"

M_BEGIN_PROTECTED_CODE

/**
 * @brief Define the op-list of a module.
 *
 * USAGE: MODULE_OPLIST(name [, oplist_of_the_type])
 */
#define M_MODULE_OPLIST(...)                                    \
  _M_MODULE_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)               \
                         ((__VA_ARGS__, M_DEFAULT_OPLIST),      \
                          (__VA_ARGS__ )))

/**
 * @brief Declare an auxiliary state for a static shared (singleton/global) instance.
 *
 * Use this in your corresponding .h file. This should accompanied by the
 * #M_MODULE_STATE_DEF(storage, name, type) macro in a .c file.
 *
 * USAGE: M_MODULE_DEF(storage, name, type[, oplist])
 *
 * @param storage The storage to use. I.e: @c extern
 * @param name The name of the module entity.
 * @param type The name of the module's type of the structure (aka. @code struct my_module_s @endcode).
 */
#define M_MODULE_DEF(storage, name, ...)                                         \
  storage M_RET_ARG1(__VA_ARGS__) M_PRIVATE(M_I(name, instance));                \
  storage atomic_int              M_PRIVATE(M_I(name, cpt));                     \
  storage m_oncei_t               M_PRIVATE(M_I(name, once));                    \
                                                                                 \
  _M_MODULE_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                   \
                   ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)()),  \
                    (name, __VA_ARGS__ )))

/**
 * @brief Define an auxiliary state for a static shared (singleton/global) instance.
 * 
 * Use this in your corresponding .c file to define the necessary data state for the module.
 * This should accompany the #M_MODULE_DEF(storage, name, type) macro in a .h file.
 *
 * @param storage The storage to use. I.e: @c extern
 * @param name The name of the module.
 * @param type The name of the module's type of the structure (aka. @code struct test_s @endcode).
 */
#define M_MODULE_STATE_DEF(storage, name, type)              \
  type       M_PRIVATE(M_I(name, instance));                 \
  atomic_int M_PRIVATE(M_I(name, cpt));                      \
  m_oncei_t  M_PRIVATE(M_I(name, once)) = M_ONCEI_INIT_VALUE;

/********************************** INTERNAL ************************************/

// Deferred evaluation
#define _M_MODULE_OPLIST_P1(arg) _M_MODULE_OPLIST_P2 arg

/* Validation of the given op-list */
#define _M_MODULE_OPLIST_P2(name, oplist)				\
  M_IF_OPLIST(oplist)(_MODULE_OPLIST_P3, _M_MODULE_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define _M_MODULE_OPLIST_FAILURE(name, oplist)			\
  ((M_LIB_ERROR(ARGUMENT_OF_MODULE_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

// Define the oplist
#define _M_MODULE_OPLIST_P3(name, oplist) (                             \
  INIT(API_4(M_F3(name, M_NAMING_INIT, once))),                         \
  INIT_SET(API_4(M_F(name, M_NAMING_INIT_SET))),				                \
  SET(M_F(name, M_NAMING_SET) M_IPTR),						                      \
  CLEAR(M_F(name, M_NAMING_CLEAR) M_IPTR),						                  \
  CLEAN(M_F(name, M_NAMING_CLEAN) M_IPTR),					                    \
  TYPE(M_T(name, t)),                                                   \
  OPLIST(oplist),                                                       \
  SUBTYPE(M_T(name, type_t))						                                \
  )

// Deferred evaluation
#define _M_MODULE_DEF_P1(arg) _M_MODULE_DEF_P2 arg

/* Validate the oplist before going further */
#define _M_MODULE_DEF_P2(name, type, oplist)                         \
  M_IF_OPLIST(oplist)(_M_MODULE_DEF_P3, _M_MODULE_STATE_DEF_FAILURE)(name, type, oplist)

/* Stop processing with a compilation failure */
#define _M_MODULE_STATE_DEF_FAILURE(name, type, oplist)   \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(M_MODULE_DEF): the given argument is not a valid op-list: " #oplist)

#define _M_MODULE_DEF_P3(name, type, oplist)                                \
                                                                            \
  typedef type *M_T(name, t);						                                    \
  typedef type M_T(name, type_t);					                                  \
                                                                            \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                          \
                                                                            \
  static inline void                                                        \
  M_PRIVATE(M_F(name, init_static_once))()						                      \
  {                                                                         \
    if (atomic_fetch_add(&(M_PRIVATE(M_I(name, cpt))), 1) == 0) {           \
      M_CALL_INIT(oplist, (M_PRIVATE(M_I(name, instance))));                \
    }                                                                       \
  }                                                                         \
                                                                            \
  static inline M_T(name, t)                                                \
  M_F3(name, M_NAMING_INIT, once)()						                              \
  {									                                                        \
    m_oncei_call(M_PRIVATE(M_I(name, once)),                                \
                 M_PRIVATE(M_I(name, init_static_once)));                   \
    return &(M_PRIVATE(M_I(name, instance)));                               \
  }									                                                        \
                                                                            \
  static inline M_T(name, t)                                                \
  M_F(name, M_NAMING_INIT_SET)(M_T(name, t) shared)				                  \
  {									                                                        \
    if (M_LIKELY(shared != NULL)) {                                         \
      assert(shared == &(M_PRIVATE(M_I(name, instance))));                  \
      int n = atomic_fetch_add(&(M_PRIVATE(M_I(name, cpt))), 1);	      	  \
      (void) n;								                                              \
      return shared;                                                        \
    }									                                                      \
    return NULL;                                                            \
  }		                                                                      \
                                                                            \
  static inline bool                                                        \
  M_F(name, M_NAMING_TEST_NULL)(M_T(name, t) handle)						            \
  {									                                                        \
    return (handle == NULL) ||                                              \
           (atomic_load(&(M_PRIVATE(M_I(name, cpt)))) == 0);                \
  }									                                                        \
                                                                            \
  static inline bool                                                        \
  M_P(name, active)()						                                            \
  {									                                                        \
    return (atomic_load(&(M_PRIVATE(M_I(name, cpt)))) != 0);                \
  }									                                                        \
  									                                                        \
  static inline void				                                                \
  M_F(name, M_NAMING_CLEAR)(M_T(name, t) *handle)                           \
  {									                                                        \
    assert(handle != NULL);                                                 \
    if (*handle == &(M_PRIVATE(M_I(name, instance))))	                      \
    {						                                                            \
      if (atomic_fetch_sub(&(M_PRIVATE(M_I(name, cpt))), 1) == 1)	          \
      {                                                                     \
        M_CALL_CLEAR(oplist, **handle);                                     \
        handle = NULL;                                                      \
      }									                                                    \
    }                                                                       \
  }								                                                    	    \
  									                                                        \
  static inline void				                                                \
  M_F(name, M_NAMING_CLEAN)(M_T(name, t) *shared)                           \
  {									                                                        \
    M_F(name, M_NAMING_CLEAR)(shared);						                          \
  }                                                                         \
                                                                            \
  static inline void				                                                \
  M_F(name, M_NAMING_SET)(M_T(name, t) *ptr, M_T(name, t) shared)           \
  {									                                                        \
    assert(ptr != NULL);                                                    \
    if (M_LIKELY(*ptr != shared))                                           \
    {                                                                       \
      M_F(name, M_NAMING_CLEAR)(ptr);						                            \
      *ptr = M_F(name, M_NAMING_INIT_SET)(shared);				                  \
    }                                                                       \
  }

M_END_PROTECTED_CODE

#endif
