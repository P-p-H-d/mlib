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
#include "m-genint.h"

/* Define the oplist of a shared pointer.
   USAGE: SHARED_OPLIST(name [, oplist_of_the_type]) */
#define SHARED_PTR_OPLIST(...)                                          \
  SHAREDI_PTR_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
                     ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ), \
                      _OPLIST(__VA_ARGS__ )))

/* Define shared pointer and its function.
   USAGE: SHARED_PTR_DEF(name, type, [, oplist]) */
#define SHARED_PTR_DEF(name, ...)                                       \
  SHAREDI_PTR_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                  ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), SHAREDI_ATOMIC_OPLIST ), \
                   (name, __VA_ARGS__ , SHAREDI_ATOMIC_OPLIST)))

/* Define relaxed shared pointer and its function (thread unsafe).
   USAGE: SHARED_PTR_RELAXED_DEF(name, type, [, oplist]) */
#define SHARED_PTR_RELAXED_DEF(name, ...)                               \
  SHAREDI_PTR_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                  ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), SHAREDI_INTEGER_OPLIST ), \
                   (name, __VA_ARGS__ , SHAREDI_INTEGER_OPLIST)))

/* Define shared ressource and its function.
   USAGE: SHARED_RESSOURCE_DEF(name, type, [, oplist]) */
#define SHARED_RESSOURCE_DEF(name, ...)                                 \
  SHAREDI_RESSOURCE_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                     \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ), \
                         (name, __VA_ARGS__)))



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

// OPLIST to handle a counter of atomic type
#define SHAREDI_ATOMIC_OPLIST (TYPE(atomic_int),                        \
                               INIT_SET(atomic_init),                   \
                               ADD(atomic_fetch_add),                   \
                               SUB(atomic_fetch_sub),                   \
                               IT_CREF(atomic_load))

// OPLIST to handle a counter of non-atomic type with its helper functions.
#define SHAREDI_INTEGER_OPLIST (TYPE(int),                              \
                                INIT_SET(sharedi_integer_init_set),     \
                                ADD(sharedi_integer_add),               \
                                SUB(sharedi_integer_sub),               \
                                IT_CREF(sharedi_integer_cref))
static inline void sharedi_integer_init_set(int *p, int val) { *p = val; }
static inline int sharedi_integer_add(int *p, int val) { int r = *p;  *p += val; return r; }
static inline int sharedi_integer_sub(int *p, int val) { int r = *p;  *p -= val; return r; }
static inline int sharedi_integer_cref(int *p) { return *p; }

#define SHAREDI_CONTRACT(shared, cpt_oplist) do {                       \
    assert(shared != NULL);                                             \
    assert(*shared == NULL || M_GET_IT_CREF cpt_oplist ( &(*shared)->cpt) >= 1); \
  } while (0)

