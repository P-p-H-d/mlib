/*
 * M*LIB - SHARED Module
 *
 * Copyright (c) 2017-2021, Patrick Pelissier
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
#ifndef MSTARLIB_SHARED_PTR_H
#define MSTARLIB_SHARED_PTR_H

#include "m-core.h"
#include "m-atomic.h"
#include "m-genint.h"

M_BEGIN_PROTECTED_CODE

/* Define the oplist of a shared pointer.
   USAGE: SHARED_OPLIST(name [, oplist_of_the_type]) */
#define SHARED_PTR_OPLIST(...)                                                \
  SHAREDI_PTR_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                        ((__VA_ARGS__, M_DEFAULT_OPLIST ),                    \
                         (__VA_ARGS__ )))


/* Define shared pointer and its function.
   USAGE: SHARED_PTR_DEF(name, type, [, oplist]) */
#define SHARED_PTR_DEF(name, ...)                                             \
  M_BEGIN_PROTECTED_CODE                                                      \
  SHAREDI_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
                     ((name, __VA_ARGS__,                                     \
                       M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),                 \
                       SHAREDI_ATOMIC_OPLIST, M_T(name, t) ),                 \
                      (name, __VA_ARGS__,                                     \
                       SHAREDI_ATOMIC_OPLIST, M_T(name, t) )))                \
  M_END_PROTECTED_CODE


/* Define shared pointer and its function
   as the given name name_t
   USAGE: SHARED_PTR_DEF_AS(name, name_t, type, [, oplist]) */
#define SHARED_PTR_DEF_AS(name, name_t, ...)                                  \
  M_BEGIN_PROTECTED_CODE                                                      \
  SHAREDI_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
                     ((name, __VA_ARGS__,                                     \
                       M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),                 \
                       SHAREDI_ATOMIC_OPLIST, name_t ),                       \
                      (name, __VA_ARGS__,                                     \
                       SHAREDI_ATOMIC_OPLIST, name_t )))                      \
  M_END_PROTECTED_CODE


/* Define relaxed shared pointer and its function (thread-unsafe).
   USAGE: SHARED_PTR_RELAXED_DEF(name, type, [, oplist]) */
#define SHARED_PTR_RELAXED_DEF(name, ...)                                     \
  M_BEGIN_PROTECTED_CODE                                                      \
  SHAREDI_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
                     ((name, __VA_ARGS__,                                     \
                       M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),                 \
                       SHAREDI_INTEGER_OPLIST, M_T(name, t) ),                \
                      (name, __VA_ARGS__,                                     \
                       SHAREDI_INTEGER_OPLIST, M_T(name, t) )))               \
  M_END_PROTECTED_CODE


/* Define relaxed shared pointer and its function (thread-unsafe)
   as the given name name_t
   USAGE: SHARED_PTR_RELAXED_DEF(name, type, [, oplist]) */
#define SHARED_PTR_RELAXED_DEF_AS(name, name_t, ...)                          \
  M_BEGIN_PROTECTED_CODE                                                      \
  SHAREDI_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
                     ((name, __VA_ARGS__,                                     \
                       M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),                 \
                       SHAREDI_INTEGER_OPLIST, name_t ),                      \
                      (name, __VA_ARGS__,                                     \
                       SHAREDI_INTEGER_OPLIST, name_t )))                     \
  M_END_PROTECTED_CODE


/* Define shared resource and its function.
   This is a bounded pool of resource shared by multiple owners.
   USAGE: SHARED_RESOURCE_DEF(name, type, [, oplist]) */
#define SHARED_RESOURCE_DEF(name, ...)                                        \
  M_BEGIN_PROTECTED_CODE                                                      \
  SHAREDI_RESOURCE_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                         \
                          ((name, __VA_ARGS__,                                \
                            M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),            \
                            M_T(name, t), M_T(name, it, t)  ),                \
                           (name, __VA_ARGS__,                                \
                            M_T(name, t), M_T(name, it, t)  )))               \
  M_END_PROTECTED_CODE


/* Define shared resource and its function
   as the given name named_t and the iterator it_t
   This is a bounded pool of resource shared by multiple owners.
   USAGE: SHARED_RESOURCE_DEF_AS(name, name_t, it_t, type, [, oplist]) */
#define SHARED_RESOURCE_DEF_AS(name, name_t, it_t, ...)                            \
  M_BEGIN_PROTECTED_CODE                                                           \
  SHAREDI_RESOURCE_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
                          ((name, __VA_ARGS__,                                     \
                            M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
                           (name, __VA_ARGS__,                                     \
                            name_t, it_t )))                                       \
  M_END_PROTECTED_CODE



