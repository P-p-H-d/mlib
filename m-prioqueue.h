/*
 * M*LIB - dynamic priority queue module
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
#ifndef __M_PRIOQUEUE_H
#define __M_PRIOQUEUE_H

#include "m-core.h"
#include "m-array.h"

/* Priority queue based on binary heap implementation */

/* Define an prioqueue of a given type.
   USAGE: PRIOQUEUE_DEF(name, type [, oplist_of_the_type]) */
#define PRIOQUEUE_DEF(name, ...)                                            \
  PRIOQUEUEI_DEF(M_IF_NARGS_EQ1(__VA_ARGS__)                                \
  ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), M_C(name,_t) ), \
   (name, __VA_ARGS__,                                      M_C(name,_t))))

/* Define the oplist of an prioqueue of type.
   USAGE: PRIOQUEUE_OPLIST(name[, oplist of the type]) */
#define PRIOQUEUE_OPLIST(...)                                           \
  PRIOQUEUEI_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                         \
                    ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ), \
                     (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

#define PRIOQUEUEI_OPLIST(arg) PRIOQUEUEI_OPLIST2 arg
#define PRIOQUEUEI_DEF(arg)    PRIOQUEUEI_DEF2 arg

#define PRIOQUEUEI_OPLIST2(name, oplist)                                \
  (INIT(M_C(name, _init))						\
   ,INIT_SET(M_C(name, _init_set))					\
   ,SET(M_C(name, _set))						\
   ,CLEAR(M_C(name, _clear))						\
   ,INIT_MOVE(M_C(name, _init_move))					\
   ,MOVE(M_C(name, _move))						\
   ,SWAP(M_C(name, _swap))						\
   ,TYPE(M_C(name,_t))							\
   ,SUBTYPE(M_C(name, _type_t))						\
   ,CLEAN(M_C(name,_clean))						\
   ,PUSH(M_C(name,_push))						\
   ,POP(M_C(name,_pop))                                                 \
   ,OPLIST(oplist)                                                      \
   )

#define PRIOQUEUEI_DEF2(name, type, oplist, prioqueue_t)                \
  ARRAY_DEF(M_C(name, _array), type, oplist)                            \
                                                                        \
  typedef type M_C(name, _type_t);					\
									\
  typedef struct M_C(name, _s) {                                        \
    M_C(name, _array_t) array;                                          \
  } prioqueue_t[1];                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init)(prioqueue_t p)                                       \
  {                                                                     \
    M_C(name, _array_init)(p->array);                                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init_set)(prioqueue_t p, prioqueue_t o)                    \
  {                                                                     \
    M_C(name, _array_init_set)(p->array, o->array);                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _set)(prioqueue_t p, prioqueue_t o)                         \
  {                                                                     \
    M_C(name, _array_set)(p->array, o->array);                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _clear)(prioqueue_t p)                                      \
  {                                                                     \
    M_C(name, _array_clear)(p->array);                                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init_move)(prioqueue_t p, prioqueue_t o)                   \
  {                                                                     \
    M_C(name, _array_init_move)(p->array, o->array);                    \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _move)(prioqueue_t p, prioqueue_t o)                        \
  {                                                                     \
    M_C(name, _array_move)(p->array, o->array);                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _swap)(prioqueue_t p, prioqueue_t o)                        \
  {                                                                     \
    M_C(name, _array_swap)(p->array, o->array);                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _clean)(prioqueue_t p)                                      \
  {                                                                     \
    M_C(name, _array_clean)(p->array);                                  \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _i_parent)(size_t i)                                        \
  {                                                                     \
    assert (i > 0);                                                     \
    return (i - 1) / 2;                                                 \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _i_lchild)(size_t i)                                        \
  {                                                                     \
    assert(i <= ((-(size_t)1)-2)/2);                                    \
    return 2*i + 1;                                                     \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _i_rchild)(size_t i)                                        \
  {                                                                     \
    assert(i <= ((-(size_t)1)-2)/2);                                   \
    return 2*i + 2;                                                     \
  }                                                                     \
                                                                        \
  static inline int                                                     \
  M_C(name, _i_cmp)(prioqueue_t p, size_t i, size_t j)                  \
  {                                                                     \
    return M_GET_CMP oplist (*M_C(name, _array_get)(p->array, i),       \
                             *M_C(name, _array_get)(p->array, j));      \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _empty_p)(prioqueue_t p)                                    \
  {                                                                     \
    return M_C(name, _array_empty_p)(p->array);                         \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _size)(prioqueue_t p)                                       \
  {                                                                     \
    return M_C(name, _array_size)(p->array);                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _push)(prioqueue_t p, type const x)                         \
  {                                                                     \
    M_C(name, _array_push_back)(p->array, x);                           \
                                                                        \
    size_t i = M_C(name, _array_size)(p->array)-1;                      \
    while (i > 0) {                                                     \
      size_t j = M_C(name, _i_parent)(i);                               \
      if (M_C(name, _i_cmp)(p, j, i) <= 0)                              \
        break;                                                          \
      M_C(name, _array_swap_at) (p->array, i, j);                       \
      i = j;                                                            \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C(name, _front)(prioqueue_t p)                                      \
  {                                                                     \
    return M_C(name, _array_get)(p->array, 0);                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _pop)(type *x, prioqueue_t p)                               \
  {                                                                     \
    size_t size = M_C(name, _array_size)(p->array)-1;                   \
    M_C(name, _array_swap_at) (p->array, 0, size);                      \
    M_C(name, _array_pop_back)(x, p->array);                            \
                                                                        \
    size_t i = 0;                                                       \
    while (true) {                                                      \
      size_t child = M_C(name, _i_lchild)(i);                           \
      if (child >= size) break;                                         \
      size_t otherChild = M_C(name, _i_rchild)(i);                      \
      if (otherChild < size                                             \
          && M_C(name, _i_cmp)(p, otherChild, child) < 0 ) {            \
        child = otherChild;                                             \
      }                                                                 \
      if (M_C(name, _i_cmp)(p, i, child) <= 0) break;                   \
      M_C(name, _array_swap_at) (p->array, i, child);                   \
      i = child;                                                        \
    }                                                                   \
  }                                                                     \


#endif
