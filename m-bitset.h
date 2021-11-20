/*
 * M*LIB - BITSET module
 *
 * Copyright (c) 2017-2021, Patrick Pelissier
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
#ifndef MSTARLIB_BITSET_H
#define MSTARLIB_BITSET_H

#include <stdint.h>
#include "m-core.h"

/********************************** INTERNAL ************************************/

M_BEGIN_PROTECTED_CODE

// Define the basic limb of a bitset
typedef uint64_t bitset_limb_ct;
// And its size in bits
#define M_B1TSET_LIMB_BIT (sizeof(bitset_limb_ct) * CHAR_BIT)

// bitset grow policy. n is limb size
#define M_B1TSET_INC_ALLOC_SIZE(n) ((n) < 4 ? 4 : (n) * 2)

// Compute the number of allocated limbs needed to handle 'n' bits.
#define M_B1TSET_TO_ALLOC(n)       (((n) + M_B1TSET_LIMB_BIT - 1) / M_B1TSET_LIMB_BIT)

// Compute the number of bits available from the allocated size in limbs
#define M_B1TSET_FROM_ALLOC(n)     ((n) * M_B1TSET_LIMB_BIT)

// Contract of a bitset
#define M_B1TSET_CONTRACT(t) do {                                             \
    M_ASSERT (t != NULL);                                                     \
    M_ASSERT (t->size <= M_B1TSET_FROM_ALLOC (t->alloc));                     \
    M_ASSERT (t->alloc <= ((size_t)-1) / M_B1TSET_LIMB_BIT);                  \
    M_ASSERT (t->size < ((size_t)-1) - M_B1TSET_LIMB_BIT);                    \
    M_ASSERT (t->size == 0 || t->ptr != NULL);                                \
    M_ASSERT (t->alloc == 0 || t->ptr != NULL);                               \
    M_ASSERT ((t->size % M_B1TSET_LIMB_BIT) == 0 || (t->ptr[ (t->size-1) / M_B1TSET_LIMB_BIT] & ~(((((bitset_limb_ct)1)<<(t->size % M_B1TSET_LIMB_BIT))<<1)-1)) == 0); \
  } while (0)


/********************************** EXTERNAL ************************************/

/* Define a type of variable 'bits' or array of packed booleans */
typedef struct bitset_s {
  size_t size;            // Size is the number of bits
  size_t alloc;           // Alloc is the number of allocated limbs
  bitset_limb_ct *ptr;    // Pointer to the allocated limbs
} bitset_t[1];

/* Pointer to a bitset_t */
typedef struct bitset_s *bitset_ptr;

/* Constant Pointer to a bitset_t */
typedef const struct bitset_s *bitset_srcptr;

/* Iterator on a bitset */
typedef struct bitset_it_s {
  size_t index;           // index to the array of bit
  bool   value;           // value used for _ref & _cref to store the value
  struct bitset_s *set;   // the associated bitset
} bitset_it_t[1];

/* Initialize a bitset (CONSTRUCTOR) */
static inline void
bitset_init(bitset_t t)
{
  M_ASSERT (t != NULL);
  M_STATIC_ASSERT (M_POWEROF2_P(M_B1TSET_LIMB_BIT), MLIB_INTERNAL, "M*LIB: BITSET LIMB shall be a power of 2.");
  t->size = 0;
  t->alloc = 0;
  t->ptr = NULL;
  M_B1TSET_CONTRACT(t);
}

/* Clean a bitset */
static inline void
bitset_reset(bitset_t t)
{
  M_B1TSET_CONTRACT(t);
  t->size = 0;
}

/* Clear a bitset (DESTRUCTOR) */
static inline void
bitset_clear(bitset_t t)
{
  bitset_reset(t);
  M_MEMORY_FREE(t->ptr);
  // This is not really needed, but is safer
  // This representation is invalid and will be detected by the contract.
  // A C compiler should be able to optimize out theses initializations.
  t->alloc = 1;
  t->ptr = NULL;
}

