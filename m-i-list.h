/*
 * M*LIB - Intrusive List module
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
#ifndef __M_I_LIST_H
#define __M_I_LIST_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "m-core.h"

/* Interface to add to a structure to allow intrusive support.
   name: name of the intrusive list.
   type: name of the type of the structure (aka. struct test_s) - not used currently
*/
#define ILIST_INTERFACE(name, type)             \
  struct ilist_head_s name

/* Define a list of a given type.
   LIST_DEF(name, type [, oplist_of_the_type]) */
#define ILIST_DEF(name, ...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ILISTI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST ),                   \
   ILISTI_DEF2(name, __VA_ARGS__ ))

/* Define the oplist of a ilist of type.
   USAGE: LIST_OPLIST(name [, oplist_of_the_type]) */
#define ILIST_OPLIST(...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                        \
  (ILISTI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),                    \
   ILISTI_OPLIST(__VA_ARGS__ ))


/********************************** INTERNAL ************************************/

/* Define the structure to be added in all objects. */
typedef struct ilist_head_s {
  struct ilist_head_s *next;
  struct ilist_head_s *prev;
} ilist_head_t;

/* Define the oplist of an ilist of type */
#define ILISTI_OPLIST(name,oplist)                                      \
  (INIT(M_C3(ilist_, name, _init)),                                     \
   CLEAR(M_C3(ilist_, name, _clear)),                                   \
   TYPE(M_C3(ilist_,name,_t)),                                          \
   SUBTYPE(M_C3(ilist_type_,name,_t)),                                  \
   IT_TYPE(M_C3(ilist_it_,name,_t)),                                    \
   IT_FIRST(M_C3(ilist_,name,_it)),                                     \
   IT_SET(M_C3(ilist_,name,_it_set)),                                   \
   IT_LAST(M_C3(ilist_,name,_it_last)),                                 \
   IT_END(M_C3(ilist_,name,_it_end)),                                   \
   IT_END_P(M_C3(ilist_,name,_end_p)),                                  \
   IT_LAST_P(M_C3(ilist_,name,_last_p)),                                \
   IT_EQUAL_P(M_C3(ilist_,name,_it_equal_p)),                           \
   IT_NEXT(M_C3(ilist_,name,_next)),                                    \
   IT_PREVIOUS(M_C3(ilist_,name,_previous)),                            \
   IT_REF(M_C3(ilist_,name,_ref)),                                      \
   IT_CREF(M_C3(ilist_,name,_cref)),                                    \
   OPLIST(oplist),                                                      \
   PUSH(M_C3(ilist_,name,_push_back)),                                  \
   POP(M_C3(ilist_,name,_pop_back))                                     \
   )

/*
 * From a pointer to a 'field_type' 'field' of a 'type'structure,
 * return pointer to the structure.
 */
#define ILISTI_TYPE_FROM_FIELD(type, ptr, field_type, field)    \
  ((type *)(void*)( (char *)M_ASSIGN_CAST(field_type*, (ptr)) - offsetof(type, field) ))

