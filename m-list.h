/*
 * M*LIB - LIST module
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
#ifndef MSTARLIB_LIST_H
#define MSTARLIB_LIST_H

#include "m-core.h"

/* Define a singly linked list of a given type.
   USAGE: LIST_DEF(name, type [, oplist_of_the_type]) */
#define M_LIST_DEF(name, ...)                                                 \
  M_LIST_DEF_AS(name, M_F(name, _t), M_F(name, _it_t), __VA_ARGS__)


/* Define a singly linked list of a given type
   as the provided type name_t with the iterator named it_t
   USAGE: LIST_DEF_AS(name, name_t, it_t, type [, oplist_of_the_type]) */
#define M_LIST_DEF_AS(name, name_t, it_t, ...)                                \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_L1ST_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                   \
               ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
                (name, __VA_ARGS__,                                        name_t, it_t ))) \
  M_END_PROTECTED_CODE


/* Define a singly linked list of a given type allowing both push.
   USAGE: LIST_DUAL_PUSH_DEF_AS(name, name_t, type [, oplist_of_the_type]) */
#define M_LIST_DUAL_PUSH_DEF(name, ...)                                       \
  M_LIST_DUAL_PUSH_DEF_AS(name, M_F(name,_t), M_F(name, _it_t), __VA_ARGS__)


/* Define a singly linked list of a given type allowing both push.
   as the provided type name_t with the iterator named it_t
   USAGE: LIST_DUAL_PUSH_DEF(name, type [, oplist_of_the_type]) */
#define M_LIST_DUAL_PUSH_DEF_AS(name, name_t, it_t, ...)                      \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_L1ST_DUAL_PUSH_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                         \
                         ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
                          (name, __VA_ARGS__,                                        name_t, it_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a list of the given type.
   USAGE: LIST_OPLIST(name [, oplist_of_the_type]) */
#define M_LIST_OPLIST(...)                                                    \
  M_L1ST_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                \
                  ((__VA_ARGS__, M_BASIC_OPLIST ),                            \
                   (__VA_ARGS__ )))

/* Define an init value to init global variables of type list.
  USAGE:
    list_t global_variable = LIST_INIT_VALUE();
 */
#define M_LIST_INIT_VALUE()                                                   \
  { NULL }


/* Define an init value to init global variables of type dual push list.
  USAGE:
    list_t global_variable = LIST_DUAL_PUSH_INIT_VALUE();
 */
#define M_LIST_DUAL_PUSH_INIT_VALUE()                                         \
  { { NULL, NULL } }


/********************************** INTERNAL ************************************/

/* Deferred evaluation for the oplist definition,
   so that all arguments are evaluated before further expansion */
#define M_L1ST_OPLIST_P1(arg) M_L1ST_OPLIST_P2 arg

/* Validation of the given oplist */
#define M_L1ST_OPLIST_P2(name, oplist)                                        \
  M_IF_OPLIST(oplist)(M_L1ST_OPLIST_P3, M_L1ST_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define M_L1ST_OPLIST_FAILURE(name, oplist)                                   \
  ((M_LIB_ERROR(ARGUMENT_OF_LIST_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* OPLIST definition of a list and list_dual_push */
#define M_L1ST_OPLIST_P3(name, oplist)                                        \
  (INIT(M_F(name, _init)),                                                    \
   INIT_SET(M_F(name, _init_set)),                                            \
   INIT_WITH(API_1(M_INIT_WITH_VAI)),                                         \
   SET(M_F(name, _set)),                                                      \
   CLEAR(M_F(name, _clear)),                                                  \
   MOVE(M_F(name, _move)),                                                    \
   INIT_MOVE(M_F(name, _init_move)),                                          \
   SWAP(M_F(name, _swap)),                                                    \
   NAME(name),                                                                \
   TYPE(M_F(name,_ct)), GENTYPE(struct M_F(name,_s)**),                       \
   SUBTYPE(M_F(name,_subtype_ct)),                                            \
   EMPTY_P(M_F(name,_empty_p)),                                               \
   IT_TYPE(M_F(name, _it_ct)),                                                \
   IT_FIRST(M_F(name,_it)),                                                   \
   IT_END(M_F(name,_it_end)),                                                 \
   IT_SET(M_F(name,_it_set)),                                                 \
   IT_END_P(M_F(name,_end_p)),                                                \
   IT_EQUAL_P(M_F(name,_it_equal_p)),                                         \
   IT_LAST_P(M_F(name,_last_p)),                                              \
   IT_NEXT(M_F(name,_next)),                                                  \
   IT_REF(M_F(name,_ref)),                                                    \
   IT_CREF(M_F(name,_cref)),                                                  \
   IT_INSERT(M_F(name, _insert)),                                             \
   IT_REMOVE(M_F(name,_remove)),                                              \
   RESET(M_F(name,_reset)),                                                   \
   PUSH(M_F(name,_push_back)),                                                \
   POP(M_F(name,_pop_back)),                                                  \
   PUSH_MOVE(M_F(name,_push_move)),                                           \
   POP_MOVE(M_F(name,_pop_move))                                              \
   ,SPLICE_BACK(M_F(name,_splice_back))                                       \
   ,SPLICE_AT(M_F(name,_splice_at))                                           \
   ,REVERSE(M_F(name,_reverse))                                               \
   ,OPLIST(oplist)                                                            \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_F(name, _get_str)),)               \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_F(name, _out_str)),)               \
   ,M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_F(name, _parse_str)),)         \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_F(name, _in_str)),)                  \
   ,M_IF_METHOD(OUT_SERIAL, oplist)(OUT_SERIAL(M_F(name, _out_serial)),)      \
   ,M_IF_METHOD(IN_SERIAL, oplist)(IN_SERIAL(M_F(name, _in_serial)),)         \
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(M_F(name, _equal_p)),)                   \
   ,M_IF_METHOD(HASH, oplist)(HASH(M_F(name, _hash)),)                        \
   )

/* Deferred evaluation for the list definition,
   so that all arguments are evaluated before further expansion */
#define M_L1ST_DEF_P1(arg) M_ID( M_L1ST_DEF_P2 arg )

/* Validate the oplist before going further */
#define M_L1ST_DEF_P2(name, type, oplist, list_t, it_t)                       \
  M_IF_OPLIST(oplist)(M_L1ST_DEF_P3, M_L1ST_DEF_FAILURE)(name, type, oplist, list_t, it_t)

/* Stop processing with a compilation failure */
#define M_L1ST_DEF_FAILURE(name, type, oplist, list_t, it_t)                  \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(LIST_DEF): the given argument is not a valid oplist: " #oplist)

/* Define allocation functions. If MEMPOOL, we need to define it */
#define M_L1ST_MEMPOOL_DEF(name, type, oplist, list_t, list_it_t)             \
  M_IF_METHOD(MEMPOOL, oplist)(                                               \
                                                                              \
    MEMPOOL_DEF(M_F(name, _mempool), struct M_F(name, _s))                    \
    M_GET_MEMPOOL_LINKAGE oplist M_F(name, _mempool_t) M_GET_MEMPOOL oplist;  \
    M_INLINE struct M_F(name, _s) *M_C3(m_l1st_,name,_new)(void) {            \
      return M_F(name, _mempool_alloc)(M_GET_MEMPOOL oplist);                 \
    }                                                                         \
    M_INLINE void M_C3(m_l1st_,name,_del)(struct M_F(name, _s) *ptr) {        \
      M_F(name, _mempool_free)(M_GET_MEMPOOL oplist, ptr);                    \
    }                                                                         \
                                                                              \
    , /* No mempool allocation */                                             \
                                                                              \
    M_INLINE struct M_F(name, _s) *M_C3(m_l1st_,name,_new)(void) {            \
      return M_CALL_NEW(oplist, struct M_F(name, _s));                        \
    }                                                                         \
    M_INLINE void M_C3(m_l1st_,name,_del)(struct M_F(name, _s) *ptr) {        \
      M_CALL_DEL(oplist, ptr);                                                \
    }                                                                         \
    )                                                                         \


