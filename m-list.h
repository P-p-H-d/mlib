/*
 * M*LIB - LIST module
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
#ifndef __M_LIST_H
#define __M_LIST_H

#include "m-core.h"

/* Define a singly linked list of a given type.
   USAGE: LIST_DEF(name, type [, oplist_of_the_type]) */
#define LIST_DEF(name, ...)                                             \
  LISTI_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
  ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), M_C(name,_t), M_C(name, _it_t) ), \
   (name, __VA_ARGS__,                                      M_C(name,_t), M_C(name, _it_t) )))

/* Define a dual-push singly linked list of a given type.
   USAGE: LIST_DEF(name, type [, oplist_of_the_type]) */
#define LIST_DUAL_PUSH_DEF(name, ...)                                   \
  LISTI_DUAL_PUSH_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                       \
  ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), M_C(name,_t), M_C(name, _it_t) ), \
   (name, __VA_ARGS__,                                      M_C(name,_t), M_C(name, _it_t) )))

/* Define the oplist of a list of type.
   USAGE: LIST_OPLIST(name [, oplist_of_the_type]) */
#define LIST_OPLIST(...)                                             \
  LISTI_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
               ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ), \
                (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

#define LISTI_OPLIST(arg) LISTI_OPLIST2 arg

#define LISTI_OPLIST2(name, oplist)                                     \
  (INIT(M_C(name, _init)),						\
   INIT_SET(M_C(name, _init_set)),					\
   SET(M_C(name, _set)),						\
   CLEAR(M_C(name, _clear)),						\
   MOVE(M_C(name, _move)),						\
   INIT_MOVE(M_C(name, _init_move)),					\
   SWAP(M_C(name, _swap)),						\
   TYPE(M_C(name,_t)),							\
   SUBTYPE(M_C(name, _type_t)),						\
   IT_TYPE(M_C(name, _it_t)),						\
   IT_FIRST(M_C(name,_it)),						\
   IT_END(M_C(name,_it_end)),						\
   IT_SET(M_C(name,_it_set)),						\
   IT_END_P(M_C(name,_end_p)),						\
   IT_EQUAL_P(M_C(name,_it_equal_p)),					\
   IT_LAST_P(M_C(name,_last_p)),					\
   IT_NEXT(M_C(name,_next)),						\
   IT_REF(M_C(name,_ref)),						\
   IT_CREF(M_C(name,_cref)),						\
   IT_REMOVE(M_C(name,_remove)),					\
   CLEAN(M_C(name,_clean)),						\
   PUSH(M_C(name,_push_back)),						\
   POP(M_C(name,_pop_back)),						\
   PUSH_MOVE(M_C(name,_push_move)),                                     \
   POP_MOVE(M_C(name,_pop_move))                                        \
   ,SPLICE_BACK(M_C(name,_splice_back))                                 \
   ,REVERSE(M_C(name,_reverse))						\
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C(name, _get_str)),)		\
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C(name, _out_str)),)		\
   ,M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_C(name, _parse_str)),)   \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C(name, _in_str)),)		\
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C(name, _equal_p)),)		\
   ,M_IF_METHOD(HASH, oplist)(HASH(M_C(name, _hash)),)			\
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

#define LISTI_DEF(arg) LISTI_DEF2 arg

/* Define allocation functions. If MEMPOOL, we need to define it */
#define LISTI_MEMPOOL_DEF(name, type, oplist, list_t, list_it_t)        \
  M_IF_METHOD(MEMPOOL, oplist)(                                         \
			       						\
    MEMPOOL_DEF(M_C(name, _mempool), struct M_C(name, _s))              \
    M_GET_MEMPOOL_LINKAGE oplist M_C(name, _mempool_t) M_GET_MEMPOOL oplist; \
    static inline struct M_C(name, _s) *M_C(name, _int_new)(void) {	\
      return M_C(name, _mempool_alloc)(M_GET_MEMPOOL oplist);		\
    }                                                                   \
    static inline void M_C(name,_int_del)(struct M_C(name, _s) *ptr) {	\
      M_C(name, _mempool_free)(M_GET_MEMPOOL oplist, ptr);		\
    }                                                                   \
									\
    , /* No mempool allocation */                                       \
									\
    static inline struct M_C(name, _s) *M_C(name, _int_new)(void) {	\
      return M_GET_NEW oplist (struct M_C(name, _s));			\
    }                                                                   \
    static inline void M_C(name,_int_del)(struct M_C(name, _s) *ptr) {	\
      M_GET_DEL oplist (ptr);                                           \
    }                                                                   \
    )                                                                   \