/********************************** INTERNAL ************************************/

// deferred evaluation
#define SHAREDI_PTR_OPLIST_P1(arg) SHAREDI_PTR_OPLIST_P2 arg

/* Validation of the given, shared_t oplist */
#define SHAREDI_PTR_OPLIST_P2(name, oplist)                                   \
  M_IF_OPLIST(oplist)(SHAREDI_PTR_OPLIST_P3, SHAREDI_PTR_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define SHAREDI_PTR_OPLIST_FAILURE(name, oplist)                              \
  ((M_LIB_ERROR(ARGUMENT_OF_SHARED_PTR_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

#define SHAREDI_PTR_OPLIST_P3(name, oplist) (                                 \
  INIT(M_F(name, M_NAMING_INITIALIZE)),                                             \
  CLEAR(M_F(name, M_NAMING_FINALIZE)),                                        \
  INIT_SET(M_F(name, M_NAMING_INIT_WITH)),                                    \
  SET(M_F(name, M_NAMING_SET_AS)),                                            \
  INIT_MOVE(M_F(name, init_move)),                                            \
  CLEAN(M_F(name, M_NAMING_CLEAN)),                                           \
  MOVE(M_F(name, move)),                                                      \
  SWAP(M_F(name, swap)),                                                      \
  TYPE(M_T(name, ct)),                                                        \
  M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),),                           \
  M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),),               \
  M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                            \
  )

// OPLIST to handle a counter of atomic type
#define SHAREDI_ATOMIC_OPLIST (TYPE(atomic_int),                     \
                               INIT_SET(atomic_init),                \
                               ADD(atomic_fetch_add),                \
                               SUB(atomic_fetch_sub),                \
                               IT_CREF(atomic_load))

// OPLIST to handle a counter of non-atomic type
#define SHAREDI_INTEGER_OPLIST (TYPE(int),                                          \
                                INIT_SET(M_F(sharedi_integer, M_NAMING_INIT_WITH)), \
                                ADD(sharedi_integer_add),                           \
                                SUB(sharedi_integer_sub),                           \
                                IT_CREF(sharedi_integer_cref))

/* Atomic like interface for basic integers */
static inline void M_F(sharedi_integer, M_NAMING_INIT_WITH)(int *p, int val) { *p = val; }
static inline int sharedi_integer_add(int *p, int val) { int r = *p;  *p += val; return r; }
static inline int sharedi_integer_sub(int *p, int val) { int r = *p;  *p -= val; return r; }
static inline int sharedi_integer_cref(int *p) { return *p; }

/* Contract of a shared pointer */
#define SHAREDI_CONTRACT(shared, cpt_oplist) do {                             \
    M_ASSERT(shared != NULL);                                                 \
    M_ASSERT(*shared == NULL || M_CALL_IT_CREF(cpt_oplist, &(*shared)->cpt) >= 1); \
  } while (0)

// deferred evaluation
#define SHAREDI_PTR_DEF_P1(arg) SHAREDI_PTR_DEF_P2 arg

/* Validate the oplist before going further */
#define SHAREDI_PTR_DEF_P2(name, type, oplist, cpt_oplist, shared_t)          \
  M_IF_OPLIST(oplist)(SHAREDI_PTR_DEF_P3, SHAREDI_PTR_DEF_FAILURE)(name, type, oplist, cpt_oplist, shared_t)

/* Stop processing with a compilation failure */
#define SHAREDI_PTR_DEF_FAILURE(name, type, oplist, cpt_oplist, shared_t)     \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SHARED_PTR_DEF): the given argument is not a valid oplist: " #oplist)

