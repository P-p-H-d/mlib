/*
 * M*LIB - SHARED PTR Module
 *
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
#ifndef __M_SHARED_PTR__H
#define __M_SHARED_PTR__H

#include "m-core.h"
#include "m-atomic.h"

/* Define the oplist of a shared pointer.
   USAGE: SHARED_OPLIST(name [, oplist_of_the_type]) */
#define SHARED_PTR_OPLIST(...)                                          \
  SHAREDI_PTR_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
                     ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ), \
                      _OPLIST(__VA_ARGS__ )))

/* Define shared pointer and its function.
   USAGE: SHARED_PTR_DEF(name[, oplist]) */
#define SHARED_PTR_DEF(name, ...)                                       \
  SHAREDI_PTR_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                  ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ), \
                   (name, __VA_ARGS__ )))

/********************************** INTERNAL ************************************/

// deferred
#define SHAREDI_PTR_OPLIST(arg) SHAREDI_PTR_OPLIST2 arg

#define SHAREDI_PTR_OPLIST2(name, oplist) (                             \
  INIT(M_C(name, _init)),                                               \
  CLEAR(M_C(name, _clear)),                                             \
  INIT_SET(M_C(name, _init_set)),                                       \
  SET(M_C(name, _set))                                                  \
  INIT_MOVE(M_C(name, _init_move)),                                     \
  CLEAN(M_C(name, _clean)),                                             \
  MOVE(M_C(name, _move)),                                               \
  SWAP(M_C(name, _swap))                                                \
  ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                     \
  ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)         \
  ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                     \
  )

// deferred
#define SHAREDI_PTR_DEF(arg) SHAREDI_PTR_DEF2 arg