#define LISTI_DEF2(name, type, oplist, list_t, list_it_t)               \
									\
  typedef struct M_C(name, _s) {					\
    struct M_C(name, _s) *next;						\
    type data;                                                          \
  } *list_t[1];                                                         \
									\
  typedef type M_C(name, _type_t);					\
                                                                        \
  typedef struct M_C(name, _it_s) {					\
    struct M_C(name, _s) *previous;					\
    struct M_C(name, _s) *current;                                      \
  } list_it_t[1];                                                       \
                                                                        \
  LISTI_MEMPOOL_DEF(name, type, oplist, list_t, list_it_t)              \
  LISTI_DEF3(name, type, oplist, list_t, list_it_t)                     \
  LISTI_ITBASE_DEF(name, type, oplist, list_t, list_it_t)

#define LISTI_CONTRACT(v) do {                                          \
    assert (v != NULL);                                                 \
  } while (0)

#define LISTI_DEF3(name, type, oplist, list_t, list_it_t)               \
  									\
  static inline void                                                    \
  M_C(name, _init)(list_t v)						\
  {                                                                     \
    assert (v != NULL);                                                 \
    *v = NULL;                                                          \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _clean)(list_t v)						\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    struct M_C(name, _s) *it = *v;					\
    *v = NULL;                                                          \
    while (it != NULL) {                                                \
      struct M_C(name, _s) *next = it->next;				\
      M_GET_CLEAR oplist(it->data);                                     \
      M_C(name,_int_del)(it);						\
      it = next;                                                        \
    }                                                                   \
    LISTI_CONTRACT(v);                                                  \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _clear)(list_t v)						\
  {                                                                     \
    M_C(name, _clean)(v);						\
  }                                                                     \
  									\
  static inline const type *                                            \
  M_C(name, _back)(const list_t v)					\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    assert(*v != NULL);                                                 \
    return M_CONST_CAST(type, &((*v)->data));                           \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_raw)(list_t v)					\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    struct M_C(name, _s) *next;						\
    next = M_C(name, _int_new)();					\
    if (M_UNLIKELY (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_C(name, _s)));			\
      return NULL;                                                      \
    }                                                                   \
    type *ret = &next->data;                                            \
    next->next = *v;                                                    \
    *v = next;                                                          \
    LISTI_CONTRACT(v);                                                  \
    return ret;                                                         \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_back)(list_t v, type const x)				\
  {                                                                     \
    type *data = M_C(name, _push_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                    \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_new)(list_t v)					\
  {                                                                     \
    type *data = M_C(name, _push_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                           \
    return data;                                                        \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _pop_back)(type *data, list_t v)				\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    assert(*v != NULL);                                                 \
    if (data != NULL) {                                                 \
      M_DO_MOVE (oplist, *data, (*v)->data);                            \
    } else {                                                            \
      M_GET_CLEAR oplist((*v)->data);                                   \
    }                                                                   \
    struct M_C(name, _s) *tofree = *v;					\
    *v = (*v)->next;                                                    \
    M_C(name,_int_del)(tofree);						\
    LISTI_CONTRACT(v);                                                  \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_move)(list_t v, type *x)				\
  {                                                                     \
    assert (x != NULL);                                                 \
    type *data = M_C(name, _push_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return;                                                           \
    M_DO_INIT_MOVE (oplist, *data, *x);                                 \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _pop_move)(type *data, list_t v)				\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    assert(*v != NULL && data != NULL);                                 \
    M_DO_INIT_MOVE (oplist, *data, (*v)->data);                         \
    struct M_C(name, _s) *tofree = *v;					\
    *v = (*v)->next;                                                    \
    M_C(name,_int_del)(tofree);						\
    LISTI_CONTRACT(v);                                                  \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _empty_p)(const list_t v)					\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    return *v == NULL;                                                  \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _swap)(list_t l, list_t v)					\
  {                                                                     \
    LISTI_CONTRACT(l);                                                  \
    LISTI_CONTRACT(v);                                                  \
    struct M_C(name, _s) *tmp = *l;					\
    *l = *v;                                                            \
    *v = tmp;                                                           \
    LISTI_CONTRACT(l);                                                  \
    LISTI_CONTRACT(v);                                                  \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it)(list_it_t it, const list_t v)				\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    assert (it != NULL);                                                \
    it->current = *v;                                                   \
    it->previous = NULL;                                                \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_set)(list_it_t it1, const list_it_t it2)		\
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    it1->current  = it2->current;                                       \
    it1->previous = it2->previous;                                      \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_end)(list_it_t it1, const list_t v)			\
  {                                                                     \
    LISTI_CONTRACT(v);                                                  \
    assert (it1 != NULL);                                               \
    (void)v; /* unused */                                               \
    it1->current  = NULL;                                               \
    it1->previous = NULL;                                               \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _end_p)(const list_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    return it->current == NULL;                                         \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _last_p)(const list_it_t it)                                \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->current == NULL || it->current->next == NULL;            \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _next)(list_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->current != NULL);                          \
    it->previous = it->current;                                         \
    it->current  = it->current->next;                                   \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _it_equal_p)(const list_it_t it1, const list_it_t it2)	\
  {                                                                     \
    assert(it1 != NULL && it2 != NULL);                                 \
    return it1->current == it2->current;                                \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _ref)(const list_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->current != NULL);                          \
    return &(it->current->data);					\
  }                                                                     \
  									\
  static inline const type *                                            \
  M_C(name, _cref)(const list_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->current != NULL);                          \
    return M_CONST_CAST(type, &(it->current->data));                    \
  }                                                                     \
  									\
  static inline size_t                                                  \
  M_C(name, _size)(const list_t list)					\
  {                                                                     \
    LISTI_CONTRACT(list);                                               \
    size_t size = 0;                                                    \
    struct M_C(name, _s) *it = *list;					\
    while (it != NULL) {                                                \
      size ++;                                                          \
      it = it->next;                                                    \
    }                                                                   \
    return size;                                                        \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _sublist_p)(const list_t list, const list_it_t itsub)       \
  {                                                                     \
    LISTI_CONTRACT(list);                                               \
    assert (itsub != NULL);                                             \
    struct M_C(name, _s) *it = *list;					\
    while (it != NULL) {                                                \
      if (it == itsub->current) return true;                            \
      it = it->next;                                                    \
    }                                                                   \
    return (itsub->current == NULL);                                    \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _get)(const list_t list, size_t i)				\
  {                                                                     \
    LISTI_CONTRACT(list);                                               \
    struct M_C(name, _s) *it = *list;					\
    /* FIXME: How to avoid the double iteration over the list? */       \
    size_t len = M_C(name,_size)(list);					\
    assert (i < len);                                                   \
    size_t j = len-1;                                                   \
    while (true) {                                                      \
      assert (it != NULL);                                              \
      if (i == j) return &it->data;                                     \
      it = it->next;                                                    \
      j--;                                                              \
    }                                                                   \
  }                                                                     \
  									\
  static inline const type *                                            \
  M_C(name, _cget)(const list_t l, size_t i)				\
  {                                                                     \
    return M_CONST_CAST(type, M_C(name, _get)(l,i));			\
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _insert)(list_t list, list_it_t insertion_point,            \
		     type const x)					\
  {                                                                     \
    LISTI_CONTRACT(list);                                               \
    assert (insertion_point != NULL);                                   \
    assert(M_C(name, _sublist_p)(list, insertion_point));		\
    struct M_C(name, _s) *next = M_C(name, _int_new)();                 \
    if (M_UNLIKELY (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_C(name, _s)));			\
      return;                                                           \
    }                                                                   \
    M_GET_INIT_SET oplist(next->data, x);                               \
    struct M_C(name, _s) *current = insertion_point->current;           \
    if (M_UNLIKELY (current == NULL)) {                                 \
      next->next = *list;                                               \
      *list = next;                                                     \
    } else {                                                            \
      next->next = current->next;                                       \
      current->next = next;                                             \
    }                                                                   \
    LISTI_CONTRACT(list);                                               \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _remove)(list_t list, list_it_t removing_point)             \
  {                                                                     \
    LISTI_CONTRACT(list);                                               \
    assert (removing_point != NULL);                                    \
    assert (removing_point->current != NULL);                           \
    assert(M_C(name, _sublist_p)(list, removing_point));		\
    struct M_C(name, _s) *next = removing_point->current->next;		\
    if (M_UNLIKELY (removing_point->previous == NULL)) {                \
      *list = next;                                                     \
    } else {                                                            \
      removing_point->previous->next = next;                            \
    }                                                                   \
    M_GET_CLEAR oplist(removing_point->current->data);                  \
    M_C(name,_int_del) (removing_point->current);			\
    removing_point->current = next;                                     \
    LISTI_CONTRACT(list);                                               \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _set)(list_t list, const list_t org)			\
  {                                                                     \
    LISTI_CONTRACT(list);                                               \
    LISTI_CONTRACT(org);                                                \
    struct M_C(name, _s) *next, *it_org;				\
    struct M_C(name, _s) **update_list;					\
    if (list == org) return;                                            \
    M_C(name, _clean)(list);						\
    update_list = list;                                                 \
    it_org = *org;                                                      \
    while (it_org != NULL) {                                            \
      next = M_C(name, _int_new)();					\
      *update_list = next;                                              \
      if (M_UNLIKELY (next == NULL)) {                                  \
        M_MEMORY_FULL(sizeof (struct M_C(name, _s)));			\
        /* FIXME: Partialy initialized list. What to do? */             \
        return;                                                         \
      }                                                                 \
      update_list = &next->next;                                        \
      M_GET_INIT_SET oplist(next->data, it_org->data);                  \
      it_org = it_org->next;                                            \
    }                                                                   \
    *update_list = NULL;                                                \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _init_set)(list_t list, const list_t org)			\
  {                                                                     \
    M_C(name, _init)(list);						\
    M_C(name, _set)(list, org);						\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _init_move)(list_t list, list_t org)			\
  {                                                                     \
    LISTI_CONTRACT(org);                                                \
    assert (list != NULL && list != org);                               \
    *list = *org;                                                       \
    *org = NULL;  /* safer */                                           \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _move)(list_t list, list_t org)				\
  {                                                                     \
    assert (list != org);                                               \
    M_C(name, _clear)(list);						\
    M_C(name, _init_move)(list, org);					\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _splice_back)(list_t nv, list_t ov, list_it_t it)		\
  {                                                                     \
    LISTI_CONTRACT(nv);                                                 \
    LISTI_CONTRACT(ov);                                                 \
    assert (it != NULL);                                                \
    assert (it->current != NULL);                                       \
    assert (M_C(name, _sublist_p)(ov, it));				\
    /* Remove the item 'it' from the list 'ov' */                       \
    struct M_C(name, _s) *current = it->current;                        \
    struct M_C(name, _s) *next    = current->next;			\
    if (it->previous == NULL) {                                         \
      *ov = next;                                                       \
    } else {                                                            \
      it->previous->next = next;                                        \
    }                                                                   \
    /* Update the item 'it' to point to the next element */             \
    it->current = next;                                                 \
    /* Move current in nv */                                            \
    current->next = *nv;                                                \
    *nv = current;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _splice_at)(list_t nlist, list_it_t npos,                   \
                        list_t olist, list_it_t opos)                   \
  {                                                                     \
    LISTI_CONTRACT(nlist);                                              \
    LISTI_CONTRACT(olist);                                              \
    assert (npos != NULL);                                              \
    assert (opos != NULL);                                              \
    assert (M_C(name, _sublist_p)(nlist, npos));                        \
    assert (M_C(name, _sublist_p)(olist, opos));                        \
    /* Remove the item 'opos' from the list 'olist' */                  \
    struct M_C(name, _s) *current = opos->current;                      \
    struct M_C(name, _s) *next    = current->next;			\
    if (opos->previous == NULL) {                                       \
      *olist = next;                                                    \
    } else {                                                            \
      opos->previous->next = next;                                      \
    }                                                                   \
    /* Update 'opos' to point to the next element */                    \
    opos->current = next;                                               \
    /* Insert 'current' into 'nlist' just after 'npos' */               \
    struct M_C(name, _s) *previous = npos->current;                     \
    if (M_UNLIKELY (previous == NULL)) {                                \
      current->next = *nlist;                                           \
      *nlist = current;                                                 \
    } else {                                                            \
      current->next = previous->next;                                   \
      previous->next = current;                                         \
    }                                                                   \
    LISTI_CONTRACT(nlist);                                              \
    LISTI_CONTRACT(olist);                                              \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _splice)(list_t list1, list_t list2)			\
  {                                                                     \
    LISTI_CONTRACT(list1);                                              \
    LISTI_CONTRACT(list2);                                              \
    assert (list1 != list2);                                            \
    struct M_C(name, _s) **update_list = list1;				\
    struct M_C(name, _s) *it = *list1;					\
    while (it != NULL) {                                                \
      update_list = &it->next;                                          \
      it = it->next;							\
    }                                                                   \
    *update_list = *list2;                                              \
    *list2 = NULL;                                                      \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _reverse)(list_t list)					\
  {                                                                     \
    LISTI_CONTRACT(list);                                               \
    struct M_C(name, _s) *previous = NULL, *it = *list, *next;		\
    while (it != NULL) {                                                \
      next = it->next;                                                  \
      it->next = previous;                                              \
      previous = it;                                                    \
      it = next;                                                        \
    }                                                                   \
    *list = previous;                                                   \
  }                                                                     \

