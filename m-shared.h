/*
 * M*LIB - SHARED Pointer Module
 *
 * Copyright (c) 2017-2025, Patrick Pelissier
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

#if defined(__STDC_VERSION__) && __STDC_VERSION__ > 201710L
#warning The header m-shared.h is obsolete. Use m-shared-ptr header instead.
#endif

M_BEGIN_PROTECTED_CODE

/* Define shared pointer and its function.
   USAGE: SHARED_PTR_DEF(name, type, [, oplist]) */
#define M_SHARED_PTR_DEF(name, ...)                                           \
  M_SHARED_PTR_DEF_AS(name, M_F(name,_t), __VA_ARGS__)


/* Define shared pointer and its function
   as the given name name_t
   USAGE: SHARED_PTR_DEF_AS(name, name_t, type, [, oplist]) */
#define M_SHARED_PTR_DEF_AS(name, name_t, ...)                                \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_SHAR3D_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                     ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_SHAR3D_ATOMIC_OPLIST, name_t ), \
                      (name, __VA_ARGS__ ,                                       M_SHAR3D_ATOMIC_OPLIST, name_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a shared pointer.
   USAGE: SHARED_OPLIST(name [, oplist_of_the_type]) */
#define M_SHARED_PTR_OPLIST(...)                                              \
  M_SHAR3D_PTR_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                        ((__VA_ARGS__, M_BASIC_OPLIST ),                      \
                         (__VA_ARGS__ )))


/* Define relaxed shared pointer and its function (thread unsafe).
   USAGE: SHARED_PTR_RELAXED_DEF(name, type, [, oplist]) */
#define M_SHARED_PTR_RELAXED_DEF(name, ...)                                   \
  M_SHARED_PTR_RELAXED_DEF_AS(name, M_F(name,_t), __VA_ARGS__)


/* Define relaxed shared pointer and its function (thread unsafe)
   as the given name name_t
   USAGE: SHARED_PTR_RELAXED_DEF(name, type, [, oplist]) */
#define M_SHARED_PTR_RELAXED_DEF_AS(name, name_t, ...)                        \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_SHAR3D_PTR_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                     ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_SHAR3D_INTEGER_OPLIST, name_t ), \
                      (name, __VA_ARGS__,                                        M_SHAR3D_INTEGER_OPLIST, name_t ))) \
  M_END_PROTECTED_CODE


/* Define shared resource and its function.
   This is a bounded pool of resource shared by multiple owners.
   USAGE: SHARED_RESOURCE_DEF(name, type, [, oplist]) */
#define M_SHARED_RESOURCE_DEF(name, ...)                                      \
  M_SHARED_RESOURCE_DEF_AS(name, M_F(name,_t), M_F(name,_it_t), __VA_ARGS__)


/* Define shared resource and its function
   as the given name named_t and the iterator it_t
   This is a bounded pool of resource shared by multiple owners.
   USAGE: SHARED_RESOURCE_DEF_AS(name, name_t, it_t, type, [, oplist]) */
#define M_SHARED_RESOURCE_DEF_AS(name, name_t, it_t, ...)                     \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_SHAR3D_RESOURCE_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
                          ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
                           (name, __VA_ARGS__,                                        name_t, it_t ))) \
  M_END_PROTECTED_CODE



/*****************************************************************************/
/********************************** INTERNAL *********************************/
/*****************************************************************************/

// deferred evaluation
#define M_SHAR3D_PTR_OPLIST_P1(arg) M_SHAR3D_PTR_OPLIST_P2 arg

