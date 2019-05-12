/*
 * M*LIB - DEQUE module
 *
 * Copyright (c) 2017-2019, Patrick Pelissier
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
#ifndef MSTARLIB_DEQUE_H
#define MSTARLIB_DEQUE_H

#include "m-i-list.h"

/* Define a deque of a given type and its associated functions.
   USAGE: DEQUE_DEF(name, type [, oplist_of_the_type]) */
#define DEQUE_DEF(name, ...)                                            \
  DEQUEI_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                                \
             ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name,_t), M_C(name,_it_t), M_C(name, _node_t) ), \
   (name, __VA_ARGS__,                                      M_C(name,_t), M_C(name,_it_t), M_C(name, _node_t))))


/* Define the oplist of a deque of a type.
   USAGE: DEQUE_OPLIST(name[, oplist of the type]) */
#define DEQUE_OPLIST(...)                                            \
  DEQUEI_OPLIST (M_IF_NARGS_EQ1(__VA_ARGS__)                         \
                 ((__VA_ARGS__, M_DEFAULT_OPLIST),		     \
                  (__VA_ARGS__ )))

/********************************** INTERNAL ************************************/

#define DEQUEUI_DEFAULT_SIZE  8

#define DEQUEI_CONTRACT(d) do {						\
    assert ((d) != NULL);						\
    assert ((d)->default_size >= DEQUEUI_DEFAULT_SIZE);			\
    assert ((d)->front->node != NULL);					\
    assert ((d)->front->index <= (d)->front->node->size);		\
    assert ((d)->back->node != NULL);					\
    assert ((d)->back->index <= (d)->back->node->size);			\
    assert ((d)->front->node != (d)->back->node ||			\
	    (d)->front->index <= (d)->back->index);			\
    assert ((d)->front->node != (d)->back->node ||			\
	    (d)->back->index - (d)->front->index == (d)->count);	\
  } while (0)

#define DEQUEI_DEF(arg) DEQUEI_DEF2 arg