#define LISTI_ITBASE_DEF(name, type, oplist, list_t, list_it_t)         \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _get_str)(string_t str, const list_t list,			\
		      bool append)					\
  {                                                                     \
    assert (str != NULL && list != NULL);                               \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    M_C(name, _it_t) it;						\
    for (M_C(name, _it)(it, list) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      const type *item = M_C(name, _cref)(it);				\
      M_GET_GET_STR oplist (str, *item, true);                          \
      if (!M_C(name, _last_p)(it))					\
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    string_push_back (str, ']');                                        \
  }                                                                     \
  , /* no str */ )                                                      \
			      						\
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *file, const list_t list)			\
  {                                                                     \
    assert (file != NULL && list != NULL);                              \
    fputc ('[', file);                                                  \
    M_C(name, _it_t) it;						\
    for (M_C(name, _it)(it, list) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      const type *item = M_C(name, _cref)(it);				\
      M_GET_OUT_STR oplist (file, *item);                               \
      if (!M_C(name, _last_p)(it))					\
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fputc (']', file);                                                  \
  }                                                                     \
  , /* no out_str */ )                                                  \
                                                                        \
  M_IF_METHOD(PARSE_STR, oplist)(                                       \
  static inline bool                                                    \
  M_C(name, _parse_str)(list_t list, const char str[], const char **endp) \
  {                                                                     \
    assert (str != NULL && list != NULL);                               \
    M_C(name,_clean)(list);						\
    bool success = false;                                               \
    int c = *str++;                                                     \
    if (M_UNLIKELY (c != '[')) goto exit;                               \
    c = *str++;                                                         \
    if (M_UNLIKELY (c == ']')) { success = true; goto exit;}            \
    if (M_UNLIKELY (c == 0)) goto exit;                                 \
    str--;                                                              \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_PARSE_STR oplist (item, str, &str);                \
      do { c = *str++; } while (isspace(c));                            \
      if (b == false || c == 0) { goto exit; }				\
      M_C(name, _push_back)(list, item);				\
    } while (c == M_GET_SEPARATOR oplist);				\
    M_GET_CLEAR oplist (item);                                          \
    M_C(name, _reverse)(list);						\
    success = (c == ']');                                               \
  exit:                                                                 \
    if (endp) *endp = str;                                              \
    return success;                                                     \
  }                                                                     \
  , /* no parse_str */ )                                                \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C(name, _in_str)(list_t list, FILE *file)				\
  {                                                                     \
    assert (file != NULL && list != NULL);                              \
    M_C(name,_clean)(list);						\
    int c = fgetc(file);						\
    if (M_UNLIKELY (c != '[')) return false;                            \
    c = fgetc(file);                                                    \
    if (M_UNLIKELY (c == ']')) return true;                             \
    if (M_UNLIKELY (c == EOF)) return false;                            \
    ungetc(c, file);                                                    \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_IN_STR oplist (item, file);                        \
      do { c = fgetc(file); } while (isspace(c));                       \
      if (b == false || c == EOF) { break; }				\
      M_C(name, _push_back)(list, item);				\
    } while (c == M_GET_SEPARATOR oplist);				\
    M_GET_CLEAR oplist (item);                                          \
    M_C(name, _reverse)(list);						\
    return c == ']';                                                    \
  }                                                                     \
  , /* no in_str */ )                                                   \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  M_C(name, _equal_p)(const list_t list1, const list_t list2)		\
  {                                                                     \
    assert (list1 != NULL && list2 != NULL);                            \
    M_C(name, _it_t) it1;						\
    M_C(name, _it_t) it2;						\
    if (list1 == list2) return true;                                    \
    M_C(name, _it)(it1, list1);						\
    M_C(name, _it)(it2, list2);						\
    while (!M_C(name, _end_p)(it1)					\
           &&!M_C(name, _end_p)(it2)) {					\
      const type *item1 = M_C(name, _cref)(it1);			\
      const type *item2 = M_C(name, _cref)(it2);			\
      bool b = M_GET_EQUAL oplist (*item1, *item2);                     \
      if (!b) return false;                                             \
      M_C(name, _next)(it1);						\
      M_C(name, _next)(it2);						\
    }                                                                   \
    return M_C(name, _end_p)(it1)					\
      && M_C(name, _end_p)(it2);					\
  }                                                                     \
  , /* no equal */ )                                                    \
			    						\
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t                                                  \
  M_C(name, _hash)(const list_t list)					\
  {                                                                     \
    assert (list != NULL);                                              \
    M_HASH_DECL(hash);                                                  \
    M_C(name, _it_t) it;						\
    for(M_C(name, _it)(it, list) ;					\
        !M_C(name, _end_p)(it);						\
        M_C(name, _next)(it)) {						\
      const type *item = M_C(name, _cref)(it);				\
      size_t hi = M_GET_HASH oplist (*item);                            \
      M_HASH_UP(hash, hi);                                              \
    }                                                                   \
    return M_HASH_FINAL (hash);						\
  }                                                                     \
  , /* no hash */ )                                                     \

