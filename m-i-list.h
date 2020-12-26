/*
 * M*LIB - Intrusive List module
 *
 * Copyright (c) 2017-2020, Patrick Pelissier
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
#ifndef MSTARLIB_I_LIST_H
#define MSTARLIB_I_LIST_H

#include "m-core.h"
#include "m-list.h" // For LISTI_ITBASE_DEF

/* Interface to add to a structure to enable intrusive doubly-linked support.
   name: name of the intrusive list.
   type: name of the type of the structure (aka. struct test_s) - not used currently
   USAGE:
     typedef struct tmp_str_s {
      ...
      ILIST_INTERFACE(tmpstr, struct tmp_str_s);
      ...
     } tmp_str_t;
*/
#define ILIST_INTERFACE(name, type)                                           \
  struct ilist_head_s name


/* Define a doubly-linked intrusive list of a given type.
   The type needs to have ILIST_INTERFACE().
   USAGE:
     ILIST_DEF(name, type [, oplist_of_the_type]) */
#define ILIST_DEF(name, ...)                                                  \
  M_BEGIN_PROTECTED_CODE                                                      \
  ILISTI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                   \
                ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_T(name, t), M_C(name, _it_t) ), \
                 (name, __VA_ARGS__,                                      M_T(name, t), M_C(name, _it_t) ))) \
  M_END_PROTECTED_CODE


/* Define a doubly-linked intrusive list of a given type
   as the provided type name_t with the iterator named it_t.
   The type needs to have ILIST_INTERFACE().
   USAGE:
     ILIST_DEF_AS(name, name_t, it_t, type [, oplist_of_the_type]) */
