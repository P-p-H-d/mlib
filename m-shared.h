/*
 * M*LIB - SHARED PTR Module
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
#ifndef __M_SHARED_PTR__H
#define __M_SHARED_PTR__H

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>

#include "m-core.h"

/* Define the oplist of a shared pointer.
   USAGE: SHARED_OPLIST(name [, oplist_of_the_type]) */
#define SHARED_PTR_OPLIST(...)                                       \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                        \
  (SHAREDI_PTR_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),               \
   SHAREDI_PTR_OPLIST(__VA_ARGS__ ))

/* Define shared pointer and its function.
   USAGE: SHARED_PTR_DEF(name[, oplist]) */
#define SHARED_PTR_DEF(name, ...)                              \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                  \
  (SHAREDI_PTR_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST ),     \
   SHAREDI_PTR_DEF2(name, __VA_ARGS__ ))

/********************************** INTERNAL ************************************/

#define SHAREDI_PTR_OPLIST(name, oplist) (       \
  INIT(M_C3(shared_, name, _init)),              \
  CLEAR(M_C3(shared_, name, _clear)),            \
  INIT_SET(M_C3(shared_, name, _init_set)),      \
  SET(M_C3(shared_, name, _set))                 \
  INIT_MOVE(M_C3(shared_, name, _init_move)),    \
  CLEAN(M_C3(shared_, name, _clean)),            \
  MOVE(M_C3(shared_, name, _move)),              \
  SWAP(M_C3(shared_, name, _swap))               \
  )

