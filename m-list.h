/*
 * MLIB - LIST module
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
#define LIST_DEF(name, ...)                                    \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                  \
  (LISTI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST ),           \
   LISTI_DEF2(name, __VA_ARGS__ ))

/* Define the oplist of a list of type.
   USAGE: LIST_OPLIST(name [, oplist_of_the_type]) */
#define LIST_OPLIST(...)                                             \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                        \
  (LISTI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),                     \
   LISTI_OPLIST(__VA_ARGS__ ))


/********************************** INTERNAL ************************************/

#define LISTI_OPLIST(name, oplist)                                      \
  (INIT(CAT3(list_, name, _init)),                                      \
   INIT_SET(CAT3(list_, name, _init_set)),                              \
   SET(CAT3(list_, name, _set)),                                        \
   CLEAR(CAT3(list_, name, _clear)),                                    \
   MOVE(CAT3(list_, name, _move)),                                      \
   INIT_MOVE(CAT3(list_, name, _init_move)),                            \
   TYPE(CAT3(list_,name,_t)),                                           \
   SUBTYPE(CAT3(list_type_,name,_t)),                                   \
   IT_TYPE(CAT3(list_it_,name,_t)),                                     \
   IT_FIRST(CAT3(list_,name,_it)),                                      \
   IT_SET(CAT3(list_,name,_it_set)),                                    \
   IT_END_P(CAT3(list_,name,_end_p)),                                   \
   IT_EQUAL_P(CAT3(list_,name,_it_equal_p)),                            \
   IT_LAST_P(CAT3(list_,name,_last_p)),                                 \
   IT_NEXT(CAT3(list_,name,_next)),                                     \
   IT_REF(CAT3(list_,name,_ref)),                                       \
   IT_CREF(CAT3(list_,name,_cref)),                                     \
   CLEAN(CAT3(list_,name,_clean)),                                      \
   PUSH(CAT3(list_,name,_push_back)),                                   \
   POP(CAT3(list_,name,_pop_back))                                      \
   ,REVERSE(CAT3(list_,name,_reverse))                                  \
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(CAT3(list_, name, _get_str)),) \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(CAT3(list_, name, _out_str)),) \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(CAT3(list_, name, _in_str)),)    \
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(CAT3(list_, name, _equal_p)),)     \
   )

