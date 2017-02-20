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
#define ILIST_INTERFACE(name, type)                                     \
  struct M_C3(ilist_head_, name, _s) {                                  \
    struct M_C3(ilist_head_, name, _s) *next;                           \
    struct M_C3(ilist_head_, name, _s) *prev;                           \
  } name

/* Define a list of a given type.
   LIST_DEF(name, type [, oplist_of_the_type]) */
#define ILIST_DEF(name, ...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (ILISTI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST, ILISTI_CHECK_INSTANCE ), \
   ILISTI_DEF2(name, __VA_ARGS__, ILISTI_CHECK_INSTANCE ))

/* Define the oplist of an ilist of type */
#define ILIST_OPLIST(name)                                              \
  (INIT(M_C3(ilist_, name, _init)),                                     \
   CLEAR(M_C3(ilist_, name, _clear)),                                   \
   TYPE(M_C3(ilist_,name,_t)),                                          \
   SUBTYPE(M_C3(ilist_type_,name,_t)),                                  \
   IT_TYPE(M_C3(ilist_it_,name,_t)),                                    \
   IT_FIRST(M_C3(ilist_,name,_it)),                                     \
   IT_SET(M_C3(ilist_,name,_it_set)),                                   \
   IT_END_P(M_C3(ilist_,name,_end_p)),                                  \
   IT_LAST_P(M_C3(ilist_,name,_last_p)),                                \
   IT_EQUAL_P(M_C3(ilist_,name,_it_equal_p)),                           \
   IT_NEXT(M_C3(ilist_,name,_next)),                                    \
   IT_REF(M_C3(ilist_,name,_ref)),                                      \
   IT_CREF(M_C3(ilist_,name,_cref)),                                    \
   PUSH(M_C3(ilist_,name,_push_back)),                                  \
   POP(M_C3(ilist_,name,_pop_back))                                     \
   )
//TODO: Add oplist as optional argument of ILIST_OPLIST

/********************************** INTERNAL ************************************/
/* Check if there is only one instance on the list in debug mode.
 * It doesn't work with C++ (as global vars with the same name are not merged).
 * (Not sure if it is really mandatory, nor a good idea) */
#if !defined(__cplusplus) && !defined (NDEBUG)
# define ILISTI_CHECK_INSTANCE 1
#else
# define ILISTI_CHECK_INSTANCE 0
#endif

/*
 * From a pointer to a 'field_type' 'field' of a 'type'structure,
 * return pointer to the structure.
 * Note: Lot of cast. Can be unsafe.
 */
#define ILISTI_TYPE_FROM_FIELD(type, ptr, field_type, field)    \
  ( ( (void)((ptr) == (field_type*)(ptr))) ,                    \
    ((type *)(void*)( (char *)(ptr) - offsetof(type, field) )) )