/* Validation of the given, shared_t oplist */
#define M_SHAR3D_PTR_OPLIST_P2(name, oplist)                                  \
  M_IF_OPLIST(oplist)(M_SHAR3D_PTR_OPLIST_P3, M_SHAR3D_PTR_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define M_SHAR3D_PTR_OPLIST_FAILURE(name, oplist)                             \
  ((M_LIB_ERROR(ARGUMENT_OF_SHARED_PTR_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

#define M_SHAR3D_PTR_OPLIST_P3(name, oplist) (                                \
  INIT(M_F(name, _init)),                                                     \
  CLEAR(M_F(name, _clear)),                                                   \
  INIT_SET(M_F(name, _init_set)),                                             \
  SET(M_F(name, _set))                                                        \
  INIT_MOVE(M_F(name, _init_move)),                                           \
  RESET(M_F(name, _reset)),                                                   \
  MOVE(M_F(name, _move)),                                                     \
  SWAP(M_F(name, _swap))                                                      \
  ,NAME(name)                                                                 \
  ,TYPE(M_F(name, _ct)), GENTYPE(struct M_F(name,_s)*)                        \
  )

// OPLIST to handle a counter of atomic type
#define M_SHAR3D_ATOMIC_OPLIST (TYPE(atomic_int),                             \
                               INIT_SET(atomic_init),                         \
                               ADD(atomic_fetch_add),                         \
                               SUB(atomic_fetch_sub),                         \
                               IT_CREF(atomic_load))

// OPLIST to handle a counter of non-atomic type
#define M_SHAR3D_INTEGER_OPLIST (TYPE(int),                                   \
                                INIT_SET(m_shar3d_integer_init_set),          \
                                ADD(m_shar3d_integer_add),                    \
                                SUB(m_shar3d_integer_sub),                    \
                                IT_CREF(m_shar3d_integer_cref))

/* Atomic like interface for basic integers */
M_INLINE void m_shar3d_integer_init_set(int *p, int val) { *p = val; }
M_INLINE int m_shar3d_integer_add(int *p, int val) { int r = *p;  *p += val; return r; }
M_INLINE int m_shar3d_integer_sub(int *p, int val) { int r = *p;  *p -= val; return r; }
M_INLINE int m_shar3d_integer_cref(int *p) { return *p; }


/********************************** INTERNAL *********************************/

/* Contract of a shared pointer */
#define M_SHAR3D_CONTRACT(shared, cpt_oplist) do {                            \
    M_ASSERT(shared != NULL);                                                 \
    M_ASSERT(*shared == NULL || M_CALL_IT_CREF(cpt_oplist, &(*shared)->cpt) >= 1); \
  } while (0)

// deferred evaluation
#define M_SHAR3D_PTR_DEF_P1(arg) M_ID( M_SHAR3D_PTR_DEF_P2 arg )

/* Validate the oplist before going further */
#define M_SHAR3D_PTR_DEF_P2(name, type, oplist, cpt_oplist, shared_t)         \
  M_IF_OPLIST(oplist)(M_SHAR3D_PTR_DEF_P3, M_SHAR3D_PTR_DEF_FAILURE)(name, type, oplist, cpt_oplist, shared_t)

/* Stop processing with a compilation failure */
#define M_SHAR3D_PTR_DEF_FAILURE(name, type, oplist, cpt_oplist, shared_t)    \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SHARED_PTR_DEF): the given argument is not a valid oplist: " #oplist)

/* Code generation */
#define M_SHAR3D_PTR_DEF_P3(name, type, oplist, cpt_oplist, shared_t)         \
  M_SHAR3D_PTR_DEF_TYPE(name, type, oplist, cpt_oplist, shared_t)             \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
  M_SHAR3D_PTR_DEF_CORE(name, type, oplist, cpt_oplist, shared_t)             \
  M_EMPLACE_QUEUE_DEF(name, cpt_oplist, M_F(name, _init_with), oplist, M_SHAR3D_PTR_DEF_EMPLACE)

/* Define the types */
#define M_SHAR3D_PTR_DEF_TYPE(name, type, oplist, cpt_oplist, shared_t)       \
                                                                              \
  typedef struct M_F(name, _s){                                               \
    type *data;                        /* Pointer to the data */              \
    M_GET_TYPE cpt_oplist  cpt; /* Counter of how many refs the data */       \
    bool  combineAlloc; /* Does the data and the ptr share the slot? */       \
  } *shared_t[1];                                                             \
  typedef struct M_F(name, _s) *M_F(name, _ptr);                              \
  typedef const struct M_F(name, _s) *M_F(name, _srcptr);                     \
                                                                              \
  /* Internal type for oplist */                                              \
  typedef shared_t M_F(name, _ct);                                            \
  typedef type     M_F(name, _subtype_ct);                                    \
                                                                              \
  M_IF(M_OR(M_TEST_METHOD_P(INIT, oplist), M_TEST_METHOD_ALTER_P(EMPLACE_TYPE, oplist)))( \
    typedef struct M_F(name, _combine_s) {                                    \
      struct M_F(name, _s) ptr;                                               \
      type data;                                                              \
    } M_F(name, combine_ct)[1];                                               \
  , /* nothing to do */ )