#define ILISTI_DEF2(name, type, oplist)                                 \
  typedef struct M_C3(ilist_, name, _s) {                               \
    struct ilist_head_s name;                                           \
  } M_C3(ilist_, name, _t)[1];                                          \
                                                                        \
  typedef type M_C3(ilist_type_,name, _t);                              \
                                                                        \
  typedef struct M_C3(ilist_it_, name, _s) {                            \
    struct ilist_head_s *head;                                          \
    struct ilist_head_s *previous;                                      \
    struct ilist_head_s *current;                                       \
    struct ilist_head_s *next;                                          \
  } M_C3(ilist_it_, name, _t)[1];                                       \
                                                                        \
  typedef union {                                                       \
    type *ptr;                                                          \
    const type *cptr;                                                   \
  } M_C3(ilist_union_, name,_t);                                        \
                                                                        \
  static inline const type *                                            \
  M_C3(ilist_, name, _const_cast)(type *ptr)                            \
  {                                                                     \
    M_C3(ilist_union_, name,_t) u;                                      \
    u.ptr = ptr;                                                        \
    return u.cptr;                                                      \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _init)(M_C3(ilist_, name, _t) list) \
  {                                                                     \
    assert (list != NULL);                                              \
    list->name.next = &list->name;                                      \
    list->name.prev = &list->name;                                      \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _clean)(M_C3(ilist_, name, _t) list) \
  {                                                                     \
    assert (list != NULL);                                              \
    list->name.next = &list->name;                                      \
    list->name.prev = &list->name;                                      \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _clear)(M_C3(ilist_, name, _t) list) \
  {                                                                     \
    M_C3(ilist_, name, _clean)(list);                                   \
  }                                                                     \
                                                                        \
  static inline bool M_C3(ilist_, name, _empty_p)(const M_C3(ilist_, name, _t) list) \
  {                                                                     \
    return list->name.next == &list->name;                              \
  }                                                                     \
                                                                        \
  static inline size_t M_C3(ilist_, name, _size)(const M_C3(ilist_, name, _t) list) \
  {                                                                     \
    size_t s = 0;                                                       \
    for(const struct ilist_head_s *it = list->name.next ;               \
        it != &list->name; it = it->next)                               \
      s++;                                                              \
    return s;                                                           \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _push_back)(M_C3(ilist_, name, _t) list, \
                                                    type *obj)          \
  {                                                                     \
    assert (list != NULL && obj != NULL);                               \
    struct ilist_head_s *prev = list->name.prev;                        \
    list->name.prev = &obj->name;                                       \
    obj->name.prev = prev;                                              \
    obj->name.next = &list->name;                                       \
    prev->next = &obj->name;                                            \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _push_front)(M_C3(ilist_, name, _t) list, \
                                                     type *obj)         \
  {                                                                     \
    assert (list != NULL && obj != NULL);                               \
    struct ilist_head_s *next = list->name.next;                        \
    list->name.next = &obj->name;                                       \
    obj->name.next = next;                                              \
    obj->name.prev = &list->name;                                       \
    next->prev = &obj->name;                                            \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _push_after)(type *obj_it,      \
                                                     type *obj_ins)     \
  {                                                                     \
    assert (obj_it != NULL && obj_ins != NULL);                         \
    struct ilist_head_s *next = obj_it->name.next;                      \
    obj_it->name.next = &obj_ins->name;                                 \
    obj_ins->name.next = next;                                          \
    obj_ins->name.prev = &obj_it->name;                                 \
    next->prev = &obj_ins->name;                                        \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _init_field)(type *obj)         \
  {                                                                     \
    assert (obj != NULL);                                               \
    obj->name.next = NULL;                                              \
    obj->name.prev = NULL;                                              \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _unlink)(type *obj)             \
  {                                                                     \
    assert (obj != NULL);                                               \
    struct ilist_head_s *next = obj->name.next;                         \
    struct ilist_head_s *prev = obj->name.prev;                         \
    next->prev = prev;                                                  \
    prev->next = next;                                                  \
    /* Note: not really needed, but safer */                            \
    obj->name.next = NULL;                                              \
    obj->name.prev = NULL;                                              \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _back)(const M_C3(ilist_, name,_t) list)           \
  {                                                                     \
    assert(list != NULL && !M_C3(ilist_, name, _empty_p)(list));        \
    return ILISTI_TYPE_FROM_FIELD(type, list->name.prev,                \
                                  struct ilist_head_s, name);           \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _front)(const M_C3(ilist_, name,_t) list)          \
  {                                                                     \
    assert(list != NULL && !M_C3(ilist_, name, _empty_p)(list));        \
    return ILISTI_TYPE_FROM_FIELD(type, list->name.next,                \
                                  struct ilist_head_s, name);           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _it)(M_C3(ilist_it_, name,_t) it,                  \
                          M_C3(ilist_, name,_t) list)                   \
  {                                                                     \
    assert (it != NULL && list != NULL);                                \
    assert (list->name.next != NULL && list->name.next->next != NULL);  \
    it->head = &list->name;                                             \
    it->current = list->name.next;                                      \
    it->next = list->name.next->next;                                   \
    it->previous = &list->name;                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _it_set)(M_C3(ilist_it_, name,_t) it,              \
                              const M_C3(ilist_it_, name,_t) cit)       \
  {                                                                     \
    assert (it != NULL && cit != NULL);                                 \
    it->head = cit->head;                                               \
    it->current = cit->current;                                         \
    it->next = cit->next;                                               \
    it->previous = cit->previous;                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _it_last)(M_C3(ilist_it_, name,_t) it,             \
                               M_C3(ilist_, name,_t) list)              \
  {                                                                     \
    assert (it != NULL && list != NULL);                                \
    assert (list->name.next != NULL && list->name.next->next != NULL);  \
    it->head = &list->name;                                             \
    it->current = list->name.prev;                                      \
    it->next = &list->name;                                             \
    it->previous = list->name.prev->prev;                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _it_end)(M_C3(ilist_it_, name,_t) it,              \
                              M_C3(ilist_, name,_t) list)               \
  {                                                                     \
    assert (it != NULL && list != NULL);                                \
    it->head = &list->name;                                             \
    it->current = &list->name;                                          \
    it->next = list->name.next;                                         \
    it->previous = list->name.prev;                                     \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(ilist_, name, _end_p)(const M_C3(ilist_it_, name,_t) it)         \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->current == it->head;                                     \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(ilist_, name, _last_p)(const M_C3(ilist_it_, name,_t) it)        \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->next == it->head;                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _next)(M_C3(ilist_it_, name,_t) it)                \
  {                                                                     \
    assert (it != NULL);                                                \
    /* Note: Can't set it->previous to it->current.                     \
       it->current may have been unlinked from the list */              \
    it->current  = it->next;                                            \
    assert (it->current != NULL);                                       \
    it->next     = it->current->next;                                   \
    it->previous = it->current->prev;                                   \
    assert (it->next != NULL && it->previous != NULL);                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _previous)(M_C3(ilist_it_, name,_t) it)            \
  {                                                                     \
    assert (it != NULL);                                                \
    /* Note: Can't set it->next to it->current.                         \
       it->current may have been unlinked from the list */              \
    it->current  = it->previous;                                        \
    assert (it->current != NULL);                                       \
    it->next     = it->current->next;                                   \
    it->previous = it->current->prev;                                   \
    assert (it->next != NULL && it->previous != NULL);                  \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(ilist_, name, _it_equal_p)(const M_C3(ilist_it_, name,_t) it1,   \
                                  const M_C3(ilist_it_, name,_t) it2 )  \
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    /* No need to check for next & previous */                          \
    return it1->head == it2->head && it1->current == it2->current;      \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _ref)(const M_C3(ilist_it_, name,_t) it)           \
  {                                                                     \
    assert (it != NULL && it->current != NULL);                         \
    /* check if 'it' was not deleted */                                 \
    assert (it->current->next == it->next);                             \
    assert (it->current->prev == it->previous);                         \
    assert (!M_C3(ilist_, name, _end_p)(it));                           \
    return ILISTI_TYPE_FROM_FIELD(type, it->current,                    \
                                  struct ilist_head_s, name);           \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(ilist_, name, _cref)(const M_C3(ilist_it_, name,_t) it)          \
  {                                                                     \
    type *ptr = M_C3(ilist_, name, _ref)(it);                           \
    return M_C3(ilist_, name, _const_cast)(ptr);                        \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _pop_back)(M_C3(ilist_, name,_t) list)             \
  {                                                                     \
    assert (!M_C3(ilist_, name, _empty_p)(list));                       \
    type *obj = M_C3(ilist_, name, _back)(list);                        \
    list->name.prev = list->name.prev->prev;                            \
    list->name.prev->next = &list->name;                                \
    return obj;                                                         \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _pop_front)(M_C3(ilist_, name,_t) list)            \
  {                                                                     \
    assert (!M_C3(ilist_, name, _empty_p)(list));                       \
    type *obj = M_C3(ilist_, name, _front)(list);                       \
    list->name.next = list->name.next->next;                            \
    list->name.next->prev = &list->name;                                \
    return obj;                                                         \
  }                                                                     \
  
#endif