/* Dual Push singly linked list.
   Support Push Back / Push Front / Pop back in O(1).
   Doesn't support Pop front.
   This is done by keeping a pointer to both back & front
 */
#define LISTI_DUAL_PUSH_DEF(arg) LISTI_DUAL_PUSH_DEF2 arg

#define LISTI_DUAL_PUSH_DEF2(name, type, oplist, list_t, list_it_t)     \
									\
  struct M_C(name, _s) {                                                \
    struct M_C(name, _s) *next;						\
    type data;                                                          \
  };                                                                    \
                                                                        \
  typedef struct {                                                      \
    struct M_C(name, _s) *front;                                        \
    struct M_C(name, _s) *back;                                         \
  } list_t[1];                                                          \
                                                                        \
  typedef type M_C(name, _type_t);					\
                                                                        \
  typedef struct M_C(name, _it_s) {					\
    struct M_C(name, _s) *previous;					\
    struct M_C(name, _s) *current;					\
  } list_it_t[1];                                                       \
                                                                        \
  LISTI_MEMPOOL_DEF(name, type, oplist, list_t, list_it_t)              \
  LISTI_DUAL_PUSH_DEF3(name, type, oplist, list_t, list_it_t)           \
  LISTI_ITBASE_DEF(name, type, oplist, list_t, list_it_t)

