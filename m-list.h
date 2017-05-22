/*
 * M*LIB - LIST module
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
#ifndef __M_LIST_H
#define __M_LIST_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "m-core.h"

/* Define a list of a given type.
   USAGE: LIST_DEF(name, type [, oplist_of_the_type]) */
#define LIST_DEF(name, ...)                                             \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (LISTI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST, M_C3(list_, name,_t), M_C3(list_it_, name,_t) ), \
   LISTI_DEF2(name, __VA_ARGS__, M_C3(list_, name,_t), M_C3(list_it_, name,_t) ))

/* Define the oplist of a list of type.
   USAGE: LIST_OPLIST(name [, oplist_of_the_type]) */
#define LIST_OPLIST(...)                                             \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                        \
  (LISTI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),                     \
   LISTI_OPLIST(__VA_ARGS__ ))


/********************************** INTERNAL ************************************/

#define LISTI_OPLIST(name, oplist)                                      \
  (INIT(M_C3(list_, name, _init)),                                      \
   INIT_SET(M_C3(list_, name, _init_set)),                              \
   SET(M_C3(list_, name, _set)),                                        \
   CLEAR(M_C3(list_, name, _clear)),                                    \
   MOVE(M_C3(list_, name, _move)),                                      \
   INIT_MOVE(M_C3(list_, name, _init_move)),                            \
   SWAP(M_C3(list_, name, _swap)),                                      \
   TYPE(M_C3(list_,name,_t)),                                           \
   SUBTYPE(M_C3(list_type_,name,_t)),                                   \
   IT_TYPE(M_C3(list_it_,name,_t)),                                     \
   IT_FIRST(M_C3(list_,name,_it)),                                      \
   IT_END(M_C3(list_,name,_it_end)),                                    \
   IT_SET(M_C3(list_,name,_it_set)),                                    \
   IT_END_P(M_C3(list_,name,_end_p)),                                   \
   IT_EQUAL_P(M_C3(list_,name,_it_equal_p)),                            \
   IT_LAST_P(M_C3(list_,name,_last_p)),                                 \
   IT_NEXT(M_C3(list_,name,_next)),                                     \
   IT_REF(M_C3(list_,name,_ref)),                                       \
   IT_CREF(M_C3(list_,name,_cref)),                                     \
   IT_REMOVE(M_C3(list_,name,_remove)),                                 \
   CLEAN(M_C3(list_,name,_clean)),                                      \
   PUSH(M_C3(list_,name,_push_back)),                                   \
   POP(M_C3(list_,name,_pop_back))                                      \
   ,REVERSE(M_C3(list_,name,_reverse))                                  \
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C3(list_, name, _get_str)),) \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C3(list_, name, _out_str)),) \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C3(list_, name, _in_str)),)    \
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C3(list_, name, _equal_p)),)     \
   ,M_IF_METHOD(HASH, oplist)(HASH(M_C3(list_, name, _hash)),)          \
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

#define LISTI_DEF2(name, type, oplist, list_t, list_it_t)               \
  typedef struct M_C3(list_, name, _s) {                                \
    struct M_C3(list_, name, _s) *next;                                 \
    type data;                                                          \
  } *list_t[1];                                                         \
                                                                        \
  typedef type M_C3(list_type_,name, _t);                               \
                                                                        \
  typedef struct M_C3(list_it_, name, _s) {                             \
    struct M_C3(list_, name, _s) *previous;                             \
    list_t   l;                                                         \
  } list_it_t[1];                                                       \
                                                                        \
  typedef union {                                                       \
    type *ptr;                                                          \
    const type *cptr;                                                   \
  } M_C3(list_union_, name,_t);                                         \
                                                                        \
  /* If MEMPOOL is required, we need to define it */                    \
  M_IF_METHOD(MEMPOOL, oplist)(                                         \
    MEMPOOL_DEF(M_C(list_,name), struct M_C3(list_, name, _s))          \
    M_GET_MEMPOOL_LINKAGE oplist M_C3(mempool_list_, name, _t) M_GET_MEMPOOL oplist; \
    static inline struct M_C3(list_, name, _s) *M_C3(listi_,name,_new)(void) { \
      return M_C3(mempool_list_, name, _alloc)(M_GET_MEMPOOL oplist);   \
    }                                                                   \
    static inline void M_C3(listi_,name,_del)(struct M_C3(list_, name, _s) *ptr) { \
      M_C3(mempool_list_, name, _free)(M_GET_MEMPOOL oplist, ptr);      \
    }                                                                   \
    LISTI_DEF3(name, type, oplist, list_t, list_it_t)                   \
    , /* No mempool allocation */                                       \
    static inline struct M_C3(list_, name, _s) *M_C3(listi_,name,_new)(void) { \
      return M_GET_NEW oplist (struct M_C3(list_, name, _s));           \
    }                                                                   \
    static inline void M_C3(listi_,name,_del)(struct M_C3(list_, name, _s) *ptr) { \
      M_GET_DEL oplist (ptr);                                           \
    }                                                                   \
    LISTI_DEF3(name, type, oplist, list_t, list_it_t) )