/* Set a bitset to another one */
static inline void
bitset_set(bitset_t d, const bitset_t s)
{
  M_B1TSET_CONTRACT(d);
  M_B1TSET_CONTRACT(s);
  if (M_UNLIKELY (d == s)) return;
  const size_t needAlloc = M_B1TSET_TO_ALLOC (s->size);
  if (s->size > M_B1TSET_FROM_ALLOC (d->alloc)) {
    bitset_limb_ct *ptr = M_MEMORY_REALLOC (bitset_limb_ct, d->ptr, needAlloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(needAlloc);
      return ;
    }
    d->ptr = ptr;
    d->alloc = needAlloc;
  }
  memcpy (d->ptr, s->ptr, needAlloc * sizeof(bitset_limb_ct) );
  d->size = s->size;
  M_B1TSET_CONTRACT(d);
}

/* Initialize & set a bitset to another one (CONSTRUCTOR) */
static inline void
bitset_init_set(bitset_t d, const bitset_t s)
{
  M_ASSERT (d != s);
  bitset_init(d);
  bitset_set(d, s);
}

/* Initialize & move a bitset (CONSTRUCTOR) from another one (DESTRUCTOR) */
static inline void
bitset_init_move(bitset_t d, bitset_t s)
{
  M_B1TSET_CONTRACT(s);
  d->size  = s->size;
  d->alloc = s->alloc;
  d->ptr   = s->ptr;
  // Illegal representation of a bitset, to be detectable
  s->alloc = 1;
  s->ptr = NULL;
  M_B1TSET_CONTRACT(d);  
}

/* Move a bitset from another one (DESTRUCTOR) */
static inline void
bitset_move(bitset_t d, bitset_t s)
{
  bitset_clear(d);
  bitset_init_move (d, s);
}

/* Set the bit 'i' in the bitset to the value 'x' */
static inline void
bitset_set_at(bitset_t v, size_t i, bool x)
{
  M_B1TSET_CONTRACT(v);
  M_ASSERT (v->ptr != NULL);
  M_ASSERT_INDEX(i, v->size);
  const size_t offset = i / M_B1TSET_LIMB_BIT;
  const size_t index  = i % M_B1TSET_LIMB_BIT;
  // This is a branchless version as x can only be 0 or 1 with only one variable shift.
  const bitset_limb_ct mask = ((bitset_limb_ct)1)<<index;
  v->ptr[offset] = (v->ptr[offset] & ~mask) | (mask & (0-(bitset_limb_ct)x));
  M_B1TSET_CONTRACT (v);
}

/* Flip the bit 'i' in the bitset */
static inline void
bitset_flip_at(bitset_t v, size_t i)
{
  M_B1TSET_CONTRACT(v);
  M_ASSERT (v->ptr != NULL);
  M_ASSERT_INDEX(i, v->size);
  size_t offset = i / M_B1TSET_LIMB_BIT;
  size_t index  = i % M_B1TSET_LIMB_BIT;
  v->ptr[offset] ^= ((bitset_limb_ct)1)<<index;
  M_B1TSET_CONTRACT (v);
}

/* Push back the boolean 'x' in the bitset (increasing the bitset) */
static inline void
bitset_push_back (bitset_t v, bool x)
{
  M_B1TSET_CONTRACT (v);
  if (M_UNLIKELY (v->size >= M_B1TSET_FROM_ALLOC (v->alloc))) {
    // Compute the needed allocation.
    const size_t needAlloc = M_B1TSET_INC_ALLOC_SIZE(v->alloc);
    // Check for integer overflow
    if (M_UNLIKELY (needAlloc <= v->alloc)) {
      M_MEMORY_FULL(needAlloc * sizeof(bitset_limb_ct));
      return;
    }
    // Alloc memory
    bitset_limb_ct *ptr = M_MEMORY_REALLOC (bitset_limb_ct, v->ptr, needAlloc);
    // Check if success
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(needAlloc * sizeof(bitset_limb_ct));
      return;
    }
    v->ptr = ptr;
    v->alloc = needAlloc;
  }
  M_ASSERT(v->ptr != NULL);

  const size_t i = v->size;
  const size_t offset = i / M_B1TSET_LIMB_BIT;
  const size_t index  = i % M_B1TSET_LIMB_BIT;
  if (M_UNLIKELY(index == 0)) {
    // A new limb if used. Clear it before using it.
    v->ptr[offset] = 0;
  }
  // This is a branchless version as x can only be 0 or 1 with only one variable shift.
  const bitset_limb_ct mask = ((bitset_limb_ct)1)<<index;
  v->ptr[offset] = (v->ptr[offset] & ~mask) | (mask & (0-(bitset_limb_ct)x));
  v->size ++;
  M_B1TSET_CONTRACT (v);
}