/* Code generation */
#define SHAREDI_PTR_DEF_P3(name, type, oplist, cpt_oplist, shared_t)           \
                                                                               \
  typedef struct M_T(name, s){                                                 \
    type *data;                        /* Pointer to the data */               \
    M_GET_TYPE cpt_oplist  cpt; /* Counter of how many refs the data */        \
    bool  combineAlloc; /* Does the data and the ptr share the slot? */        \
  } *shared_t[1];                                                              \
  typedef struct M_T(name, s) *M_T(name, ptr);                                 \
  typedef const struct M_T(name, s) *M_T(name, srcptr);                        \
                                                                               \
  /* Internal type for oplist */                                               \
  typedef shared_t M_T(name, ct);                                              \
                                                                               \
  typedef struct M_T(name, combine, s) {                                       \
    type data;                                                                 \
    struct M_T(name, s) ptr;                                                   \
  } M_T(name, combine, ct)[1];                                                 \
                                                                               \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                             \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_INITIALIZE)(shared_t shared)                                    \
  {                                                                            \
    *shared = NULL;                                                            \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, init2)(shared_t shared, type *data)                                \
  {                                                                            \
    M_ASSERT(shared != NULL);                                                  \
    /* The shared ptr get exclusive access to data */                          \
    struct M_T(name, s) *ptr;                                                  \
    if (M_UNLIKELY(data == NULL)) {                                            \
      *shared = NULL;                                                          \
      return;                                                                  \
    }                                                                          \
    ptr = M_CALL_NEW(oplist, struct M_T(name, s));                             \
    if (M_UNLIKELY(ptr == NULL)) {                                             \
      M_MEMORY_FULL(sizeof(struct M_T(name, s)));                              \
      return;                                                                  \
    }                                                                          \
    ptr->data = data;                                                          \
    M_CALL_INIT_SET(cpt_oplist, &ptr->cpt, 1);                                 \
    ptr->combineAlloc = false;                                                 \
    *shared = ptr;                                                             \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_INITIALIZE_NEW)(shared_t shared)                                \
  {                                                                            \
    /* NOTE: Alloc 1 struct with both structures. */                           \
    struct M_T(name, combine, s) *p =                                          \
      M_CALL_NEW(oplist, struct M_T(name, combine, s));                        \
    if (M_UNLIKELY (p == NULL)) {                                              \
      M_MEMORY_FULL(sizeof(struct M_T(name, combine, s)));                     \
      return;                                                                  \
    }                                                                          \
    struct M_T(name, s) *ptr = &p->ptr;                                        \
    type *data = &p->data;                                                     \
    M_CALL_INIT( oplist, *data);                                               \
    ptr->data = data;                                                          \
    M_CALL_INIT_SET(cpt_oplist, &ptr->cpt, 1);                                 \
    ptr->combineAlloc = true;                                                  \
    *shared = ptr;                                                             \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
  }                                                                            \
                                                                               \
  static inline bool                                                           \
  M_F(name, M_NAMING_TEST_NULL)(const shared_t shared)                         \
  {                                                                            \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
    return *shared == NULL;                                                    \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_INIT_WITH)(shared_t dest,                                 \
                                const shared_t shared)                         \
  {                                                                            \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
    M_ASSERT(dest != shared);                                                  \
    *dest = *shared;                                                           \
    if (*dest != NULL) {                                                       \
      int n = M_CALL_ADD(cpt_oplist, &((*dest)->cpt), 1);                      \
      (void) n; /* unused return value */                                      \
    }                                                                          \
    SHAREDI_CONTRACT(dest, cpt_oplist);                                        \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_FINALIZE)(shared_t dest)                                  \
  {                                                                            \
    SHAREDI_CONTRACT(dest, cpt_oplist);                                        \
    if (*dest != NULL)  {                                                      \
      if (M_CALL_SUB(cpt_oplist, &((*dest)->cpt), 1) == 1)  {                  \
        bool combineAlloc = (*dest)->combineAlloc;                             \
        /* Note: if combineAlloc is true, the address of the slot              \
           combining both data & ptr is the same as the address of the         \
           first element, aka data itself. Static analyzer tools don't         \
           seem to detect this and report error. */                            \
        M_CALL_CLEAR(oplist, *(*dest)->data);                                  \
        M_CALL_DEL(oplist, (*dest)->data);                                     \
        if (combineAlloc == false)                                             \
          M_CALL_DEL(oplist, *dest);                                           \
      }                                                                        \
      *dest = NULL;                                                            \
    }                                                                          \
    SHAREDI_CONTRACT(dest, cpt_oplist);                                        \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_CLEAN)(shared_t dest)                                     \
  {                                                                            \
    /* NOTE: Clear will also set dest to NULL */                               \
    M_F(name, M_NAMING_FINALIZE)(dest);                                        \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_SET_AS)(shared_t dest,                                    \
                      const shared_t shared)                                   \
  {                                                                            \
    SHAREDI_CONTRACT(dest, cpt_oplist);                                        \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
    M_F(name, M_NAMING_FINALIZE)(dest);                                        \
    M_F(name, M_NAMING_INIT_WITH)(dest, shared);                               \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, init_move)(shared_t dest,                                          \
                       shared_t shared)                                        \
  {                                                                            \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
    M_ASSERT(dest != NULL && dest != shared);                                  \
    *dest = *shared;                                                           \
    *shared = NULL;                                                            \
    SHAREDI_CONTRACT(dest, cpt_oplist);                                        \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, move)(shared_t dest,                                               \
              shared_t shared)                                                 \
  {                                                                            \
    SHAREDI_CONTRACT(dest, cpt_oplist);                                        \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
    M_ASSERT(dest != shared);                                                  \
    M_F(name, M_NAMING_FINALIZE)(dest);                                        \
    M_F(name, init_move)(dest, shared);                                        \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, swap)(shared_t p1,                                                 \
              shared_t p2)                                                     \
  {                                                                            \
    SHAREDI_CONTRACT(p1, cpt_oplist);                                          \
    SHAREDI_CONTRACT(p2, cpt_oplist);                                          \
    /* NOTE: SWAP is not atomic */                                             \
    M_SWAP(struct M_T(name, s)*, *p1, *p2);                                    \
    SHAREDI_CONTRACT(p1, cpt_oplist);                                          \
    SHAREDI_CONTRACT(p2, cpt_oplist);                                          \
  }                                                                            \
                                                                               \
  static inline bool                                                           \
  M_F(name, M_NAMING_TEST_EQUAL_TO)(const shared_t p1,                         \
                                    const shared_t p2)                         \
  {                                                                            \
    SHAREDI_CONTRACT(p1, cpt_oplist);                                          \
    SHAREDI_CONTRACT(p2, cpt_oplist);                                          \
    return *p1 == *p2;                                                         \
  }                                                                            \
                                                                               \
  static inline type const *                                                   \
  M_F(name, cref)(const shared_t shared)                                       \
  {                                                                            \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
    M_ASSERT(*shared != NULL);                                                 \
    type *data = (*shared)->data;                                              \
    M_ASSERT(data != NULL);                                                    \
    return M_CONST_CAST(type, data);                                           \
  }                                                                            \
                                                                               \
  static inline type *                                                         \
  M_F(name, ref)(shared_t shared)                                              \
  {                                                                            \
    SHAREDI_CONTRACT(shared, cpt_oplist);                                      \
    M_ASSERT(*shared != NULL);                                                 \
    type *data = (*shared)->data;                                              \
    M_ASSERT(data != NULL);                                                    \
    return data;                                                               \
  }                                                                            \
  