#define SHAREDI_PTR_DEF2(name, type, oplist)                            \
  typedef struct M_C3(shared_, name, _s){				\
    type *data;	/* Pointer to the data */                               \
    int   cpt;  /* Counter of how many points to the data */            \
    bool  combineAlloc; /* Does the data and the ptr share the slot? */ \
  } *M_C3(shared_, name, _t)[1];					\
									\
  typedef struct M_C3(shared_contain_, name, _s) {                      \
    type data;                                                          \
    struct M_C3(shared_, name, _s) ptr;                                 \
  } M_C3(shared_contain_, name, _t)[1];                                 \
                                                                        \
  typedef union {                                                       \
    type *ptr;                                                          \
    const type *cptr;                                                   \
  } M_C3(shared_union_, name,_t);                                       \
                                                                        \
  static inline const type *                                            \
  M_C3(shared_, name, _const_cast)(type *ptr)                           \
  {                                                                     \
    M_C3(shared_union_, name,_t) u;                                     \
    u.ptr = ptr;                                                        \
    return u.cptr;                                                      \
  }                                                                     \
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _init)(M_C3(shared_, name, _t) shared)            \
  {									\
    *shared = NULL;                                                     \
  }                                                                     \
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _init2)(M_C3(shared_, name, _t) shared, type *data) \
  {									\
    struct M_C3(shared_, name, _s) *ptr;				\
    if (data == NULL) {                                                 \
      *shared = NULL;                                                   \
      return;                                                           \
    }                                                                   \
    ptr = M_GET_NEW oplist (struct M_C3(shared_, name, _s));		\
    if (ptr == NULL) {                                                  \
      M_MEMORY_FULL(sizeof(struct M_C3(shared_, name, _s)));            \
      return;                                                           \
    }                                                                   \
    ptr->data = data;							\
    ptr->cpt = 1;                                                       \
    ptr->combineAlloc = false;                                          \
    *shared = ptr;							\
    SHAREDI_CONTRACT(shared);                                           \
  }									\
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _init_new)(M_C3(shared_, name, _t) shared)        \
  {									\
    /* NOTE: Alloc 1 struct with both structures. */                    \
    struct M_C3(shared_contain_, name, _s) *p =                         \
      M_GET_NEW oplist (struct M_C3(shared_contain_, name, _s));        \
    if (p == NULL) {                                                    \
      M_MEMORY_FULL(sizeof(struct M_C3(shared_contain_, name, _s)));    \
      return;                                                           \
    }                                                                   \
    struct M_C3(shared_, name, _s) *ptr = &p->ptr;                      \
    type *data = &p->data;                                              \
    M_GET_INIT oplist(*data);                                           \
    ptr->data = data;							\
    ptr->cpt = 1;                                                       \
    ptr->combineAlloc = true;                                           \
    *shared = ptr;							\
    SHAREDI_CONTRACT(shared);                                           \
  }									\
                                                                        \
  static inline bool				                        \
  M_C3(shared_, name, _NULL_p)(const M_C3(shared_, name, _t) shared)	\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    return *shared == NULL;						\
  }									\
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _init_set)(M_C3(shared_, name, _t) dest,		\
				 const M_C3(shared_, name, _t) shared)	\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    assert (dest != shared);                                            \
    *dest = *shared;							\
    if (*dest != NULL)							\
      atomic_fetch_add(&((*dest)->cpt), 1);                             \
    SHAREDI_CONTRACT(dest);                                             \
  }									\
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _clear)(M_C3(shared_, name, _t) dest)		\
  {									\
    SHAREDI_CONTRACT(dest);                                             \
    if (*dest != NULL)	{						\
      if (atomic_fetch_sub(&((*dest)->cpt), 1) == 1)	{		\
        bool combineAlloc = (*dest)->combineAlloc;                      \
        /* Note: if combineAlloc is true, the address of the slot       \
           combining both data & ptr is the same as the address of the  \
           first element, aka data itself. */                           \
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
  M_C3(shared_, name, _clean)(M_C3(shared_, name, _t) dest)		\
  {									\
    M_C3(shared_, name, _clear)(dest);                                  \
    *dest = NULL;                                                       \
  }                                                                     \
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _set)(M_C3(shared_, name, _t) dest,		\
			    const M_C3(shared_, name, _t) shared)	\
  {									\
    SHAREDI_CONTRACT(dest);                                             \
    SHAREDI_CONTRACT(shared);                                           \
    M_C3(shared_, name, _clear)(dest);                                  \
    M_C3(shared_, name, _init_set)(dest, shared);			\
  }									\
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _init_move)(M_C3(shared_, name, _t) dest,		\
                                  M_C3(shared_, name, _t) shared)	\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    assert (dest != NULL && dest != shared);                            \
    *dest = *shared;							\
    *shared = NULL;                                                     \
    SHAREDI_CONTRACT(dest);                                             \
  }									\
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _move)(M_C3(shared_, name, _t) dest,		\
                             M_C3(shared_, name, _t) shared)            \
  {									\
    SHAREDI_CONTRACT(dest);                                             \
    SHAREDI_CONTRACT(shared);                                           \
    assert (dest != shared);                                            \
    M_C3(shared_, name, _clear)(dest);                                  \
    M_C3(shared_, name, _init_move)(dest, shared);			\
  }									\
                                                                        \
  static inline void				                        \
  M_C3(shared_, name, _swap)(M_C3(shared_, name, _t) p1,		\
                             M_C3(shared_, name, _t) p2)                \
  {									\
    SHAREDI_CONTRACT(p1);                                               \
    SHAREDI_CONTRACT(p2);                                               \
    M_SWAP (struct M_C3(shared_, name, _s)*, *p1, *p2);                  \
    SHAREDI_CONTRACT(p1);                                               \
    SHAREDI_CONTRACT(p2);                                               \
  }									\
                                                                        \
  static inline const type *						\
  M_C3(shared_, name, _cref)(const M_C3(shared_, name, _t) shared)	\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    return M_C3(shared_, name, _const_cast) ((*shared)->data);          \
  }									\
									\
  static inline type *				                        \
  M_C3(shared_, name, _ref)(M_C3(shared_, name, _t) shared)		\
  {									\
    SHAREDI_CONTRACT(shared);                                           \
    return (*shared)->data;						\
  }									\

/********************************** INTERNAL ************************************/

#define SHAREDI_CONTRACT(shared) do {                                   \
    assert(shared != NULL);                                             \
    assert(*shared == NULL || (*shared)->cpt >= 1);                     \
  } while (0)

#endif