/* Resize the bitset to have exactly 'size' bits */
static inline void
bitset_resize (bitset_t v, size_t size)
{
  M_B1TSET_CONTRACT (v);
  // Check for overflow
  if (M_UNLIKELY (size >= ((size_t)-1) - M_B1TSET_LIMB_BIT)) {
    M_MEMORY_FULL((size_t) -1);
    return;
  }
  // Compute the needed allocation.
  size_t newAlloc = M_B1TSET_TO_ALLOC (size);
  if (newAlloc > v->alloc) {
    // Allocate more limbs to store the bitset.
    bitset_limb_ct *ptr = M_MEMORY_REALLOC (bitset_limb_ct, v->ptr, newAlloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(newAlloc * sizeof(bitset_limb_ct));
      return;
    }
    v->ptr = ptr;
    v->alloc = newAlloc;
  }
  // Resize the bitsets
  const size_t old_size = v->size;
  const size_t offset = size / M_B1TSET_LIMB_BIT;
  const size_t index  = size % M_B1TSET_LIMB_BIT;
  const bitset_limb_ct mask = (((bitset_limb_ct)1)<<index)-1;
  if (size < old_size) {
    // Resize down the bitset: clear unused bits
    if (M_LIKELY(index != 0)) {
      // Mask the last limb to clear the last bits
      v->ptr[offset] &= mask;
    }
  } else if (size > old_size) {
    // Resize up the bitset: set to 0 new bits.
    const size_t old_offset = old_size / M_B1TSET_LIMB_BIT;
    for(size_t i = old_offset+1 ; i < offset; i++) {
      v->ptr[i] = 0;
    }
    if (M_LIKELY(index != 0)) {
      // Mask the last limb to clear the last bits
      v->ptr[offset] = 0;
    }
  }
  v->size = size;
  M_B1TSET_CONTRACT (v);
}

/* Reserve allocation in the bitset to accomodate at least 'size' bits without reallocation */
static inline void
bitset_reserve (bitset_t v, size_t alloc)
{
  M_B1TSET_CONTRACT (v);
  size_t oldAlloc = M_B1TSET_TO_ALLOC (v->size);
  size_t newAlloc = M_B1TSET_TO_ALLOC (alloc);
  // We refuse to reduce allocation below current size
  if (oldAlloc > newAlloc) {
    newAlloc = oldAlloc;
  }
  if (M_UNLIKELY (newAlloc == 0)) {
    // Free all memory used by the bitsets
    M_MEMORY_FREE (v->ptr);
    v->size = v->alloc = 0;
    v->ptr = NULL;
  } else {
    // Allocate more memory or reduce memory usage
    bitset_limb_ct *ptr = M_MEMORY_REALLOC (bitset_limb_ct, v->ptr, newAlloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(newAlloc * sizeof(bitset_limb_ct));
      return;
    }
    v->ptr = ptr;
    v->alloc = newAlloc;
  }
  M_B1TSET_CONTRACT (v);
}

/* Return the value of the boolean at index 'i'.    
 * NOTE: Interface is a little bit different:
 * It doesn't return a pointer to the data, but the data itself.
 */