#define DEQUEI_DEF2(name, type, oplist, deque_t, it_t, node_t)		\
									\
  typedef struct M_C(name, _node_s) {					\
    ILIST_INTERFACE(M_C(name, _node_list), struct M_C(name, _node_s));	\
    size_t size;							\
    type  data[M_MIN_FLEX_ARRAY_SIZE];					\
  } node_t;								\
									\
  /* Each node is allocated with a variable size (so we use             \
     M_GET_REALLOC for the allocation). But we need to delete the nodes \
     automatically with the intrusive list used for storing the nodes:  \
     so we register as a DEL operator the FREE operator of the oplist.  \
     The interfaces are compatible.                                     \
  */                                                                    \
  ILIST_DEF(M_C(name, _node_list), node_t, (DEL(M_GET_FREE oplist)) )	\
  									\
  typedef struct M_C(name, _it2_s) {					\
    node_t *node;							\
    size_t        index;						\
  } M_C(name, _it2_t)[1];						\
  									\
  typedef struct M_C(name, _s) {					\
    M_C(name, _node_list_t) list;					\
    M_C(name, _it2_t)       front;					\
    M_C(name, _it2_t)       back;					\
    size_t                  default_size;				\
    size_t                  count;					\
  } deque_t[1];								\
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
		 							\
  typedef struct M_C(name, _it_s) {					\
    node_t *node;							\
    size_t        index;						\
    const struct M_C(name, _s) *deque;					\
  } it_t[1];								\
  									\
  typedef type M_C(name, _type_t);					\
  									\
  static inline M_C(name, _node_t)*                                     \
  M_C(name, _int_new_node)(deque_t d)					\
  {									\
    size_t def = d->default_size;					\
    if (M_UNLIKELY (def > SIZE_MAX / sizeof (type) - sizeof(M_C(name, _node_t)))) { \
      M_MEMORY_FULL(sizeof(M_C(name, _node_t))+def * sizeof(type));     \
      return NULL;							\
    }									\
    M_C(name, _node_t)*n = (M_C(name, _node_t)*) (void*)                \
      M_CALL_REALLOC(oplist, char, NULL,                               \
                     sizeof(M_C(name, _node_t)) + def * sizeof(type) ); \
    if (n==NULL) {							\
      M_MEMORY_FULL(sizeof(M_C(name, _node_t))+def * sizeof(type));     \
      return NULL;							\
    }									\
    n->size = def;							\
    M_C(name, _node_list_init_field)(n);                                \
    /* Do not increase it too much if there are few items */            \
    def = M_MIN(def, d->count);                                         \
    d->default_size = M_CALL_INC_ALLOC(oplist, def);                    \
    return n;								\
  }									\
									\
  static inline void							\
  M_C(name, _init)(deque_t d)						\
  {									\
    M_C(name, _node_list_init)(d->list);				\
    d->default_size = DEQUEUI_DEFAULT_SIZE;				\
    d->count        = 0;						\
    M_C(name, _node_t) *n = M_C(name, _int_new_node)(d);                \
    if (n == NULL) return;						\
    M_C(name, _node_list_push_back)(d->list, n);                        \
    d->front->node  = n;						\
    d->front->index = DEQUEUI_DEFAULT_SIZE/2;				\
    d->back->node   = n;						\
    d->back->index  = DEQUEUI_DEFAULT_SIZE/2;				\
    DEQUEI_CONTRACT(d);							\
  }									\
									\
  static inline void							\
  M_C(name, _clean)(deque_t d)						\
  {									\
    DEQUEI_CONTRACT(d);							\
    M_C(name, _node_t) *min_node = NULL;                                \
    for(M_C(name, _node_t) *n = d->front->node;                         \
	n != NULL ;                                                     \
	n = (n == d->back->node) ? NULL :                               \
          M_C(name, _node_list_next_obj)(d->list, n) ){                 \
      size_t min = n == d->front->node ? d->front->index : 0;		\
      size_t max = n == d->back->node ? d->back->index : n->size;	\
      for(size_t i = min; i < max; i++) {				\
	M_CALL_CLEAR(oplist, n->data[i]);				\
      }									\
      min_node = (min_node == NULL || min_node->size > n->size) ? n : min_node; \
    }									\
    assert (min_node != NULL);                                          \
    d->front->node = min_node;						\
    d->front->index = min_node->size / 2;				\
    d->back->node = min_node;						\
    d->back->index = min_node->size / 2;				\
    d->count = 0;							\
    DEQUEI_CONTRACT(d);							\
  }									\
									\
  static inline void							\
  M_C(name, _clear)(deque_t d)						\
  {									\
    DEQUEI_CONTRACT(d);							\
    M_C(name, _clean)(d);						\
    /* We have registered the delete operator to clear all objects */   \
    M_C(name, _node_list_clear)(d->list);				\
    /* It is safer to clean some variables */				\
    d->front->node  = NULL;						\
    d->back->node   = NULL;						\
    d->count = 0;							\
  }									\
									\
  static inline type *							\
  M_C(name, _push_back_raw)(deque_t d)					\
  {									\
    DEQUEI_CONTRACT(d);							\
    M_C(name, _node_t) *n = d->back->node;                              \
    size_t index = d->back->index;					\
    if (M_UNLIKELY (n->size <= index)) {				\
      n = M_C(name, _node_list_next_obj)(d->list, n);			\
      if (n == NULL) {							\
	n = M_C(name, _int_new_node)(d);				\
	if (M_UNLIKELY (n == NULL)) return NULL;			\
	M_C(name, _node_list_push_back)(d->list, n);			\
      }									\
      d->back->node = n;						\
      index = 0;							\
    }									\
    type *ret = &n->data[index];					\
    index++;								\
    d->count ++;							\
    d->back->index = index;						\
    DEQUEI_CONTRACT(d);							\
    return ret;								\
  }									\
									\
  static inline void							\
  M_C(name, _push_back)(deque_t d, type const x)			\
  {									\
    type *p = M_C(name, _push_back_raw)(d);				\
    if (M_LIKELY(p != NULL)) {						\
      M_CALL_INIT_SET(oplist, *p, x);					\
    }									\
  }									\
									\
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline type *							\
  M_C(name, _push_back_new)(deque_t d)					\
  {									\
    type *p = M_C(name, _push_back_raw)(d);				\
    if (M_LIKELY(p != NULL)) {						\
      M_CALL_INIT(oplist, *p);						\
    }									\
    return p;								\
  }									\
  , )                                                                   \
									\
  static inline void							\
  M_C(name, _push_back_move)(deque_t d, type *x)			\
  {									\
    assert (x != NULL);                                                 \
    type *p = M_C(name, _push_back_raw)(d);				\
    if (M_UNLIKELY(p == NULL)) 						\
      return;                                                           \
    M_DO_INIT_MOVE (oplist, *p, *x);					\
  }									\
									\
  static inline type*							\
  M_C(name, _push_front_raw)(deque_t d)					\
  {									\
    DEQUEI_CONTRACT(d);							\
    M_C(name, _node_t) *n = d->front->node;                             \
    size_t index = d->front->index;					\
    index --;								\
    /* If overflow */							\
    if (M_UNLIKELY (n->size <= index)) {				\
      n = M_C(name, _node_list_previous_obj)(d->list, n);		\
      if (n == NULL) {							\
	n = M_C(name, _int_new_node)(d);				\
	if (M_UNLIKELY (n == NULL)) return NULL;			\
	M_C(name, _node_list_push_front)(d->list, n);			\
      }									\
      d->front->node = n;						\
      index = n->size -1;						\
    }									\
    type *ret = &n->data[index];					\
    d->count ++;							\
    d->front->index = index;						\
    DEQUEI_CONTRACT(d);							\
    return ret;								\
  }									\
									\
  static inline void							\
  M_C(name, _push_front)(deque_t d, type const x)			\
  {									\
    type *p = M_C(name, _push_front_raw)(d);				\
    if (M_LIKELY(p != NULL)) {						\
      M_CALL_INIT_SET(oplist, *p, x);					\
    }									\
  }									\
									\
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline type *							\
  M_C(name, _push_front_new)(deque_t d)					\
  {									\
    type *p = M_C(name, _push_front_raw)(d);				\
    if (M_LIKELY(p != NULL)) {						\
      M_CALL_INIT(oplist, *p);						\
    }									\
    return p;								\
  }									\
  ,)                                                                    \
									\
  static inline void							\
  M_C(name, _push_front_move)(deque_t d, type *x)			\
  {									\
    assert (x != NULL);                                                 \
    type *p = M_C(name, _push_front_raw)(d);				\
    if (M_UNLIKELY(p == NULL)) 						\
      return;                                                           \
    M_DO_INIT_MOVE (oplist, *p, *x);					\
  }									\
									\
  static inline void							\
  M_C(name, _pop_back)(type *ptr, deque_t d)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert(d->count > 0);						\
    M_C(name, _node_t) *n = d->back->node;                              \
    size_t index = d->back->index;					\
    index --;								\
    if (M_UNLIKELY (n->size <= index)) {				\
      /* If there is a next node,                                       \
         pop the back node and push it back to the front. This          \
         reduce the used memory if the deque is used as a FIFO queue.*/ \
      M_C(name, _node_t) *next = M_C(name, _node_list_next_obj)(d->list, n); \
      if (next != NULL) {                                               \
        next = M_C(name, _node_list_pop_back)(d->list);                 \
        assert (next != n);                                             \
        M_C(name, _node_list_push_front)(d->list, next);                \
      }                                                                 \
      n = M_C(name, _node_list_previous_obj)(d->list, n);               \
      assert (n != NULL);						\
      d->back->node = n;						\
      index = n->size-1;						\
    }									\
    if (ptr != NULL)							\
      M_IF_METHOD(MOVE, oplist) (                                       \
      M_CALL_MOVE(oplist, *ptr, n->data[index]); else                   \
      ,                                                                 \
      M_CALL_SET(oplist, *ptr, n->data[index]);				\
      )                                                                 \
    M_CALL_CLEAR(oplist,  n->data[index]);				\
    d->count --;							\
    d->back->index = index;						\
    DEQUEI_CONTRACT(d);							\
  }									\
									\
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void							\
  M_C(name, _pop_back_move)(type *ptr, deque_t d)			\
  {									\
    assert(ptr != NULL);                                                \
    /* Note: Lazy implementation. Can be improved if needed */          \
    M_CALL_INIT(oplist, *ptr);                                          \
    M_C(name, _pop_back)(ptr, d);                                       \
  }                                                                     \
  , )                                                                   \
									\
  static inline void							\
  M_C(name, _pop_front)(type *ptr, deque_t d)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert(d->count > 0);						\
    M_C(name, _node_t) *n = d->front->node;                             \
    size_t index = d->front->index;					\
    if (M_UNLIKELY (n->size <= index)) {				\
      /* If there is a previous node,                                   \
         pop the front node and push it back to the back. This          \
         reduce the used memory if the deque is used as a FIFO queue.*/ \
      M_C(name,_node_t) *prev = M_C(name, _node_list_previous_obj)(d->list, n); \
      if (prev != NULL) {                                               \
        prev = M_C(name, _node_list_pop_front)(d->list);                \
        assert (prev != n);                                             \
        M_C(name, _node_list_push_back)(d->list, prev);                 \
      }                                                                 \
      n = M_C(name, _node_list_next_obj)(d->list, n);                   \
      assert (n != NULL);						\
      d->front->node = n;						\
      index = 0;							\
    }									\
    if (ptr != NULL)							\
      M_IF_METHOD(MOVE, oplist) (                                       \
      M_CALL_MOVE(oplist, *ptr, n->data[index]); else                   \
      ,                                                                 \
      M_CALL_SET(oplist, *ptr, n->data[index]);				\
      )                                                                 \
    M_CALL_CLEAR(oplist, n->data[index]);				\
    index++;								\
    d->count --;							\
    d->front->index = index;						\
  }									\
									\
  M_IF_METHOD(INIT, oplist)(                                            \
  static inline void							\
  M_C(name, _pop_front_move)(type *ptr, deque_t d)			\
  {									\
    assert(ptr != NULL);                                                \
    /* Note: Lazy implementation */                                     \
    M_CALL_INIT(oplist, *ptr);                                          \
    M_C(name, _pop_front)(ptr, d);                                      \
  }                                                                     \
  ,)                                                                    \
                                                                        \
  static inline type *							\
  M_C(name, _back)(const deque_t d)					\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (d->count > 0);						\
    size_t i = d->back->index;						\
    M_C(name, _node_t) *n = d->back->node;                              \
    if (M_UNLIKELY (i == 0)) {						\
      n = M_C(name, _node_list_previous_obj)(d->list, n);               \
      assert (n != NULL);						\
      i = n->size;							\
    }									\
    return &n->data[i-1];						\
  }									\
									\
  static inline type *							\
  M_C(name, _front)(const deque_t d)					\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (d->count > 0);						\
    size_t i = d->front->index;						\
    M_C(name, _node_t) *n = d->front->node;                             \
    if (M_UNLIKELY (n->size <= i)) {					\
      n = M_C(name, _node_list_next_obj)(d->list, n);                   \
      assert (n != NULL);						\
      i = 0;								\
    }									\
    return &n->data[i];							\
  }									\
									\
  static inline size_t							\
  M_C(name, _size)(const deque_t d)					\
  {									\
    DEQUEI_CONTRACT(d);							\
    return d->count;							\
  }									\
                                                                        \
  static inline size_t                                                  \
  M_C(name, _capacity_back)(const deque_t v)                            \
  {                                                                     \
    DEQUEI_CONTRACT(v);                                                 \
    size_t s = 0;                                                       \
    for(M_C(name, _node_t) *n = M_C(name, _node_list_back)(v->list);    \
	n != NULL ;                                                     \
	n = (n == v->back->node) ? NULL :                               \
          M_C(name, _node_list_previous_obj)(v->list, n) ){             \
      s += (n == v->back->node ? v->back->index : n->size);             \
    }                                                                   \
    return s;                                                           \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _capacity_front)(const deque_t v)                           \
  {                                                                     \
    DEQUEI_CONTRACT(v);                                                 \
    size_t s = 0;                                                       \
    for(M_C(name, _node_t) *n = M_C(name, _node_list_front)(v->list);   \
	n != NULL ;                                                     \
	n = (n == v->front->node) ? NULL :                              \
          M_C(name, _node_list_next_obj)(v->list, n) ){                 \
      s += n->size - (n == v->front->node ? v->front->index : 0);       \
    }                                                                   \
    return s;                                                           \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _capacity)(const deque_t v)					\
  {                                                                     \
    return M_C(name, _capacity_back)(v)+M_C(name, _capacity_front)(v);  \
  }                                                                     \
									\
  static inline bool							\
  M_C(name, _empty_p)(const deque_t d)					\
  {									\
    DEQUEI_CONTRACT(d);							\
    return d->count == 0;						\
  }									\
									\
  static inline void							\
  M_C(name, _it)(it_t it, const deque_t d)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (it != NULL);						\
    it->node  = d->front->node;						\
    it->index = d->front->index;					\
    it->deque = d;							\
  }									\
									\
  static inline void							\
  M_C(name, _it_last)(it_t it, const deque_t d)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (it != NULL);						\
    it->node  = d->back->node;						\
    it->index = d->back->index - 1;					\
    it->deque = d;							\
    if (M_UNLIKELY (it->index >= it->node->size)) {			\
      it->node = M_C(name, _node_list_previous_obj)(d->list, it->node);	\
      assert (it->node != NULL);					\
      it->index = it->node->size-1;					\
    }									\
  }									\
									\
  static inline void							\
  M_C(name, _it_end)(it_t it, const deque_t d)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (it != NULL);						\
    it->node  = d->back->node;						\
    it->index = d->back->index;						\
    it->deque = d;							\
  }									\
									\
  static inline void							\
  M_C(name, _it_set)(it_t it1, const it_t it2)				\
  {									\
    assert (it1 != NULL);						\
    assert (it2 != NULL);						\
    it1->node  = it2->node;						\
    it1->index = it2->index;						\
    it1->deque = it2->deque;						\
  }									\
									\
  static inline bool							\
  M_C(name, _end_p)(it_t it)						\
  {									\
    assert (it != NULL);						\
    return (it->node == it->deque->back->node				\
	    && it->index >= it->deque->back->index)			\
      || (it->node == it->deque->front->node				\
	  && it->index < it->deque->front->index);			\
  }									\
									\
  static inline void							\
  M_C(name, _next)(it_t it)						\
  {									\
    assert (it != NULL);						\
    M_C(name, _node_t) *n = it->node;                                   \
    it->index ++;							\
    if (M_UNLIKELY (it->index >= n->size)) {				\
      n = M_C(name, _node_list_next_obj)(it->deque->list, n);           \
      if (M_UNLIKELY (n == NULL)) {					\
	/* Point to 'end' (can't undo it) */				\
	it->node  = it->deque->back->node;				\
	it->index = it->deque->back->node->size;			\
	return;								\
      }									\
      it->node = n;							\
      it->index = 0;							\
    }									\
  }									\
									\
  static inline void							\
  M_C(name, _previous)(it_t it)						\
  {									\
    assert (it != NULL);						\
    M_C(name, _node_t) *n = it->node;                                   \
    it->index --;							\
    if (M_UNLIKELY (it->index >= n->size)) {				\
      n = M_C(name, _node_list_previous_obj)(it->deque->list, n);       \
      if (M_UNLIKELY (n == NULL)) {					\
	/* Point to 'end' (can't undo it) */				\
	it->node  = it->deque->back->node;				\
	it->index = it->deque->back->node->size;			\
	return;								\
      }									\
      it->node = n;							\
      it->index = 0;							\
    }									\
  }									\
									\
  static inline bool							\
  M_C(name, _last_p)(it_t it)						\
  {									\
    assert (it != NULL);						\
    it_t it2;								\
    M_C(name, _it_set)(it2, it);					\
    M_C(name, _next)(it2);						\
    return M_C(name, _end_p)(it2);					\
  }									\
									\
  static inline bool							\
  M_C(name, _it_equal_p)(it_t it1, const it_t it2)			\
  {									\
    assert (it1 != NULL);						\
    assert (it2 != NULL);						\
    return it1->deque == it2->deque					\
      && it1->node == it2->node						\
      && it1->index == it2->index;					\
  }									\
									\
  static inline type *							\
  M_C(name, _ref)(it_t it)						\
  {									\
    assert (it != NULL);						\
    assert (it->index < it->node->size);				\
    return &it->node->data[it->index];					\
  }									\
									\
  static inline type const *						\
  M_C(name, _cref)(it_t it)						\
  {									\
    assert (it != NULL);						\
    assert (it->index < it->node->size);				\
    return M_CONST_CAST(type, &it->node->data[it->index]);		\
  }									\
									\
  static inline void							\
  M_C(name, _init_set)(deque_t d, const deque_t src)			\
  {									\
    DEQUEI_CONTRACT(src);						\
    assert (d != NULL);							\
    M_C(name, _node_list_init)(d->list);				\
    d->default_size = DEQUEUI_DEFAULT_SIZE + src->count;		\
    d->count        = src->count;					\
    M_C(name, _node_t) *n = M_C(name, _int_new_node)(d);                \
    if (n == NULL) return;						\
    d->default_size /= 2;						\
    M_C(name, _node_list_push_back)(d->list, n);                        \
    d->front->node  = n;						\
    d->front->index = DEQUEUI_DEFAULT_SIZE/2;				\
    d->back->node   = n;						\
    d->back->index  = DEQUEUI_DEFAULT_SIZE/2 + src->count;		\
    it_t it;								\
    size_t i = DEQUEUI_DEFAULT_SIZE/2;					\
    for(M_C(name, _it)(it, src); !M_C(name, _end_p)(it) ; M_C(name, _next)(it)) { \
      type const *obj = M_C(name, _cref)(it);				\
      M_CALL_INIT_SET(oplist, n->data[i], *obj);                        \
      i++;								\
      assert (i <= d->back->index);					\
    }									\
    DEQUEI_CONTRACT(d);							\
  }									\
									\
  static inline void							\
  M_C(name, _set)(deque_t d, deque_t const src)				\
  {									\
    if (M_UNLIKELY (src == d))						\
      return;								\
    /* TODO: Reuse memory of d! */					\
    M_C(name, _clear)(d);						\
    M_C(name, _init_set)(d, src);					\
  }									\
									\
  static inline void							\
  M_C(name, _init_move)(deque_t d, deque_t src)				\
  {									\
    DEQUEI_CONTRACT(src);						\
    assert (d!= NULL);							\
    M_C(name,_node_list_init_move)(d->list, src->list);                 \
    d->front->node  = src->front->node;                                 \
    d->front->index = src->front->index;                                \
    d->back->node   = src->back->node;                                  \
    d->back->index  = src->back->index;                                 \
    d->default_size = src->default_size;                                \
    d->count        = src->count;                                       \
    memset(src, 0, sizeof(deque_t));					\
    DEQUEI_CONTRACT(d);							\
  }									\
									\
  static inline void							\
  M_C(name, _move)(deque_t d, deque_t src)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    DEQUEI_CONTRACT(src);						\
    M_C(name, _clear)(d);						\
    M_C(name, _init_move)(d, src);					\
    DEQUEI_CONTRACT(d);							\
  }									\
									\
  static inline void							\
  M_C(name, _swap)(deque_t d, deque_t e)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    DEQUEI_CONTRACT(e);							\
    M_C(name, _node_list_swap) (d->list, e->list);                      \
    M_SWAP(node_t *, d->front->node, e->front->node);			\
    M_SWAP(node_t *, d->back->node, e->back->node);			\
    M_SWAP(size_t, d->front->index, e->front->index);			\
    M_SWAP(size_t, d->back->index, e->back->index);			\
    M_SWAP(size_t, d->default_size, e->default_size);			\
    M_SWAP(size_t, d->count, e->count);					\
    DEQUEI_CONTRACT(d);							\
    DEQUEI_CONTRACT(e);							\
  }									\
									\
  static inline type*							\
  M_C(name, _get)(deque_t d, size_t key)				\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (key < d->count);						\
    const size_t index0 = d->front->index;				\
    size_t count = 0;							\
    /* This loop is in log(N) since the size increase exponentially.*/	\
    for(M_C(name, _node_t) *n = d->front->node;                         \
	n != NULL ;                                                     \
	n = (n == d->back->node) ? NULL :                               \
          M_C(name, _node_list_next_obj)(d->list, n) ){                 \
      if (index0 + key < count + n->size) {				\
	return &n->data[index0 + key - count];				\
      }									\
      count += n->size;							\
    }									\
    assert(false);							\
    return NULL;							\
  }									\
									\
  static inline type const *						\
  M_C(name, _cget)(deque_t d, size_t key)				\
  {									\
    return M_CONST_CAST(type, M_C(name, _get)(d, key));			\
  }									\
									\
  static inline void							\
  M_C(name, _set_at)(deque_t d, size_t key, type const x)		\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (key < d->count);						\
    type *p = M_C(name, _get)(d, key);					\
    M_CALL_SET(oplist, *p, x);						\
  }									\
									\
  M_IF_METHOD(EQUAL, oplist)(						\
  static inline bool				                        \
  M_C(name, _equal_p)(const deque_t d1, const deque_t d2)		\
  {									\
    DEQUEI_CONTRACT(d1);						\
    DEQUEI_CONTRACT(d2);						\
    if (d1->count != d2->count)						\
      return false;							\
    it_t it1;								\
    it_t it2;								\
    for(M_C(name, _it)(it1, d1), M_C(name,_it)(it2, d2);		\
	!M_C(name, _end_p)(it1) ;					\
	M_C(name, _next)(it1), M_C(name, _next)(it2)) {			\
      type const *obj1 = M_C(name, _cref)(it1);				\
      type const *obj2 = M_C(name, _cref)(it2);				\
      if (M_CALL_EQUAL(oplist, *obj1, *obj2) == false)			\
	return false;							\
    }									\
    assert (M_C(name, _end_p)(it2));					\
    return true;							\
  }									\
  , /* NO EQUAL */)							\
									\
  M_IF_METHOD(HASH, oplist)(						\
  static inline size_t				                        \
  M_C(name, _hash)(const deque_t d)					\
  {									\
    DEQUEI_CONTRACT(d);							\
    M_HASH_DECL(hash);							\
    it_t it;								\
    for(M_C(name, _it)(it, d); !M_C(name, _end_p)(it); M_C(name, _next)(it)) { \
      type const *obj = M_C(name, _cref)(it);				\
      M_HASH_UP (hash, M_CALL_HASH(oplist, *obj));			\
    }									\
    return M_HASH_FINAL(hash);						\
  }									\
  , /* NO HASH */)							\
									\
  M_IF_METHOD(SWAP, oplist)(			                        \
  static inline void				                        \
  M_C(name, _swap_at)(deque_t d, size_t i, size_t j)			\
  {									\
    DEQUEI_CONTRACT(d);							\
    assert (i < d->count);						\
    assert (j < d->count);						\
    type *obj1 = M_C(name, _get)(d, i);					\
    type *obj2 = M_C(name, _get)(d, j);					\
    M_CALL_SWAP(oplist, *obj1, *obj2);					\
    DEQUEI_CONTRACT(d);							\
  }									\
  , /* NO SWAP */)                                                      \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _get_str)(string_t str, deque_t const deque, bool append)	\
  {                                                                     \
    STRINGI_CONTRACT(str);                                              \
    DEQUEI_CONTRACT(deque);                                             \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    it_t it;                                                            \
    for (M_C(name, _it)(it, deque) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      type const *item = M_C(name, _cref)(it);				\
      M_CALL_GET_STR(oplist, str, *item, true);                         \
      if (!M_C(name, _last_p)(it))					\
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
    }                                                                   \
    string_push_back (str, ']');                                        \
    STRINGI_CONTRACT(str);                                              \
  }                                                                     \
  , /* no GET_STR */ )                                                  \
			      						\
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *file, const deque_t deque)			\
  {                                                                     \
    DEQUEI_CONTRACT(deque);                                             \
    assert (file != NULL);                                              \
    fputc ('[', file);                                                  \
    it_t it;                                                            \
    for (M_C(name, _it)(it, deque) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)) {                                        \
      type const *item = M_C(name, _cref)(it);				\
      M_CALL_OUT_STR(oplist, file, *item);                              \
      if (!M_C(name, _last_p)(it))					\
        fputc (M_GET_SEPARATOR oplist, file);                           \
    }                                                                   \
    fputc (']', file);                                                  \
  }                                                                     \
  , /* no OUT_STR */ )                                                  \
                                                                        \
  M_IF_METHOD2(PARSE_STR, INIT, oplist)(                                \
  static inline bool                                                    \
  M_C(name, _parse_str)(deque_t deque, const char str[], const char **endp) \
  {                                                                     \
    DEQUEI_CONTRACT(deque);                                             \
    assert (str != NULL);                                               \
    M_C(name,_clean)(deque);						\
    bool success = false;                                               \
    int c = *str++;                                                     \
    if (M_UNLIKELY (c != '[')) goto exit;                               \
    c = *str++;                                                         \
    if (M_UNLIKELY (c == ']')) {success = true; goto exit;}             \
    if (M_UNLIKELY (c == 0)) goto exit;                                 \
    str--;                                                              \
    type item;                                                          \
    M_CALL_INIT(oplist, item);                                          \
    do {                                                                \
      bool b = M_CALL_PARSE_STR(oplist, item, str, &str);               \
      do { c = *str++; } while (isspace(c));                            \
      if (b == false || c == 0) { goto exit; }				\
      M_C(name, _push_back)(deque, item);				\
    } while (c == M_GET_SEPARATOR oplist);				\
    M_CALL_CLEAR(oplist, item);                                         \
    DEQUEI_CONTRACT(deque);                                             \
    success = (c == ']');                                               \
  exit:                                                                 \
    if (endp) *endp = str;                                              \
    return success;                                                     \
  }                                                                     \
  , /* no PARSE_STR */ )                                                \
                                                                        \
  M_IF_METHOD2(IN_STR, INIT, oplist)(                                   \
  static inline bool                                                    \
  M_C(name, _in_str)(deque_t deque, FILE *file)				\
  {                                                                     \
    DEQUEI_CONTRACT(deque);                                             \
    assert (file != NULL);                                              \
    M_C(name,_clean)(deque);						\
    int c = fgetc(file);						\
    if (M_UNLIKELY (c != '[')) return false;                            \
    c = fgetc(file);                                                    \
    if (M_UNLIKELY (c == ']')) return true;                             \
    if (M_UNLIKELY (c == EOF)) return false;                            \
    ungetc(c, file);                                                    \
    type item;                                                          \
    M_CALL_INIT(oplist, item);                                          \
    do {                                                                \
      bool b = M_CALL_IN_STR(oplist, item, file);                       \
      do { c = fgetc(file); } while (isspace(c));                       \
      if (b == false || c == EOF) { break; }				\
      M_C(name, _push_back)(deque, item);				\
    } while (c == M_GET_SEPARATOR oplist);				\
    M_CALL_CLEAR(oplist, item);                                         \
    DEQUEI_CONTRACT(deque);                                             \
    return c == ']';                                                    \
  }                                                                     \
  , /* no IN_STR */ )                                                   \
                                                                        \
  M_IF_METHOD(OUT_SERIAL, oplist)(                                      \
  static inline m_serial_return_code_t                                  \
  M_C(name, _out_serial)(m_serial_write_t f, const deque_t deque)       \
  {                                                                     \
    DEQUEI_CONTRACT(deque);                                             \
    assert (f != NULL && f->interface != NULL);                         \
    m_serial_return_code_t ret;                                         \
    bool first_done = false;                                            \
    ret = f->interface->write_array_start(f, deque->count);             \
    M_C(name, _it_t) it;						\
    for (M_C(name, _it)(it, deque) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      type const *item = M_C(name, _cref)(it);				\
      if (first_done)                                                   \
        ret |= f->interface->write_array_next(f);                       \
      ret |= M_CALL_OUT_SERIAL(oplist, f, *item);                       \
      first_done = true;                                                \
    }                                                                   \
    ret |= f->interface->write_array_end(f);                            \
    return ret & M_SERIAL_FAIL;                                         \
  }                                                                     \
  , /* no OUT_SERIAL */ )                                               \
                                                                        \
  M_IF_METHOD2(IN_SERIAL, INIT, oplist)(                                \
  static inline m_serial_return_code_t                                  \
  M_C(name, _in_serial)(deque_t deque, m_serial_read_t f)               \
  {                                                                     \
    DEQUEI_CONTRACT(deque);                                             \
    assert (f != NULL && f->interface != NULL);                         \
    m_serial_return_code_t ret;                                         \
    size_t estimated_size = 0;                                          \
    M_C(name,_clean)(deque);						\
    ret = f->interface->read_array_start(f, &estimated_size);           \
    if (M_UNLIKELY (ret != M_SERIAL_OK_CONTINUE)) return ret;           \
    type item;                                                          \
    M_CALL_INIT(oplist, item);                                          \
    do {                                                                \
      ret = M_CALL_IN_SERIAL(oplist, item, f);                          \
      if (ret != M_SERIAL_OK_DONE) { break; }				\
      M_C(name, _push_back)(deque, item);				\
    } while ((ret = f->interface->read_array_next(f)) == M_SERIAL_OK_CONTINUE); \
    M_CALL_CLEAR(oplist, item);                                         \
    return ret;                                                         \
  }                                                                     \
  , /* no IN_SERIAL & INIT */ )                                         \
                                                                        \


