/*
 * M*LIB - GENINT module
 *
 * Copyright (c) 2017-2018, Patrick Pelissier
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

// TODO: We may want a specialization with constant N value.

typedef unsigned long long genint_limb_t;

typedef struct genint_s {
  size_t n;
  size_t max;
  genint_limb_t mask0; 
  genint_limb_t mask_master;
  atomic_ullong master;
  atomic_ullong *data;
} genint_t[1];

#define GENINT_LIMBSIZE (sizeof(genint_limb_t) * CHAR_BIT)

#define GENINT_CONTRACT(s)                              do {    \
    assert (s != NULL);                                         \
    assert (s->n > 0);                                          \
    assert (s->data != NULL);                                   \
  } while (0)

#define GENINT_ONE  ((genint_limb_t)1)

static inline void genint_init(genint_t s, size_t n)
{
  assert (s != NULL && n > 0 && n <= GENINT_LIMBSIZE * GENINT_LIMBSIZE );
  const size_t alloc = (n + GENINT_LIMBSIZE - 1) / GENINT_LIMBSIZE;
  atomic_ullong *ptr = M_MEMORY_REALLOC (atomic_ullong, NULL, alloc);
  s->n = n;
  s->data = ptr;
  if (M_UNLIKELY (ptr == NULL)) {
    M_MEMORY_FULL(alloc);
    return;
  }
  s->max = alloc-1;
  const size_t index  = s->n % GENINT_LIMBSIZE;
  s->mask0 = (index == 0) ? -GENINT_ONE : ~((GENINT_ONE<<(GENINT_LIMBSIZE-index))-1);
  s->mask_master = ((GENINT_ONE << alloc) - 1) << (GENINT_LIMBSIZE-alloc);
  atomic_init (&s->master, (genint_limb_t)0);
  for(size_t i = 0; i < alloc; i++)
    atomic_init(&s->data[i], (genint_limb_t)0);
  GENINT_CONTRACT(s);
}

static inline void genint_clear(genint_t s)
{
  GENINT_CONTRACT(s);
  M_MEMORY_FREE(s->data);
  s->data = NULL;
}

static inline size_t genint_pop(genint_t s)
{
  GENINT_CONTRACT(s);
  // First read master to see which limb is not full.
  genint_limb_t master = atomic_load(&s->master);
  while (master != s->mask_master) {
    // Let's get the index i of the first not full limb.
    unsigned int i = m_core_clz(~master);
    // An compute the mask of this limb being full
    const size_t mask = (i == s->max) ? s->mask0 : -GENINT_ONE;
    unsigned int bit;
    // Let's load this limb,
    genint_limb_t next, org = atomic_load(&s->data[i]);
    do {
      // If it is now full, we have been prempted by another. Let's restart.
      if (M_UNLIKELY (org == mask))
        goto next_element;
      // At least one bit is free in the limb
      bit = GENINT_LIMBSIZE - 1 - m_core_clz(~org);
      assert ((org & (GENINT_ONE<<bit)) == 0);
      assert (i * GENINT_LIMBSIZE + GENINT_LIMBSIZE - 1 - bit < s->n);
      // Set & reserve the integer as being used.
      next = org | (GENINT_ONE << bit);
    } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
    // If the limb is now full, try to update master
    while (M_UNLIKELY(next == mask
                      && !atomic_compare_exchange_weak (&s->master, &master, master|(GENINT_ONE << (GENINT_LIMBSIZE - 1 - i) ))
                      && ((master & (GENINT_ONE << (GENINT_LIMBSIZE - 1 - i))) == 0) )) {
      // Fail to update. Reload limb to check if it is still full.
      next = atomic_load(&s->data[i]);
    }
    // Return the new number
    GENINT_CONTRACT(s);
    return i * GENINT_LIMBSIZE + GENINT_LIMBSIZE - 1 - bit;
  next_element:
    // Reload master
    master = atomic_load(&s->master);
  }
  GENINT_CONTRACT(s);
  return -(size_t)1; // No more ressource available
}

static inline void genint_push(genint_t s, size_t n)
{
  GENINT_CONTRACT(s);
  assert (n < s->n);
  size_t i    = n / GENINT_LIMBSIZE;
  size_t bit  = GENINT_LIMBSIZE - 1 - (n % GENINT_LIMBSIZE);
  genint_limb_t next, org = atomic_load(&s->data[i]);
  do {
    assert ((org & (GENINT_ONE << bit)) != 0);
    next = org & (~(GENINT_ONE << bit));
  } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
  genint_limb_t master = atomic_load(&s->master);
  genint_limb_t mask_i = GENINT_ONE << (GENINT_LIMBSIZE - 1 - i);
  if (M_UNLIKELY (master & mask_i)) {
    while (!atomic_compare_exchange_weak (&s->master, &master, master & (~mask_i)) && (master & mask_i)) {
      const size_t mask = (i == s->max) ? s->mask0 : -GENINT_ONE;
      org = atomic_load(&s->data[i]);
      if (org == mask)
        return;
    }
  }
  GENINT_CONTRACT(s);
}

#endif