static inline bool
bitset_get(const bitset_t v, size_t i)
{
  M_B1TSET_CONTRACT(v);
  M_ASSERT (v->ptr != NULL);
  M_ASSERT_INDEX(i, v->size);
  size_t offset = i / M_B1TSET_LIMB_BIT;
  size_t index  = i % M_B1TSET_LIMB_BIT;
  return ( v->ptr[offset] & (((bitset_limb_ct)1) << index) ) != 0;
}

/* bitset_cget is the exact same service than bitset_get */
#define bitset_cget bitset_get

/* Pop back the last bit in the bitset */
static inline void
bitset_pop_back(bool *dest, bitset_t v)
{
  M_B1TSET_CONTRACT (v);
  M_ASSERT_INDEX (0, v->size);
  // Remove one item from the bitset
  v->size--;
  // Prepare clearing popped bit
  const size_t offset = v->size / M_B1TSET_LIMB_BIT;
  const size_t index  = v->size % M_B1TSET_LIMB_BIT;
  const bitset_limb_ct mask = ((bitset_limb_ct)1)<<index;
  if (dest) {
    // Read popped bit
    *dest = (v->ptr[offset] & mask) != 0;
  }
  v->ptr[offset] &= mask-1;
  M_B1TSET_CONTRACT (v);
}

/* Return the front bit value in the bitset */
static inline bool
bitset_front(bitset_t v)
{
  M_B1TSET_CONTRACT (v);
  M_ASSERT_INDEX (0, v->size);
  return bitset_get(v, 0);
}

/* Return the back bit value in the bitset */
static inline bool
bitset_back(bitset_t v)
{
  M_B1TSET_CONTRACT (v);
  M_ASSERT_INDEX (0, v->size);
  return bitset_get(v, v->size-1);
}

/* Test if the bitset is empty (no bits stored)*/
static inline bool
bitset_empty_p(bitset_t v)
{
  M_B1TSET_CONTRACT (v);
  return v->size == 0;
}

/* Return the number of bits of the bitset */
static inline size_t
bitset_size(bitset_t v)
{
  M_B1TSET_CONTRACT (v);
  return v->size;
}

/* Return the capacity in limbs of the bitset */
static inline size_t
bitset_capacity(bitset_t v)
{
  M_B1TSET_CONTRACT (v);
  return M_B1TSET_FROM_ALLOC (v->alloc);
}

/* Swap the bit at index i and j of the bitset */
static inline void
bitset_swap_at (bitset_t v, size_t i, size_t j)
{
  M_ASSERT_INDEX(i, v->size);
  M_ASSERT_INDEX(j, v->size);

  bool i_val = bitset_get(v, i);
  bool j_val = bitset_get(v, j);
  bitset_set_at (v, i, j_val);
  bitset_set_at (v, j, i_val);
}

/* Swap the bitsets */
static inline void
bitset_swap (bitset_t v1, bitset_t v2)
{
  M_B1TSET_CONTRACT (v1);
  M_B1TSET_CONTRACT (v2);
  M_SWAP (size_t, v1->size, v2->size);
  M_SWAP (size_t, v1->alloc, v2->alloc);
  M_SWAP (bitset_limb_ct *, v1->ptr, v2->ptr);
  M_B1TSET_CONTRACT (v1);
  M_B1TSET_CONTRACT (v2);
}

/* (INTERNAL) Left shift of the bitset (ptr+size) by 1 bit,
 * integrating the carry in the lowest position.
 * Return the new carry.
 */
static inline bitset_limb_ct
m_b1tset_lshift(bitset_limb_ct ptr[], size_t n, bitset_limb_ct carry)
{
  for(size_t i = 0; i < n; i++) {
    bitset_limb_ct v = ptr[i];
    ptr[i] = (v << 1) | carry;
    carry = (v >> (M_B1TSET_LIMB_BIT-1) );
  }
  return carry;
}