/* Define the core functions */
#define M_SHAR3D_PTR_DEF_CORE(name, type, oplist, cpt_oplist, shared_t)       \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init)(shared_t shared)                                           \
  {                                                                           \
    *shared = NULL;                                                           \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init2)(shared_t shared, type *data)                              \
  {                                                                           \
    M_ASSERT (shared != NULL);                                                \
    /* The shared ptr get exclusive access to data */                         \
    struct M_F(name, _s) *ptr;                                                \
    if (M_UNLIKELY (data == NULL)) {                                          \
      *shared = NULL;                                                         \
      return;                                                                 \
    }                                                                         \
    ptr = M_CALL_NEW(oplist, struct M_F(name, _s));                           \
    if (M_UNLIKELY_NOMEM (ptr == NULL)) {                                     \
      M_MEMORY_FULL(struct M_F(name, _s), 1);                                 \
    }                                                                         \
    ptr->data = data;                                                         \
    M_CALL_INIT_SET(cpt_oplist, &ptr->cpt, 1);                                \
    ptr->combineAlloc = false;                                                \
    *shared = ptr;                                                            \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
  }                                                                           \
                                                                              \
  M_IF_METHOD(INIT, oplist)(                                                  \
  M_INLINE void                                                               \
  M_F(name, _init_new)(shared_t shared)                                       \
  {                                                                           \
    /* NOTE: Alloc 1 struct with both structures. */                          \
    struct M_F(name, _combine_s) *p =                                         \
      M_CALL_NEW(oplist, struct M_F(name, _combine_s));                       \
    if (M_UNLIKELY_NOMEM (p == NULL)) {                                       \
      M_MEMORY_FULL(struct M_F(name, _combine_s), 1);                         \
    }                                                                         \
    struct M_F(name, _s) *ptr = &p->ptr;                                      \
    ptr->combineAlloc = true;                                                 \
    type *data = &p->data;                                                    \
    M_CALL_INIT( oplist, *data);                                              \
    ptr->data = data;                                                         \
    M_CALL_INIT_SET(cpt_oplist, &ptr->cpt, 1);                                \
    *shared = ptr;                                                            \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
  }                                                                           \
  , /* No INIT */ )                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _NULL_p)(const shared_t shared)                                   \
  {                                                                           \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
    return *shared == NULL;                                                   \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init_set)(shared_t dest,                                         \
                       const shared_t shared)                                 \
  {                                                                           \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
    M_ASSERT (dest != shared);                                                \
    *dest = *shared;                                                          \
    if (*dest != NULL) {                                                      \
      int n = M_CALL_ADD(cpt_oplist, &((*dest)->cpt), 1);                     \
      (void) n;        /* unused return value */                              \
    }                                                                         \
    M_SHAR3D_CONTRACT(dest, cpt_oplist);                                      \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _clear)(shared_t dest)                                            \
  {                                                                           \
    M_SHAR3D_CONTRACT(dest, cpt_oplist);                                      \
    if (*dest != NULL)        {                                               \
      if (M_CALL_SUB(cpt_oplist, &((*dest)->cpt), 1) == 1)        {           \
        bool combineAlloc = (*dest)->combineAlloc;                            \
        /* Note: if combineAlloc is true, the address of the slot             \
           combining both data & ptr is the same as the address of the        \
           first element, aka data itself. Static analyzer tools don't        \
           seem to detect this and report error. */                           \
        M_CALL_CLEAR(oplist, *(*dest)->data);                                 \
        if (combineAlloc == false) {                                          \
          M_CALL_DEL(oplist, (*dest)->data);                                  \
        }                                                                     \
        M_CALL_DEL(oplist, *dest);                                            \
      }                                                                       \
      *dest = NULL;                                                           \
    }                                                                         \
    M_SHAR3D_CONTRACT(dest, cpt_oplist);                                      \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _reset)(shared_t dest)                                            \
  {                                                                           \
    /* NOTE: Clear will also set dest to NULL */                              \
    M_F(name, _clear)(dest);                                                  \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _set)(shared_t dest,                                              \
                  const shared_t shared)                                      \
  {                                                                           \
    M_SHAR3D_CONTRACT(dest, cpt_oplist);                                      \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
    M_F(name, _clear)(dest);                                                  \
    M_F(name, _init_set)(dest, shared);                                       \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init_move)(shared_t dest,                                        \
                        shared_t shared)                                      \
  {                                                                           \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
    M_ASSERT (dest != NULL && dest != shared);                                \
    *dest = *shared;                                                          \
    *shared = NULL;                                                           \
    M_SHAR3D_CONTRACT(dest, cpt_oplist);                                      \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _move)(shared_t dest,                                             \
                   shared_t shared)                                           \
  {                                                                           \
    M_SHAR3D_CONTRACT(dest, cpt_oplist);                                      \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
    M_ASSERT (dest != shared);                                                \
    M_F(name, _clear)(dest);                                                  \
    M_F(name, _init_move)(dest, shared);                                      \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _swap)(shared_t p1,                                               \
                   shared_t p2)                                               \
  {                                                                           \
    M_SHAR3D_CONTRACT(p1, cpt_oplist);                                        \
    M_SHAR3D_CONTRACT(p2, cpt_oplist);                                        \
    /* NOTE: SWAP is not atomic */                                            \
    M_SWAP (struct M_F(name, _s)*, *p1, *p2);                                 \
    M_SHAR3D_CONTRACT(p1, cpt_oplist);                                        \
    M_SHAR3D_CONTRACT(p2, cpt_oplist);                                        \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _equal_p)(const shared_t p1,                                      \
                      const shared_t p2)                                      \
  {                                                                           \
    M_SHAR3D_CONTRACT(p1, cpt_oplist);                                        \
    M_SHAR3D_CONTRACT(p2, cpt_oplist);                                        \
    return *p1 == *p2;                                                        \
  }                                                                           \
                                                                              \
  M_INLINE type const *                                                       \
  M_F(name, _cref)(const shared_t shared)                                     \
  {                                                                           \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
    M_ASSERT(*shared != NULL);                                                \
    type *data = (*shared)->data;                                             \
    M_ASSERT (data != NULL);                                                  \
    return M_CONST_CAST (type, data);                                         \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _ref)(shared_t shared)                                            \
  {                                                                           \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
    M_ASSERT(*shared != NULL);                                                \
    type *data = (*shared)->data;                                             \
    M_ASSERT (data != NULL);                                                  \
    return data;                                                              \
  }                                                                           \