#define SHAREDI_PTR_DEF2(name, type, oplist, cpt_oplist)                \
									\
  typedef struct M_C(name, _s){						\
    type *data;	                /* Pointer to the data */               \
    M_GET_TYPE cpt_oplist  cpt; /* Counter of how many points to the data */ \
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
    M_GET_INIT_SET cpt_oplist (&ptr->cpt, 1);                           \
    ptr->combineAlloc = false;                                          \
    *shared = ptr;							\
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
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
    M_GET_INIT_SET cpt_oplist (&ptr->cpt, 1);                           \
    ptr->combineAlloc = true;                                           \
    *shared = ptr;							\
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
  }									\
									\
  static inline bool				                        \
  M_C(name, _NULL_p)(const M_C(name, _t) shared)			\
  {									\
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
    return *shared == NULL;						\
  }									\
									\
  static inline void				                        \
  M_C(name, _init_set)(M_C(name, _t) dest,				\
		       const M_C(name, _t) shared)			\
  {									\
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
    assert (dest != shared);                                            \
    *dest = *shared;							\
    if (*dest != NULL) {						\
      int n = M_GET_ADD cpt_oplist ( &((*dest)->cpt), 1);               \
      (void) n;	/* unused return value */				\
    }									\
    SHAREDI_CONTRACT(dest, cpt_oplist);                                 \
  }									\
									\
  static inline void				                        \
  M_C(name, _clear)(M_C(name, _t) dest)					\
  {									\
    SHAREDI_CONTRACT(dest, cpt_oplist);                                 \
    if (*dest != NULL)	{						\
      if (M_GET_SUB cpt_oplist ( &((*dest)->cpt), 1) == 1)	{       \
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
    SHAREDI_CONTRACT(dest, cpt_oplist);                                 \
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
    SHAREDI_CONTRACT(dest, cpt_oplist);                                 \
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
    M_C(name, _clear)(dest);						\
    M_C(name, _init_set)(dest, shared);					\
  }									\
									\
  static inline void				                        \
  M_C(name, _init_move)(M_C(name, _t) dest,				\
			M_C(name, _t) shared)				\
  {									\
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
    assert (dest != NULL && dest != shared);                            \
    *dest = *shared;							\
    *shared = NULL;                                                     \
    SHAREDI_CONTRACT(dest, cpt_oplist);                                 \
  }									\
									\
  static inline void				                        \
  M_C(name, _move)(M_C(name, _t) dest,					\
		   M_C(name, _t) shared)				\
  {									\
    SHAREDI_CONTRACT(dest, cpt_oplist);                                 \
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
    assert (dest != shared);						\
    M_C(name, _clear)(dest);						\
    M_C(name, _init_move)(dest, shared);				\
  }									\
									\
  static inline void				                        \
  M_C(name, _swap)(M_C(name, _t) p1,					\
		   M_C(name, _t) p2)					\
  {									\
    SHAREDI_CONTRACT(p1, cpt_oplist);                                   \
    SHAREDI_CONTRACT(p2, cpt_oplist);                                   \
    M_SWAP (struct M_C(name, _s)*, *p1, *p2);				\
    SHAREDI_CONTRACT(p1, cpt_oplist);                                   \
    SHAREDI_CONTRACT(p2, cpt_oplist);                                   \
  }									\
									\
  static inline bool				                        \
  M_C(name, _equal_p)(const M_C(name, _t) p1,				\
		      const M_C(name, _t) p2)				\
  {									\
    SHAREDI_CONTRACT(p1, cpt_oplist);                                   \
    SHAREDI_CONTRACT(p2, cpt_oplist);                                   \
    return *p1 == *p2;							\
  }									\
									\
  static inline const type *						\
  M_C(name, _cref)(const M_C(name, _t) shared)				\
  {									\
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
    assert(*shared != NULL);                                            \
    type *data = (*shared)->data;                                       \
    assert (data != NULL);                                              \
    return M_CONST_CAST (type, data);                                   \
  }									\
									\
  static inline type *				                        \
  M_C(name, _ref)(M_C(name, _t) shared)					\
  {									\
    SHAREDI_CONTRACT(shared, cpt_oplist);                               \
    assert(*shared != NULL);                                            \
    type *data = (*shared)->data;                                       \
    assert (data != NULL);                                              \
    return data;                                                        \
  }									\
  
/********************************** SHARED RESSOURCE ************************************/

#define SHAREDI_RESSOURCE_CONTRACT(s) do {      \
    assert (s != NULL);                         \
    assert (s->buffer != NULL);                 \
  } while (0)

// deferred
#define SHAREDI_RESSOURCE_DEF(arg) SHAREDI_RESSOURCE_DEF2 arg

#define SHAREDI_RESSOURCE_DEF2(name, type, oplist)                      \
                                                                        \
  /* Create an aligned type to avoid false sharing between threads */   \
  typedef struct M_C(name, _atype_s) {                                  \
    atomic_uint  cpt;                                                   \
    type         x;							\
    char align[M_ALIGN_FOR_CACHELINE_EXCLUSION > sizeof(type) +sizeof(atomic_uint) ? M_ALIGN_FOR_CACHELINE_EXCLUSION - sizeof(type) -sizeof(atomic_uint) : 1]; \
  } M_C(name, _atype_t);                                                \
                                                                        \
  typedef struct M_C(name, _s) {                                        \
    genint_t             core;                                          \
    M_C(name, _atype_t) *buffer;                                        \
  } M_C(name, _t)[1];                                                   \
                                                                        \
  typedef struct M_C(name, _it_s) {                                     \
    unsigned int idx;                                                   \
    struct M_C(name, _s) *ref;                                          \
  } M_C(name, _it_t)[1];                                                \
                                                                        \
  static inline void                                                    \
  M_C(name, _init)(M_C(name, _t) s, size_t n)                           \
  {                                                                     \
    assert(s != NULL);                                                  \
    s->buffer = M_GET_REALLOC oplist (M_C(name, _atype_t), NULL, n);    \
    if (M_UNLIKELY (s->buffer == NULL)) {                               \
      M_MEMORY_FULL(sizeof(M_C(name, _atype_t)) * n);                   \
      return;                                                           \
    }                                                                   \
    for(size_t i = 0; i < n; i++) {                                     \
      M_GET_INIT oplist (s->buffer[i].x);                               \
      atomic_init (&s->buffer[i].cpt, 0);                               \
    }                                                                   \
    genint_init(s->core, n);                                            \
    SHAREDI_RESSOURCE_CONTRACT(s);                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _clear)(M_C(name, _t) s)                                    \
  {                                                                     \
    SHAREDI_RESSOURCE_CONTRACT(s);                                      \
    size_t n = genint_size(s->core);                                    \
    for(size_t i = 0; i < n; i++) {                                     \
      M_GET_CLEAR oplist (s->buffer[i].x);                              \
    }                                                                   \
    M_GET_FREE oplist (s->buffer);                                      \
    s->buffer = NULL;                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it)(M_C(name, _it_t) it, M_C(name, _t) s)                  \
  {                                                                     \
    SHAREDI_RESSOURCE_CONTRACT(s);                                      \
    assert (it != NULL);                                                \
    unsigned int idx = genint_pop(s->core);                             \
    it->idx = idx;                                                      \
    it->ref = s;                                                        \
    if (M_LIKELY (idx != -1U)) {                                        \
      assert(atomic_load(&s->buffer[idx].cpt) == 0);                    \
      atomic_store(&s->buffer[idx].cpt, 1U);                            \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _end_p)(M_C(name, _it_t) it)                                \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->idx != -1U;                                              \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C(name, _ref)(M_C(name, _it_t) it)                                  \
  {                                                                     \
    assert (it != NULL && it->ref != NULL && it->idx != -1U);           \
    return &it->ref->buffer[it->idx].x;                                 \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C(name, _cref)(M_C(name, _it_t) it)                                 \
  {                                                                     \
    assert (it != NULL && it->ref != NULL && it->idx != -1U);           \
    return M_CONST_CAST (type, &it->ref->buffer[it->idx].x);            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _end)(M_C(name, _it_t) it, M_C(name, _t) s)                 \
  {                                                                     \
    assert (it != NULL);                                                \
    assert (it->ref == s);                                              \
    unsigned int idx = it->idx;                                         \
    if (M_LIKELY (idx != -1U)) {                                        \
      unsigned int c = atomic_fetch_sub (&it->ref->buffer[idx].cpt, 1); \
      if (c == 1) {                                                     \
        genint_push(it->ref->core, idx);                                \
      }                                                                 \
      it->idx = -1U;                                                    \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it_set)(M_C(name, _it_t) itd, M_C(name, _it_t) its)        \
  {                                                                     \
    assert (itd != NULL && its != NULL);                                \
    itd->ref = its->ref;                                                \
    unsigned int idx = its->idx;                                        \
    itd->idx = idx;                                                     \
    if (M_LIKELY (idx != -1U)) {                                        \
      unsigned int c = atomic_fetch_add(&itd->ref->buffer[idx].cpt, 1); \
      assert (c >= 1);                                                  \
    }                                                                   \
  }                                                                     \


#endif