#define SHAREDI_PTR_DEF2(name, type, oplist)				\
									\
  typedef struct M_C(name, _s){						\
    type *data;	        /* Pointer to the data */                       \
    atomic_int   cpt;   /* Counter of how many points to the data */    \
    bool  combineAlloc; /* Does the data and the ptr share the slot? */ \
  } *M_C(name, _t)[1];							\
		  							\
  typedef struct M_C(name, combine_s) {                                 \
    type data;                                                          \
    struct M_C(name, _s) ptr;						\
  } M_C(name, combine_t)[1];						\
                                                                        \
  static inline void				                        \
  M_C(name, _init)(M_C(name, _t) shared)                                \
  {									\
    *shared = NULL;                                                     \
  }                                                                     \
                                                                        \
  static inline void				                        \
  M_C(name, _init2)(M_C(name, _t) shared, type *data)			\
  {									\
    assert (shared != NULL);                                            \
    /* The shared ptr get exclusive access to data */                   \
    struct M_C(name, _s) *ptr;						\
    if (M_UNLIKELY (data == NULL)) {                                    \
      *shared = NULL;                                                   \
      return;                                                           \
    }                                                                   \
    ptr = M_GET_NEW oplist (struct M_C(name, _s));			\
    if (M_UNLIKELY (ptr == NULL)) {                                     \
      M_MEMORY_FULL(sizeof(struct M_C(name, _s)));			\
      return;                                                           \
    }                                                                   \
    ptr->data = data;							\
    atomic_init (&ptr->cpt, 1);                                         \
    ptr->combineAlloc = false;                                          \
    *shared = ptr;							\
    SHAREDI_CONTRACT(shared);                                           \
  }									\
									\
  static inline void				                        \
  M_C(name, _init_new)(M_C(name, _t) shared)				\
  {									\
    /* NOTE: Alloc 1 struct with both structures. */                    \
    struct M_C(name, combine_s) *p =					\
      M_GET_NEW oplist (struct M_C(name, combine_s));			\
    if (M_UNLIKELY (p == NULL)) {                                       \
      M_MEMORY_FULL(sizeof(struct M_C(name, combine_s)));		\
      return;								\
    }                                                                   \
    struct M_C(name, _s) *ptr = &p->ptr;				\
    type *data = &p->data;                                              \
    M_GET_INIT oplist(*data);                                           \
    ptr->data = data;							\
    atomic_init (&ptr->cpt, 1);                                         \
    ptr->combineAlloc = true;                                           \
    *shared = ptr;							\
    SHAREDI_CONTRACT(shared);                                           \
  }									\
									\
  static inline bool				                        \
  M_C(name, _NULL_p)(const M_C(name, _t) shared)			\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    return *shared == NULL;						\
  }									\
									\
  static inline void				                        \
  M_C(name, _init_set)(M_C(name, _t) dest,				\
		       const M_C(name, _t) shared)			\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    assert (dest != shared);                                            \
    *dest = *shared;							\
    if (*dest != NULL) {						\
      int n = atomic_fetch_add(&((*dest)->cpt), 1);			\
      (void) n;	/* unused return value */				\
    }									\
    SHAREDI_CONTRACT(dest);                                             \
  }									\
									\
  static inline void				                        \
  M_C(name, _clear)(M_C(name, _t) dest)					\
  {									\
    SHAREDI_CONTRACT(dest);                                             \
    if (*dest != NULL)	{						\
      if (atomic_fetch_sub(&((*dest)->cpt), 1) == 1)	{		\
        bool combineAlloc = (*dest)->combineAlloc;                      \
        /* Note: if combineAlloc is true, the address of the slot       \
           combining both data & ptr is the same as the address of the  \
           first element, aka data itself. Static analyzer tools don't  \
           seem to detect this and report error. */                     \
        M_GET_CLEAR oplist (*(*dest)->data);                            \
        M_GET_DEL oplist ((*dest)->data);                               \
        if (combineAlloc == false)                                      \
          M_GET_DEL oplist (*dest);                                     \
      }									\
      *dest = NULL;                                                     \
    }                                                                   \
    SHAREDI_CONTRACT(dest);                                             \
  }									\
                                                                        \
  static inline void				                        \
  M_C(name, _clean)(M_C(name, _t) dest)					\
  {									\
    /* NOTE: Clear will also set dest to NULL */                        \
    M_C(name, _clear)(dest);						\
  }                                                                     \
									\
  static inline void				                        \
  M_C(name, _set)(M_C(name, _t) dest,					\
		  const M_C(name, _t) shared)				\
  {									\
    SHAREDI_CONTRACT(dest);                                             \
    SHAREDI_CONTRACT(shared);                                           \
    M_C(name, _clear)(dest);						\
    M_C(name, _init_set)(dest, shared);					\
  }									\
									\
  static inline void				                        \
  M_C(name, _init_move)(M_C(name, _t) dest,				\
			M_C(name, _t) shared)				\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    assert (dest != NULL && dest != shared);                            \
    *dest = *shared;							\
    *shared = NULL;                                                     \
    SHAREDI_CONTRACT(dest);                                             \
  }									\
									\
  static inline void				                        \
  M_C(name, _move)(M_C(name, _t) dest,					\
		   M_C(name, _t) shared)				\
  {									\
    SHAREDI_CONTRACT(dest);                                             \
    SHAREDI_CONTRACT(shared);                                           \
    assert (dest != shared);						\
    M_C(name, _clear)(dest);						\
    M_C(name, _init_move)(dest, shared);				\
  }									\
									\
  static inline void				                        \
  M_C(name, _swap)(M_C(name, _t) p1,					\
		   M_C(name, _t) p2)					\
  {									\
    SHAREDI_CONTRACT(p1);                                               \
    SHAREDI_CONTRACT(p2);                                               \
    M_SWAP (struct M_C(name, _s)*, *p1, *p2);				\
    SHAREDI_CONTRACT(p1);                                               \
    SHAREDI_CONTRACT(p2);                                               \
  }									\
									\
  static inline bool				                        \
  M_C(name, _equal_p)(const M_C(name, _t) p1,				\
		      const M_C(name, _t) p2)				\
  {									\
    SHAREDI_CONTRACT(p1);                                               \
    SHAREDI_CONTRACT(p2);                                               \
    return *p1 == *p2;							\
  }									\
									\
  static inline const type *						\
  M_C(name, _cref)(const M_C(name, _t) shared)				\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    assert(*shared != NULL);                                            \
    type *data = (*shared)->data;                                       \
    assert (data != NULL);                                              \
    return M_CONST_CAST (type, data);                                   \
  }									\
									\
  static inline type *				                        \
  M_C(name, _ref)(M_C(name, _t) shared)					\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    assert(*shared != NULL);                                            \
    type *data = (*shared)->data;                                       \
    assert (data != NULL);                                              \
    return data;                                                        \
  }									\
  
/********************************** INTERNAL ************************************/

#define SHAREDI_CONTRACT(shared) do {                                   \
    assert(shared != NULL);                                             \
    assert(*shared == NULL || atomic_load (&(*shared)->cpt) >= 1);      \
  } while (0)

#endif