/* Definition of the emplace_back function for arrays */
#define M_SHAR3D_PTR_DEF_EMPLACE(name, cpt_oplist, function_name, oplist, init_func, exp_emplace_type) \
  M_INLINE void                                                               \
  function_name(M_F(name, _ct) shared                                         \
                M_EMPLACE_LIST_TYPE_VAR(a, exp_emplace_type) )                \
  {                                                                           \
    /* NOTE: Alloc 1 struct with both structures. */                          \
    struct M_F(name, _combine_s) *p =                                         \
      M_CALL_NEW(oplist, struct M_F(name, _combine_s));                       \
    if (M_UNLIKELY_NOMEM (p == NULL)) {                                       \
      M_MEMORY_FULL(struct M_F(name, _combine_s), 1);                         \
    }                                                                         \
    struct M_F(name, _s) *ptr = &p->ptr;                                      \
    ptr->combineAlloc = true;                                                 \
    M_F(name, _subtype_ct) *data = &p->data;                                  \
    M_EMPLACE_CALL_FUNC(a, init_func, oplist, *data, exp_emplace_type);       \
    ptr->data = data;                                                         \
    M_CALL_INIT_SET(cpt_oplist, &ptr->cpt, 1);                                \
    *shared = ptr;                                                            \
    M_SHAR3D_CONTRACT(shared, cpt_oplist);                                    \
  }                                                                           \