#define LISTI_DEF3(name, type, oplist, list_t, list_it_t)               \
  static inline const type *                                            \
  M_C3(list_, name, _const_cast)(type *ptr)                             \
  {                                                                     \
    M_C3(list_union_, name,_t) u;                                       \
    u.ptr = ptr;                                                        \
    return u.cptr;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _init)(list_t v)                                    \
  {                                                                     \
    assert( v != NULL);                                                 \
    *v = NULL;                                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _clean)(list_t v)                                   \
  {                                                                     \
    assert (v != NULL);                                                 \
    struct M_C3(list_, name, _s) *it = *v;                              \
    *v = NULL;                                                          \
    while (it != NULL) {                                                \
      struct M_C3(list_, name, _s) *next = it->next;                    \
      M_GET_CLEAR oplist(it->data);                                     \
      M_C3(listi_,name,_del)(it);                                       \
      it = next;                                                        \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _clear)(list_t v)                                   \
  {                                                                     \
    M_C3(list_, name, _clean)(v);                                       \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(list_, name, _back)(const list_t v)                              \
  {                                                                     \
    assert(v != NULL && *v != NULL);                                    \
    return M_C3(list_, name, _const_cast)(&((*v)->data));               \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(list_, name, _push_raw)(list_t v)                                \
  {                                                                     \
    assert (v != NULL);                                                 \
    struct M_C3(list_, name, _s) *next;                                 \
    next = M_C3(listi_,name,_new)();                                    \
    if (M_UNLIKELY (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_C3(list_, name, _s)));             \
      return NULL;                                                      \
    }                                                                   \
    type *ret = &next->data;                                            \
    next->next = *v;                                                    \
    *v = next;                                                          \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _push_back)(list_t v, type const x)                 \
  {                                                                     \
    type *data = M_C3(list_, name, _push_raw)(v);                       \
    if (M_UNLIKELY (data == NULL))                                      \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                    \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(list_, name, _push_new)(list_t v)                                \
  {                                                                     \
    type *data = M_C3(list_, name, _push_raw)(v);                       \
    if (M_UNLIKELY (data == NULL))                                      \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                           \
    return data;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _pop_back)(type *data, list_t v)                    \
  {                                                                     \
    assert(v != NULL && *v != NULL);                                    \
    if (data != NULL) {                                                 \
      M_GET_SET oplist(*data, (*v)->data);                              \
    }                                                                   \
    M_GET_CLEAR oplist((*v)->data);                                     \
    struct M_C3(list_, name, _s) *tofree = *v;                          \
    *v = (*v)->next;                                                    \
    M_C3(listi_,name,_del)(tofree);                                     \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(list_, name, _empty_p)(const list_t v)                           \
  {                                                                     \
    assert (v!= NULL);                                                  \
    return *v == NULL;                                                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _swap)(list_t l, list_t v)                          \
  {                                                                     \
    assert (v != NULL && l != NULL);                                    \
    struct M_C3(list_, name, _s) *tmp = *l;                             \
    *l = *v;                                                            \
    *v = tmp;                                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _it)(list_it_t l, const list_t v)                   \
  {                                                                     \
    assert (v != NULL && l[0].l != NULL);                               \
    l[0].l[0] = *v;                                                     \
    l[0].previous = NULL;                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _it_set)(list_it_t it1, const list_it_t it2)        \
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    it1[0].l[0]     = it2[0].l[0];                                      \
    it1[0].previous = it2[0].previous;                                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _it_end)(list_it_t it1, const list_t v)             \
  {                                                                     \
    assert (it1 != NULL && v != NULL);                                  \
    (void)v; /* unused */                                               \
    it1[0].l[0]     = NULL;                                             \
    it1[0].previous = NULL;                                             \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(list_, name, _end_p)(const list_it_t v)                          \
  {                                                                     \
    assert (v!= NULL);                                                  \
    return v[0].l[0] == NULL;                                           \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(list_, name, _last_p)(const list_it_t v)                         \
  {                                                                     \
    assert (v!= NULL);                                                  \
    return v[0].l[0] == NULL || v[0].l[0]->next == NULL;                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _next)(list_it_t tail)                              \
  {                                                                     \
    assert(tail != NULL && tail[0].l[0] != NULL);                       \
    tail[0].previous = tail[0].l[0];                                    \
    tail[0].l[0] = tail[0].l[0]->next;                                  \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(list_, name, _it_equal_p)(const list_it_t it1, const list_it_t it2) \
  {                                                                     \
    assert(it1 != NULL && it2 != NULL);                                 \
    return it1[0].l[0] == it2[0].l[0];                                  \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(list_, name, _ref)(list_it_t v)                                  \
  {                                                                     \
    assert(v != NULL && v[0].l[0] != NULL);                             \
    return &(v[0].l[0]->data);                                          \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(list_, name, _cref)(const list_it_t v)                           \
  {                                                                     \
    assert(v != NULL && v[0].l[0] != NULL);                             \
    return M_C3(list_, name, _const_cast)(&(v[0].l[0]->data));          \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C3(list_, name, _size)(const list_t list)                           \
  {                                                                     \
    assert (list != NULL);                                              \
    size_t size = 0;                                                    \
    /* FIXME: Store size field within the list to get faster? */        \
    struct M_C3(list_, name, _s) *it = *list;                           \
    while (it != NULL) {                                                \
      size ++;                                                          \
      it = it->next;                                                    \
    }                                                                   \
    return size;                                                        \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(list_, name, _sublist_p)(const list_t list,                      \
                                const list_it_t sublist)                \
  {                                                                     \
    assert (list != NULL && sublist != NULL);                           \
    struct M_C3(list_, name, _s) *it = *list;                           \
    while (it != NULL) {                                                \
      if (it == sublist[0].l[0]) return true;                           \
      it = it->next;                                                    \
    }                                                                   \
    return (sublist[0].l[0] == NULL);                                   \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(list_, name, _get)(const list_t list, size_t i)                  \
  {                                                                     \
    assert (list != NULL);                                              \
    struct M_C3(list_, name, _s) *it = *list;                           \
    /* FIXME: How to avoid the double iteration over the list? */       \
    size_t len = M_C3(list_,name,_size)(list);                          \
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
  M_C3(list_, name, _cget)(const list_t l, size_t i)                    \
  {                                                                     \
    return M_C3(list_, name, _const_cast)(M_C3(list_, name, _get)(l,i)); \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _insert)(list_t list,                               \
                             list_it_t insertion_point,                 \
                             type const x)                              \
  {                                                                     \
    assert (list != NULL && insertion_point != NULL);                   \
    assert(M_C3(list_, name, _sublist_p)(list, insertion_point));       \
    struct M_C3(list_, name, _s) *next;                                 \
    next = M_C3(listi_,name,_new)();                                    \
    if (M_UNLIKELY (next == NULL)) {                                    \
      M_MEMORY_FULL(sizeof (struct M_C3(list_, name, _s)));             \
      return;                                                           \
    }                                                                   \
    M_GET_INIT_SET oplist(next->data, x);                               \
    if (M_UNLIKELY (insertion_point[0].l[0] == NULL)) {                 \
      next->next = *list;                                               \
      *list = next;                                                     \
    } else {                                                            \
      next->next = insertion_point[0].l[0]->next;                       \
      insertion_point[0].l[0]->next = next;                             \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _remove)(list_t list,                               \
                             list_it_t removing_point)                  \
  {                                                                     \
    assert (list != NULL && removing_point != NULL);                    \
    assert(removing_point[0].l[0] != NULL);                             \
    assert(M_C3(list_, name, _sublist_p)(list, removing_point));        \
    struct M_C3(list_, name, _s) *next = removing_point[0].l[0]->next;  \
    if (M_UNLIKELY (removing_point->previous == NULL)) {                \
      *list = next;                                                     \
    } else {                                                            \
      removing_point->previous->next = next;                            \
    }                                                                   \
    M_GET_CLEAR oplist(removing_point[0].l[0]->data);                   \
    M_C3(listi_,name,_del) (removing_point[0].l[0]);                    \
    removing_point[0].l[0] = next;                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _move_back)(list_t nv, list_t ov, list_it_t it)     \
  {                                                                     \
    assert (nv != NULL && ov != NULL && it != NULL && nv != ov);        \
    assert(it[0].l[0] != NULL);                                         \
    assert(M_C3(list_, name, _sublist_p)(ov, it));                      \
    /* Remove it from ov */                                             \
    struct M_C3(list_, name, _s) *next = it[0].l[0]->next;              \
    struct M_C3(list_, name, _s) *current = it[0].l[0];                 \
    if (it->previous == NULL) {                                         \
      *ov = next;                                                       \
    } else {                                                            \
      it->previous->next = next;                                        \
    }                                                                   \
    /* Update it to next element */                                     \
    it[0].l[0] = next;                                                  \
    /* Move current in nv */                                            \
    current->next = *nv;                                                \
    *nv = current;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _set)(list_t list, const list_t org)                \
  {                                                                     \
    assert (list != NULL && org != NULL);                               \
    struct M_C3(list_, name, _s) *next, *it_org;                        \
    struct M_C3(list_, name, _s) **update_list;                         \
    if (list == org) return;                                            \
    M_C3(list_, name, _clean)(list);                                    \
    update_list = list;                                                 \
    it_org = *org;                                                      \
    while (it_org != NULL) {                                            \
      next = M_C3(listi_,name,_new)();                                  \
      *update_list = next;                                              \
      if (M_UNLIKELY (next == NULL)) {                                  \
        M_MEMORY_FULL(sizeof (struct M_C3(list_, name, _s)));           \
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
  M_C3(list_, name, _init_set)(list_t list, const list_t org)           \
  {                                                                     \
    assert (list != NULL && org != NULL && list != org);                \
    M_C3(list_, name, _init)(list);                                     \
    M_C3(list_, name, _set)(list, org);                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _init_move)(list_t list, list_t org)                \
  {                                                                     \
    assert (list != NULL && org != NULL && list != org);                \
    *list = *org;                                                       \
    *org = NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _move)(list_t list, list_t org)                     \
  {                                                                     \
    assert (list != NULL && org != NULL && list != org);                \
    M_C3(list_, name, _clear)(list);                                    \
    M_C3(list_, name, _init_move)(list, org);                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _slice)(list_t list1, list_t list2)                 \
  {                                                                     \
    assert (list1 != NULL && list2 != NULL && list1 != list2);          \
    struct M_C3(list_, name, _s) **update_list = list1;                 \
    struct M_C3(list_, name, _s) *it = *list1;                          \
    while (it != NULL) {                                                \
      update_list = &it->next;                                          \
     it = it->next;                                                     \
    }                                                                   \
    *update_list = *list2;                                              \
    *list2 = NULL;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(list_, name, _reverse)(list_t list)                              \
  {                                                                     \
    assert (list != NULL);                                              \
    struct M_C3(list_, name, _s) *previous = NULL, *it = *list, *next;  \
    while (it != NULL) {                                                \
      next = it->next;                                                  \
      it->next = previous;                                              \
      previous = it;                                                    \
      it = next;                                                        \
    }                                                                   \
    *list = previous;                                                   \
  }                                                                     \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C3(list_, name, _get_str)(string_t str, const list_t list,          \
                              bool append)                              \
  {                                                                     \
    assert (str != NULL && list != NULL);                               \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    M_C3(list_it_, name, _t) it;                                        \
    for (M_C3(list_, name, _it)(it, list) ;                             \
         !M_C3(list_, name, _end_p)(it);                                \
         M_C3(list_, name, _next)(it)){                                 \
      const type *item = M_C3(list_, name, _cref)(it);                  \
      M_GET_GET_STR oplist (str, *item, true);                          \
      if (!M_C3(list_, name, _last_p)(it))                              \
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    string_push_back (str, ']');                                        \
  }                                                                     \
  , /* no str */ )                                                      \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C3(list_, name, _out_str)(FILE *file, const list_t list)            \
  {                                                                     \
    assert (file != NULL && list != NULL);                              \
    fputc ('[', file);                                                  \
    M_C3(list_it_, name, _t) it;                                        \
    for (M_C3(list_, name, _it)(it, list) ;                             \
         !M_C3(list_, name, _end_p)(it);                                \
         M_C3(list_, name, _next)(it)){                                 \
      const type *item = M_C3(list_, name, _cref)(it);                  \
      M_GET_OUT_STR oplist (file, *item);                               \
      if (!M_C3(list_, name, _last_p)(it))                              \
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fputc (']', file);                                                  \
  }                                                                     \
  , /* no out_str */ )                                                  \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C3(list_, name, _in_str)(list_t list, FILE *file)                   \
  {                                                                     \
    assert (file != NULL && list != NULL);                              \
    M_C3(list_, name,_clean)(list);                                     \
    char c = fgetc(file);                                               \
    if (c != '[') return false;                                         \
    c = fgetc(file);                                                    \
    if (c == ']') return true;                                          \
    ungetc(c, file);                                                    \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_IN_STR oplist (item, file);                        \
      c = fgetc(file);                                                  \
      if (!b) { break; }                                                \
      M_C3(list_, name, _push_back)(list, item);                        \
    } while (c == M_GET_SEPARATOR oplist && !feof(file) && !ferror(file)); \
    M_GET_CLEAR oplist (item);                                          \
    M_C3(list_, name, _reverse)(list);                                  \
    return c == ']';                                                    \
  }                                                                     \
  , /* no in_str */ )                                                   \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  M_C3(list_, name, _equal_p)(const list_t list1, const list_t list2)   \
  {                                                                     \
    assert (list1 != NULL && list2 != NULL);                            \
    M_C3(list_it_, name, _t) it1;                                       \
    M_C3(list_it_, name, _t) it2;                                       \
    if (list1 == list2) return true;                                    \
    M_C3(list_, name, _it)(it1, list1);                                 \
    M_C3(list_, name, _it)(it2, list2);                                 \
    while (!M_C3(list_, name, _end_p)(it1)                              \
           &&!M_C3(list_, name, _end_p)(it2)) {                         \
      const type *item1 = M_C3(list_, name, _cref)(it1);                \
      const type *item2 = M_C3(list_, name, _cref)(it2);                \
      bool b = M_GET_EQUAL oplist (*item1, *item2);                     \
      if (!b) return false;                                             \
      M_C3(list_, name, _next)(it1);                                    \
      M_C3(list_, name, _next)(it2);                                    \
    }                                                                   \
    return M_C3(list_, name, _end_p)(it1)                               \
      && M_C3(list_, name, _end_p)(it2);                                \
  }                                                                     \
  , /* no equal */ )                                                    \
                                                                        \
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t                                                  \
  M_C3(list_, name, _hash)(const list_t list)                           \
  {                                                                     \
    assert (list != NULL);                                              \
    M_HASH_DECL(hash);                                                  \
    M_C3(list_it_, name, _t) it;                                        \
    for(M_C3(list_, name, _it)(it, list) ;                              \
        !M_C3(list_, name, _end_p)(it);                                 \
        M_C3(list_, name, _next)(it)) {                                 \
      const type *item = M_C3(list_, name, _cref)(it);                  \
      size_t hi = M_GET_HASH oplist (*item);                            \
      M_HASH_UP(hash, hi);                                              \
    }                                                                   \
    return hash;                                                        \
  }                                                                     \
  , /* no hash */ )                                                     \
                                                                        \
                          
#endif
