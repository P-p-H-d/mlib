/*
 * M*LIB - GENINT module
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
#ifndef __M_GENINT_H
#define __M_GENINT_H

#include "m-core.h"
#include "m-atomic.h"

/* GENINT is a container of integers with the following properties:

   - it stores integer from [0..N) (N is fixed).
   - an integer can have only one occurence in the container
   (makeing the container a kind of permutation of [0..N) )
   - you can atomically push in / pop out integer from this container.

   This can be used to map integers to index of ressources in a table.
*/

typedef unsigned long genint_limb_t;

typedef struct genint_s {
  size_t n;
  atomic_ulong *data;
} genint_t[1];

#define GENINT_LIMBSIZE (sizeof(genint_limb_t) * CHAR_BIT)

#define GENINT_CONTRACT(s)                              do {    \
    assert (s != NULL);                                         \
    assert (s->n > 0);                                          \
    assert (s->data != NULL);                                   \
  } while (0)

#define GENINT_ONE ((genint_limb_t)1)

static inline void genint_init(genint_t s, size_t n)
{
  assert (s != NULL && n > 0);
  const size_t alloc = (n + GENINT_LIMBSIZE - 1) / GENINT_LIMBSIZE;
  atomic_ulong *ptr = M_MEMORY_REALLOC (atomic_ulong, NULL, alloc);
  s->n = n;
  s->data = ptr;
  if (M_UNLIKELY (ptr == NULL)) {
    M_MEMORY_FULL(alloc);
    return;
  }
  GENINT_CONTRACT(s);
}

static inline void genint_clear(genint_t s)
{
  GENINT_CONTRACT(s);
  M_MEMORY_FREE(s->data);
  s->data = NULL;
}

static inline unsigned int m_core_clz(unsigned long limb)
{
  return limb == 0 ? GENINT_LIMBSIZE : __builtin_clzl(limb);
}

static inline size_t genint_pop(genint_t s)
{
  GENINT_CONTRACT(s);
  const size_t max = (s->n + GENINT_LIMBSIZE - 1) / GENINT_LIMBSIZE;
  const size_t index  = s->n % GENINT_LIMBSIZE;
  const genint_limb_t mask0 = (index == 0) ? -GENINT_ONE : (GENINT_ONE<<index)-1;
  // O(n) approach, but with a really low constant factor
  for(size_t i = 0; i < max; i++)
    {
      const size_t mask = (i+1 == max) ? mask0 : -GENINT_ONE;
      unsigned int bit;
      genint_limb_t next, org = atomic_load(&s->data[i]);
      do {
        if (org == mask)
          goto next_element;
        // At least one bit is free in the limb
        bit = GENINT_LIMBSIZE - m_core_clz(org);
        assert ((org & (GENINT_ONE<<bit)) == 0);
        // Set & reserve the integer as being used.
        next = org | (GENINT_ONE << bit);
      } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
      // Return the new number
      GENINT_CONTRACT(s);
      return i * GENINT_LIMBSIZE + bit;
    next_element: (void)1;
    }
  GENINT_CONTRACT(s);
  return -(size_t)1; // No more ressource available
}

static inline void genint_push(genint_t s, size_t n)
{
  GENINT_CONTRACT(s);
  assert (n < s->n);
  size_t i    = n / GENINT_LIMBSIZE;
  size_t bit  = n % GENINT_LIMBSIZE;
  genint_limb_t next, org = atomic_load(&s->data[i]);
  do {
    assert ((org & (GENINT_ONE<<bit)) != 0);
    next = org & (~(GENINT_ONE << bit));
  } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
  GENINT_CONTRACT(s);
}

#endif