/********************************** INTERNAL *********************************/

#define M_SHAR3D_RESOURCE_CONTRACT(s) do {                                    \
    M_ASSERT (s != NULL);                                                     \
    M_ASSERT (s->buffer != NULL);                                             \
  } while (0)

// deferred
#define M_SHAR3D_RESOURCE_DEF_P1(arg) M_ID( M_SHAR3D_RESOURCE_DEF_P2 arg )

/* Validate the oplist before going further */
#define M_SHAR3D_RESOURCE_DEF_P2(name, type, oplist, shared_t, it_t)          \
  M_IF_OPLIST(oplist)(M_SHAR3D_RESOURCE_DEF_P3, M_SHAR3D_RESOURCE_DEF_FAILURE)(name, type, oplist, shared_t, it_t)

/* Stop processing with a compilation failure */
#define M_SHAR3D_RESOURCE_DEF_FAILURE(name, type, oplist, shared_t, it_t)     \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SHARED_RESOURCE_DEF): the given argument is not a valid oplist: " #oplist)

#define M_SHAR3D_RESOURCE_DEF_P3(name, type, oplist, shared_t, it_t)          \
  M_SHAR3D_RESOURCE_DEF_TYPE(name, type, oplist, shared_t, it_t)              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
  M_SHAR3D_RESOURCE_DEF_CORE(name, type, oplist, shared_t, it_t)              \

/* Define the types */
#define M_SHAR3D_RESOURCE_DEF_TYPE(name, type, oplist, shared_t, it_t)        \
                                                                              \
  /* Create an aligned type to avoid false sharing between threads */         \
  typedef struct M_F(name, _atype_s) {                                        \
    atomic_uint  cpt;                                                         \
    type         x;                                                           \
    M_CACHELINE_ALIGN(align, type, atomic_uint);                              \
  } M_F(name, _atype_ct);                                                     \
                                                                              \
  typedef struct M_F(name, _s) {                                              \
    m_genint_t             core;                                              \
    M_F(name, _atype_ct) *buffer;                                             \
  } shared_t[1];                                                              \
                                                                              \
  typedef struct M_F(name, _it_s) {                                           \
    unsigned int idx;                                                         \
    struct M_F(name, _s) *ref;                                                \
  } it_t[1];                                                                  \
                                                                              \
  /* Internal Types for oplist */                                             \
  typedef shared_t M_F(name, _ct);                                            \
  typedef type     M_F(name, _subtype_ct);                                    \