/* (INTERNAL) Right shift of the bitset (ptr+size) by 1 bit,
 * integrating the carry in the lowest position.
 * Return the new carry.
 */
static inline bitset_limb_ct
m_b1tset_rshift(bitset_limb_ct ptr[], size_t n, bitset_limb_ct carry)
{
  for(size_t i = n - 1; i < n; i--) {
    bitset_limb_ct v = ptr[i];
    ptr[i] = (v >> 1) | (carry << (M_B1TSET_LIMB_BIT-1) );
    carry = v & 1;
  }
  return carry;
}

/* Insert a new bit at position 'key' of value 'value' in the bitset 'set'
  shifting the set accordingly */
static inline void
bitset_push_at(bitset_t set, size_t key, bool value)
{
  M_B1TSET_CONTRACT (set);
  // First push another value to extend the array to the right size
  bitset_push_back(set, false);
  M_ASSERT (set->ptr != NULL);
  M_ASSERT_INDEX(key, set->size);

  // Then shift it
  size_t offset = key / M_B1TSET_LIMB_BIT;
  size_t index  = key % M_B1TSET_LIMB_BIT;
  bitset_limb_ct v = set->ptr[offset];
  bitset_limb_ct mask = (((bitset_limb_ct)1)<<index)-1;
  bitset_limb_ct carry = (v >> (M_B1TSET_LIMB_BIT-1) );
  v = (v & mask) | ((unsigned int) value << index) | ((v & ~mask) << 1);
  set->ptr[offset] = v;
  size_t size = (set->size + M_B1TSET_LIMB_BIT - 1) / M_B1TSET_LIMB_BIT;
  M_ASSERT (size >= offset + 1);
  v = m_b1tset_lshift(&set->ptr[offset+1], size - offset - 1, carry);
  // v is unused as it should be zero.
  M_ASSERT(v == 0);
  (void) v;
  M_B1TSET_CONTRACT (set);
}

/* Pop a new bit at position 'key' in the bitset 
 * and return in *dest its value if *dest exists */
static inline void
bitset_pop_at(bool *dest, bitset_t set, size_t key)
{
  M_B1TSET_CONTRACT (set);
  M_ASSERT (set->ptr != NULL);
  M_ASSERT_INDEX(key, set->size);

   if (dest) {
     *dest = bitset_get (set, key);
   }
   // Shift it
   size_t offset = key / M_B1TSET_LIMB_BIT;
   size_t index  = key % M_B1TSET_LIMB_BIT;
   size_t size = (set->size + M_B1TSET_LIMB_BIT - 1) / M_B1TSET_LIMB_BIT;
   bitset_limb_ct v, mask, carry;
   carry = m_b1tset_rshift(&set->ptr[offset+1], size - offset - 1, false);
   v = set->ptr[offset];
   mask = (((bitset_limb_ct)1)<<index)-1;
   v = (v & mask) | ((v>>1) & ~mask) | (carry << (M_B1TSET_LIMB_BIT-1)) ;
   set->ptr[offset] = v;
   // Decrease size
   set->size --;
   M_B1TSET_CONTRACT (set);
}

/* Test if two bitsets are equal */
static inline bool
bitset_equal_p (const bitset_t set1, const bitset_t set2)
{
  M_B1TSET_CONTRACT (set1);
  M_B1TSET_CONTRACT (set2);
  if (set1->size != set2->size)
    return false;
  /* We won't compare each bit individualy,
     but instead compare them per limb */
  const size_t limbSize = (set1->size + M_B1TSET_LIMB_BIT -1) / M_B1TSET_LIMB_BIT;
  for(size_t i = 0 ; i < limbSize;i++)
    if (set1->ptr[i] != set2->ptr[i])
      return false;
  return true;
}

/* Initialize an iterator to the first bit of the biset */
static inline void
bitset_it(bitset_it_t it, bitset_t set)
{
   M_B1TSET_CONTRACT (set);
   it->index = 0;
   it->set = set;
}

