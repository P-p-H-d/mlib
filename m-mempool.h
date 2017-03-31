/*
 * M*LIB - MEMPOOL module
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
#ifndef __M_MEMPOOL_H
#define __M_MEMPOOL_H

#include <stdlib.h>
#include <assert.h>
#include "m-core.h"

/* Fast Fixed Size thread unsafe allocator.
   USAGE:
     MEMPOOL_DEF(uint, unsigned int)
     ...
     memppol_uint_t m;
     mempool_uint_init(m);
     // alloc with mempool_uint_alloc(m);
     // free with mempool_uin_free(m, ptr);
     mempool_uint_clear(m); // Give back memory to system
*/
// NOTE: Can not use m-list since it may be expanded from LIST_DEF
// FIXME: Shall this use standard memory function (malloc) or M_MEMORY_ALLOC?

#define MEMPOOL_DEF(name, type)                                         \
                                                                        \
  typedef union M_C3(mempool_,name,_union_s) {                          \
    type t;                                                             \
    union M_C3(mempool_,name,_union_s) *next;                           \
  } M_C3(mempool_,name,_union_t);                                       \
                                                                        \
  typedef struct M_C3(mempool_,name,_segment_s) {                       \
    unsigned int count;                                                 \
    struct M_C3(mempool_,name,_segment_s) *next;                        \
    M_C3(mempool_,name,_union_t)	tab[];                          \
  } M_C3(mempool_,name,_segment_t);                                     \
                                                                        \
  typedef struct {                                                      \
    M_C3(mempool_,name,_union_t)   *free_list;                          \
    M_C3(mempool_,name,_segment_t) *current_segment;                    \
    size_t             count;                                           \
  } M_C3(mempool_,name,_t)[1];                                          \
                                                                        \
  static inline void                                                    \
  M_C3(mempool_,name,_init)(M_C3(mempool_,name,_t) mem)                 \
  {                                                                     \
    mem->free_list = NULL;                                              \
    const size_t size = sizeof (M_C3(mempool_,name,_segment_t)) +       \
      MEMPOOLI_MAX_PER_SEGMENT(type) * sizeof(M_C3(mempool_,name,_union_t)); \
    mem->current_segment = malloc (size);                               \
    if (mem->current_segment == NULL) {                                 \
      M_MEMORY_FULL(size);                                              \
      return;                                                           \
    }                                                                   \
    mem->current_segment->next = NULL;                                  \
    mem->current_segment->count = 0;                                    \
    mem->count = 0;                                                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(mempool_,name,_clear)(M_C3(mempool_,name,_t) mem)                \
  {                                                                     \
    assert(mem -> count == 0);                                          \
    M_C3(mempool_,name,_segment_t) *segment = mem->current_segment;     \
    while (segment != NULL) {                                           \
      M_C3(mempool_,name,_segment_t) *next = segment->next;             \
      free(segment);                                                    \
      segment = next;                                                   \
    }                                                                   \
    mem->count = 0;                                                     \
    mem->free_list = NULL;                                              \
    mem->current_segment = NULL;                                        \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(mempool_,name,_alloc)(M_C3(mempool_,name,_t) mem)                \
  {                                                                     \
    mem->count++;                                                       \
    M_C3(mempool_,name,_union_t) *ret = mem->free_list;                 \
    if (ret != NULL) {                                                  \
      mem->free_list = ret->next;                                       \
      return &ret->t;                                                   \
    }                                                                   \
    M_C3(mempool_,name,_segment_t) *segment = mem->current_segment;     \
    assert(segment != NULL);                                            \
    unsigned int count = segment->count;                                \
    if (count >= MEMPOOLI_MAX_PER_SEGMENT(type)) {                      \
      const size_t s = sizeof (M_C3(mempool_,name,_segment_t)) +        \
        MEMPOOLI_MAX_PER_SEGMENT(type) * sizeof(M_C3(mempool_,name,_union_t)); \
      M_C3(mempool_,name,_segment_t) *new_segment = malloc (s);         \
      if (new_segment == NULL) {                                        \
        M_MEMORY_FULL(s);                                               \
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
    return &ret->t;                                                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(mempool_,name,_free)(M_C3(mempool_,name,_t) mem, type *ptr)      \
  {                                                                     \
    /* NOTE: Unsafe cast: suppose that the given pointer                \
       was allocated by the previous alloc function. */                 \
    M_C3(mempool_,name,_union_t) *ret = (M_C3(mempool_,name,_union_t) *)ptr; \
    ret->next = mem->free_list;                                         \
    mem->free_list = ret;                                               \
    mem->count--;                                                       \
  }                                                                     \

/********************************** INTERNAL ************************************/

#define MEMPOOLI_MAX_PER_SEGMENT(type) M_MAX(16*1024 / sizeof (type), 256U)

#endif