/********************************** SHARED RESOURCE ************************************/

#define SHAREDI_RESOURCE_CONTRACT(s) do {                                     \
    M_ASSERT(s != NULL);                                                      \
    M_ASSERT(s->buffer != NULL);                                              \
  } while (0)

// deferred
#define SHAREDI_RESOURCE_DEF_P1(arg) SHAREDI_RESOURCE_DEF_P2 arg

/* Validate the oplist before going further */
#define SHAREDI_RESOURCE_DEF_P2(name, type, oplist, shared_t, it_t)           \
  M_IF_OPLIST(oplist)(SHAREDI_RESOURCE_DEF_P3, SHAREDI_RESOURCE_DEF_FAILURE)(name, type, oplist, shared_t, it_t)

/* Stop processing with a compilation failure */
#define SHAREDI_RESOURCE_DEF_FAILURE(name, type, oplist, shared_t, it_t)      \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SHARED_RESOURCE_DEF): the given argument is not a valid oplist: " #oplist)

#define SHAREDI_RESOURCE_DEF_P3(name, type, oplist, shared_t, it_t)            \
                                                                               \
  /* Create an aligned type to avoid false sharing between threads */          \
  typedef struct M_T(name, atype_s) {                                          \
    atomic_uint  cpt;                                                          \
    type         x;                                                            \
    M_CACHELINE_ALIGN(align, type, atomic_uint);                               \
  } M_T(name, atype_ct);                                                       \
                                                                               \
  typedef struct M_T(name, s) {                                                \
    genint_t              core;                                                \
    M_T(name, atype, ct) *buffer;                                              \
  } shared_t[1];                                                               \
                                                                               \
  typedef struct M_T(name, it, s) {                                            \
    unsigned int         idx;                                                  \
    struct M_T(name, s) *ref;                                                  \
  } it_t[1];                                                                   \
                                                                               \
  /* Internal Types for oplist */                                              \
  typedef shared_t M_T(name, ct);                                              \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_INITIALIZE)(shared_t s, size_t n)                               \
  {                                                                            \
    M_ASSERT(s != NULL);                                                       \
    M_ASSERT(n > 0 && n < UINT_MAX);                                           \
    s->buffer = M_CALL_REALLOC(oplist, M_C(name, _atype_ct), NULL, n);         \
    if (M_UNLIKELY (s->buffer == NULL)) {                                      \
      M_MEMORY_FULL(sizeof(M_C(name, _atype_ct)) * n);                         \
      return;                                                                  \
    }                                                                          \
    for(size_t i = 0; i < n; i++) {                                            \
      M_CALL_INIT(oplist, s->buffer[i].x);                                     \
      atomic_init(&s->buffer[i].cpt, 0U);                                      \
    }                                                                          \
    M_F(genint, M_NAMING_INITIALIZE)(s->core, (unsigned int) n);                     \
    SHAREDI_RESOURCE_CONTRACT(s);                                              \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_FINALIZE)(shared_t s)                                     \
  {                                                                            \
    SHAREDI_RESOURCE_CONTRACT(s);                                              \
    size_t n = M_F(genint, M_NAMING_GET_SIZE)(s->core);                        \
    for(size_t i = 0; i < n; i++) {                                            \
      M_CALL_CLEAR(oplist, s->buffer[i].x);                                    \
    }                                                                          \
    M_CALL_FREE(oplist, s->buffer);                                            \
    s->buffer = NULL;                                                          \
    M_F(genint, M_NAMING_FINALIZE)(s->core);                                   \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_IT_FIRST)(it_t it, shared_t s)                            \
  {                                                                            \
    SHAREDI_RESOURCE_CONTRACT(s);                                              \
    M_ASSERT(it != NULL);                                                      \
    unsigned int idx = genint_pop(s->core);                                    \
    it->idx = idx;                                                             \
    it->ref = s;                                                               \
    if (M_LIKELY (idx != GENINT_ERROR)) {                                      \
      M_ASSERT(atomic_load(&s->buffer[idx].cpt) == 0);                         \
      atomic_store(&s->buffer[idx].cpt, 1U);                                   \
    }                                                                          \
  }                                                                            \
                                                                               \
  static inline bool                                                           \
  M_F(name, M_NAMING_IT_TEST_END)(it_t it)                                     \
  {                                                                            \
    M_ASSERT (it != NULL);                                                     \
    return it->idx == GENINT_ERROR;                                            \
  }                                                                            \
                                                                               \
  static inline type *                                                         \
  M_F(name, ref)(it_t it)                                                      \
  {                                                                            \
    M_ASSERT (it != NULL && it->ref != NULL && it->idx != GENINT_ERROR);       \
    SHAREDI_RESOURCE_CONTRACT(it->ref);                                        \
    return &it->ref->buffer[it->idx].x;                                        \
  }                                                                            \
                                                                               \
  static inline type const *                                                   \
  M_F(name, cref)(it_t it)                                                     \
  {                                                                            \
    M_ASSERT (it != NULL && it->ref != NULL && it->idx != GENINT_ERROR);       \
    SHAREDI_RESOURCE_CONTRACT(it->ref);                                        \
    return M_CONST_CAST(type, &it->ref->buffer[it->idx].x);                    \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, end)(it_t it, shared_t s)                                          \
  {                                                                            \
    SHAREDI_RESOURCE_CONTRACT(s);                                              \
    M_ASSERT(it != NULL);                                                      \
    M_ASSERT(it->ref == s);                                                    \
    unsigned int idx = it->idx;                                                \
    if (M_LIKELY(idx != GENINT_ERROR)) {                                       \
      unsigned int c = atomic_fetch_sub(&it->ref->buffer[idx].cpt, 1U);        \
      if (c == 1) {                                                            \
        genint_push(it->ref->core, idx);                                       \
      }                                                                        \
      it->idx = GENINT_ERROR;                                                  \
    }                                                                          \
  }                                                                            \
                                                                               \
  static inline void                                                           \
  M_F(name, M_NAMING_IT_SET)(it_t itd, it_t its)                               \
  {                                                                            \
    M_ASSERT(itd != NULL && its != NULL);                                      \
    SHAREDI_RESOURCE_CONTRACT(its->ref);                                       \
    itd->ref = its->ref;                                                       \
    unsigned int idx = its->idx;                                               \
    itd->idx = idx;                                                            \
    if (M_LIKELY(idx != GENINT_ERROR)) {                                       \
      unsigned int c = atomic_fetch_add(&itd->ref->buffer[idx].cpt, 1U);       \
      M_ASSERT(c >= 1);                                                        \
    }                                                                          \
  }                                                                            \

M_END_PROTECTED_CODE

#endif