#define LISTI_DUAL_PUSH_CONTRACT(l) do {                                \
    assert (l != NULL);                                                 \
    assert ( (l->back == NULL && l->front == NULL)                      \
             || (l->back != NULL && l->front != NULL));                 \
  } while (0)

#define LISTI_DUAL_PUSH_DEF3(name, type, oplist, list_t, list_it_t)     \
  									\
  static inline void                                                    \
  M_C(name, _init)(list_t v)						\
  {                                                                     \
    assert( v != NULL);                                                 \
    v->front = NULL;                                                    \
    v->back = NULL;                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _clean)(list_t v)						\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    struct M_C(name, _s) *it = v->back;                                 \
    while (it != NULL) {                                                \
      struct M_C(name, _s) *next = it->next;                            \
      M_GET_CLEAR oplist(it->data);                                     \
      M_C(name,_int_del)(it);						\
      it = next;                                                        \
    }                                                                   \
    v->front = NULL;                                                    \
    v->back = NULL;                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _clear)(list_t v)						\
  {                                                                     \
    M_C(name, _clean)(v);						\
  }                                                                     \
  									\
  static inline const type *                                            \
  M_C(name, _back)(const list_t v)					\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    assert (v->back != NULL);                                           \
    return M_CONST_CAST(type, &(v->back->data));                        \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_back_raw)(list_t v)					\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    struct M_C(name, _s) *next = M_C(name, _int_new)();                 \
    if (M_UNLIKELY (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_C(name, _s)));			\
      return NULL;                                                      \
    }                                                                   \
    type *ret = &next->data;                                            \
    next->next = v->back;                                               \
    v->back = next;                                                     \
    /* Update front too if the list was empty */                        \
    /* This C code shall generate branchless code */                    \
    struct M_C(name, _s) *front = v->front;                             \
    front = (front == NULL) ? next : front;                             \
    v->front = front;                                                   \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    return ret;                                                         \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_back)(list_t v, type const x)				\
  {                                                                     \
    type *data = M_C(name, _push_back_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                    \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_back_new)(list_t v)					\
  {                                                                     \
    type *data = M_C(name, _push_back_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                           \
    return data;                                                        \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_back_move)(list_t v, type *x)				\
  {                                                                     \
    assert (x != NULL);                                                 \
    type *data = M_C(name, _push_back_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return;                                                           \
    M_DO_INIT_MOVE (oplist, *data, *x);                                 \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_move)(list_t v, type *x)				\
  {                                                                     \
    M_C(name, _push_back_move)(v, x);                                   \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _pop_back)(type *data, list_t v)				\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    assert (v->back != NULL);                                           \
    struct M_C(name, _s) *tofree = v->back;                             \
    if (data != NULL) {                                                 \
      M_GET_SET oplist(*data, tofree->data);                            \
    }                                                                   \
    M_GET_CLEAR oplist(tofree->data);                                   \
    v->back = tofree->next;                                             \
    M_C(name,_int_del)(tofree);						\
    /* Update front too if the list became empty */                     \
    /* This C code shall generate branchless code */                    \
    struct M_C(name, _s) *front = v->front;                             \
    front = (front == tofree) ? NULL : front;                           \
    v->front = front;                                                   \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _pop_move)(type *data, list_t v)				\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    assert (v->back != NULL);                                           \
    assert (data != NULL);                                              \
    struct M_C(name, _s) *tofree = v->back;                             \
    M_DO_INIT_MOVE (oplist, *data, tofree->data);                       \
    v->back = tofree->next;                                             \
    M_C(name,_int_del)(tofree);						\
    /* Update front too if the list became empty */                     \
    /* This C code shall generate branchless code */                    \
    struct M_C(name, _s) *front = v->front;                             \
    front = (front == tofree) ? NULL : front;                           \
    v->front = front;                                                   \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C(name, _front)(list_t v)                                           \
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    assert (v->front != NULL);                                          \
    return M_CONST_CAST(type, &(v->front->data));                       \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_front_raw)(list_t v)					\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    struct M_C(name, _s) *next = M_C(name, _int_new)();                 \
    if (M_UNLIKELY (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_C(name, _s)));			\
      return NULL;                                                      \
    }                                                                   \
    type *ret = &next->data;                                            \
    next->next = NULL;                                                  \
    if (M_LIKELY(v->front != NULL)) {                                   \
      v->front->next = next;                                            \
    } else {                                                            \
      /* Update back too as the list was empty */                       \
      v->back = next;                                                   \
    }                                                                   \
    v->front = next;                                                    \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    return ret;                                                         \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_front)(list_t v, type const x)                        \
  {                                                                     \
    type *data = M_C(name, _push_front_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                    \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _push_front_move)(list_t v, type *x)                        \
  {                                                                     \
    assert (x != NULL);                                                 \
    type *data = M_C(name, _push_front_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return;                                                           \
    M_DO_INIT_MOVE (oplist, *data, *x);                                 \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _push_front_new)(list_t v)					\
  {                                                                     \
    type *data = M_C(name, _push_back_raw)(v);				\
    if (M_UNLIKELY (data == NULL))                                      \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                           \
    return data;                                                        \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _empty_p)(const list_t v)                                   \
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    return v->back == NULL;                                             \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _swap)(list_t l, list_t v)					\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(l);                                        \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    M_SWAP(struct M_C(name, _s) *, l->front, v->front);                 \
    M_SWAP(struct M_C(name, _s) *, l->back, v->back);                   \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it)(list_it_t it, const list_t v)				\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    assert (it != NULL);                                                \
    it->current  = v->back;                                             \
    it->previous = NULL;                                                \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_set)(list_it_t it1, const list_it_t it2)		\
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    it1->current  = it2->current;                                       \
    it1->previous = it2->previous;                                      \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_end)(list_it_t it1, const list_t v)			\
  {                                                                     \
    assert (it1 != NULL);                                               \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    (void)v; /* unused */                                               \
    it1->current  = NULL;                                               \
    it1->previous = NULL;                                               \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _end_p)(const list_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    return it->current == NULL;                                         \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _last_p)(const list_it_t it)                                \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->current == NULL || it->current->next == NULL;            \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _next)(list_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->current != NULL);                          \
    it->previous = it->current;                                         \
    it->current  = it->current->next;                                   \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _it_equal_p)(const list_it_t it1, const list_it_t it2)	\
  {                                                                     \
    assert(it1 != NULL && it2 != NULL);                                 \
    return it1->current == it2->current;                                \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name, _ref)(const list_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->current != NULL);                          \
    return &(it->current->data);                                        \
  }                                                                     \
  									\
  static inline const type *                                            \
  M_C(name, _cref)(const list_it_t it)					\
  {                                                                     \
    assert(it != NULL && it->current != NULL);                          \
    return M_CONST_CAST(type, &(it->current->data));                    \
  }                                                                     \
  									\
  static inline size_t                                                  \
  M_C(name, _size)(const list_t v)					\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(v);                                        \
    size_t size = 0;                                                    \
    struct M_C(name, _s) *it = v->back;					\
    while (it != NULL) {                                                \
      size ++;                                                          \
      it = it->next;                                                    \
    }                                                                   \
    return size;                                                        \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _insert)(list_t list, list_it_t insertion_point,            \
		     type const x)					\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(list);                                     \
    assert (insertion_point != NULL);                                   \
    struct M_C(name, _s) *next = M_C(name, _int_new)();                 \
    if (M_UNLIKELY (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_C(name, _s)));			\
      return;                                                           \
    }                                                                   \
    M_GET_INIT_SET oplist(next->data, x);                               \
    if (M_UNLIKELY (insertion_point->current == NULL)) {                \
      next->next = list->back;                                          \
      list->back = next;                                                \
      /* update front if list is empty */                               \
      struct M_C(name, _s) *front = list->front;                        \
      front = (front == NULL) ? next : front;                           \
      list->front = front;                                              \
    } else {                                                            \
      next->next = insertion_point->current->next;                      \
      insertion_point->current->next = next;                            \
      /* update front if current == front */                            \
      struct M_C(name, _s) *front = list->front;                        \
      front = (front == insertion_point->current) ? next : front;       \
      list->front = front;                                              \
    }                                                                   \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _remove)(list_t list, list_it_t removing_point)             \
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(list);                                     \
    assert (removing_point != NULL);                                    \
    assert(removing_point->current != NULL);                            \
    struct M_C(name, _s) *next = removing_point->current->next;		\
    struct M_C(name, _s) *previous = removing_point->previous;		\
    if (M_UNLIKELY (previous == NULL)) {                                \
      list->back = next;                                                \
    } else {                                                            \
      previous->next = next;                                            \
    }                                                                   \
    /* Update front  */                                                 \
    struct M_C(name, _s) *front = list->front;                          \
    front = (next == NULL) ? previous : front;                          \
    list->front = front;                                                \
    /* Remove node */                                                   \
    M_GET_CLEAR oplist(removing_point->current->data);                  \
    M_C(name,_int_del) (removing_point->current);			\
    removing_point->current = next;                                     \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _set)(list_t list, const list_t org)			\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(list);                                     \
    LISTI_DUAL_PUSH_CONTRACT(org);                                      \
    struct M_C(name, _s) *next = NULL;                                  \
    struct M_C(name, _s) *it_org;                                       \
    struct M_C(name, _s) **update_list;					\
    if (M_UNLIKELY (list == org)) return;                               \
    M_C(name, _clean)(list);						\
    update_list = &list->back;                                          \
    it_org = org->back;                                                 \
    while (it_org != NULL) {                                            \
      next = M_C(name, _int_new)();					\
      *update_list = next;                                              \
      if (M_UNLIKELY (next == NULL)) {                                  \
        M_MEMORY_FULL(sizeof (struct M_C(name, _s)));			\
        return;                                                         \
      }                                                                 \
      update_list = &next->next;                                        \
      M_GET_INIT_SET oplist(next->data, it_org->data);                  \
      it_org = it_org->next;                                            \
    }                                                                   \
    list->front = next;                                                 \
    *update_list = NULL;                                                \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _init_set)(list_t list, const list_t org)			\
  {                                                                     \
    assert (list != org);                                               \
    M_C(name, _init)(list);						\
    M_C(name, _set)(list, org);						\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _init_move)(list_t list, list_t org)			\
  {                                                                     \
    assert (list != org);                                               \
    list->back  = org->back;                                            \
    list->front = org->front;                                           \
    org->back = NULL;                                                   \
    org->front = NULL;                                                  \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _move)(list_t list, list_t org)				\
  {                                                                     \
    M_C(name, _clear)(list);						\
    M_C(name, _init_move)(list, org);					\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _splice_back)(list_t list1, list_t list2, list_it_t it)     \
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(list1);                                    \
    LISTI_DUAL_PUSH_CONTRACT(list2);                                    \
    assert(it->current != NULL);                                        \
    /* First remove 'it' from list2 */                                  \
    struct M_C(name, _s) *current = it->current;                        \
    struct M_C(name, _s) *next = current->next;                         \
    if (it->previous == NULL) {                                         \
      list2->back = next;                                               \
    } else {                                                            \
      it->previous->next = next;                                        \
    }                                                                   \
    /* Update front  */                                                 \
    struct M_C(name, _s) *front = list2->front;                         \
    front = (next == NULL) ? it->previous : front;                      \
    list2->front = front;                                               \
    /* Update it to next element */                                     \
    it->current = next;                                                 \
    /* Move current in nv */                                            \
    current->next = list1->back;                                        \
    list1->back = current;                                              \
    /* Update front too if the list was empty */                        \
    /* This C code shall generate branchless code */                    \
    front = list1->front;                                               \
    front = (front == NULL) ? current : front;                          \
    list1->front = front;                                               \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _splice)(list_t list1, list_t list2)			\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(list1);                                    \
    LISTI_DUAL_PUSH_CONTRACT(list2);                                    \
    assert (list1 != list2);                                            \
    if (M_LIKELY (list1->front != NULL)) {                              \
      list1->front->next = list2->back;                                 \
      list1->front = list2->front;                                      \
    } else {                                                            \
      /* list1 is empty */                                              \
      list1->back  = list2->back;                                       \
      list1->front = list2->front;                                      \
    }                                                                   \
    list2->back = NULL;                                                 \
    list2->front = NULL;                                                \
    LISTI_DUAL_PUSH_CONTRACT(list1);                                    \
    LISTI_DUAL_PUSH_CONTRACT(list2);                                    \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _reverse)(list_t list)					\
  {                                                                     \
    LISTI_DUAL_PUSH_CONTRACT(list);                                     \
    list->front = list->back;                                           \
    struct M_C(name, _s) *previous = NULL, *it = list->back, *next;     \
    while (it != NULL) {                                                \
      next = it->next;                                                  \
      it->next = previous;                                              \
      previous = it;                                                    \
      it = next;                                                        \
    }                                                                   \
    list->back = previous;                                              \
    LISTI_DUAL_PUSH_CONTRACT(list);                                     \
  }                                                                     \

#endif