/* Define the core functions */
#define M_SHAR3D_RESOURCE_DEF_CORE(name, type, oplist, shared_t, it_t)        \
  M_INLINE void                                                               \
  M_F(name, _init)(shared_t s, size_t n)                                      \
  {                                                                           \
    M_ASSERT(s != NULL);                                                      \
    M_ASSERT (n > 0 && n < UINT_MAX);                                         \
    s->buffer = M_CALL_REALLOC(oplist, M_F(name, _atype_ct), NULL, 0, n);     \
    if (M_UNLIKELY_NOMEM (s->buffer == NULL)) {                               \
      M_MEMORY_FULL(M_F(name, _atype_ct), n);                                 \
    }                                                                         \
    for(size_t i = 0; i < n; i++) {                                           \
      M_CALL_INIT(oplist, s->buffer[i].x);                                    \
      atomic_init (&s->buffer[i].cpt, 0U);                                    \
    }                                                                         \
    m_genint_init(s->core, (unsigned int) n);                                 \
    M_SHAR3D_RESOURCE_CONTRACT(s);                                            \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _clear)(shared_t s)                                               \
  {                                                                           \
    M_SHAR3D_RESOURCE_CONTRACT(s);                                            \
    size_t n = m_genint_size(s->core);                                        \
    for(size_t i = 0; i < n; i++) {                                           \
      M_CALL_CLEAR(oplist, s->buffer[i].x);                                   \
    }                                                                         \
    M_CALL_FREE(oplist, M_F(name, _atype_ct), s->buffer, n);                  \
    s->buffer = NULL;                                                         \
    m_genint_clear(s->core);                                                  \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it)(it_t it, shared_t s)                                         \
  {                                                                           \
    M_SHAR3D_RESOURCE_CONTRACT(s);                                            \
    M_ASSERT (it != NULL);                                                    \
    unsigned int idx = m_genint_pop(s->core);                                 \
    it->idx = idx;                                                            \
    it->ref = s;                                                              \
    if (M_LIKELY (idx != M_GENINT_ERROR)) {                                   \
      M_ASSERT(atomic_load(&s->buffer[idx].cpt) == 0);                        \
      atomic_store(&s->buffer[idx].cpt, 1U);                                  \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _end_p)(it_t it)                                                  \
  {                                                                           \
    M_ASSERT (it != NULL);                                                    \
    return it->idx == M_GENINT_ERROR;                                         \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _ref)(it_t it)                                                    \
  {                                                                           \
    M_ASSERT (it != NULL && it->ref != NULL && it->idx != M_GENINT_ERROR);    \
    M_SHAR3D_RESOURCE_CONTRACT(it->ref);                                      \
    return &it->ref->buffer[it->idx].x;                                       \
  }                                                                           \
                                                                              \
  M_INLINE type const *                                                       \
  M_F(name, _cref)(it_t it)                                                   \
  {                                                                           \
    M_ASSERT (it != NULL && it->ref != NULL && it->idx != M_GENINT_ERROR);    \
    M_SHAR3D_RESOURCE_CONTRACT(it->ref);                                      \
    return M_CONST_CAST (type, &it->ref->buffer[it->idx].x);                  \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _end)(it_t it, shared_t s)                                        \
  {                                                                           \
    M_SHAR3D_RESOURCE_CONTRACT(s);                                            \
    M_ASSERT (it != NULL);                                                    \
    M_ASSERT (it->ref == s);                                                  \
    unsigned int idx = it->idx;                                               \
    if (M_LIKELY (idx != M_GENINT_ERROR)) {                                   \
      unsigned int c = atomic_fetch_sub (&it->ref->buffer[idx].cpt, 1U);      \
      if (c == 1) {                                                           \
        m_genint_push(it->ref->core, idx);                                    \
      }                                                                       \
      it->idx = M_GENINT_ERROR;                                               \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it_set)(it_t itd, it_t its)                                      \
  {                                                                           \
    M_ASSERT (itd != NULL && its != NULL);                                    \
    M_SHAR3D_RESOURCE_CONTRACT(its->ref);                                     \
    itd->ref = its->ref;                                                      \
    unsigned int idx = its->idx;                                              \
    itd->idx = idx;                                                           \
    if (M_LIKELY (idx != M_GENINT_ERROR)) {                                   \
      unsigned int c = atomic_fetch_add(&itd->ref->buffer[idx].cpt, 1U);      \
      M_ASSERT (c >= 1);                                                      \
    }                                                                         \
  }                                                                           \

M_END_PROTECTED_CODE

/********************************** INTERNAL *********************************/

#if M_USE_SMALL_NAME
#define SHARED_PTR_OPLIST M_SHARED_PTR_OPLIST
#define SHARED_PTR_DEF M_SHARED_PTR_DEF
#define SHARED_PTR_DEF_AS M_SHARED_PTR_DEF_AS
#define SHARED_PTR_RELAXED_DEF M_SHARED_PTR_RELAXED_DEF
#define SHARED_PTR_RELAXED_DEF_AS M_SHARED_PTR_RELAXED_DEF_AS
#define SHARED_RESOURCE_DEF M_SHARED_RESOURCE_DEF
#define SHARED_RESOURCE_DEF_AS M_SHARED_RESOURCE_DEF_AS
#endif

#endif