/* Internal list definition
   - name: prefix to be used
   - type: type of the elements of the list
   - oplist: oplist of the type of the elements of the container
   - list_t: alias for M_F(name, _t) [ type of the container ]
   - it_t: alias for M_F(name, _it_t) [ iterator of the container ]
   - node_t: alias for M_F(name, _node_t) [ node ]
 */
#define M_L1ST_DEF_P3(name, type, oplist, list_t, it_t)                       \
  M_L1ST_DEF_TYPE(name, type, oplist, list_t, it_t)                           \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
  M_L1ST_MEMPOOL_DEF(name, type, oplist, list_t, it_t)                        \
  M_L1ST_DEF_P4(name, type, oplist, list_t, it_t)                             \
  M_EMPLACE_QUEUE_DEF(name, list_t, M_F(name, _emplace_back), oplist, M_L1ST_EMPLACE_DEF) \
  M_L1ST_ITBASE_DEF(name, type, oplist, list_t, it_t)


/* Define the internal contract of a list
   (there is nothing worthy to be checked) */
#define M_L1ST_CONTRACT(v) do {                                               \
    M_ASSERT (v != NULL);                                                     \
  } while (0)


/* Define the type of a list */
#define M_L1ST_DEF_TYPE(name, type, oplist, list_t, it_t)                     \
                                                                              \
  /* Define the node of a list, and the list as a pointer to a node */        \
  typedef struct M_F(name, _s) {                                              \
    struct M_F(name, _s) *next;  /* Next node or NULL if final node */        \
    type data;                   /* The data itself */                        \
  } *list_t[1];                                                               \
                                                                              \
  /* Define an iterator of a list */                                          \
  typedef struct M_F(name, _it_s) {                                           \
    struct M_F(name, _s) *previous; /* Previous node or NULL */               \
    struct M_F(name, _s) *current;  /* Current node or NULL */                \
  } it_t[1];                                                                  \
                                                                              \
  /* Definition of the synonyms of the type */                                \
  typedef struct M_F(name, _s) *M_F(name, _ptr);                              \
  typedef const struct M_F(name, _s) *M_F(name, _srcptr);                     \
  typedef list_t M_F(name, _ct);                                              \
  typedef it_t M_F(name, _it_ct);                                             \
  typedef type M_F(name, _subtype_ct);                                        \
 

 /* Internal list function definition
   - name: prefix to be used
   - type: type of the elements of the list
   - oplist: oplist of the type of the elements of the container
   - list_t: alias for type of the container
   - it_t: alias for iterator of the container
 */
