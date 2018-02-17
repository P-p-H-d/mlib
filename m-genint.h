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
   - an integer can have only one occurrence in the container.
   - you can atomically push in / pop out integer from this container
   provided that they were not already in the container.
   - there are no order (like FIFO or stack)

   This can be used to map integers to index of resources in a table.
   At most we can support N = GENINT_LIMBSIZE * GENINT_LIMBSIZE >= 4096
   due to the master trick. For the typical usage of this container
   (mapping hardware or software limited resources), this should be
   enough.
*/

// TO DO: We may want a specialization with constant N value.

// Define the limb size 
typedef unsigned long long genint_limb_t;

typedef struct genint_s {
  size_t n;                  // size of the container
  size_t max;                // number of allocated limb - 1
  genint_limb_t mask0;       // mask of the last limb
  genint_limb_t mask_master; // mask of master
  atomic_ullong master;      // master bitfield (which informs if a limb is full or not)
  atomic_ullong *data;       // the bitfield which informs if an integer
} genint_t[1];

#define GENINT_LIMBSIZE (sizeof(genint_limb_t) * CHAR_BIT)

#define GENINT_CONTRACT(s)                              do {            \
    assert (s != NULL);                                                 \
    assert (s->n > 0 && s->n <= GENINT_LIMBSIZE * GENINT_LIMBSIZE);     \
    assert ((s->max+1) * GENINT_LIMBSIZE >= s->n);                      \
    assert (s->data != NULL);                                           \
  } while (0)

#define GENINT_ONE  ((genint_limb_t)1)

static inline void genint_init(genint_t s, size_t n)
{
  assert (s != NULL && n > 0 && n <= GENINT_LIMBSIZE * GENINT_LIMBSIZE);
  const size_t alloc = (n + GENINT_LIMBSIZE - 1) / GENINT_LIMBSIZE;
  const size_t index  = n % GENINT_LIMBSIZE;
  atomic_ullong *ptr = M_MEMORY_REALLOC (atomic_ullong, NULL, alloc);
  if (M_UNLIKELY (ptr == NULL)) {
    M_MEMORY_FULL(alloc);
    return;
  }
  s->n = n;
  s->data = ptr;
  s->max = alloc-1;
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

// Typical case: one CAS per pop.
static inline size_t genint_pop(genint_t s)
{
  GENINT_CONTRACT(s);
  // First read master to see which limb is not full.
  genint_limb_t master = atomic_load(&s->master);
  // While master is not full
  while (master != s->mask_master) {
    // Let's get the index i of the first not full limb according to master.
    unsigned int i = m_core_clz(~master);
    // Let's compute the mask of this limb representing the limb as being full
    const size_t mask = (i == s->max) ? s->mask0 : -GENINT_ONE;
    unsigned int bit;
    // Let's load this limb,
    genint_limb_t next, org = atomic_load(&s->data[i]);
    do {
      // If it is now full, we have been preempted by another.
      if (M_UNLIKELY (org == mask))
        goto next_element;
      // At least one bit is free in the limb. Find one.
      bit = GENINT_LIMBSIZE - 1 - m_core_clz(~org);
      assert ((org & (GENINT_ONE<<bit)) == 0);
      assert (i * GENINT_LIMBSIZE + GENINT_LIMBSIZE - 1 - bit < s->n);
      // Set the integer as being used.
      next = org | (GENINT_ONE << bit);
      // Try to reserve the integer 
    } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
    // We have reserved the integer.
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
  return -(size_t)1; // No more resource available
}

// Typical usage: one CAS per push
static inline void genint_push(genint_t s, size_t n)
{
  GENINT_CONTRACT(s);
  assert (n < s->n);
  size_t i    = n / GENINT_LIMBSIZE;
  size_t bit  = GENINT_LIMBSIZE - 1 - (n % GENINT_LIMBSIZE);
  // Load the limb
  genint_limb_t next, org = atomic_load(&s->data[i]);
  do {
    assert ((org & (GENINT_ONE << bit)) != 0);
    // Reset it
    next = org & (~(GENINT_ONE << bit));
    //  Try to unreserve it.
  } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
  genint_limb_t master = atomic_load(&s->master);
  genint_limb_t mask_i = GENINT_ONE << (GENINT_LIMBSIZE - 1 - i);
  // if the limb  was marked as full by master
  if (M_UNLIKELY (master & mask_i)) {
    // Let's try to update master to reflech that limb is not full
    while (!atomic_compare_exchange_weak (&s->master, &master, master & (~mask_i)) && (master & mask_i)) {
      // Fail to update master. Check if limb is still free
      const size_t mask = (i == s->max) ? s->mask0 : -GENINT_ONE;
      org = atomic_load(&s->data[i]);
      if (org == mask)
        // Limb is full once again. Stop
        return;
    }
  }
  GENINT_CONTRACT(s);
}

#endif
