/*
 * M*LIB - GENINT module
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
#ifndef MSTARLIB_GENINT_H
#define MSTARLIB_GENINT_H

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
  unsigned int n;            // size of the container
  unsigned int max;          // number of allocated limb - 1
  genint_limb_t mask0;       // mask of the last limb
  genint_limb_t mask_master; // mask of master
  atomic_ullong master;      // master bitfield (which informs if a limb is full or not)
  atomic_ullong *data;       // the bitfield which informs if an integer
} genint_t[1];

// Define the max supported value
#define GENINT_MAX_ALLOC (GENINT_LIMBSIZE * (GENINT_LIMBSIZE - GENINT_ABA_CPT))

// Define the size of a limb in bits.
#define GENINT_LIMBSIZE (sizeof(genint_limb_t) * CHAR_BIT)

#define GENINT_CONTRACT(s)                              do {            \
    assert (s != NULL);                                                 \
    assert (s->n > 0 && s->n <= GENINT_MAX_ALLOC);                      \
    assert ((s->max+1) * GENINT_LIMBSIZE >= s->n);                      \
    assert (s->data != NULL);                                           \
  } while (0)

#define GENINT_ONE  ((genint_limb_t)1)

// Value returned in case of error.
#define GENINT_ERROR (-1U)

// 32 bits of master are kept for handling the ABA problems.
#define GENINT_ABA_CPT 32
#define GENINT_ABA_CPT_T uint32_t

// Set the bit i of master
#define GENINT_MASTER_SET(master, i)                            \
  ((((master)& (~((GENINT_ONE<< GENINT_ABA_CPT)-1))) | (GENINT_ONE << (GENINT_LIMBSIZE - 1 - i))) \
   |((GENINT_ABA_CPT_T)((master) + 1)))

// Reset the bit i of master
#define GENINT_MASTER_RESET(master, i)                           \
  (((master) & (~((GENINT_ONE<< GENINT_ABA_CPT)-1)) & ~(GENINT_ONE << (GENINT_LIMBSIZE - 1 - i)))       \
   |((GENINT_ABA_CPT_T)((master) + 1)))

static inline void genint_init(genint_t s, unsigned int n)
{
  assert (s != NULL && n > 0 && n <= GENINT_MAX_ALLOC);
  const size_t alloc = (n + GENINT_LIMBSIZE - 1) / GENINT_LIMBSIZE;
  const unsigned int index  = n % GENINT_LIMBSIZE;
  atomic_ullong *ptr = M_MEMORY_REALLOC (atomic_ullong, NULL, alloc);
  if (M_UNLIKELY (ptr == NULL)) {
    M_MEMORY_FULL(alloc);
    return;
  }
  s->n = n;
  s->data = ptr;
  s->max = alloc-1;
  s->mask0 = (index == 0) ? -GENINT_ONE : ~((GENINT_ONE<<(GENINT_LIMBSIZE-index))-1);
  s->mask_master = (((GENINT_ONE << alloc) - 1) << (GENINT_LIMBSIZE-alloc)) >> GENINT_ABA_CPT;
  atomic_init (&s->master, (genint_limb_t)0);
  for(unsigned int i = 0; i < alloc; i++)
    atomic_init(&s->data[i], (genint_limb_t)0);
  GENINT_CONTRACT(s);
}

static inline void genint_clear(genint_t s)
{
  GENINT_CONTRACT(s);
  M_MEMORY_FREE(s->data);
  s->data = NULL;
}

static inline size_t genint_size(genint_t s)
{
  GENINT_CONTRACT(s);
  return s->n;
}

// For a typical case, the amortized cost is one CAS per pop.
static inline unsigned int genint_pop(genint_t s)
{
  GENINT_CONTRACT(s);
  // First read master to see which limb is not full.
  genint_limb_t master = atomic_load(&s->master);
  // While master is not full
  while ((master >> GENINT_ABA_CPT) != s->mask_master) {
    // Let's get the index i of the first not full limb according to master.
    unsigned int i = m_core_clz64(~master);
    assert (i < GENINT_LIMBSIZE);
    // Let's compute the mask of this limb representing the limb as being full
    genint_limb_t mask = s->mask0;
    mask = (i == s->max) ? mask : -GENINT_ONE;
    unsigned int bit;
    // Let's load this limb,
    genint_limb_t next, org = atomic_load(&s->data[i]);
    do {
      // If it is now full, we have been preempted by another.
      if (M_UNLIKELY (org == mask))
        goto next_element;
      assert (org != -GENINT_ONE);
      // At least one bit is free in the limb. Find one.
      bit = GENINT_LIMBSIZE - 1 - m_core_clz64(~org);
      assert (bit < GENINT_LIMBSIZE);
      assert ((org & (GENINT_ONE<<bit)) == 0);
      assert (i * GENINT_LIMBSIZE + GENINT_LIMBSIZE - 1 - bit < s->n);
      // Set the integer as being used.
      next = org | (GENINT_ONE << bit);
      // Try to reserve the integer 
    } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
    // We have reserved the integer.
    // If the limb is now full, try to update master
    if (M_UNLIKELY(next == mask)) {
      while (true) {
        genint_limb_t newMaster;
        if (next == mask) {
          newMaster = GENINT_MASTER_SET(master, i);
        } else {
          newMaster = GENINT_MASTER_RESET(master, i);
        }
        if (atomic_compare_exchange_weak (&s->master, &master, newMaster))
          break;
        // Fail to update. Reload limb to check if it is still full.
        next = atomic_load(&s->data[i]);
      }
    }
    // Return the new number
    GENINT_CONTRACT(s);
    return i * GENINT_LIMBSIZE + GENINT_LIMBSIZE - 1 - bit;
  next_element:
    // Reload master
    master = atomic_load(&s->master);
  }
  GENINT_CONTRACT(s);
  return GENINT_ERROR; // No more resource available
}

// For a typical case, the amortized cost is one CAS per pop.
static inline void genint_push(genint_t s, unsigned int n)
{
  GENINT_CONTRACT(s);
  assert (n < s->n);
  const unsigned int i    = n / GENINT_LIMBSIZE;
  const unsigned int bit  = GENINT_LIMBSIZE - 1 - (n % GENINT_LIMBSIZE);
  genint_limb_t master = atomic_load(&s->master);
  // Load the limb
  genint_limb_t next, org = atomic_load(&s->data[i]);
  do {
    assert ((org & (GENINT_ONE << bit)) != 0);
    // Reset it
    next = org & (~(GENINT_ONE << bit));
    //  Try to unreserve it.
  } while (!atomic_compare_exchange_weak (&s->data[i], &org, next));
  // if the limb  was marked as full by master
  genint_limb_t mask = s->mask0;
  mask = (i == s->max) ? mask : -GENINT_ONE;
  if (M_UNLIKELY (next != mask)) {
    // Let's compute the mask of this limb representing the limb as being full
    // Let's try to update master to say that this limb is not full
    while (true) {
      genint_limb_t newMaster;
      if (next == mask) {
        newMaster = GENINT_MASTER_SET(master, i);
      } else {
        newMaster = GENINT_MASTER_RESET(master, i);
      }
      if (atomic_compare_exchange_weak (&s->master, &master, newMaster))
        break;
      // Fail to update. Reload limb to check if it is still full.
      next = atomic_load(&s->data[i]);
    }
  }
  GENINT_CONTRACT(s);
}

#endif