#define ILIST_DEF_AS(name, name_t, it_t, ...)                                 \
  M_BEGIN_PROTECTED_CODE                                                      \
  ILISTI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                   \
                ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
                 (name, __VA_ARGS__,                                        name_t, it_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a doubly-linked instrusive list of type.
   USAGE:
     ILIST_OPLIST(name [, oplist_of_the_type]) */
#define ILIST_OPLIST(...)                                                     \
  ILISTI_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                \
                ((__VA_ARGS__, M_DEFAULT_OPLIST),                             \
                 (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

/* Define the basic structure to be added in all objects. */
typedef struct ilist_head_s {
  struct ilist_head_s *next;
  struct ilist_head_s *prev;
} ilist_head_ct;

/* Indirection call to allow expanding all arguments */
#define ILISTI_OPLIST_P1(arg) ILISTI_OPLIST_P2 arg

/* Validation of the given oplist */
#define ILISTI_OPLIST_P2(name, oplist)                                        \
  M_IF_OPLIST(oplist)(ILISTI_OPLIST_P3, ILISTI_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define ILISTI_OPLIST_FAILURE(name, oplist)                                   \
  ((M_LIB_ERROR(ARGUMENT_OF_ILIST_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* Define the oplist of an ilist of type */
#define ILISTI_OPLIST_P3(name, oplist)                                         \
    (INIT(M_F(name, M_NAMING_INIT)),                                           \
     CLEAR(M_F(name, M_NAMING_CLEAR)),                                         \
     INIT_MOVE(M_F(name, init_move)),                                          \
     MOVE(M_F(name, move)),                                                    \
     TYPE(M_T(name, ct)),                                                      \
     CLEAN(M_F(name, M_NAMING_CLEAN)),                                         \
     SUBTYPE(M_T(name, subtype_ct)),                                           \
     TEST_EMPTY(M_F(name, M_NAMING_TEST_EMPTY)),                               \
     IT_TYPE(M_T(name, it_ct)),                                                \
     IT_FIRST(M_F(name, M_NAMING_IT_FIRST)),                                   \
     IT_SET(M_F(name, M_NAMING_IT_SET)),                                       \
     IT_LAST(M_F(name, M_NAMING_IT_LAST)),                                     \
     IT_END(M_F(name, M_NAMING_IT_END)),                                       \
     IT_END_P(M_F(name, M_NAMING_IT_TEST_END)),                                \
     IT_LAST_P(M_F(name, M_NAMING_IT_TEST_LAST)),                              \
     IT_EQUAL_P(M_F(name, M_NAMING_IT_TEST_EQUAL)),                            \
     IT_NEXT(M_F(name, next)),                                                 \
     IT_PREVIOUS(M_F(name, previous)),                                         \
     IT_REF(M_F(name, ref)),                                                   \
     IT_CREF(M_F(name, cref)),                                                 \
     IT_REMOVE(M_F(name, remove)),                                             \
     M_IF_METHOD(NEW, oplist)(IT_INSERT(M_C(name, _insert)), ),                \
     OPLIST(oplist),                                                           \
     SPLICE_BACK(M_C(name, _splice_back)),                     \
     M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist), ),                        \
     M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist), ))

/* Contract respected by all intrusive lists */
#define ILISTI_CONTRACT(name, list) do {                                      \
    M_ASSERT(list != NULL);                                                   \
    M_ASSERT(list->name.prev != NULL);                                        \
    M_ASSERT(list->name.next != NULL);                                        \
    M_ASSERT(list->name.next->prev == &list->name);                           \
    M_ASSERT(list->name.prev->next == &list->name);                           \
    M_ASSERT(!(list->name.prev == &list->name) || list->name.prev == list->name.next); \
  } while (0)

#define ILISTI_NODE_CONTRACT(node) do {                                       \
    M_ASSERT((node) != NULL);                                                 \
    M_ASSERT((node)->prev != NULL);                                           \
    M_ASSERT((node)->next != NULL);                                           \
    M_ASSERT((node)->next->prev == node);                                     \
    M_ASSERT((node)->prev->next == node);                                     \
  } while (0)

/* Indirection call to allow expanding all arguments (TBC) */
#define ILISTI_DEF_P1(arg) ILISTI_DEF_P2 arg

/* Validate the oplist before going further */
#define ILISTI_DEF_P2(name, type, oplist, list_t, it_t)                       \
  M_IF_OPLIST(oplist)(ILISTI_DEF_P3, ILISTI_DEF_FAILURE)(name, type, oplist, list_t, it_t)

/* Stop processing with a compilation failure */
#define ILISTI_DEF_FAILURE(name, type, oplist, list_t, it_t)                  \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(ILIST_DEF): the given argument is not a valid oplist: " #oplist)

/* Definition of the type and function for an intrusive doubly-linked list.
   USAGE:
    name: name of the intrusive list
    type: type of the object
    oplist: oplist of the type
    list_t: type of the intrusive list (name##_t)
    it_t: iterator of the intrusive list (name##_it_t)
*/
#define ILISTI_DEF_P3(name, type, oplist, list_t, it_t)                 \
									                                                      \
  /* Define the list as a structure containing pointers                       \
   * to the front & back nodes */                                             \
  typedef struct M_T(name, s) {					                              \
    struct ilist_head_s name;                                           \
  } list_t[1];                                                          \
                                                                              \
  /* Define internal types pointers to such a list */                         \
  typedef struct M_T(name, s) *M_C(name, _ptr);                              \
  typedef const struct M_T(name, s) *M_C(name, _srcptr);                     \
                                                                              \
  /* Define iterator of such a list */                                        \
  typedef struct M_C(name, _it_s) {					                            \
    struct ilist_head_s *head;                                          \
    struct ilist_head_s *previous;                                      \
    struct ilist_head_s *current;                                       \
    struct ilist_head_s *next;                                          \
  } it_t[1];                                                            \
  									                                                    \
  /* Define types used by oplist */                                           \
  typedef type   M_C(name, _subtype_ct);                                      \
  typedef list_t M_T(name, ct);                                              \
  typedef it_t   M_C(name, _it_ct);                                           \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline void M_F(name, M_NAMING_INIT)(list_t list)			          \
  {                                                                           \
    M_ASSERT(list != NULL);                                                  \
    list->name.next = &list->name;                                      \
    list->name.prev = &list->name;                                      \
    ILISTI_CONTRACT(name, list);					                              \
  }                                                                     \
                                                                        \
  static inline void M_F(name, M_NAMING_CLEAN)(list_t list)			        \
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    for(struct ilist_head_s *it = list->name.next, *next ;		          \
        it != &list->name; it = next) {					                        \
      /* Cannot check node contract as previous node may be deleted */  \
      type *obj = M_TYPE_FROM_FIELD(type, it,				                    \
				    struct ilist_head_s, name);	                              	\
      /* Read next now before the object is destroyed */		            \
      next = it->next;							                                    \
      M_ASSERT(next != NULL);                                                \
      M_CALL_CLEAR(oplist, *obj);					                              \
      /* Delete also the object if a DELETE operand is registered */    \
      M_IF_METHOD(DEL, oplist)(M_CALL_DEL(oplist, obj), (void) 0);	    \
    }									                                                  \
    /* Nothing remains in the list anymore */                           \
    list->name.next = &list->name;                                      \
    list->name.prev = &list->name;                                      \
    ILISTI_CONTRACT(name, list);					                              \
  }                                                                     \
                                                                        \
  static inline void M_F(name, M_NAMING_CLEAR)(list_t list)			          \
  {                                                                           \
    M_F(name, M_NAMING_CLEAN)(list);                                          \
    /* For safety purpose (create invalid represenation of object) */         \
    list->name.next = NULL;                                                   \
    list->name.prev = NULL;                                                   \
  }                                                                           \
                                                                              \
  static inline bool M_F(name, M_NAMING_TEST_EMPTY)(const list_t list)	\
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    return list->name.next == &list->name;                              \
  }                                                                     \
                                                                        \
  									                                                    \
  static inline void M_F(name, init_move)(list_t list, list_t ref)     \
  {                                                                           \
    ILISTI_CONTRACT(name, ref);                                               \
    M_ASSERT(list != ref);                                               \
    M_F(name, M_NAMING_INIT)(list);                                      \
    if (!M_F(name, M_NAMING_TEST_EMPTY)(ref)) {                          \
      list->name.next = ref->name.next;                                 \
      list->name.prev = ref->name.prev;                                 \
      list->name.next->prev = &list->name;                              \
      list->name.prev->next = &list->name;                              \
    }                                                                   \
    ref->name.next = NULL;                                              \
    ref->name.prev = NULL;                                              \
    ILISTI_CONTRACT(name, list);					                              \
  }                                                                     \
  								                                                    	\
  static inline void                                                    \
  M_F(name, move)(list_t list, list_t ref)          \
  {                                                                           \
    M_F(name, M_NAMING_CLEAR)(list);                                    \
    M_F(name, init_move)(list, ref);                                   \
  }                                                                           \
                                                                              \
  static inline size_t                                                  \
  M_F(name, M_NAMING_SIZE)(const list_t list)		  \
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    size_t s = 0;                                                       \
    /* Scan the full list to count the number of elements */                  \
    for(const struct ilist_head_s *it = list->name.next ;                     \
        it != &list->name; it = it->next) {                                   \
      ILISTI_NODE_CONTRACT(it);                                               \
      s++;                                                                    \
    }                                                                         \
    return s;                                                                 \
  }                                                                           \
                                                                              \
  static inline void                                                    \
  M_F(name, push_back)(list_t list,	type *obj)			\
  {                                                                     \
    ILISTI_CONTRACT(name, list);																				\
    M_ASSERT (obj != NULL);                                                   \
    struct ilist_head_s *prev = list->name.prev;                        \
    list->name.prev = &obj->name;                                       \
    obj->name.prev = prev;                                              \
    obj->name.next = &list->name;                                       \
    prev->next = &obj->name;                                            \
    ILISTI_CONTRACT(name, list);																				\
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_F(name, push_front)(list_t list, type *obj)			\
  {                                                                     \
    ILISTI_CONTRACT(name, list);																				\
    M_ASSERT(obj != NULL);                                                   \
    struct ilist_head_s *next = list->name.next;                        \
    list->name.next = &obj->name;                                       \
    obj->name.next = next;                                              \
    obj->name.prev = &list->name;                                       \
    next->prev = &obj->name;                                            \
    ILISTI_CONTRACT(name, list);																				\
  }                                                                     \
                                                                        \
  static inline void                                                          \
  M_F(name, push_after)(type *obj_pos, type *obj)                            \
  {                                                                           \
    M_ASSERT(obj_pos != NULL && obj != NULL);                                \
    /* We don't have the list, so we have no contract at list level */        \
    ILISTI_NODE_CONTRACT(&obj_pos->name);                                     \
    struct ilist_head_s *next = obj_pos->name.next;                           \
    obj_pos->name.next = &obj->name;                                          \
    obj->name.next = next;                                                    \
    obj->name.prev = &obj_pos->name;                                          \
    next->prev = &obj->name;                                                  \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, init_field)(type *obj)                                           \
  {                                                                           \
    M_ASSERT(obj != NULL);                                                   \
    /* Init the fields of the node. To be used in object constructor */       \
    obj->name.next = NULL;                                                    \
    obj->name.prev = NULL;                                                    \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, unlink)(type *obj)                                               \
  {                                                                           \
    M_ASSERT(obj != NULL);                                                   \
    /* We don't have the list, so we have no contract at list level */        \
    ILISTI_NODE_CONTRACT(&obj->name);                                         \
    struct ilist_head_s *next = obj->name.next;                               \
    struct ilist_head_s *prev = obj->name.prev;                               \
    next->prev = prev;                                                        \
    prev->next = next;                                                        \
    /* Note: not really needed, but safer */                                  \
    obj->name.next = NULL;                                                    \
    obj->name.prev = NULL;                                                    \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F(name, back)(const list_t list)																		\
  {                                                                     \
    ILISTI_CONTRACT(name, list);																				\
    M_ASSERT(!M_F(name, M_NAMING_TEST_EMPTY)(list));													\
    return M_TYPE_FROM_FIELD(type, list->name.prev,                     \
                             struct ilist_head_s, name);                \
  }                                                                     \
  																																			\
  static inline type *                                                  \
  M_F(name, front)(const list_t list)																	\
  {                                                                     \
    ILISTI_CONTRACT(name, list);																				\
    M_ASSERT(!M_F(name, M_NAMING_TEST_EMPTY)(list));													\
    return M_TYPE_FROM_FIELD(type, list->name.next,                     \
                             struct ilist_head_s, name);                \
  }                                                                     \
                                                                        \
  static inline type *																									\
  M_F(name, next_obj)(const list_t list, type const *obj)							\
  {																																			\
    ILISTI_CONTRACT(name, list);																				\
    M_ASSERT(obj != NULL);                                                   \
    ILISTI_NODE_CONTRACT(&obj->name);                                   \
    return obj->name.next == &list->name ? NULL :												\
      M_TYPE_FROM_FIELD(type, obj->name.next,														\
			struct ilist_head_s, name);																				\
  }																																			\
																																				\
  static inline type *																									\
  M_F(name, previous_obj)(const list_t list, type const *obj)					\
  {																																			\
    ILISTI_CONTRACT(name, list);																				\
    M_ASSERT(obj != NULL);                                                   \
    ILISTI_NODE_CONTRACT(&obj->name);                                   \
    return obj->name.prev == &list->name ? NULL :												\
      M_TYPE_FROM_FIELD(type, obj->name.prev,														\
			struct ilist_head_s, name);																				\
  }																																			\
																																				\
  static inline void                                                    \
  M_F(name, M_NAMING_IT_FIRST)(it_t it, const list_t list)                            \
  {                                                                     \
    ILISTI_CONTRACT(name, list);																				\
    M_ASSERT(it != NULL);                                                    \
    it->head = list->name.next->prev;                                         \
    it->current = list->name.next;                                            \
    it->next = list->name.next->next;                                         \
    it->previous = it->head;                                                  \
    ILISTI_NODE_CONTRACT(it->current);                                        \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_SET)(it_t it, const it_t cit)                   \
  {                                                                           \
    M_ASSERT(it != NULL && cit != NULL);                                     \
    it->head = cit->head;                                               \
    it->current = cit->current;                                         \
    it->next = cit->next;                                               \
    it->previous = cit->previous;                                       \
    ILISTI_NODE_CONTRACT(it->current);                                  \
  }                                                                     \
  									                                                    \
  static inline void                                                    \
  M_F(name, M_NAMING_IT_LAST)(it_t it, list_t const list)			                  \
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    M_ASSERT(it != NULL);                                                    \
    it->head = list->name.next->prev;                                   \
    it->current = list->name.prev;                                      \
    it->next = it->head;                                                \
    it->previous = list->name.prev->prev;                               \
    ILISTI_NODE_CONTRACT(it->current);                                  \
  }                                                                     \
  									                                                    \
  static inline void                                                    \
  M_F(name, M_NAMING_IT_END)(it_t it, list_t const list)                        \
  {                                                                           \
    M_ASSERT(it != NULL && list != NULL);                                    \
    it->head = list->name.next->prev;                                   \
    it->current = it->head;                                             \
    it->next = list->name.next;                                         \
    it->previous = list->name.prev;                                     \
    ILISTI_NODE_CONTRACT(it->current);                                  \
  }                                                                     \
  									                                                    \
  static inline bool                                                    \
  M_F(name, M_NAMING_IT_TEST_END)(const it_t it)					                    \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    ILISTI_NODE_CONTRACT(it->current);                                  \
    return it->current == it->head;                                     \
  }                                                                     \
  									                                                    \
  static inline bool                                                    \
  M_F(name, M_NAMING_IT_TEST_LAST)(const it_t it)                             \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    ILISTI_NODE_CONTRACT(it->current);                                  \
    return it->next == it->head || it->current == it->head;		          \
  }                                                                     \
  									                                                    \
  static inline void                                                    \
  M_F(name, next)(it_t it)                                             \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    /* Cannot check node for it->current: it may have been deleted! */        \
    /* Note: Can't set it->previous to it->current.                           \
       it->current may have been unlinked from the list */                    \
    it->current  = it->next;                                                  \
    M_ASSERT(it->current != NULL);                                           \
    it->next     = it->current->next;                                         \
    it->previous = it->current->prev;                                         \
    M_ASSERT(it->next != NULL && it->previous != NULL);                      \
    ILISTI_NODE_CONTRACT(it->current);                                        \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, previous)(it_t it)                                         \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    /* Cannot check node for it->current: it may have been deleted! */        \
    /* Note: Can't set it->next to it->current.                               \
       it->current may have been unlinked from the list */                    \
    it->current  = it->previous;                                              \
    M_ASSERT(it->current != NULL);                                           \
    it->next     = it->current->next;                                         \
    it->previous = it->current->prev;                                         \
    M_ASSERT(it->next != NULL && it->previous != NULL);                      \
    ILISTI_NODE_CONTRACT(it->current);                                  \
  }                                                                     \
  									                                                    \
  static inline bool                                                    \
  M_F(name, M_NAMING_IT_TEST_EQUAL)(const it_t it1, const it_t it2 )       \
  {                                                                           \
    M_ASSERT(it1 != NULL && it2 != NULL);                                    \
    /* No need to check for next & previous */                          \
    return it1->head == it2->head && it1->current == it2->current;      \
  }                                                                     \
  									                                                    \
  static inline type *                                                  \
  M_F(name, ref)(const it_t it)					                              \
  {                                                                           \
    M_ASSERT (it != NULL && it->current != NULL);                             \
    ILISTI_NODE_CONTRACT(it->current);                                        \
    /* check if 'it' was not deleted */                                       \
    M_ASSERT(it->current->next == it->next);                             \
    M_ASSERT(it->current->prev == it->previous);                         \
    M_ASSERT(!M_F(name, M_NAMING_IT_TEST_END)(it));				                  	\
    return M_TYPE_FROM_FIELD(type, it->current,                         \
			     struct ilist_head_s, name);	                              	\
  }                                                                     \
  								                                                    	\
  static inline type const *                                            \
  M_F(name, cref)(const it_t it)					                              \
  {                                                                           \
    type *ptr = M_F(name, ref)(it);					                          \
    return M_CONST_CAST(type, ptr);                                     \
  }                                                                     \
  									                                                    \
  static inline void							                                      \
  M_F(name, remove)(list_t list, it_t it)				                      \
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    ILISTI_NODE_CONTRACT(it->current);                                  \
    (void)list;	/* list param is not used */				                    \
    type *obj = M_TYPE_FROM_FIELD(type, it->current,			              \
				  struct ilist_head_s, name);		                                \
    M_F(name, unlink)(obj);						                                \
    M_CALL_CLEAR(oplist, obj);						                              \
    M_IF_METHOD(DEL, oplist)(M_CALL_DEL(oplist, obj), (void) 0);        \
    M_F(name, next)(it);						                                    \
  }									                                                    \
									                                                      \
  M_IF_METHOD(NEW, oplist)(						                                  \
  static inline void							                                      \
  M_F(name, insert)(list_t list, it_t it, type x)			                \
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    ILISTI_NODE_CONTRACT(it->current);                                  \
    type *p = M_CALL_NEW(oplist, type);                                 \
    if (M_UNLIKELY(p == NULL)) {					                              \
      M_MEMORY_FULL (sizeof (type));					                          \
      return ;								                                          \
    }									                                                  \
    M_CALL_INIT_SET(oplist, *p, x);					                            \
    type *obj = M_F(name, ref)(it);					                          \
    M_F(name, push_after)(obj, p);					                            \
    it->current = p;                                                          \
    (void) list;							                                          \
    ILISTI_CONTRACT(name, list);					                              \
  }									                                                    \
  , /* NEW not defined */)						                                  \
									                                                      \
  static inline type *                                                  \
  M_F(name, pop_back)(list_t list)					                            \
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    M_ASSERT(!M_F(name, M_NAMING_TEST_EMPTY)(list));				                \
    type *obj = M_F(name, back)(list);					                        \
    list->name.prev = list->name.prev->prev;                            \
    list->name.prev->next = &list->name;                                \
    return obj;                                                         \
  }                                                                     \
  									                                                    \
  static inline type *                                                  \
  M_F(name, pop_front)(list_t list)					                          \
  {                                                                     \
    ILISTI_CONTRACT(name, list);					                              \
    M_ASSERT(!M_F(name, M_NAMING_TEST_EMPTY)(list));				                \
    type *obj = M_F(name, front)(list);				                        \
    list->name.next = list->name.next->next;                            \
    list->name.next->prev = &list->name;                                \
    return obj;                                                         \
  }                                                                     \
									                                                      \
  static inline void                                                    \
  M_F(name, splice)(list_t list1, list_t list2)			                  \
  {                                                                     \
    ILISTI_CONTRACT(name, list1);					                              \
    ILISTI_CONTRACT(name, list2);					                              \
    struct ilist_head_s *midle1 = list1->name.prev;			                \
    struct ilist_head_s *midle2 = list2->name.next;			                \
    midle1->next = midle2;						                                  \
    midle2->prev = midle1;						                                  \
    list1->name.prev = list2->name.prev;				                        \
    list2->name.prev->next = &list1->name;			                       	\
    list2->name.next = &list2->name;					                          \
    list2->name.prev = &list2->name;					                          \
    ILISTI_CONTRACT(name, list1);					                              \
    ILISTI_CONTRACT(name, list2);					                              \
  }									                                                    \
									                                                      \
  static inline void                                                    \
  M_F(name, splice_back)(list_t nv, list_t ov, it_t it)		            \
  {                                                                     \
    ILISTI_CONTRACT(name, nv);					                              	\
    ILISTI_CONTRACT(name, ov);						                              \
    ILISTI_NODE_CONTRACT(it->current);                                  \
    M_ASSERT(it != NULL);                                                    \
    (void) ov;								                                          \
    type *obj = M_F(name, ref)(it);					                          \
    M_F(name, unlink)(obj);						                                \
    M_F(name, push_back)(nv, obj);					                            \
    M_F(name, next)(it);                                               \
    ILISTI_CONTRACT(name, nv);						                              \
    ILISTI_CONTRACT(name, ov);						                              \
  }									                                                    \
									                                                      \
  static inline void							                                      \
  M_F(name, splice_at)(list_t nlist, it_t npos,                        \
                        list_t olist, it_t opos)                        \
  {                                                                     \
    ILISTI_CONTRACT(name, nlist);					                              \
    ILISTI_CONTRACT(name, olist);				                              	\
    M_ASSERT(npos != NULL && opos != NULL);                              \
    M_ASSERT(!M_F(name, M_NAMING_IT_TEST_END)(opos));					                \
    /* npos may be end */                                                     \
    (void) olist, (void) nlist;                                               \
    type *obj = M_F(name, ref)(opos);					                        \
    struct ilist_head_s *ref = npos->current;                           \
    /* Remove object */                                                 \
    M_F(name, unlink)(obj);						                                \
    /* Push 'obj' after 'ref' */                                        \
    struct ilist_head_s *next = ref->next;                              \
    ref->next = &obj->name;                                             \
    obj->name.next = next;                                              \
    obj->name.prev = ref;                                               \
    next->prev = &obj->name;                                            \
    /* Move iterator in old list */                                     \
    M_F(name, next)(opos);                                             \
    /* Set npos iterator to new position of object */                   \
    npos->previous = ref;                                               \
    npos->current = &obj->name;                                         \
    npos->next = next;                                                  \
    ILISTI_CONTRACT(name, nlist);			                              		\
    ILISTI_CONTRACT(name, olist);			                              		\
  }									                                                    \
									                                                      \
  static inline void							                                      \
  M_F(name, swap)(list_t d, list_t e)					                        \
  {									                                                    \
    ILISTI_CONTRACT(name, d);						                                \
    ILISTI_CONTRACT(name, e);					                                	\
    struct ilist_head_s *d_item = d->name.next;                         \
    struct ilist_head_s *e_item = e->name.next;                         \
    d->name.next = e_item == &e->name ? &d->name : e_item;              \
    e->name.next = d_item == &d->name ? &e->name : d_item;              \
    d_item = d->name.prev;                                              \
    e_item = e->name.prev;                                              \
    d->name.prev = e_item == &e->name ? &d->name : e_item;              \
    e->name.prev = d_item == &d->name ? &e->name : d_item;              \
    d->name.next->prev = &d->name;                                      \
    d->name.prev->next = &d->name;                                      \
    e->name.next->prev = &e->name;                                      \
    e->name.prev->next = &e->name;                                      \
    ILISTI_CONTRACT(name, d);						                                \
    ILISTI_CONTRACT(name, e);					                                	\
  }									                                                    \
									                                                      \
  static inline void						                                        \
  M_F(name, reverse)(list_t list)                                      \
  {									                                                    \
    ILISTI_CONTRACT(name, list);                                        \
    struct ilist_head_s *next, *it;                                     \
    for(it = list->name.next ; it != &list->name; it = next) {          \
      next = it->next;                                                  \
      it->next = it->prev;                                              \
      it->prev = next;                                                  \
    }                                                                   \
    next = it->next;                                                    \
    it->next = it->prev;                                                \
    it->prev = next;                                                    \
    ILISTI_CONTRACT(name, list);                                        \
  }									                                                    \
                                                                        \
  /* Used of macro from m-list */                                             \
  LISTI_ITBASE_DEF(name, type, oplist, list_t, it_t)


#endif // MSTARLIB_I_LIST_H
