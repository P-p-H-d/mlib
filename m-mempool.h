/*
 * M*LIB - MEMPOOL module
 *
 * Copyright 2020 - 2020, SP Vladislav Dmitrievich Turbanov
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
#ifndef MSTARLIB_MEMPOOL_H
#define MSTARLIB_MEMPOOL_H

#include "m-core.h"

/* Fast Fixed Size thread unsafe allocator based on memory region.
   USAGE:
     MEMPOOL_DEF(memppol_uint, unsigned int)
     ...
     memppol_uint_t m;
     mempool_uint_init(m);
     unsigned int *ptr = mempool_uint_alloc(m);
     *ptr = 17;
     mempool_uint_free(m, ptr);
     mempool_uint_clear(m); // Give back memory to system
*/
// NOTE: Can not use m-list since it may be expanded from LIST_DEF

#define MEMPOOL_DEF(name, type)                                         \
  									\
  typedef union M_C(name,_union_s) {					\
    type t;                                                             \
    union M_C(name,_union_s) *next;					\
  } M_C(name,_union_t);							\
  									\
  typedef struct M_C(name,_segment_s) {					\
    unsigned int count;                                                 \
    struct M_C(name,_segment_s) *next;					\
    M_C(name,_union_t)	tab[MEMPOOL_MAX_PER_SEGMENT(type)];		\
  } M_C(name,_segment_t);						\
  									\
  typedef struct M_C(name, _s) {					\
    M_C(name,_union_t)   *free_list;					\
    M_C(name,_segment_t) *current_segment;				\
  } M_C(name,_t)[1];							\
									\
  static inline void                                                    \
  M_C(name,M_NAMING_INIT)(M_C(name,_t) mem)                                     \
  {                                                                     \
    mem->free_list = NULL;                                              \
    mem->current_segment = M_MEMORY_ALLOC(M_C(name,_segment_t));	\
    if (mem->current_segment == NULL) {                                 \
      M_MEMORY_FULL(sizeof (M_C(name,_segment_t)));			\
      return;                                                           \
    }                                                                   \
    mem->current_segment->next = NULL;                                  \
    mem->current_segment->count = 0;                                    \
    MEMPOOLI_CONTRACT(mem, type);                                       \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name,M_NAMING_CLEAR)(M_C(name,_t) mem)					\
  {                                                                     \
    MEMPOOLI_CONTRACT(mem, type);                                       \
    M_C(name,_segment_t) *segment = mem->current_segment;		\
    while (segment != NULL) {                                           \
      M_C(name,_segment_t) *next = segment->next;			\
      M_MEMORY_DEL (segment);						\
      segment = next;                                                   \
    }                                                                   \
    /* Clean pointers to be safer */					\
    mem->free_list = NULL;                                              \
    mem->current_segment = NULL;                                        \
  }                                                                     \
  									\
  static inline type *                                                  \
  M_C(name,_alloc)(M_C(name,_t) mem)					\
  {                                                                     \
    MEMPOOLI_CONTRACT(mem, type);                                       \
    M_C(name,_union_t) *ret = mem->free_list;				\
    if (ret != NULL) {                                                  \
      mem->free_list = ret->next;                                       \
      return &ret->t;                                                   \
    }                                                                   \
    M_C(name,_segment_t) *segment = mem->current_segment;		\
    assert(segment != NULL);                                            \
    unsigned int count = segment->count;                                \
    if (M_UNLIKELY (count >= MEMPOOL_MAX_PER_SEGMENT(type))) {		\
      M_C(name,_segment_t) *new_segment = M_MEMORY_ALLOC (M_C(name,_segment_t)); \
      if (M_UNLIKELY (new_segment == NULL)) {				\
        M_MEMORY_FULL(sizeof (M_C(name,_segment_t)));			\
        return NULL;                                                    \
      }                                                                 \
      new_segment->next = segment;                                      \
      new_segment->count = 0;                                           \
      mem->current_segment = new_segment;                               \
      segment = new_segment;                                            \
      count = 0;                                                        \
    }                                                                   \
    ret = &segment->tab[count];                                         \
    segment->count = count + 1;                                         \
    MEMPOOLI_CONTRACT(mem, type);                                       \
    return &ret->t;                                                     \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name,_free)(M_C(name,_t) mem, type *ptr)				\
  {                                                                     \
    MEMPOOLI_CONTRACT(mem, type);                                       \
    /* NOTE: Unsafe cast: suppose that the given pointer                \
       was allocated by the previous alloc function. */                 \
    M_C(name,_union_t) *ret = (M_C(name,_union_t) *)(uintptr_t)ptr;	\
    ret->next = mem->free_list;                                         \
    mem->free_list = ret;                                               \
    MEMPOOLI_CONTRACT(mem, type);                                       \
  }                                                                     \

/* User shall be able to cutomize the size of the segment and/or
   the minimun number of elements.
*/
#ifndef MEMPOOL_MAX_PER_SEGMENT
#define MEMPOOL_MAX_PER_SEGMENT(type)					\
  M_MAX((16*1024-sizeof(unsigned int) - 2*sizeof(void*)) / sizeof (type), 256U)
#endif

/********************************** INTERNAL ************************************/


#define MEMPOOLI_CONTRACT(mempool, type) do {                           \
    assert((mempool) != NULL);                                          \
    assert((mempool)->current_segment != NULL);                         \
    assert((mempool)->current_segment->count <= MEMPOOL_MAX_PER_SEGMENT(type)); \
  } while (0)

#endif