/* Initialize an iterator to reference the same bit as the given one*/
static inline void
bitset_it_set(bitset_it_t it, const bitset_it_t itorg)
{
  M_ASSERT (it != NULL && itorg != NULL);
  it->index = itorg->index;
  it->set = itorg->set;
}

/* Initialize an iterator to reference the last bit of the bitset*/
static inline void
bitset_it_last(bitset_it_t it, bitset_t set)
{
   M_B1TSET_CONTRACT (set);
   it->index = set->size-1;
   it->set = set;
}

/* Initialize an iterator to reference no valid bit of the bitset*/
static inline void
bitset_it_end(bitset_it_t it, bitset_t set)
{
   M_B1TSET_CONTRACT (set);
   it->index = set->size;
   it->set = set;
}

/* Test if an iterator references no valid bit of the bitset anymore */
static inline bool
bitset_end_p(const bitset_it_t it)
{
  M_ASSERT (it != NULL && it->set != NULL);
  return (it->index) >= (it->set->size);
}

/* Test if an iterator references the last (or end) bit of the bitset anymore */
static inline bool
bitset_last_p(const bitset_it_t it)
{
  M_ASSERT (it != NULL && it->set != NULL);
  /* NOTE: Can not compute 'size-1' due to potential overflow
     if size is 0 */
  return (it->index+1) >= (it->set->size);
}

/* Test if both iterators reference the same bit */
static inline bool
bitset_it_equal_p(const bitset_it_t it1, const bitset_it_t it2)
{
  M_ASSERT (it1 != NULL && it2 != NULL);
  return it1->index == it2->index && it1->set == it2->set;
}

/* Move the iterator to the next bit */
static inline void
bitset_next(bitset_it_t it)
{
  M_ASSERT (it != NULL && it->set != NULL);
  it->index++;
}

/* Move the iterator to the previous bit */
static inline void
bitset_previous(bitset_it_t it)
{
  M_ASSERT (it != NULL && it->set != NULL);
  it->index--;
}

// There is no _ref as it is not possible to modify the value using the IT interface

/* Return a pointer to the bit referenced by the iterator 
 * Only one reference is possible at a time per iterator */
static inline const bool *
bitset_cref(bitset_it_t it)
{
  M_ASSERT (it != NULL && it->set != NULL);
  it->value = bitset_get(it->set, it->index);
  return &it->value;
}

/* Output the bitset as a formatted text in a FILE */
static inline void
bitset_out_str(FILE *file, const bitset_t set)
{
  M_B1TSET_CONTRACT (set);
  M_ASSERT(file != NULL);
  fputc ('[', file);
  for(size_t i = 0; i < set->size; i++) {
    const bool b = bitset_get (set, i);
    const char c = b ? '1' : '0';
    fputc (c, file);
  }
  fputc (']', file);
}

/* Input the bitset from a formatted text in a FILE */
static inline bool
bitset_in_str(bitset_t set, FILE *file)
{
  M_B1TSET_CONTRACT (set);
  M_ASSERT(file != NULL);
  bitset_reset(set);
  int c = fgetc(file);
  if (M_UNLIKELY (c != '[')) return false;
  c = fgetc(file);
  while (c == '0' || c == '1') {
    const bool b = (c == '1');
    bitset_push_back (set, b);
    c = fgetc(file);
  }
  M_B1TSET_CONTRACT (set);
  return c == ']';
}

/* Parse the bitset from a formatted text in a C string */
static inline bool
bitset_parse_str(bitset_t set, const char str[], const char **endptr)
{
  M_B1TSET_CONTRACT (set);
  M_ASSERT(str != NULL);
  bool success = false;
  bitset_reset(set);
  char c = *str++;
  if (M_UNLIKELY(c != '[')) goto exit;
  c = *str++;
  do {
    if (M_UNLIKELY(c != '0' && c != '1')) goto exit;
    const bool b = (c == '1');
    bitset_push_back (set, b);
    c = *str++;
  } while (c != ']' && c != 0);
  M_B1TSET_CONTRACT (set);
  success = (c == ']');
 exit:
  if (endptr) *endptr = str;
  return success;
}