#define ILISTI_DEF2(name, type, oplist, check_instance)                 \
  typedef struct M_C3(ilist_head_, name, _s) M_C3(ilist_, name, _t)[1]; \
                                                                        \
  typedef type M_C3(ilist_type_,name, _t);                              \
                                                                        \
  typedef struct M_C3(ilist_it_, name, _s) {                            \
    struct M_C3(ilist_head_, name, _s) *head;                           \
    struct M_C3(ilist_head_, name, _s) *previous;                       \
    struct M_C3(ilist_head_, name, _s) *current;                        \
    struct M_C3(ilist_head_, name, _s) *next;                           \
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
  M_IF(check_instance)(bool M_C3(ilist_instance_, name, _g);, )         \
                                                                        \
  static inline void M_C3(ilist_, name, _init)(M_C3(ilist_, name, _t) list) \
  {                                                                     \
    assert (list != NULL);                                              \
    M_IF(check_instance)(assert (M_C3(ilist_instance_, name, _g) == false),); \
    M_IF(check_instance)(M_C3(ilist_instance_, name, _g) = true,);      \
    list->next = list;                                                  \
    list->prev = list;                                                  \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _clean)(M_C3(ilist_, name, _t) list) \
  {                                                                     \
    assert (list != NULL);                                              \
    list->next = list;                                                  \
    list->prev = list;                                                  \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _clear)(M_C3(ilist_, name, _t) list) \
  {                                                                     \
    M_IF(check_instance)(assert (M_C3(ilist_instance_, name, _g) == true),); \
    M_IF(check_instance)(M_C3(ilist_instance_, name, _g) = false,);     \
    M_C3(ilist_, name, _clean)(list);                                   \
  }                                                                     \
                                                                        \
  static inline bool M_C3(ilist_, name, _empty_p)(const M_C3(ilist_, name, _t) list) \
  {                                                                     \
    return list->next == list;                                          \
  }                                                                     \
                                                                        \
  static inline size_t M_C3(ilist_, name, _size)(const M_C3(ilist_, name, _t) list) \
  {                                                                     \
    size_t s = 0;                                                       \
    for(const struct M_C3(ilist_head_, name, _s) *it = list->next ;     \
        it != list; it = it->next)                                      \
      s++;                                                              \
    return s;                                                           \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _push_back)(M_C3(ilist_, name, _t) list, \
                                                    type *obj)          \
  {                                                                     \
    assert (list != NULL && obj != NULL);                               \
    struct M_C3(ilist_head_, name, _s) *prev = list->prev;              \
    list->prev = &obj->name;                                            \
    obj->name.prev = prev;                                              \
    obj->name.next = list;                                              \
    prev->next = &obj->name;                                            \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _push_front)(M_C3(ilist_, name, _t) list, \
                                                     type *obj)         \
  {                                                                     \
    assert (list != NULL && obj != NULL);                               \
    struct M_C3(ilist_head_, name, _s) *next = list->next;              \
    list->next = &obj->name;                                            \
    obj->name.next = next;                                              \
    obj->name.prev = list;                                              \
    next->prev = &obj->name;                                            \
  }                                                                     \
                                                                        \
  static inline void M_C3(ilist_, name, _push_after)(type *obj_it,      \
                                                     type *obj_ins)     \
  {                                                                     \
    assert (obj_it != NULL && obj_ins != NULL);                         \
    struct M_C3(ilist_head_, name, _s) *next = obj_it->name.next;       \
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
    struct M_C3(ilist_head_, name, _s) *next = obj->name.next;          \
    struct M_C3(ilist_head_, name, _s) *prev = obj->name.prev;          \
    next->prev = prev;                                                  \
    prev->next = next;                                                  \
    obj->name.next = NULL;                                              \
    obj->name.prev = NULL;                                              \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _back)(const M_C3(ilist_, name,_t) list)           \
  {                                                                     \
    assert(list != NULL && !M_C3(ilist_, name, _empty_p)(list));        \
    return ILISTI_TYPE_FROM_FIELD(type, list->prev,                     \
                             struct M_C3(ilist_head_, name, _s), name); \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _front)(const M_C3(ilist_, name,_t) list)          \
  {                                                                     \
    assert(list != NULL && !M_C3(ilist_, name, _empty_p)(list));        \
    return ILISTI_TYPE_FROM_FIELD(type, list->next,                     \
                             struct M_C3(ilist_head_, name, _s), name); \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _it)(M_C3(ilist_it_, name,_t) it,                  \
                          M_C3(ilist_, name,_t) list)                   \
  {                                                                     \
    assert (it != NULL && list != NULL);                                \
    assert (list->next != NULL && list->next->next != NULL);            \
    it->head = list;                                                    \
    it->current = list->next;                                           \
    it->next = list->next->next;                                        \
    it->previous = list;                                                \
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
  static inline bool                                                    \
  M_C3(ilist_, name, _end_p)(const M_C3(ilist_it_, name,_t) it)         \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->current == it->head;                                     \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(ilist_, name, _last_p)(const M_C3(ilist_it_, name,_t) it)    \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->next == it->head;                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(ilist_, name, _next)(M_C3(ilist_it_, name,_t) it)                \
  {                                                                     \
    assert (it != NULL);                                                \
    /* Can't set it->previous to it->current.                           \
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
    /* Can't set it->next to it->current.                               \
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
                             struct M_C3(ilist_head_, name, _s), name); \
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
    list->prev = list->prev->prev;                                      \
    list->prev->next = list;                                            \
    return obj;                                                         \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(ilist_, name, _pop_front)(M_C3(ilist_, name,_t) list)            \
  {                                                                     \
    assert (!M_C3(ilist_, name, _empty_p)(list));                       \
    type *obj = M_C3(ilist_, name, _front)(list);                       \
    list->next = list->next->next;                                      \
    list->next->prev = list;                                            \
    return obj;                                                         \
  }                                                                     \
  
//TODO: C++ compatibility

#endif