#define DEQUEI_OPLIST(arg) DEQUEI_OPLIST2 arg

#define DEQUEI_OPLIST2(name, oplist)					\
  (INIT(M_C(name, _init))						\
   ,INIT_SET(M_C(name, _init_set))					\
   ,INIT_WITH(API_1(M_INIT_VAI))                                        \
   ,SET(M_C(name, _set))						\
   ,CLEAR(M_C(name, _clear))						\
   ,INIT_MOVE(M_C(name, _init_move))					\
   ,MOVE(M_C(name, _move))						\
   ,SWAP(M_C(name, _swap))						\
   ,TYPE(M_C(name,_t))							\
   ,SUBTYPE(M_C(name, _type_t))						\
   ,TEST_EMPTY(M_C(name,_empty_p))                                      \
   ,IT_TYPE(M_C(name,_it_t))						\
   ,IT_FIRST(M_C(name,_it))						\
   ,IT_LAST(M_C(name,_it_last))						\
   ,IT_END(M_C(name,_it_end))						\
   ,IT_SET(M_C(name,_it_set))						\
   ,IT_END_P(M_C(name,_end_p))						\
   ,IT_LAST_P(M_C(name,_last_p))					\
   ,IT_EQUAL_P(M_C(name,_it_equal_p))					\
   ,IT_NEXT(M_C(name,_next))						\
   ,IT_PREVIOUS(M_C(name,_previous))					\
   ,IT_REF(M_C(name,_ref))						\
   ,IT_CREF(M_C(name,_cref))						\
   ,CLEAN(M_C(name,_clean))						\
   ,GET_SIZE(M_C(name, _size))                                          \
   ,PUSH(M_C(name,_push_back))						\
   ,POP(M_C(name,_pop_back))						\
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C(name, _get_str)),)		\
   ,M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_C(name, _parse_str)),)   \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C(name, _out_str)),)		\
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C(name, _in_str)),)		\
   ,M_IF_METHOD(OUT_SERIAL, oplist)(OUT_SERIAL(M_C(name, _out_serial)),) \
   ,M_IF_METHOD(IN_SERIAL, oplist)(IN_SERIAL(M_C(name, _in_serial)),)   \
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C(name, _equal_p)),)		\
   ,M_IF_METHOD(HASH, oplist)(HASH(M_C(name, _hash)),)			\
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

#endif