/* Set the bitset from a formatted text in a C string */
static inline bool
bitset_set_str(bitset_t dest, const char str[])
{
  return bitset_parse_str(dest, str, NULL);
}

/* Perform an AND operation between the bitsets,
 * up to the minimum size of both bitsets */
static inline void
bitset_and(bitset_t dest, const bitset_t src)
{
  M_B1TSET_CONTRACT(dest);
  M_B1TSET_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = (s + M_B1TSET_LIMB_BIT -1) / M_B1TSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] &= src->ptr[i];
  // Reduce the dest size to the minimum size between both
  dest->size = s;
  M_B1TSET_CONTRACT(dest);
}

/* Perform an OR operation between the bitsets,
 * up to the minimum size of both bitsets */
static inline void
bitset_or(bitset_t dest, const bitset_t src)
{
  M_B1TSET_CONTRACT(dest);
  M_B1TSET_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = (s + M_B1TSET_LIMB_BIT - 1) / M_B1TSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] |= src->ptr[i];
  // Reduce the dest size to the minimum size between both
  dest->size = s;
  M_B1TSET_CONTRACT(dest);
}

/* Perform an XOR operation between the bitsets,
 * up to the minimum size of both bitsets */
static inline void
bitset_xor(bitset_t dest, const bitset_t src)
{
  M_B1TSET_CONTRACT(dest);
  M_B1TSET_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = s / M_B1TSET_LIMB_BIT;
  size_t m = s % M_B1TSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] ^= src->ptr[i];
  if (M_LIKELY(m)) {
    // Last limb needs to be masked too
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    dest->ptr[n] = (dest->ptr[n] ^ src->ptr[n]) & mask;
  }
  // Reduce the dest size to the minimum size between both
  dest->size = s;
  M_B1TSET_CONTRACT(dest);
}

/* Perform a NOT operation of the bitset */
static inline void
bitset_not(bitset_t dest)
{
  M_B1TSET_CONTRACT(dest);
  size_t s = dest->size;
  size_t n = s / M_B1TSET_LIMB_BIT;
  size_t m = s % M_B1TSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] = ~ (dest->ptr[i]);
  if (M_LIKELY(m)) {
    // Last limb needs to be masked too
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    dest->ptr[n] = (~ dest->ptr[n]) & mask;
  }
  M_B1TSET_CONTRACT(dest);
}

/* Copute a hash of the bitset */
static inline size_t
bitset_hash(const bitset_t set)
{
  M_B1TSET_CONTRACT(set);
  size_t s = set->size;
  size_t n = (s + M_B1TSET_LIMB_BIT-1) / M_B1TSET_LIMB_BIT;
  M_HASH_DECL(hash);
  for(size_t i = 0 ; i < n; i++)
    M_HASH_UP(hash, set->ptr[i]);
  return M_HASH_FINAL (hash);
}

/* Count the number of leading zero */
static inline size_t
bitset_clz(const bitset_t set)
{
  M_B1TSET_CONTRACT(set);
  size_t s = set->size;
  if (M_UNLIKELY (s == 0)) {
    return 0;
  }
  size_t n = (s -1) / M_B1TSET_LIMB_BIT;
  size_t m = s % M_B1TSET_LIMB_BIT;
  bitset_limb_ct limb = set->ptr[n];
  if (m) {
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    limb &= mask;
  } else {
    m = M_B1TSET_LIMB_BIT;
  }
  s = 0;
  while (limb == 0 && n > 0) {
    s += m;
    limb = set->ptr[--n];
    m = M_B1TSET_LIMB_BIT;
  }
  s += m_core_clz64(limb) - (M_B1TSET_LIMB_BIT - m);
  return s;
}