#define M_L1ST_DEF_P4(name, type, oplist, list_t, it_t)                       \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init)(list_t v)                                                  \
  {                                                                           \
    M_ASSERT (v != NULL);                                                     \
    *v = NULL;                                                                \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _reset)(list_t v)                                                 \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    struct M_F(name, _s) *it = *v;                                            \
    *v = NULL;                                                                \
    while (it != NULL) {                                                      \
      struct M_F(name, _s) *next = it->next;                                  \
      M_CALL_CLEAR(oplist, it->data);                                         \
      M_C3(m_l1st_,name,_del)(it);                                            \
      it = next;                                                              \
    }                                                                         \
    M_L1ST_CONTRACT(v);                                                       \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _clear)(list_t v)                                                 \
  {                                                                           \
    M_F(name, _reset)(v);                                                     \
  }                                                                           \
                                                                              \
  M_INLINE type  *                                                            \
  M_F(name, _back)(const list_t v)                                            \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    M_ASSERT(*v != NULL);                                                     \
    return &((*v)->data);                                                     \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _push_raw)(list_t v)                                              \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    struct M_F(name, _s) *next;                                               \
    next = M_C3(m_l1st_,name,_new)();                                         \
    if (M_UNLIKELY_NOMEM (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_F(name, _s)));                           \
      return NULL;                                                            \
    }                                                                         \
    type *ret = &next->data;                                                  \
    next->next = *v;                                                          \
    *v = next;                                                                \
    M_L1ST_CONTRACT(v);                                                       \
    return ret;                                                               \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _push_back)(list_t v, type const x)                               \
  {                                                                           \
    type *data = M_F(name, _push_raw)(v);                                     \
    if (M_UNLIKELY (data == NULL))                                            \
      return;                                                                 \
    M_IF_EXCEPTION(struct M_F(name, _s) *next = *v );                         \
    M_ON_EXCEPTION( *v = next->next, M_C3(m_l1st_,name,_del)(next)) {         \
      M_CALL_INIT_SET(oplist, *data, x);                                      \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_IF_METHOD(INIT, oplist)(                                                  \
  M_INLINE type *                                                             \
  M_F(name, _push_new)(list_t v)                                              \
  {                                                                           \
    type *data = M_F(name, _push_raw)(v);                                     \
    if (M_UNLIKELY (data == NULL))                                            \
      return NULL;                                                            \
    M_IF_EXCEPTION(struct M_F(name, _s) *next = *v );                         \
    M_ON_EXCEPTION( *v = next->next, M_C3(m_l1st_,name,_del)(next)) {         \
      M_CALL_INIT(oplist, *data);                                             \
    }                                                                         \
    return data;                                                              \
  }                                                                           \
  , /* No INIT */)                                                            \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _pop_back)(type *data, list_t v)                                  \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    M_ASSERT(*v != NULL);                                                     \
    if (data != NULL) {                                                       \
      M_DO_MOVE (oplist, *data, (*v)->data);                                  \
    } else {                                                                  \
      M_CALL_CLEAR(oplist, (*v)->data);                                       \
    }                                                                         \
    struct M_F(name, _s) *tofree = *v;                                        \
    *v = (*v)->next;                                                          \
    M_C3(m_l1st_,name,_del)(tofree);                                          \
    M_L1ST_CONTRACT(v);                                                       \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _push_move)(list_t v, type *x)                                    \
  {                                                                           \
    M_ASSERT (x != NULL);                                                     \
    type *data = M_F(name, _push_raw)(v);                                     \
    if (M_UNLIKELY (data == NULL))                                            \
      return;                                                                 \
    M_DO_INIT_MOVE (oplist, *data, *x);                                       \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _pop_move)(type *data, list_t v)                                  \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    M_ASSERT(*v != NULL && data != NULL);                                     \
    M_DO_INIT_MOVE (oplist, *data, (*v)->data);                               \
    struct M_F(name, _s) *tofree = *v;                                        \
    *v = (*v)->next;                                                          \
    M_C3(m_l1st_,name,_del)(tofree);                                          \
    M_L1ST_CONTRACT(v);                                                       \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _empty_p)(const list_t v)                                         \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    return *v == NULL;                                                        \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _swap)(list_t l, list_t v)                                        \
  {                                                                           \
    M_L1ST_CONTRACT(l);                                                       \
    M_L1ST_CONTRACT(v);                                                       \
    M_SWAP(struct M_F(name, _s) *, *l, *v);                                   \
    M_L1ST_CONTRACT(l);                                                       \
    M_L1ST_CONTRACT(v);                                                       \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it)(it_t it, const list_t v)                                     \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    M_ASSERT (it != NULL);                                                    \
    it->current = *v;                                                         \
    it->previous = NULL;                                                      \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it_set)(it_t it1, const it_t it2)                                \
  {                                                                           \
    M_ASSERT (it1 != NULL && it2 != NULL);                                    \
    it1->current  = it2->current;                                             \
    it1->previous = it2->previous;                                            \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it_end)(it_t it1, const list_t v)                                \
  {                                                                           \
    M_L1ST_CONTRACT(v);                                                       \
    M_ASSERT (it1 != NULL);                                                   \
    (void)v; /* unused */                                                     \
    it1->current  = NULL;                                                     \
    it1->previous = NULL;                                                     \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _end_p)(const it_t it)                                            \
  {                                                                           \
    M_ASSERT (it != NULL);                                                    \
    return it->current == NULL;                                               \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _last_p)(const it_t it)                                           \
  {                                                                           \
    M_ASSERT (it != NULL);                                                    \
    return it->current == NULL || it->current->next == NULL;                  \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _next)(it_t it)                                                   \
  {                                                                           \
    M_ASSERT(it != NULL && it->current != NULL);                              \
    it->previous = it->current;                                               \
    it->current  = it->current->next;                                         \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _it_equal_p)(const it_t it1, const it_t it2)                      \
  {                                                                           \
    M_ASSERT(it1 != NULL && it2 != NULL);                                     \
    return it1->current == it2->current;                                      \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _ref)(const it_t it)                                              \
  {                                                                           \
    M_ASSERT(it != NULL && it->current != NULL);                              \
    return &(it->current->data);                                              \
  }                                                                           \
                                                                              \
  M_INLINE type const *                                                       \
  M_F(name, _cref)(const it_t it)                                             \
  {                                                                           \
    M_ASSERT(it != NULL && it->current != NULL);                              \
    return M_CONST_CAST(type, &(it->current->data));                          \
  }                                                                           \
                                                                              \
  M_INLINE size_t                                                             \
  M_F(name, _size)(const list_t list)                                         \
  {                                                                           \
    M_L1ST_CONTRACT(list);                                                    \
    size_t size = 0;                                                          \
    struct M_F(name, _s) *it = *list;                                         \
    while (it != NULL) {                                                      \
      size ++;                                                                \
      it = it->next;                                                          \
    }                                                                         \
    return size;                                                              \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _sublist_p)(const list_t list, const it_t itsub)                  \
  {                                                                           \
    M_L1ST_CONTRACT(list);                                                    \
    M_ASSERT (itsub != NULL);                                                 \
    struct M_F(name, _s) *it = *list;                                         \
    while (it != NULL) {                                                      \
      if (it == itsub->current) return true;                                  \
      it = it->next;                                                          \
    }                                                                         \
    /* Not found. Check if search item is NULL */                             \
    return (itsub->current == NULL);                                          \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _get)(const list_t list, size_t i)                                \
  {                                                                           \
    M_L1ST_CONTRACT(list);                                                    \
    struct M_F(name, _s) *it = *list;                                         \
    /* FIXME: How to avoid the double iteration over the list? */             \
    size_t len = M_F(name,_size)(list);                                       \
    M_ASSERT_INDEX (i, len);                                                  \
    size_t j = len-1;                                                         \
    while (true) {                                                            \
      M_ASSERT (it != NULL);                                                  \
      if (i == j) return &it->data;                                           \
      it = it->next;                                                          \
      j--;                                                                    \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE type const *                                                       \
  M_F(name, _cget)(const list_t l, size_t i)                                  \
  {                                                                           \
    return M_CONST_CAST(type, M_F(name, _get)(l,i));                          \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _insert)(list_t list, it_t insertion_point,                       \
                     type const x)                                            \
  {                                                                           \
    M_L1ST_CONTRACT(list);                                                    \
    M_ASSERT (insertion_point != NULL);                                       \
    M_ASSERT(M_F(name, _sublist_p)(list, insertion_point));                   \
    struct M_F(name, _s) *next = M_C3(m_l1st_,name,_new)();                   \
    if (M_UNLIKELY_NOMEM (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_F(name, _s)));                           \
      return;                                                                 \
    }                                                                         \
    M_ON_EXCEPTION( M_C3(m_l1st_,name,_del)(next))                            \
      M_CALL_INIT_SET(oplist, next->data, x);                                 \
    struct M_F(name, _s) *current = insertion_point->current;                 \
    if (M_UNLIKELY (current == NULL)) {                                       \
      next->next = *list;                                                     \
      *list = next;                                                           \
    } else {                                                                  \
      next->next = current->next;                                             \
      current->next = next;                                                   \
    }                                                                         \
    /* Update insertion_point to this element */                              \
    insertion_point->current = next;                                          \
    insertion_point->previous = current;                                      \
    M_L1ST_CONTRACT(list);                                                    \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _remove)(list_t list, it_t removing_point)                        \
  {                                                                           \
    M_L1ST_CONTRACT(list);                                                    \
    M_ASSERT (removing_point != NULL);                                        \
    M_ASSERT (removing_point->current != NULL);                               \
    M_ASSERT(M_F(name, _sublist_p)(list, removing_point));                    \
    struct M_F(name, _s) *next = removing_point->current->next;               \
    if (M_UNLIKELY (removing_point->previous == NULL)) {                      \
      *list = next;                                                           \
    } else {                                                                  \
      removing_point->previous->next = next;                                  \
    }                                                                         \
    M_CALL_CLEAR(oplist, removing_point->current->data);                      \
    M_C3(m_l1st_,name,_del) (removing_point->current);                        \
    removing_point->current = next;                                           \
    M_L1ST_CONTRACT(list);                                                    \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init_set)(list_t list, const list_t org)                         \
  {                                                                           \
    M_L1ST_CONTRACT(org);                                                     \
    struct M_F(name, _s) *m_volatile next = NULL;                             \
    struct M_F(name, _s) *it_org;                                             \
    struct M_F(name, _s) **update_list;                                       \
    update_list = list;                                                       \
    it_org = *org;                                                            \
    /* On exceptions, free node and clear list*/                              \
    M_IF_EXCEPTION(*update_list = NULL);                                      \
    M_ON_EXCEPTION(M_C3(m_l1st_,name,_del)(next), M_F(name, _clear)(list) )   \
    while (it_org != NULL) {                                                  \
      next = M_C3(m_l1st_,name,_new)();                                       \
      if (M_UNLIKELY_NOMEM (next == NULL)) {                                  \
        M_MEMORY_FULL(sizeof (struct M_F(name, _s)));                         \
        *update_list = NULL;                                                  \
        return;                                                               \
      }                                                                       \
      M_CALL_INIT_SET(oplist, next->data, it_org->data);                      \
      *update_list = next;                                                    \
      update_list = &next->next;                                              \
      it_org = it_org->next;                                                  \
      M_IF_EXCEPTION(next = NULL, *update_list = NULL);                       \
    }                                                                         \
    *update_list = NULL;                                                      \
    M_L1ST_CONTRACT(list);                                                    \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _set)(list_t list, const list_t org)                              \
  {                                                                           \
    if (M_UNLIKELY (list == org)) return;                                     \
    M_ON_EXCEPTION(M_F(name, _init)(list) ) {                                 \
      M_F(name, _clear)(list);                                                \
      M_F(name, _init_set)(list, org);                                        \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init_move)(list_t list, list_t org)                              \
  {                                                                           \
    M_L1ST_CONTRACT(org);                                                     \
    M_ASSERT (list != NULL && list != org);                                   \
    *list = *org;                                                             \
    *org = NULL;  /* safer */                                                 \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _move)(list_t list, list_t org)                                   \
  {                                                                           \
    M_ASSERT (list != org);                                                   \
    M_F(name, _clear)(list);                                                  \
    M_F(name, _init_move)(list, org);                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _splice_back)(list_t nv, list_t ov, it_t it)                      \
  {                                                                           \
    M_L1ST_CONTRACT(nv);                                                      \
    M_L1ST_CONTRACT(ov);                                                      \
    M_ASSERT (it != NULL);                                                    \
    M_ASSERT (it->current != NULL);                                           \
    M_ASSERT (M_F(name, _sublist_p)(ov, it));                                 \
    /* Remove the item 'it' from the list 'ov' */                             \
    struct M_F(name, _s) *current = it->current;                              \
    struct M_F(name, _s) *next    = current->next;                            \
    if (it->previous == NULL) {                                               \
      *ov = next;                                                             \
    } else {                                                                  \
      it->previous->next = next;                                              \
    }                                                                         \
    /* Update the item 'it' to point to the next element */                   \
    /* it->previous doesn't need to be updated */                             \
    it->current = next;                                                       \
    /* Push back extracted 'current' in the list 'nv' */                      \
    current->next = *nv;                                                      \
    *nv = current;                                                            \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _splice_at)(list_t nlist, it_t npos,                              \
                        list_t olist, it_t opos)                              \
  {                                                                           \
    M_L1ST_CONTRACT(nlist);                                                   \
    M_L1ST_CONTRACT(olist);                                                   \
    M_ASSERT (npos != NULL);                                                  \
    M_ASSERT (opos != NULL);                                                  \
    M_ASSERT (M_F(name, _sublist_p)(nlist, npos));                            \
    M_ASSERT (M_F(name, _sublist_p)(olist, opos));                            \
    /* Remove the item 'opos' from the list 'olist' */                        \
    struct M_F(name, _s) *current = opos->current;                            \
    /* current shall reference a valid element of the list */                 \
    M_ASSERT (current != NULL);                                               \
    struct M_F(name, _s) *next    = current->next;                            \
    if (opos->previous == NULL) {                                             \
      *olist = next;                                                          \
    } else {                                                                  \
      opos->previous->next = next;                                            \
    }                                                                         \
    /* Update 'opos' to point to the next element */                          \
    opos->current = next;                                                     \
    /* Insert 'current' into 'nlist' just after 'npos' */                     \
    struct M_F(name, _s) *previous = npos->current;                           \
    if (M_UNLIKELY (previous == NULL)) {                                      \
      current->next = *nlist;                                                 \
      *nlist = current;                                                       \
    } else {                                                                  \
      current->next = previous->next;                                         \
      previous->next = current;                                               \
    }                                                                         \
    /* Update 'npos' to point to the new current element */                   \
    npos->previous = npos->current;                                           \
    npos->current  = current;                                                 \
    M_L1ST_CONTRACT(nlist);                                                   \
    M_L1ST_CONTRACT(olist);                                                   \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _splice)(list_t list1, list_t list2)                              \
  {                                                                           \
    M_L1ST_CONTRACT(list1);                                                   \
    M_L1ST_CONTRACT(list2);                                                   \
    M_ASSERT (list1 != list2);                                                \
    struct M_F(name, _s) **update_list = list1;                               \
    struct M_F(name, _s) *it = *list1;                                        \
    while (it != NULL) {                                                      \
      update_list = &it->next;                                                \
      it = it->next;                                                          \
    }                                                                         \
    *update_list = *list2;                                                    \
    *list2 = NULL;                                                            \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _reverse)(list_t list)                                            \
  {                                                                           \
    M_L1ST_CONTRACT(list);                                                    \
    struct M_F(name, _s) *previous = NULL, *it = *list, *next;                \
    while (it != NULL) {                                                      \
      next = it->next;                                                        \
      it->next = previous;                                                    \
      previous = it;                                                          \
      it = next;                                                              \
    }                                                                         \
    *list = previous;                                                         \
  }                                                                           \


/* Internal list function definition using only iterator functions 
   which is common for all kind of lists.
   It shall therefore only used the public interface of a list
   and no contract can be checked at this level.
   - name: prefix to be used
   - type: type of the elements of the list
   - oplist: oplist of the type of the elements of the container
   - list_t: alias for M_F(name, _t) [ type of the container ]
   - it_t: alias for M_F(name, _it_t) [ iterator of the container ]
 */
#define M_L1ST_ITBASE_DEF(name, type, oplist, list_t, it_t)                   \
                                                                              \
  M_IF_METHOD(GET_STR, oplist)(                                               \
  M_INLINE void                                                               \
  M_F(name, _get_str)(m_string_t str, const list_t list,                      \
                      bool append)                                            \
  {                                                                           \
    M_ASSERT (str != NULL && list != NULL);                                   \
    (append ? m_string_cat_cstr : m_string_set_cstr) (str, "[");              \
    it_t it;                                                                  \
    for (M_F(name, _it)(it, list) ;                                           \
         !M_F(name, _end_p)(it);                                              \
         M_F(name, _next)(it)){                                               \
      type const *item = M_F(name, _cref)(it);                                \
      M_CALL_GET_STR(oplist, str, *item, true);                               \
      if (!M_F(name, _last_p)(it))                                            \
        m_string_push_back (str, M_GET_SEPARATOR oplist);                     \
    }                                                                         \
    m_string_push_back (str, ']');                                            \
  }                                                                           \
  , /* no str */ )                                                            \
                                                                              \
  M_IF_METHOD(OUT_STR, oplist)(                                               \
  M_INLINE void                                                               \
  M_F(name, _out_str)(FILE *file, const list_t list)                          \
  {                                                                           \
    M_ASSERT (file != NULL && list != NULL);                                  \
    fputc ('[', file);                                                        \
    it_t it;                                                                  \
    for (M_F(name, _it)(it, list) ;                                           \
         !M_F(name, _end_p)(it);                                              \
         M_F(name, _next)(it)){                                               \
      type const *item = M_F(name, _cref)(it);                                \
      M_CALL_OUT_STR(oplist, file, *item);                                    \
      if (!M_F(name, _last_p)(it))                                            \
        fputc (M_GET_SEPARATOR oplist, file);                                 \
    }                                                                         \
    fputc (']', file);                                                        \
  }                                                                           \
  , /* no out_str */ )                                                        \
                                                                              \
  M_IF_METHOD2(PARSE_STR, INIT, oplist)(                                      \
  M_INLINE bool                                                               \
  M_F(name, _parse_str)(list_t list, const char str[], const char **endp)     \
  {                                                                           \
    M_ASSERT (str != NULL && list != NULL);                                   \
    M_F(name,_reset)(list);                                                   \
    bool success = false;                                                     \
    int c = *str++;                                                           \
    if (M_UNLIKELY (c != '[')) goto exit;                                     \
    c = *str++;                                                               \
    if (M_UNLIKELY (c == ']')) { success = true; goto exit;}                  \
    if (M_UNLIKELY (c == 0)) goto exit;                                       \
    str--;                                                                    \
    M_QLET(1, item, type, oplist) {                                           \
      do {                                                                    \
        bool b = M_CALL_PARSE_STR(oplist, item, str, &str);                   \
        c = m_core_str_nospace(&str);                                         \
        if (b == false || c == 0) { c = 0; break; }                           \
        M_F(name, _push_back)(list, item);                                    \
      } while (c == M_GET_SEPARATOR oplist);                                  \
      success = (c == ']');                                                   \
    }                                                                         \
    M_F(name, _reverse)(list);                                                \
  exit:                                                                       \
    if (endp) *endp = str;                                                    \
    return success;                                                           \
  }                                                                           \
  , /* no PARSE_STR & INIT */ )                                               \
                                                                              \
  M_IF_METHOD2(IN_STR, INIT, oplist)(                                         \
  M_INLINE bool                                                               \
  M_F(name, _in_str)(list_t list, FILE *file)                                 \
  {                                                                           \
    M_ASSERT (file != NULL && list != NULL);                                  \
    M_F(name,_reset)(list);                                                   \
    int c = fgetc(file);                                                      \
    if (M_UNLIKELY (c != '[')) return false;                                  \
    c = fgetc(file);                                                          \
    if (M_UNLIKELY (c == ']')) return true;                                   \
    if (M_UNLIKELY (c == EOF)) return false;                                  \
    ungetc(c, file);                                                          \
    M_QLET(1, item, type, oplist) {                                           \
      do {                                                                    \
        bool b = M_CALL_IN_STR(oplist, item, file);                           \
        c = m_core_fgetc_nospace(file);                                       \
        if (b == false || c == EOF) { c = 0; break; }                         \
        M_F(name, _push_back)(list, item);                                    \
      } while (c == M_GET_SEPARATOR oplist);                                  \
    }                                                                         \
    M_F(name, _reverse)(list);                                                \
    return c == ']';                                                          \
  }                                                                           \
  , /* no IN_STR & INIT */ )                                                  \
                                                                              \
  M_IF_METHOD(OUT_SERIAL, oplist)(                                            \
  M_INLINE m_serial_return_code_t                                             \
  M_F(name, _out_serial)(m_serial_write_t f, const list_t list)               \
  {                                                                           \
    M_ASSERT (list != NULL);                                                  \
    M_ASSERT (f != NULL && f->m_interface != NULL);                           \
    m_serial_return_code_t ret;                                               \
    m_serial_local_t local;                                                   \
    bool first_done = false;                                                  \
    ret = f->m_interface->write_array_start(local, f, (size_t)-1);            \
    if (ret == M_SERIAL_FAIL_RETRY) {                                         \
      size_t n = M_F(name, _size)(list);                                      \
      ret = f->m_interface->write_array_start(local, f, n);                   \
    }                                                                         \
    it_t it;                                                                  \
    for (M_F(name, _it)(it, list) ;                                           \
         !M_F(name, _end_p)(it);                                              \
         M_F(name, _next)(it)){                                               \
      type const *item = M_F(name, _cref)(it);                                \
      if (first_done)                                                         \
        ret |= f->m_interface->write_array_next(local, f);                    \
      ret |= M_CALL_OUT_SERIAL(oplist, f, *item);                             \
      first_done = true;                                                      \
    }                                                                         \
    ret |= f->m_interface->write_array_end(local, f);                         \
    return ret & M_SERIAL_FAIL;                                               \
  }                                                                           \
  , /* no OUT_SERIAL */ )                                                     \
                                                                              \
  M_IF_METHOD2(IN_SERIAL, INIT, oplist)(                                      \
  M_INLINE m_serial_return_code_t                                             \
  M_F(name, _in_serial)(list_t list, m_serial_read_t f)                       \
  {                                                                           \
    M_ASSERT (list != NULL);                                                  \
    M_ASSERT (f != NULL && f->m_interface != NULL);                           \
    m_serial_return_code_t ret;                                               \
    m_serial_local_t local;                                                   \
    size_t estimated_size = 0;                                                \
    M_F(name,_reset)(list);                                                   \
    ret = f->m_interface->read_array_start(local, f, &estimated_size);        \
    if (M_UNLIKELY (ret != M_SERIAL_OK_CONTINUE)) return ret;                 \
    M_QLET(1, item, type, oplist) {                                           \
      do {                                                                    \
        ret = M_CALL_IN_SERIAL(oplist, item, f);                              \
        if (ret != M_SERIAL_OK_DONE) { break; }                               \
        M_F(name, _push_back)(list, item);                                    \
        ret = f->m_interface->read_array_next(local, f);                      \
      } while (ret == M_SERIAL_OK_CONTINUE);                                  \
    }                                                                         \
    M_F(name, _reverse)(list);                                                \
    return ret;                                                               \
  }                                                                           \
  , /* no IN_SERIAL & INIT */ )                                               \
                                                                              \
  M_IF_METHOD(EQUAL, oplist)(                                                 \
  M_INLINE bool                                                               \
  M_F(name, _equal_p)(const list_t list1, const list_t list2)                 \
  {                                                                           \
    M_ASSERT (list1 != NULL && list2 != NULL);                                \
    it_t it1;                                                                 \
    it_t it2;                                                                 \
    if (list1 == list2) return true;                                          \
    M_F(name, _it)(it1, list1);                                               \
    M_F(name, _it)(it2, list2);                                               \
    while (!M_F(name, _end_p)(it1)                                            \
           &&!M_F(name, _end_p)(it2)) {                                       \
      type const *item1 = M_F(name, _cref)(it1);                              \
      type const *item2 = M_F(name, _cref)(it2);                              \
      bool b = M_CALL_EQUAL(oplist, *item1, *item2);                          \
      if (!b) return false;                                                   \
      M_F(name, _next)(it1);                                                  \
      M_F(name, _next)(it2);                                                  \
    }                                                                         \
    return M_F(name, _end_p)(it1)                                             \
      && M_F(name, _end_p)(it2);                                              \
  }                                                                           \
  , /* no equal */ )                                                          \
                                                                              \
  M_IF_METHOD(HASH, oplist)(                                                  \
  M_INLINE size_t                                                             \
  M_F(name, _hash)(const list_t list)                                         \
  {                                                                           \
    M_ASSERT (list != NULL);                                                  \
    M_HASH_DECL(hash);                                                        \
    it_t it;                                                                  \
    for(M_F(name, _it)(it, list) ;                                            \
        !M_F(name, _end_p)(it);                                               \
        M_F(name, _next)(it)) {                                               \
      type const *item = M_F(name, _cref)(it);                                \
      size_t hi = M_CALL_HASH(oplist, *item);                                 \
      M_HASH_UP(hash, hi);                                                    \
    }                                                                         \
    return M_HASH_FINAL (hash);                                               \
  }                                                                           \
  , /* no hash */ )                                                           \


/* Definition of the emplace_back function for single list */
#define M_L1ST_EMPLACE_DEF(name, name_t, function_name, oplist, init_func, exp_emplace_type) \
  M_INLINE void                                                               \
  function_name(name_t v                                                      \
                M_EMPLACE_LIST_TYPE_VAR(a, exp_emplace_type) )                \
  {                                                                           \
    M_F(name, _subtype_ct) *data = M_F(name, _push_raw)(v);                   \
    if (M_UNLIKELY (data == NULL) )                                           \
      return;                                                                 \
    M_IF_EXCEPTION(struct M_F(name, _s) *next = *v );                         \
    M_ON_EXCEPTION( *v = next->next, M_C3(m_l1st_,name,_del)(next)) {         \
      M_EMPLACE_CALL_FUNC(a, init_func, oplist, *data, exp_emplace_type);     \
    }                                                                         \
  }


/* Definition of the emplace_back function for dual push list */
#define M_L1ST_EMPLACE_BACK_DEF(name, name_t, function_name, oplist, init_func, exp_emplace_type) \
  M_INLINE void                                                               \
  function_name(name_t v                                                      \
                M_EMPLACE_LIST_TYPE_VAR(a, exp_emplace_type) )                \
  {                                                                           \
    M_F(name, _subtype_ct) *data = M_F(name, _push_back_raw)(v);              \
    if (M_UNLIKELY (data == NULL) )                                           \
      return;                                                                 \
    M_IF_EXCEPTION(struct M_F(name, _s) *next = v->back);                     \
    M_ON_EXCEPTION( v->back = next->next, v->front = (v->front == next) ? NULL : v->front, M_C3(m_l1st_,name,_del)(next)) { \
      M_EMPLACE_CALL_FUNC(a, init_func, oplist, *data, exp_emplace_type);     \
    }                                                                         \
  }


/* Definition of the emplace_front function for dual push list */
#define M_L1ST_EMPLACE_FRONT_DEF(name, name_t, function_name, oplist, init_func, exp_emplace_type) \
  M_INLINE void                                                               \
  function_name(name_t v                                                      \
                M_EMPLACE_LIST_TYPE_VAR(a, exp_emplace_type) )                \
  {                                                                           \
    M_IF_EXCEPTION(struct M_F(name, _s) *front = v->front, *back = v->back);  \
    M_F(name, _subtype_ct) *data = M_F(name, _push_front_raw)(v);             \
    if (M_UNLIKELY (data == NULL) )                                           \
      return;                                                                 \
    M_IF_EXCEPTION(struct M_F(name, _s) *m_volatile tofree = v->front);       \
    M_IF_EXCEPTION(M_ASSERT(tofree != NULL));                                 \
    M_ON_EXCEPTION( v->back = back, v->front = front, (front != NULL ? front : tofree)->next = NULL, M_C3(m_l1st_,name,_del)(tofree)) { \
      M_EMPLACE_CALL_FUNC(a, init_func, oplist, *data, exp_emplace_type);     \
    }                                                                         \
  }


/* Deferred evaluation for the dual-push list definition,
   so that all arguments are evaluated before further expansion */
#define M_L1ST_DUAL_PUSH_DEF_P1(arg) M_ID( M_L1ST_DUAL_PUSH_DEF_P2 arg )

/* Validate the oplist before going further */
#define M_L1ST_DUAL_PUSH_DEF_P2(name, type, oplist, list_t, it_t)             \
  M_IF_OPLIST(oplist)(M_L1ST_DUAL_PUSH_DEF_P3, M_L1ST_DUAL_PUSH_DEF_FAILURE)(name, type, oplist, list_t, it_t)

/* Stop processing with a compilation failure */
#define M_L1ST_DUAL_PUSH_DEF_FAILURE(name, type, oplist, list_t, it_t)        \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(LIST_DUAL_PUSH_DEF): the given argument is not a valid oplist: " #oplist)


/* Define the internal contract of an dual-push list */
#define M_L1ST_DUAL_PUSH_CONTRACT(l) do {                                     \
    M_ASSERT (l != NULL);                                                     \
    M_ASSERT ( (l->back == NULL && l->front == NULL)                          \
             || (l->back != NULL && l->front != NULL));                       \
  } while (0)


/* Internal dual-push list definition
   - name: prefix to be used
   - type: type of the elements of the array
   - oplist: oplist of the type of the elements of the container
   - list_t: alias for M_F(name, _t) [ type of the container ]
   - it_t: alias for M_F(name, _it_t) [ iterator of the container ]
 */
#define M_L1ST_DUAL_PUSH_DEF_P3(name, type, oplist, list_t, it_t)             \
  M_L1ST_DUAL_PUSH_DEF_TYPE(name, type, oplist, list_t, it_t)                 \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
  M_L1ST_MEMPOOL_DEF(name, type, oplist, list_t, it_t)                        \
  M_L1ST_DUAL_PUSH_DEF_P4(name, type, oplist, list_t, it_t)                   \
  M_EMPLACE_QUEUE_DEF(name, list_t, M_F(name, _emplace_back), oplist, M_L1ST_EMPLACE_BACK_DEF) \
  M_EMPLACE_QUEUE_DEF(name, list_t, M_F(name, _emplace_front), oplist, M_L1ST_EMPLACE_FRONT_DEF) \
  M_L1ST_ITBASE_DEF(name, type, oplist, list_t, it_t)


/* Define the type of a dual-push list */
#define M_L1ST_DUAL_PUSH_DEF_TYPE(name, type, oplist, list_t, it_t)           \
  /* Node of a list (it is liked the singly linked list) */                   \
  struct M_F(name, _s) {                                                      \
    struct M_F(name, _s) *next;                                               \
    type data;                                                                \
  };                                                                          \
                                                                              \
  /* Dual Push singly linked list.                                            \
     Support Push Back / Push Front / Pop back in O(1).                       \
     Doesn't support Pop front.                                               \
     This is done by keeping a pointer to both back & front                   \
  */                                                                          \
  typedef struct M_F(name, _head_s)  {                                        \
    struct M_F(name,_s) *front; /* Pointer to the front node or NULL */       \
    struct M_F(name,_s) *back;  /* Pointer to the back node or NULL */        \
  } list_t[1];                                                                \
                                                                              \
  /* Define the iterator over a dual push singly linked list */               \
  typedef struct M_F(name, _it_s) {                                           \
    struct M_F(name, _s) *previous;                                           \
    struct M_F(name, _s) *current;                                            \
  } it_t[1];                                                                  \
                                                                              \
  /* Definition of the synonyms of the type */                                \
  typedef struct M_F(name, _head_s) *M_F(name, _ptr);                         \
  typedef const struct M_F(name, _head_s) *M_F(name, _srcptr);                \
  typedef list_t M_F(name, _ct);                                              \
  typedef it_t M_F(name, _it_ct);                                             \
  typedef type M_F(name, _subtype_ct);                                        \
                                                                              \

/* Internal dual-push list definition
   - name: prefix to be used
   - type: type of the elements of the array
   - oplist: oplist of the type of the elements of the container
   - list_t: alias for type of the container
   - it_t: alias for iterator of the container
 */
#define M_L1ST_DUAL_PUSH_DEF_P4(name, type, oplist, list_t, it_t)             \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init)(list_t v)                                                  \
  {                                                                           \
    M_ASSERT( v != NULL);                                                     \
    v->front = NULL;                                                          \
    v->back = NULL;                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _reset)(list_t v)                                                 \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    struct M_F(name, _s) *it = v->back;                                       \
    while (it != NULL) {                                                      \
      struct M_F(name, _s) *next = it->next;                                  \
      M_CALL_CLEAR(oplist, it->data);                                         \
      M_C3(m_l1st_,name,_del)(it);                                            \
      it = next;                                                              \
    }                                                                         \
    v->front = NULL;                                                          \
    v->back = NULL;                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _clear)(list_t v)                                                 \
  {                                                                           \
    M_F(name, _reset)(v);                                                     \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _back)(const list_t v)                                            \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    M_ASSERT (v->back != NULL);                                               \
    return &(v->back->data);                                                  \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _push_back_raw)(list_t v)                                         \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    struct M_F(name, _s) *next = M_C3(m_l1st_,name,_new)();                   \
    if (M_UNLIKELY_NOMEM (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_F(name, _s)));                           \
      return NULL;                                                            \
    }                                                                         \
    type *ret = &next->data;                                                  \
    next->next = v->back;                                                     \
    v->back = next;                                                           \
    /* Update front too if the list was empty */                              \
    /* This C code shall generate branchless code */                          \
    struct M_F(name, _s) *front = v->front;                                   \
    front = (front == NULL) ? next : front;                                   \
    v->front = front;                                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    return ret;                                                               \
  }                                                                           \
                                                                              \
  /* Internal, for INIT_WITH */                                               \
  M_INLINE type *                                                             \
  M_F(name, _push_raw)(list_t d)                                              \
  {                                                                           \
    return M_F(name, _push_back_raw)(d);                                      \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _push_back)(list_t v, type const x)                               \
  {                                                                           \
    type *data = M_F(name, _push_back_raw)(v);                                \
    if (M_UNLIKELY (data == NULL))                                            \
      return;                                                                 \
    M_IF_EXCEPTION(struct M_F(name, _s) *next = v->back);                     \
    M_ON_EXCEPTION( v->back = next->next, v->front = (v->front == next) ? NULL : v->front, M_C3(m_l1st_,name,_del)(next)) { \
      M_CALL_INIT_SET(oplist, *data, x);                                      \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_IF_METHOD(INIT, oplist)(                                                  \
  M_INLINE type *                                                             \
  M_F(name, _push_back_new)(list_t v)                                         \
  {                                                                           \
    type *data = M_F(name, _push_back_raw)(v);                                \
    if (M_UNLIKELY (data == NULL))                                            \
      return NULL;                                                            \
    M_IF_EXCEPTION(struct M_F(name, _s) *next = v->back);                     \
    M_ON_EXCEPTION( v->back = next->next, v->front = (v->front == next) ? NULL : v->front, M_C3(m_l1st_,name,_del)(next)) { \
      M_CALL_INIT(oplist, *data);                                             \
    }                                                                         \
    return data;                                                              \
  }                                                                           \
  , /* No INIT */ )                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _push_back_move)(list_t v, type *x)                               \
  {                                                                           \
    M_ASSERT (x != NULL);                                                     \
    type *data = M_F(name, _push_back_raw)(v);                                \
    if (M_UNLIKELY (data == NULL))                                            \
      return;                                                                 \
    M_DO_INIT_MOVE (oplist, *data, *x);                                       \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _push_move)(list_t v, type *x)                                    \
  {                                                                           \
    M_F(name, _push_back_move)(v, x);                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _pop_back)(type *data, list_t v)                                  \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    M_ASSERT (v->back != NULL);                                               \
    struct M_F(name, _s) *tofree = v->back;                                   \
    if (data != NULL) {                                                       \
      M_DO_MOVE(oplist, *data, tofree->data);                                 \
    } else {                                                                  \
      M_CALL_CLEAR(oplist, tofree->data);                                     \
    }                                                                         \
    v->back = tofree->next;                                                   \
    M_C3(m_l1st_,name,_del)(tofree);                                          \
    /* Update front too if the list became empty */                           \
    /* This C code shall generate branchless code */                          \
    struct M_F(name, _s) *front = v->front;                                   \
    front = (front == tofree) ? NULL : front;                                 \
    v->front = front;                                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _pop_move)(type *data, list_t v)                                  \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    M_ASSERT (v->back != NULL);                                               \
    M_ASSERT (data != NULL);                                                  \
    struct M_F(name, _s) *tofree = v->back;                                   \
    M_DO_INIT_MOVE (oplist, *data, tofree->data);                             \
    v->back = tofree->next;                                                   \
    M_C3(m_l1st_,name,_del)(tofree);                                          \
    /* Update front too if the list became empty */                           \
    /* This C code shall generate branchless code */                          \
    struct M_F(name, _s) *front = v->front;                                   \
    front = (front == tofree) ? NULL : front;                                 \
    v->front = front;                                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _front)(list_t v)                                                 \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    M_ASSERT (v->front != NULL);                                              \
    return &(v->front->data);                                                 \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _push_front_raw)(list_t v)                                        \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    struct M_F(name, _s) *next = M_C3(m_l1st_,name,_new)();                   \
    if (M_UNLIKELY_NOMEM (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_F(name, _s)));                           \
      return NULL;                                                            \
    }                                                                         \
    type *ret = &next->data;                                                  \
    next->next = NULL;                                                        \
    if (M_LIKELY(v->front != NULL)) {                                         \
      v->front->next = next;                                                  \
    } else {                                                                  \
      /* Update back too as the list was empty */                             \
      v->back = next;                                                         \
    }                                                                         \
    v->front = next;                                                          \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    return ret;                                                               \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _push_front)(list_t v, type const x)                              \
  {                                                                           \
    M_IF_EXCEPTION(struct M_F(name, _s) *front = v->front, *back = v->back);  \
    type *data = M_F(name, _push_front_raw)(v);                               \
    if (M_UNLIKELY (data == NULL))                                            \
      return;                                                                 \
    M_IF_EXCEPTION(struct M_F(name, _s) *m_volatile tofree = v->front);       \
    M_IF_EXCEPTION(M_ASSERT(tofree != NULL));                                 \
    M_ON_EXCEPTION( v->back = back, v->front = front, (front != NULL ? front : tofree)->next = NULL, M_C3(m_l1st_,name,_del)(tofree)) { \
      M_CALL_INIT_SET(oplist, *data, x);                                      \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _push_front_move)(list_t v, type *x)                              \
  {                                                                           \
    M_ASSERT (x != NULL);                                                     \
    type *data = M_F(name, _push_front_raw)(v);                               \
    if (M_UNLIKELY (data == NULL))                                            \
      return;                                                                 \
    M_DO_INIT_MOVE (oplist, *data, *x);                                       \
  }                                                                           \
                                                                              \
  M_IF_METHOD(INIT, oplist)(                                                  \
  M_INLINE type *                                                             \
  M_F(name, _push_front_new)(list_t v)                                        \
  {                                                                           \
    M_IF_EXCEPTION(struct M_F(name, _s) *front = v->front, *back = v->back);  \
    type *data = M_F(name, _push_front_raw)(v);                               \
    if (M_UNLIKELY (data == NULL))                                            \
      return NULL;                                                            \
    M_IF_EXCEPTION(struct M_F(name, _s) *m_volatile tofree = v->front);       \
    M_IF_EXCEPTION(M_ASSERT(tofree != NULL));                                 \
    M_ON_EXCEPTION( v->back = back, v->front = front, (front != NULL ? front : tofree)->next = NULL, M_C3(m_l1st_,name,_del)(tofree)) { \
      M_CALL_INIT(oplist, *data);                                             \
    }                                                                         \
    return data;                                                              \
  }                                                                           \
  , /* No INIT */)                                                            \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _empty_p)(const list_t v)                                         \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    return v->back == NULL;                                                   \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _swap)(list_t l, list_t v)                                        \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(l);                                             \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    M_SWAP(struct M_F(name, _s) *, l->front, v->front);                       \
    M_SWAP(struct M_F(name, _s) *, l->back, v->back);                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it)(it_t it, const list_t v)                                     \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    M_ASSERT (it != NULL);                                                    \
    it->current  = v->back;                                                   \
    it->previous = NULL;                                                      \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it_set)(it_t it1, const it_t it2)                                \
  {                                                                           \
    M_ASSERT (it1 != NULL && it2 != NULL);                                    \
    it1->current  = it2->current;                                             \
    it1->previous = it2->previous;                                            \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _it_end)(it_t it1, const list_t v)                                \
  {                                                                           \
    M_ASSERT (it1 != NULL);                                                   \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    (void)v; /* unused */                                                     \
    it1->current  = NULL;                                                     \
    it1->previous = NULL;                                                     \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _end_p)(const it_t it)                                            \
  {                                                                           \
    M_ASSERT (it != NULL);                                                    \
    return it->current == NULL;                                               \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _last_p)(const it_t it)                                           \
  {                                                                           \
    M_ASSERT (it != NULL);                                                    \
    return it->current == NULL || it->current->next == NULL;                  \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _next)(it_t it)                                                   \
  {                                                                           \
    M_ASSERT(it != NULL && it->current != NULL);                              \
    it->previous = it->current;                                               \
    it->current  = it->current->next;                                         \
  }                                                                           \
                                                                              \
  M_INLINE bool                                                               \
  M_F(name, _it_equal_p)(const it_t it1, const it_t it2)                      \
  {                                                                           \
    M_ASSERT(it1 != NULL && it2 != NULL);                                     \
    return it1->current == it2->current;                                      \
  }                                                                           \
                                                                              \
  M_INLINE type *                                                             \
  M_F(name, _ref)(const it_t it)                                              \
  {                                                                           \
    M_ASSERT(it != NULL && it->current != NULL);                              \
    return &(it->current->data);                                              \
  }                                                                           \
                                                                              \
  M_INLINE type const *                                                       \
  M_F(name, _cref)(const it_t it)                                             \
  {                                                                           \
    M_ASSERT(it != NULL && it->current != NULL);                              \
    return M_CONST_CAST(type, &(it->current->data));                          \
  }                                                                           \
                                                                              \
  M_INLINE size_t                                                             \
  M_F(name, _size)(const list_t v)                                            \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(v);                                             \
    size_t size = 0;                                                          \
    struct M_F(name, _s) *it = v->back;                                       \
    while (it != NULL) {                                                      \
      size ++;                                                                \
      it = it->next;                                                          \
    }                                                                         \
    return size;                                                              \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _insert)(list_t list, it_t insertion_point,                       \
                     type const x)                                            \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(list);                                          \
    M_ASSERT (insertion_point != NULL);                                       \
    struct M_F(name, _s) *m_volatile next = M_C3(m_l1st_,name,_new)();        \
    if (M_UNLIKELY_NOMEM (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_F(name, _s)));                           \
      return;                                                                 \
    }                                                                         \
    M_ON_EXCEPTION( M_C3(m_l1st_,name,_del)(next))                            \
      M_CALL_INIT_SET(oplist, next->data, x);                                 \
    if (M_UNLIKELY (insertion_point->current == NULL)) {                      \
      next->next = list->back;                                                \
      list->back = next;                                                      \
      /* update front if list is empty */                                     \
      struct M_F(name, _s) *front = list->front;                              \
      front = (front == NULL) ? next : front;                                 \
      list->front = front;                                                    \
    } else {                                                                  \
      next->next = insertion_point->current->next;                            \
      insertion_point->current->next = next;                                  \
      /* update front if current == front */                                  \
      struct M_F(name, _s) *front = list->front;                              \
      front = (front == insertion_point->current) ? next : front;             \
      list->front = front;                                                    \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _remove)(list_t list, it_t removing_point)                        \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(list);                                          \
    M_ASSERT (removing_point != NULL);                                        \
    M_ASSERT(removing_point->current != NULL);                                \
    struct M_F(name, _s) *next = removing_point->current->next;               \
    struct M_F(name, _s) *previous = removing_point->previous;                \
    if (M_UNLIKELY (previous == NULL)) {                                      \
      list->back = next;                                                      \
    } else {                                                                  \
      previous->next = next;                                                  \
    }                                                                         \
    /* Update front  */                                                       \
    struct M_F(name, _s) *front = list->front;                                \
    front = (next == NULL) ? previous : front;                                \
    list->front = front;                                                      \
    /* Remove node */                                                         \
    M_CALL_CLEAR(oplist, removing_point->current->data);                      \
    M_C3(m_l1st_,name,_del) (removing_point->current);                        \
    removing_point->current = next;                                           \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _set)(list_t list, const list_t org)                              \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(list);                                          \
    M_L1ST_DUAL_PUSH_CONTRACT(org);                                           \
    struct M_F(name, _s) *m_volatile next = NULL;                             \
    struct M_F(name, _s) *it_org;                                             \
    struct M_F(name, _s) **update_list;                                       \
    if (M_UNLIKELY (list == org)) return;                                     \
    M_F(name, _reset)(list);                                                  \
    update_list = &list->back;                                                \
    it_org = org->back;                                                       \
    M_ASSERT(*update_list == NULL);                                           \
    M_ON_EXCEPTION(M_C3(m_l1st_,name,_del)(next) )                            \
    while (it_org != NULL) {                                                  \
      next = M_C3(m_l1st_,name,_new)();                                       \
      if (M_UNLIKELY_NOMEM (next == NULL)) {                                  \
        M_MEMORY_FULL(sizeof (struct M_F(name, _s)));                         \
        return;                                                               \
      }                                                                       \
      M_CALL_INIT_SET(oplist, next->data, it_org->data);                      \
      /* Link new node to the previous one*/                                  \
      *update_list = next;                                                    \
      update_list = &next->next;                                              \
      it_org = it_org->next;                                                  \
      /* so that even on exception, the object is still coherent */           \
      list->front = next;                                                     \
      *update_list = NULL;                                                    \
      M_IF_EXCEPTION(next = NULL);                                            \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init_set)(list_t list, const list_t org)                         \
  {                                                                           \
    M_ASSERT (list != org);                                                   \
    M_ON_EXCEPTION( M_F(name, _clear)(list)) {                                \
      M_F(name, _init)(list);                                                 \
      M_F(name, _set)(list, org);                                             \
    }                                                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _init_move)(list_t list, list_t org)                              \
  {                                                                           \
    M_ASSERT (list != org);                                                   \
    list->back  = org->back;                                                  \
    list->front = org->front;                                                 \
    org->back = NULL;                                                         \
    org->front = NULL;                                                        \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _move)(list_t list, list_t org)                                   \
  {                                                                           \
    M_F(name, _clear)(list);                                                  \
    M_F(name, _init_move)(list, org);                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _splice_back)(list_t list1, list_t list2, it_t it)                \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(list1);                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(list2);                                         \
    M_ASSERT (it->current != NULL);                                           \
    /* First remove the item 'it' from the list 'list2' */                    \
    struct M_F(name, _s) *current = it->current;                              \
    struct M_F(name, _s) *next = current->next;                               \
    if (it->previous == NULL) {                                               \
      list2->back = next;                                                     \
    } else {                                                                  \
      it->previous->next = next;                                              \
    }                                                                         \
    /* Update the front of 'list2' if it was the last element */              \
    struct M_F(name, _s) *front = list2->front;                               \
    front = (next == NULL) ? it->previous : front;                            \
    list2->front = front;                                                     \
    /* Update 'it' to point to the next element */                            \
    it->current = next;                                                       \
    /* Move the extracted 'current' into the list 'nv' */                     \
    current->next = list1->back;                                              \
    list1->back = current;                                                    \
    /* Update the front field if the list 'nv' was empty */                   \
    /* This C code shall generate branchless code */                          \
    front = list1->front;                                                     \
    front = (front == NULL) ? current : front;                                \
    list1->front = front;                                                     \
    M_L1ST_DUAL_PUSH_CONTRACT(list1);                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(list2);                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _splice_at)(list_t nlist, it_t npos,                              \
                        list_t olist, it_t opos)                              \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(nlist);                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(olist);                                         \
    M_ASSERT (npos != NULL && opos != NULL);                                  \
    /* First remove the item 'opos' from the list 'olist' */                  \
    struct M_F(name, _s) *current = opos->current;                            \
    /* It shall refer a valid argument in the list */                         \
    M_ASSERT(current != NULL);                                                \
    struct M_F(name, _s) *next    = current->next;                            \
    if (opos->previous == NULL) {                                             \
      olist->back = next;                                                     \
    } else {                                                                  \
      opos->previous->next = next;                                            \
    }                                                                         \
    /* Update the front of 'olist' if it was the last element */              \
    struct M_F(name, _s) *front = olist->front;                               \
    front = (next == NULL) ? opos->previous : front;                          \
    olist->front = front;                                                     \
    /* Update 'opos' to point to the next element */                          \
    opos->current  = next;                                                    \
    /* opos->previous is still valid & doesn't need to be updated */          \
    /* Insert into 'nlist' */                                                 \
    struct M_F(name, _s) *npos_current = npos->current;                       \
    if (M_UNLIKELY (npos_current == NULL)) {                                  \
      current->next = nlist->back;                                            \
      nlist->back = current;                                                  \
      /* update 'front' if the list was empty (branchless) */                 \
      front = nlist->front;                                                   \
      front = (front == NULL) ? current : front;                              \
      nlist->front = front;                                                   \
    } else {                                                                  \
      current->next = npos_current->next;                                     \
      npos_current->next = current;                                           \
      /* update front if current == front (branchless) */                     \
      front = nlist->front;                                                   \
      front = (front == npos_current) ? current : front;                      \
      nlist->front = front;                                                   \
    }                                                                         \
    /* Update 'npos' to point to 'current'. */                                \
    npos->previous = npos_current;                                            \
    npos->current = current;                                                  \
    M_L1ST_DUAL_PUSH_CONTRACT(nlist);                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(olist);                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _splice)(list_t list1, list_t list2)                              \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(list1);                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(list2);                                         \
    M_ASSERT (list1 != list2);                                                \
    if (M_LIKELY (list1->front != NULL)) {                                    \
      list1->front->next = list2->back;                                       \
      list1->front = list2->front;                                            \
    } else {                                                                  \
      /* list1 is empty */                                                    \
      list1->back  = list2->back;                                             \
      list1->front = list2->front;                                            \
    }                                                                         \
    list2->back = NULL;                                                       \
    list2->front = NULL;                                                      \
    M_L1ST_DUAL_PUSH_CONTRACT(list1);                                         \
    M_L1ST_DUAL_PUSH_CONTRACT(list2);                                         \
  }                                                                           \
                                                                              \
  M_INLINE void                                                               \
  M_F(name, _reverse)(list_t list)                                            \
  {                                                                           \
    M_L1ST_DUAL_PUSH_CONTRACT(list);                                          \
    list->front = list->back;                                                 \
    struct M_F(name, _s) *previous = NULL, *it = list->back, *next;           \
    while (it != NULL) {                                                      \
      next = it->next;                                                        \
      it->next = previous;                                                    \
      previous = it;                                                          \
      it = next;                                                              \
    }                                                                         \
    list->back = previous;                                                    \
    M_L1ST_DUAL_PUSH_CONTRACT(list);                                          \
  }                                                                           \


#if M_USE_SMALL_NAME
#define LIST_DEF M_LIST_DEF
#define LIST_DEF_AS M_LIST_DEF_AS
#define LIST_DUAL_PUSH_DEF M_LIST_DUAL_PUSH_DEF
#define LIST_DUAL_PUSH_DEF_AS M_LIST_DUAL_PUSH_DEF_AS
#define LIST_OPLIST M_LIST_OPLIST
#define LIST_INIT_VALUE M_LIST_INIT_VALUE
#define LIST_DUAL_PUSH_INIT_VALUE M_LIST_DUAL_PUSH_INIT_VALUE
#endif

#endif
