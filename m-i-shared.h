/*
 * M*LIB - INTRUSIVE SHARED PTR Module
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
#ifndef __M_I_SHARED_PTR__H
#define __M_I_SHARED_PTR__H

#include "m-core.h"
#include "m-atomic.h"

/* Define the oplist of a intrusive shared pointer.
   USAGE: ISHARED_OPLIST(name [, oplist_of_the_type]) */
#define ISHARED_PTR_OPLIST(...)                                      \
  ISHAREDI_PTR_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                    \
                      ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ),             \
                       (__VA_ARGS__ )))

/* Interface to add to a structure to allow intrusive support.
   name: name of the intrusive shared pointer.
   type: name of the type of the structure (aka. struct test_s) - not used currently.
   NOTE: There can be only one interface of this kind in a type! */
#define ISHARED_PTR_INTERFACE(name, type)       \
  atomic_int M_C(name, _cpt)

/* Define the intrusive shared pointer type and its static inline functions.
   USAGE: ISHARED_PTR_DEF(name[, oplist]) */
#define ISHARED_PTR_DEF(name, ...)                                      \
  ISHAREDI_PTR_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                   ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ), \
                    (name, __VA_ARGS__ )))


/********************************** INTERNAL ************************************/

#define ISHAREDI_PTR_OPLIST(arg) ISHAREDI_PTR_OPLIST2 arg

#define ISHAREDI_PTR_OPLIST2(name, oplist) (                            \
  INIT(M_INIT_DEFAULT),                                                 \
  INIT_SET(M_C(name, _init_set2) M_IPTR),				\
  SET(M_C(name, _set) M_IPTR),						\
  CLEAR(M_C(name, _clear)),						\
  CLEAN(M_C(name, _clean) M_IPTR),					\
  TYPE(M_C(name, _type_t)),						\
  OPLIST(oplist),                                                       \
  SUBTYPE(M_C(name, _type_t))						\
  ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                     \
  ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)         \
  ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                     \
  )

#define ISHAREDI_PTR_DEF(arg) ISHAREDI_PTR_DEF2 arg

#define ISHAREDI_PTR_DEF2(name, type, oplist)                           \
                                                                        \
  typedef type *M_C(name,_t);						\
  typedef type M_C(name, _type_t);					\
                                                                        \
  static inline type *				                        \
  M_C(name, _init)(type *ptr)						\
  {									\
    if (M_LIKELY (ptr != NULL))                                         \
      atomic_init(&ptr->M_C(name, _cpt), 1);                            \
    return ptr;                                                         \
  }									\
  									\
  static inline type *				                        \
  M_C(name, _init_set)(type *shared)					\
  {									\
    if (M_LIKELY (shared != NULL))	{                               \
      int n = atomic_fetch_add(&(shared->M_C(name, _cpt)), 1);		\
      (void) n;								\
    }									\
    return shared;                                                      \
  }									\
  									\
  static inline void				                        \
  M_C(name, _init_set2)(type ** ptr, type *shared)			\
  {									\
    assert (ptr != NULL);                                               \
    *ptr = M_C(name, _init_set)(shared);				\
  }									\
  									\
  static inline type *				                        \
  M_C(name, _init_new)(void)						\
  {									\
    type *ptr = M_GET_NEW oplist (type);                                \
    if (ptr == NULL) {                                                  \
      M_MEMORY_FULL(sizeof(type));                                      \
      return NULL;                                                      \
    }                                                                   \
    M_GET_INIT oplist (*ptr);                                           \
    atomic_init (&ptr->M_C(name, _cpt), 1);                             \
    return ptr;                                                         \
  }									\
  									\
  static inline void				                        \
  M_C(name, _clear)(type *shared)                                       \
  {									\
    if (shared != NULL)	{						\
      if (atomic_fetch_sub(&(shared->M_C(name, _cpt)), 1) == 1)	{       \
        M_GET_CLEAR oplist (*shared);                                   \
        M_GET_DEL oplist (shared);                                      \
      }									\
    }                                                                   \
  }									\
  									\
  static inline void				                        \
  M_C(name, _clean)(type **shared)                                      \
  {									\
    M_C(name, _clear)(*shared);						\
    *shared = NULL;                                                     \
  }                                                                     \
                                                                        \
  static inline void				                        \
  M_C(name, _set)(type ** ptr, type *shared)                            \
  {									\
    assert (ptr != NULL);                                               \
    if (M_LIKELY (*ptr != shared)) {                                    \
      M_C(name, _clear)(*ptr);						\
      *ptr = M_C(name, _init_set)(shared);				\
    }                                                                   \
  }									\
                                                                        \

#endif
    