// For GCC or CLANG or ICC
#if defined(__GNUC__)
static inline size_t m_b1tset_popcount64(bitset_limb_ct limb)
{
  return (size_t) __builtin_popcountll(limb);
}
#else
// MSVC __popcnt64 may not exist on the target architecture (no emulation layer)
// Use emulation layer: https://en.wikipedia.org/wiki/Hamming_weight
static inline size_t m_b1tset_popcount64(bitset_limb_ct limb)
{
  limb = limb - ((limb >> 1) & 0x5555555555555555ULL);
  limb = (limb & 0x3333333333333333ULL) + ((limb >> 2) & 0x3333333333333333ULL);
  limb = (limb + (limb >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
  return (limb * 0x0101010101010101ULL) >> 56;
}
#endif

/* Count the number of 1 */
static inline size_t
bitset_popcount(const bitset_t set)
{
  M_B1TSET_CONTRACT(set);
  size_t s = 0;
  size_t n = (set->size + M_B1TSET_LIMB_BIT - 1) / M_B1TSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    s += m_b1tset_popcount64(set->ptr[i]);
  return s;
}

/* Oplist for a bitset */
#define BITSET_OPLIST                                                         \
  (INIT(bitset_init)                                                          \
   ,INIT_SET(bitset_init_set)                                                 \
   ,INIT_WITH(API_1(M_INIT_VAI))                                              \
   ,SET(bitset_set)                                                           \
   ,CLEAR(bitset_clear)                                                       \
   ,INIT_MOVE(bitset_init_move)                                               \
   ,MOVE(bitset_move)                                                         \
   ,SWAP(bitset_swap)                                                         \
   ,TYPE(bitset_t)                                                            \
   ,SUBTYPE(bool)                                                             \
   ,EMPTY_P(bitset_empty_p),                                                  \
   ,GET_SIZE(bitset_size)                                                     \
   ,IT_TYPE(bitset_it_t)                                                      \
   ,IT_FIRST(bitset_it)                                                       \
   ,IT_SET(bitset_it_set)                                                     \
   ,IT_LAST(bitset_it_last)                                                   \
   ,IT_END(bitset_it_end)                                                     \
   ,IT_END_P(bitset_end_p)                                                    \
   ,IT_LAST_P(bitset_last_p)                                                  \
   ,IT_EQUAL_P(bitset_it_equal_p)                                             \
   ,IT_NEXT(bitset_next)                                                      \
   ,IT_PREVIOUS(bitset_previous)                                              \
   ,IT_CREF(bitset_cref)                                                      \
   ,RESET(bitset_reset)                                                       \
   ,PUSH(bitset_push_back)                                                    \
   ,POP(bitset_pop_back)                                                      \
   ,HASH(bitset_hash)                                                         \
   ,GET_STR(bitset_get_str)                                                   \
   ,OUT_STR(bitset_out_str)                                                   \
   ,PARSE_STR(bitset_parse_str)                                               \
   ,IN_STR(bitset_in_str)                                                     \
   ,EQUAL(bitset_equal_p)                                                     \
   )

/* Register the OPLIST as a global one */
#define M_OPL_bitset_t() BITSET_OPLIST

// TODO: set_at2, insert_v, remove_v

M_END_PROTECTED_CODE

#endif

// NOTE: Define this function only if m-string has been included
#if !defined(MSTARLIB_BITSET_STRING_H) && defined(MSTARLIB_STRING_H)
#define MSTARLIB_BITSET_STRING_H

M_BEGIN_PROTECTED_CODE

/* Output to a string_t 'str' the formatted text representation of the bitset 'set'
   or append it to the strinf (append=true) */
static inline void
bitset_get_str(string_t str, const bitset_t set, bool append)
{
  M_B1TSET_CONTRACT (set);
  M_ASSERT(str != NULL);
  (append ? string_cat_str : string_set_str) (str, "[");
  for(size_t i = 0; i < set->size; i++) {
    const bool b = bitset_get (set, i);
    const char c = b ? '1' : '0';
    string_push_back (str, c);
  }
  string_push_back (str, ']');
}

M_END_PROTECTED_CODE

#endif