#define LISTI_DEF2(name, type, oplist)                                  \
  typedef struct CAT3(list_, name, _s) {                                \
    struct CAT3(list_, name, _s) *next;                                 \
    type data;                                                          \
  } *CAT3(list_, name,_t)[1];                                           \
                                                                        \
  typedef struct CAT3(list_, name, _s) **CAT3(list_,name, _ptr);        \
                                                                        \
  typedef const struct CAT3(list_, name, _s) *const*CAT3(list_,name, _srcptr);\
                                                                        \
  typedef type CAT3(list_type_,name, _t);                               \
                                                                        \
  typedef struct CAT3(list_it_, name, _s) {                             \
    struct CAT3(list_, name, _s) *previous;                             \
    CAT3(list_, name,_t)   l;                                           \
  } CAT3(list_it_, name,_t)[1];                                         \
                                                                        \
  typedef union {                                                       \
    type *ptr;                                                          \
    const type *cptr;                                                   \
  } CAT3(list_union_, name,_t);                                         \
                                                                        \
  static inline const type *                                            \
  CAT3(list_, name, _const_cast)(type *ptr)                             \
  {                                                                     \
    CAT3(list_union_, name,_t) u;                                       \
    u.ptr = ptr;                                                        \
    return u.cptr;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _init)(CAT3(list_, name,_t) v)                      \
  {                                                                     \
    assert( v != NULL);                                                 \
    *v = NULL;                                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _clean)(CAT3(list_, name,_t) v)                     \
  {                                                                     \
    assert (v != NULL);                                                 \
    struct CAT3(list_, name, _s) *it = *v;                              \
    while (it != NULL) {                                                \
      struct CAT3(list_, name, _s) *next = it->next;                    \
      M_GET_CLEAR oplist(it->data);                                     \
      M_MEMORY_FREE (it);                                               \
      it = next;                                                        \
    }                                                                   \
    *v = NULL;                                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _clear)(CAT3(list_, name,_t) v)                     \
  {                                                                     \
    CAT3(list_, name, _clean)(v);                                       \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  CAT3(list_, name, _back)(const CAT3(list_, name,_t) v)                \
  {                                                                     \
    assert(v != NULL && *v != NULL);                                    \
    return CAT3(list_, name, _const_cast)(&((*v)->data));               \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(list_, name, _push_raw)(CAT3(list_, name,_t) v)                  \
  {                                                                     \
    assert (v != NULL);                                                 \
    struct CAT3(list_, name, _s) *next;                                 \
    next = M_MEMORY_ALLOC (struct CAT3(list_, name, _s));               \
    if (next == NULL) {                                                 \
      M_MEMORY_FULL(sizeof (struct CAT3(list_, name, _s)));             \
      return NULL;                                                      \
    }                                                                   \
    type *ret = &next->data;                                            \
    next->next = *v;                                                    \
    *v = next;                                                          \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _push_back)(CAT3(list_, name,_t) v, type const x)   \
  {                                                                     \
    type *data = CAT3(list_, name, _push_raw)(v);                       \
    if (data == NULL)                                                   \
      return;                                                           \
    M_GET_INIT_SET oplist(*data, x);                                    \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(list_, name, _push_new)(CAT3(list_, name,_t) v)                  \
  {                                                                     \
    type *data = CAT3(list_, name, _push_raw)(v);                       \
    if (data == NULL)                                                   \
      return NULL;                                                      \
    M_GET_INIT oplist(*data);                                           \
    return data;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _pop_back)(type *data, CAT3(list_, name,_t) v)      \
  {                                                                     \
    assert(v != NULL && *v != NULL);                                    \
    if (data != NULL) {                                                 \
      M_GET_SET oplist(*data, (*v)->data);                              \
    }                                                                   \
    M_GET_CLEAR oplist((*v)->data);                                     \
    struct CAT3(list_, name, _s) *tofree = *v;                          \
    *v = (*v)->next;                                                    \
    M_MEMORY_FREE (tofree);                                             \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(list_, name, _empty_p)(const CAT3(list_, name,_t) v)             \
  {                                                                     \
    assert (v!= NULL);                                                  \
    return *v == NULL;                                                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _swap)(CAT3(list_, name,_t) l,                      \
                           CAT3(list_, name,_t) v)                      \
  {                                                                     \
    assert (v != NULL && l != NULL);                                    \
    struct CAT3(list_, name, _s) *tmp = *l;                             \
    *l = *v;                                                            \
    *v = tmp;                                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _it)(CAT3(list_it_, name,_t) l,                     \
                         const CAT3(list_, name,_t) v)                  \
  {                                                                     \
    assert (v != NULL && l[0].l != NULL);                               \
    l[0].l[0] = *v;                                                     \
    l[0].previous = NULL;                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _it_set)(CAT3(list_it_, name,_t) it1,               \
                             const CAT3(list_it_, name,_t) it2)         \
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    it1[0].l[0]     = it2[0].l[0];                                      \
    it1[0].previous = it2[0].previous;                                  \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(list_, name, _end_p)(const CAT3(list_it_, name,_t) v)            \
  {                                                                     \
    assert (v!= NULL);                                                  \
    return v[0].l[0] == NULL;                                           \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(list_, name, _last_p)(const CAT3(list_it_, name,_t) v)           \
  {                                                                     \
    assert (v!= NULL);                                                  \
    return v[0].l[0] == NULL || v[0].l[0]->next == NULL;                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _next)(CAT3(list_it_, name,_t) tail)                \
  {                                                                     \
    assert(tail != NULL && tail[0].l[0] != NULL);                       \
    tail[0].previous = tail[0].l[0];                                    \
    tail[0].l[0] = tail[0].l[0]->next;                                  \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(list_, name, _it_equal_p)(const CAT3(list_it_, name,_t) it1,     \
                                 const CAT3(list_it_, name,_t) it2)     \
  {                                                                     \
    assert(it1 != NULL && it2 != NULL);                                 \
    return it1[0].l[0] == it2[0].l[0];                                  \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(list_, name, _ref)(CAT3(list_it_, name,_t) v)                    \
  {                                                                     \
    assert(v != NULL && v[0].l[0] != NULL);                             \
    return &(v[0].l[0]->data);                                          \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  CAT3(list_, name, _cref)(const CAT3(list_it_, name,_t) v)             \
  {                                                                     \
    assert(v != NULL && v[0].l[0] != NULL);                             \
    return CAT3(list_, name, _const_cast)(&(v[0].l[0]->data));          \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  CAT3(list_, name, _size)(const CAT3(list_, name,_t) list)             \
  {                                                                     \
    assert (list != NULL);                                              \
    size_t size = 0;                                                    \
    /* FIXME: Store size field within the list to get faster? */        \
    struct CAT3(list_, name, _s) *it = *list;                           \
    while (it != NULL) {                                                \
      size ++;                                                          \
      it = it->next;                                                    \
    }                                                                   \
    return size;                                                        \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  CAT3(list_, name, _sublist_p)(const CAT3(list_, name,_t) list,        \
                                const CAT3(list_it_, name,_t) sublist)  \
  {                                                                     \
    assert (list != NULL && sublist != NULL);                           \
    struct CAT3(list_, name, _s) *it = *list;                           \
    while (it != NULL) {                                                \
      if (it == sublist[0].l[0]) return true;                           \
      it = it->next;                                                    \
    }                                                                   \
    return (sublist[0].l[0] == NULL);                                   \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  CAT3(list_, name, _get)(const CAT3(list_, name,_t) list, size_t i)    \
  {                                                                     \
    assert (list != NULL);                                              \
    struct CAT3(list_, name, _s) *it = *list;                           \
    /* FIXME: How to avoid the double iteration over the list? */       \
    size_t len = CAT3(list_,name,_size)(list);                          \
    assert (i < len);                                          \
    size_t j = len-1;                                                   \
    while (it != NULL) {                                                \
      if (i == j) return &it->data;                                     \
      it = it->next;                                                    \
      j--;                                                              \
    }                                                                   \
    assert(false);                                                      \
    return NULL; /* Can not be reached */                               \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  CAT3(list_, name, _cget)(const CAT3(list_, name,_t) l, size_t i)      \
  {                                                                     \
    return CAT3(list_, name, _const_cast)(CAT3(list_, name, _get)(l,i)); \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _insert)(CAT3(list_, name,_t) list,                 \
                             CAT3(list_it_, name,_t) insertion_point,   \
                             type const x)                                    \
  {                                                                     \
    assert (list != NULL && insertion_point != NULL);                   \
    assert(CAT3(list_, name, _sublist_p)(list, insertion_point));       \
    struct CAT3(list_, name, _s) *next;                                 \
    next = M_MEMORY_ALLOC (struct CAT3(list_, name, _s));               \
    if (next == NULL) {                                                 \
      M_MEMORY_FULL(sizeof (struct CAT3(list_, name, _s)));             \
      return;                                                           \
    }                                                                   \
    M_GET_INIT_SET oplist(next->data, x);                               \
    /* FIXME: How to set an iterator to NULL? */                        \
    if (insertion_point[0].l[0] == NULL) {                              \
      next->next = *list;                                               \
      *list = next;                                                     \
    } else {                                                            \
      next->next = insertion_point[0].l[0]->next;                       \
      insertion_point[0].l[0]->next = next;                             \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _remove)(CAT3(list_, name,_t) list,                 \
                             CAT3(list_it_, name,_t) removing_point)    \
  {                                                                     \
    assert (list != NULL && removing_point != NULL);                    \
    assert(removing_point[0].l[0] != NULL);                             \
    assert(CAT3(list_, name, _sublist_p)(list, removing_point));        \
    struct CAT3(list_, name, _s) *next = removing_point[0].l[0]->next;  \
    if (removing_point->previous == NULL) {                             \
      *list = next;                                                     \
    } else {                                                            \
      removing_point->previous->next = next;                            \
    }                                                                   \
    M_GET_CLEAR oplist(removing_point[0].l[0]->data);                   \
    M_MEMORY_FREE (removing_point[0].l[0]);                             \
    removing_point[0].l[0] = next;                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _move_back)(CAT3(list_, name,_t) nv,                \
                                CAT3(list_, name,_t) ov,                \
                                CAT3(list_it_, name,_t) it)             \
  {                                                                     \
    assert (nv != NULL && ov != NULL && it != NULL && nv != ov);        \
    assert(it[0].l[0] != NULL);                                         \
    assert(CAT3(list_, name, _sublist_p)(ov, it));                      \
    /* Remove it from ov */                                             \
    struct CAT3(list_, name, _s) *next = it[0].l[0]->next;              \
    struct CAT3(list_, name, _s) *current = it[0].l[0];                 \
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
  CAT3(list_, name, _set)(CAT3(list_, name,_t) list,                    \
                          const CAT3(list_, name,_t) org)               \
  {                                                                     \
    assert (list != NULL && org != NULL);                               \
    struct CAT3(list_, name, _s) *next, *it_org;                        \
    struct CAT3(list_, name, _s) **update_list;                         \
    if (list == org) return;                                            \
    /* TODO: Reuse memory of list */                                    \
    CAT3(list_, name, _clean)(list);                                    \
    update_list = list;                                                 \
    it_org = *org;                                                      \
    while (it_org != NULL) {                                            \
      next = M_MEMORY_ALLOC (struct CAT3(list_, name, _s));             \
      *update_list = next;                                              \
      if (next == NULL) {                                               \
        M_MEMORY_FULL(sizeof (struct CAT3(list_, name, _s)));           \
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
  CAT3(list_, name, _init_set)(CAT3(list_, name,_t) list,               \
                               const CAT3(list_, name,_t) org)          \
  {                                                                     \
    assert (list != NULL && org != NULL && list != org);                \
    CAT3(list_, name, _init)(list);                                     \
    CAT3(list_, name, _set)(list, org);                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _init_move)(CAT3(list_, name,_t) list,              \
                                CAT3(list_, name,_t) org)               \
  {                                                                     \
    assert (list != NULL && org != NULL && list != org);                \
    *list = *org;                                                       \
    *org = NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _move)(CAT3(list_, name,_t) list,                   \
                           CAT3(list_, name,_t) org)                    \
  {                                                                     \
    assert (list != NULL && org != NULL && list != org);                \
    CAT3(list_, name, _clear)(list);                                    \
    CAT3(list_, name, _init_move)(list, org);                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _slice)(CAT3(list_, name,_t) list1,                 \
                            CAT3(list_, name,_t) list2)                 \
  {                                                                     \
    assert (list1 != NULL && list2 != NULL && list1 != list2);          \
    struct CAT3(list_, name, _s) **update_list = list1;                 \
    struct CAT3(list_, name, _s) *it = *list1;                          \
    while (it != NULL) {                                                \
      update_list = &it->next;                                          \
     it = it->next;                                                     \
    }                                                                   \
    *update_list = *list2;                                              \
    *list2 = NULL;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(list_, name, _reverse)(CAT3(list_, name,_t) list)                \
  {                                                                     \
    assert (list != NULL);                                              \
    struct CAT3(list_, name, _s) *previous = NULL, *it = *list, *next;  \
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
  CAT3(list_, name, _get_str)(string_t str, const CAT3(list_, name,_t) list,  \
                              bool append)                              \
  {                                                                     \
    assert (str != NULL && list != NULL);                               \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    CAT3(list_it_, name, _t) it;                                        \
    for (CAT3(list_, name, _it)(it, list) ;                             \
         !CAT3(list_, name, _end_p)(it);                                \
         CAT3(list_, name, _next)(it)){                                 \
      const type *item = CAT3(list_, name, _cref)(it);                  \
      M_GET_GET_STR oplist (str, *item, true);                          \
      if (!CAT3(list_, name, _last_p)(it))                              \
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    string_push_back (str, ']');                                        \
  }                                                                     \
  , /* no str */ )                                                      \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  CAT3(list_, name, _out_str)(FILE *file, const CAT3(list_, name,_t) list) \
  {                                                                     \
    assert (file != NULL && list != NULL);                              \
    fputc ('[', file);                                                  \
    CAT3(list_it_, name, _t) it;                                        \
    for (CAT3(list_, name, _it)(it, list) ;                             \
         !CAT3(list_, name, _end_p)(it);                                \
         CAT3(list_, name, _next)(it)){                                 \
      const type *item = CAT3(list_, name, _cref)(it);                  \
      M_GET_OUT_STR oplist (file, *item);                               \
      if (!CAT3(list_, name, _last_p)(it))                              \
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fputc (']', file);                                                  \
  }                                                                     \
  , /* no out_str */ )                                                  \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  CAT3(list_, name, _in_str)(CAT3(list_, name,_t) list, FILE *file)     \
  {                                                                     \
    assert (file != NULL && list != NULL);                              \
    CAT3(list_, name,_clean)(list);                                     \
    char c = fgetc(file);                                               \
    if (c != '[') return false;                                         \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_IN_STR oplist (item, file);                        \
      if (!b) { c = 0; break; }                                         \
      CAT3(list_, name, _push_back)(list, item);                        \
      c = fgetc(file);                                                  \
    } while (c == M_GET_SEPARATOR oplist && !feof(file) && !ferror(file)); \
    M_GET_CLEAR oplist (item);                                          \
    CAT3(list_, name, _reverse)(list);                                  \
    return c == ']';                                                    \
  }                                                                     \
  , /* no in_str */ )                                                   \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool                                                    \
  CAT3(list_, name, _equal_p)(const CAT3(list_, name,_t) list1,         \
                              const CAT3(list_, name,_t) list2)         \
  {                                                                     \
    assert (list1 != NULL && list2 != NULL);                            \
    CAT3(list_it_, name, _t) it1;                                       \
    CAT3(list_it_, name, _t) it2;                                       \
    if (list1 == list2) return true;                                    \
    CAT3(list_, name, _it)(it1, list1);                                 \
    CAT3(list_, name, _it)(it2, list2);                                 \
    while (!CAT3(list_, name, _end_p)(it1)                              \
           &&!CAT3(list_, name, _end_p)(it2)) {                         \
      const type *item1 = CAT3(list_, name, _cref)(it1);                \
      const type *item2 = CAT3(list_, name, _cref)(it2);                \
      bool b = M_GET_EQUAL oplist (*item1, *item2);                     \
      if (!b) return false;                                             \
      CAT3(list_, name, _next)(it1);                                    \
      CAT3(list_, name, _next)(it2);                                    \
    }                                                                   \
    return CAT3(list_, name, _end_p)(it1)                               \
      && CAT3(list_, name, _end_p)(it2);                                \
  }                                                                     \
  , /* no equal */ )                                                    \
                                                                        \
                          
// TODO: Conditional HASH if HASH is present

#endif
