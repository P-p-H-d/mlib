/*
 * M*LIB - BITSET module
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
#ifndef MSTARLIB_BITSET_H
#define MSTARLIB_BITSET_H

#include <stdint.h>
#include "m-core.h"

/********************************** INTERNAL ************************************/

// Strangely required for Codacy compliance:
#ifndef M_NAMING_INITIALIZE
#define M_NAMING_INITIALIZE init
#endif

M_BEGIN_PROTECTED_CODE

// Define the basic limb of a bitset
typedef uint64_t bitset_limb_ct;
// And its size in bits
#define BITSET_LIMB_BIT (sizeof(bitset_limb_ct) * CHAR_BIT)

// bitset grow policy. n is limb size
#define BITSETI_INC_ALLOC_SIZE(n) ((n) < 4 ? 4 : (n) * 2)

// Compute the number of allocated limbs needed to handle 'n' bits.
#define BITSETI_TO_ALLOC(n)       (((n) + BITSET_LIMB_BIT - 1) / BITSET_LIMB_BIT)

// Compute the number of bits available from the allocated size in limbs
#define BITSETI_FROM_ALLOC(n)     ((n) * BITSET_LIMB_BIT)

// Contract of a bitset
#define BITSETI_CONTRACT(t) do {                                             \
    M_ASSERT(t != NULL);                                                     \
    M_ASSERT(t->size <= BITSETI_FROM_ALLOC (t->alloc));                      \
    M_ASSERT(t->alloc <= ((size_t)-1) / BITSET_LIMB_BIT);                    \
    M_ASSERT(t->size < ((size_t)-1) - BITSET_LIMB_BIT);                      \
    M_ASSERT(t->size == 0 || t->ptr != NULL);                                \
    M_ASSERT(t->alloc == 0 || t->ptr != NULL);                               \
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
M_F(bitset, M_NAMING_INITIALIZE)(bitset_t t)
{
  M_ASSERT (t != NULL);
  M_STATIC_ASSERT (M_POWEROF2_P(BITSET_LIMB_BIT), MLIB_INTERNAL, "M*LIB: BITSET LIMB shall be a power of 2.");
  t->size = 0;
  t->alloc = 0;
  t->ptr = NULL;
  BITSETI_CONTRACT(t);
}

/* Clean a bitset */
static inline void
M_F(bitset, M_NAMING_CLEAN)(bitset_t t)
{
  BITSETI_CONTRACT(t);
  t->size = 0;
}

/* Clear a bitset (DESTRUCTOR) */
static inline void
M_F(bitset, M_NAMING_FINALIZE)(bitset_t t)
{
  M_F(bitset, M_NAMING_CLEAN)(t);
  M_MEMORY_FREE(t->ptr);
  // This is not really needed, but is safer
  // This representation is invalid and will be detected by the contract.
  // A C compiler should be able to optimize out theses initializations.
  t->alloc = 1;
  t->ptr = NULL;
}

/* Set a bitset to another one */
static inline void
M_F(bitset, M_NAMING_SET_AS)(bitset_t d, const bitset_t s)
{
  BITSETI_CONTRACT(d);
  BITSETI_CONTRACT(s);
  if (M_UNLIKELY (d == s)) return;
  const size_t needAlloc = BITSETI_TO_ALLOC (s->size);
  if (s->size > BITSETI_FROM_ALLOC (d->alloc)) {
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
  BITSETI_CONTRACT(d);
}

/* Initialize & set a bitset to another one (CONSTRUCTOR) */
static inline void
M_F(bitset, M_NAMING_INIT_WITH)(bitset_t d, const bitset_t s)
{
  M_ASSERT(d != s);
  M_F(bitset, M_NAMING_INITIALIZE)(d);
  M_F(bitset, M_NAMING_SET_AS)(d, s);
}

/* Initialize & move a bitset (CONSTRUCTOR) from another one (DESTRUCTOR) */
static inline void
M_F(bitset, init_move)(bitset_t d, bitset_t s)
{
  BITSETI_CONTRACT(s);
  d->size  = s->size;
  d->alloc = s->alloc;
  d->ptr   = s->ptr;
  s->alloc = 0;
  s->ptr = NULL;
  BITSETI_CONTRACT(d);  
}

/* Move a bitset from another one (DESTRUCTOR) */
static inline void
M_F(bitset, move)(bitset_t d, bitset_t s)
{
  M_F(bitset, M_NAMING_FINALIZE)(d);
  M_F(bitset, init_move)(d, s);
}

/* Set the bit 'i' in the bitset to the value 'x' */
static inline void
M_F(bitset, M_NAMING_SET_AT)(bitset_t v, size_t i, bool x)
{
  BITSETI_CONTRACT(v);
  M_ASSERT(v->ptr != NULL);
  M_ASSERT_INDEX(i, v->size);
  const size_t offset = i / BITSET_LIMB_BIT;
  const size_t index  = i % BITSET_LIMB_BIT;
  // This is a branchless version as x can only be 0 or 1 with only one variable shift.
  const bitset_limb_ct mask = ((bitset_limb_ct)1)<<index;
  v->ptr[offset] = (v->ptr[offset] & ~mask) | (mask & (0-(bitset_limb_ct)x));
  BITSETI_CONTRACT(v);
}

/* Flip the bit 'i' in the bitset */
static inline void
bitset_flip_at(bitset_t v, size_t i)
{
  BITSETI_CONTRACT(v);
  M_ASSERT(v->ptr != NULL);
  M_ASSERT_INDEX(i, v->size);
  size_t offset = i / BITSET_LIMB_BIT;
  size_t index  = i % BITSET_LIMB_BIT;
  v->ptr[offset] ^= ((bitset_limb_ct)1) << index;
  BITSETI_CONTRACT(v);
}

/* Push back the boolean 'x' in the bitset (increasing the bitset) */
static inline void
M_F(bitset, push, back)(bitset_t v, bool x)
{
  BITSETI_CONTRACT(v);
  if (v->size >= BITSETI_FROM_ALLOC(v->alloc)) {
    // Compute the needed allocation.
    const size_t needAlloc = BITSETI_INC_ALLOC_SIZE(v->alloc);
    // Check for integer overflow
    if (M_UNLIKELY(needAlloc <= v->alloc)) {
      M_MEMORY_FULL(needAlloc * sizeof(bitset_limb_ct));
      return;
    }
    // Alloc memory
    bitset_limb_ct *ptr = M_MEMORY_REALLOC(bitset_limb_ct, v->ptr, needAlloc);
    // Check if success
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(needAlloc * sizeof(bitset_limb_ct));
      return;
    }
    // Clear allocated memory
    // FIXME: Why?
    for(size_t a = v->alloc; a < needAlloc; a++)
      ptr[a] = 0;
    v->ptr = ptr;
    v->alloc = needAlloc;
  }
  M_ASSERT(v->ptr != NULL);
  v->size ++;
  M_F(bitset, M_NAMING_SET_AT)(v, v->size - 1, x);
}

/* Resize the bitset to have exactly 'size' bits */
static inline void
M_F(bitset, resize)(bitset_t v, size_t size)
{
  BITSETI_CONTRACT(v);
  // Check for overflow
  if (M_UNLIKELY(size >= ((size_t)-1) - BITSET_LIMB_BIT)) {
    M_MEMORY_FULL((size_t) -1);
    return;
  }
  // Compute the needed allocation.
  size_t newAlloc = BITSETI_TO_ALLOC (size);
  if (newAlloc > v->alloc) {
    bitset_limb_ct *ptr = M_MEMORY_REALLOC (bitset_limb_ct, v->ptr, newAlloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(newAlloc * sizeof(bitset_limb_ct));
      return;
    }
    v->ptr = ptr;
    v->alloc = newAlloc;
  }
  //FIXME: Last bit are not cleared if new size is lower than org size. Needed?
  //TODO: Implement a more efficient version when needed.
  size_t i = v->size;
  v->size = size;
  for( ; i < size; i++)
    M_F(bitset, M_NAMING_SET_AT)(v, i, 0);
  BITSETI_CONTRACT(v);
}

/* Reserve allocation in the bitset to accommodate at least 'size' bits without reallocation */
static inline void
M_F(bitset, reserve)(bitset_t v, size_t size)
{
  BITSETI_CONTRACT(v);
  size_t oldAlloc = BITSETI_TO_ALLOC (v->size);
  size_t newAlloc = BITSETI_TO_ALLOC (size);
  if (oldAlloc > newAlloc) {
    newAlloc = oldAlloc;
  }
  if (M_UNLIKELY(newAlloc == 0)) {
    M_MEMORY_FREE(v->ptr);
    v->size = v->alloc = 0;
    v->ptr = NULL;
  } else {
    bitset_limb_ct *ptr = M_MEMORY_REALLOC(bitset_limb_ct, v->ptr, newAlloc);
    if (M_UNLIKELY(ptr == NULL) ) {
      M_MEMORY_FULL(newAlloc * sizeof(bitset_limb_ct));
      return;
    }
    v->ptr = ptr;
    v->alloc = newAlloc;
  }
  BITSETI_CONTRACT(v);
}

/* Return the value of the boolean at index 'i'.    
 * NOTE: Interface is a little bit different:
 * It doesn't return a pointer to the data, but the data itself.
 */
static inline bool
M_F(bitset, M_NAMING_GET)(const bitset_t v, size_t i)
{
  BITSETI_CONTRACT(v);
  M_ASSERT(v->ptr != NULL);
  M_ASSERT_INDEX(i, v->size);
  size_t offset = i / BITSET_LIMB_BIT;
  size_t index  = i % BITSET_LIMB_BIT;
  return ( v->ptr[offset] & (((bitset_limb_ct)1) << index) ) != 0;
}

/* bitset_cget is the exact same service than bitset_get */
static inline bool
M_F(bitset, cget)(const bitset_t v, size_t i)
{
  return M_F(bitset, M_NAMING_GET)(v, i);
}

/* Pop back the last bit in the bitset */
static inline void
M_F(bitset, pop, back)(bool *dest, bitset_t v)
{
  BITSETI_CONTRACT(v);
  M_ASSERT_INDEX(0, v->size);
  if (dest) {
    *dest = M_F(bitset, M_NAMING_GET)(v, v->size - 1);
  }
  v->size--;
  // Last popped bit is not cleared
  BITSETI_CONTRACT(v);
}

/* Return the front bit value in the bitset */
static inline bool
M_F(bitset, front)(bitset_t v)
{
  BITSETI_CONTRACT(v);
  M_ASSERT_INDEX(0, v->size);
  return M_F(bitset, M_NAMING_GET)(v, 0);
}

/* Return the back bit value in the bitset */
static inline bool
M_F(bitset, back)(bitset_t v)
{
  BITSETI_CONTRACT(v);
  M_ASSERT_INDEX(0, v->size);
  return M_F(bitset, M_NAMING_GET)(v, v->size - 1);
}

/* Test if the bitset is empty (no bits stored)*/
static inline bool
M_F(bitset, M_NAMING_TEST_EMPTY)(bitset_t v)
{
  BITSETI_CONTRACT(v);
  return v->size == 0;
}

/* Return the number of bits of the bitset */
static inline size_t
M_F(bitset, M_NAMING_GET_SIZE)(bitset_t v)
{
  BITSETI_CONTRACT(v);
  return v->size;
}

/* Return the capacity in limbs of the bitset */
static inline size_t
M_F(bitset, capacity)(bitset_t v)
{
  BITSETI_CONTRACT (v);
  return BITSETI_FROM_ALLOC (v->alloc);
}

/* Swap the bit at index i and j of the bitset */
static inline void
M_F(bitset, swap, at)(bitset_t v, size_t i, size_t j)
{
  BITSETI_CONTRACT(v);
  M_ASSERT_INDEX(i, v->size);
  M_ASSERT_INDEX(j, v->size);

  const bool i_val = M_F(bitset, M_NAMING_GET)(v, i);
  const bool j_val = M_F(bitset, M_NAMING_GET)(v, j);
  M_F(bitset, M_NAMING_SET_AT)(v, i, j_val);
  M_F(bitset, M_NAMING_SET_AT)(v, j, i_val);

  BITSETI_CONTRACT (v);
}

/* Swap the bitsets */
static inline void
M_F(bitset, swap)(bitset_t v1, bitset_t v2)
{
  BITSETI_CONTRACT (v1);
  BITSETI_CONTRACT (v2);
  M_SWAP (size_t, v1->size, v2->size);
  M_SWAP (size_t, v1->alloc, v2->alloc);
  M_SWAP (bitset_limb_ct *, v1->ptr, v2->ptr);
}

/* (INTERNAL) Left shift of the bitset (ptr+size) by 1 bit,
 * integrating the carry in the lowest position.
 * Return the new carry.
 */
static inline bitset_limb_ct
bitseti_lshift(bitset_limb_ct ptr[], size_t n, bitset_limb_ct carry)
{
  for(size_t i = 0; i < n; i++) {
    bitset_limb_ct v = ptr[i];
    ptr[i] = (v << 1) | carry;
    carry = (v >> (BITSET_LIMB_BIT-1) );
  }
  return carry;
}

/* (INTERNAL) Right shift of the bitset (ptr+size) by 1 bit,
 * integrating the carry in the lowest position.
 * Return the new carry.
 */
static inline bitset_limb_ct
bitseti_rshift(bitset_limb_ct ptr[], size_t n, bitset_limb_ct carry)
{
  for(size_t i = n - 1; i < n; i--) {
    bitset_limb_ct v = ptr[i];
    v = (v >> 1) | (carry << (BITSET_LIMB_BIT-1) );
    carry = v & 1;
  }
  return carry;
}

/* Insert a new bit at position 'key' of value 'value' in the bitset */
static inline void
M_F(bitset, push, at)(bitset_t set, size_t key, bool value)
{
  BITSETI_CONTRACT(set);
  // First push another value to extend the array to the right size
  M_F(bitset, push, back)(set, false);
  M_ASSERT(set->ptr != NULL);
  M_ASSERT_INDEX(key, set->size);

  // Then shift it
  size_t offset = key / BITSET_LIMB_BIT;
  size_t index  = key % BITSET_LIMB_BIT;
  bitset_limb_ct v = set->ptr[offset];
  bitset_limb_ct mask = (((bitset_limb_ct)1) << index) - 1;
  bitset_limb_ct carry = (v >> (BITSET_LIMB_BIT - 1));
  v = (v & mask) | ((unsigned int) value << index) | ((v & ~mask) << 1);
  set->ptr[offset] = v;
  size_t size = (set->size + BITSET_LIMB_BIT - 1) / BITSET_LIMB_BIT;
  M_ASSERT(size >= offset + 1);
  v = bitseti_lshift(&set->ptr[offset+1], size - offset - 1, carry);
  // v is unused.
  (void) v;
  BITSETI_CONTRACT(set);
}

/* Pop a new bit at position 'key' in the bitset 
 * and return in *dest its value if *dest exists */
static inline void
M_F(bitset, pop, at)(bool *dest, bitset_t set, size_t key)
{
   BITSETI_CONTRACT(set);
   M_ASSERT(set->ptr != NULL);
   M_ASSERT_INDEX(key, set->size);

   if (dest) {
     *dest = M_F(bitset, M_NAMING_GET)(set, key);
   }
  // Shift it
   size_t offset = key / BITSET_LIMB_BIT;
   size_t index  = key % BITSET_LIMB_BIT;
   size_t size = (set->size + BITSET_LIMB_BIT - 1) / BITSET_LIMB_BIT;
   bitset_limb_ct v, mask, carry;
   carry = bitseti_rshift(&set->ptr[offset+1], size - offset - 1, false);
   v = set->ptr[offset];
   mask = (((bitset_limb_ct)1)<<index)-1;
   v = (v & mask) | ((v>>1) & ~mask) | (carry << (BITSET_LIMB_BIT-1)) ;
   set->ptr[offset] = v;
   // Decrease size
   set->size --;
   BITSETI_CONTRACT(set);
}

/* Test if two bitsets are equal */
static inline bool
M_F(bitset, M_NAMING_TEST_EQUAL_TO)(const bitset_t set1, const bitset_t set2)
{
  BITSETI_CONTRACT(set1);
  BITSETI_CONTRACT(set2);
  if (set1->size != set2->size)
    return false;
  /* We won't compare each bit individually,
     but instead compare them per limb */
  const size_t limbSize = (set1->size) / BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < limbSize; i++)
    if (set1->ptr[i] != set2->ptr[i])
      return false;
  /* Compare the last limb if needed */
  const size_t index = set1->size % BITSET_LIMB_BIT;
  if (index > 0) {
    const bitset_limb_ct mask = (((bitset_limb_ct)1) << index) - 1;
    if ((set1->ptr[limbSize] & mask) != (set2->ptr[limbSize] & mask))
      return false;
  }
  return true;
}

/* Initialize an iterator to the first bit of the bitset */
static inline void
M_F(bitset, M_NAMING_IT_FIRST)(bitset_it_t it, bitset_t set)
{
   BITSETI_CONTRACT(set);
   it->index = 0;
   it->set = set;
}

/* Initialize an iterator to reference the same bit as the given one */
static inline void
M_F(bitset, M_NAMING_IT_SET)(bitset_it_t it, const bitset_it_t itorg)
{
  M_ASSERT(it != NULL && itorg != NULL);
  it->index = itorg->index;
  it->set = itorg->set;
}

/* Initialize an iterator to reference the last bit of the bitset*/
static inline void
M_F(bitset, M_NAMING_IT_LAST)(bitset_it_t it, bitset_t set)
{
   BITSETI_CONTRACT (set);
   it->index = set->size - 1;
   it->set = set;
}

/* Initialize an iterator to reference no valid bit of the bitset*/
static inline void
M_F(bitset, M_NAMING_IT_END)(bitset_it_t it, bitset_t set)
{
   BITSETI_CONTRACT (set);
   it->index = set->size;
   it->set = set;
}

/* Test if an iterator references no valid bit of the bitset anymore */
static inline bool
M_F(bitset, M_NAMING_IT_TEST_END)(const bitset_it_t it)
{
  M_ASSERT(it != NULL && it->set != NULL);
  return (it->index) >= (it->set->size);
}

/* Test if an iterator references the last (or end) bit of the bitset anymore */
static inline bool
M_F(bitset, M_NAMING_IT_TEST_LAST)(const bitset_it_t it)
{
  M_ASSERT(it != NULL && it->set != NULL);
  // A simple overflow protection is to add +1 instead
  // of subtracting it from size, that also can be zero.
  return (it->index + 1) >= (it->set->size);
}

/* Test if both iterators reference the same bit */
static inline bool
M_F(bitset, M_NAMING_IT_TEST_EQUAL)(const bitset_it_t it1, const bitset_it_t it2)
{
  M_ASSERT(it1 != NULL && it2 != NULL);
  return it1->index == it2->index && it1->set == it2->set;
}

/* Move the iterator to the next bit */
static inline void
M_F(bitset, next)(bitset_it_t it)
{
  M_ASSERT(it != NULL && it->set != NULL);
  it->index++;
}

/* Move the iterator to the previous bit */
static inline void
M_F(bitset, previous)(bitset_it_t it)
{
  M_ASSERT(it != NULL && it->set != NULL);
  it->index--;
}

// There is no _ref as it is not possible to modify the value using the IT interface

/* Return a pointer to the bit referenced by the iterator 
 * Only one reference is possible at a time per iterator */
static inline const bool *
M_F(bitset, cref)(bitset_it_t it)
{
  M_ASSERT(it != NULL && it->set != NULL);
  it->value = M_F(bitset, M_NAMING_GET)(it->set, it->index);
  return &it->value;
}

/* Output the bitset as a formatted text in a FILE */
static inline void
M_F(bitset, out_str)(FILE *file, const bitset_t set)
{
  BITSETI_CONTRACT (set);
  M_ASSERT(file != NULL);
  fputc ('[', file);
  for(size_t i = 0; i < set->size; i++) {
    const bool b = M_F(bitset, M_NAMING_GET)(set, i);
    const char c = b ? '1' : '0';
    fputc (c, file);
  }
  fputc (']', file);
}

/* Input the bitset from a formatted text in a FILE */
static inline bool
M_F(bitset, in_str)(bitset_t set, FILE *file)
{
  BITSETI_CONTRACT(set);
  M_ASSERT(file != NULL);
  M_F(bitset, M_NAMING_CLEAN)(set);
  int c = fgetc(file);
  if (c != '[') return false;
  c = fgetc(file);
  while (c == '0' || c == '1') {
    const bool b = (c == '1');
    M_F(bitset, push, back)(set, b);
    c = fgetc(file);
  }
  BITSETI_CONTRACT(set);
  return c == ']';
}

/* Parse the bitset from a formatted text in a C string */
static inline bool
M_F(bitset, parse_cstr)(bitset_t set, const char str[], const char **endptr)
{
  BITSETI_CONTRACT (set);
  M_ASSERT(str != NULL);
  bool success = false;
  M_F(bitset, M_NAMING_CLEAN)(set);
  char c = *str++;
  if (c != '[') goto exit;
  c = *str++;
  do {
    if (c != '0' && c != '1') goto exit;
    const bool b = (c == '1');
    bitset_push_back (set, b);
    c = *str++;
  } while (c != ']' && c != 0);
  BITSETI_CONTRACT (set);
  success = (c == ']');
 exit:
  if (endptr) *endptr = str;
  return success;
}

/* Set the bitset from a formatted text in a C string */
static inline bool
M_F(bitset, set_cstr)(bitset_t dest, const char str[])
{
  return M_F(bitset, parse_cstr)(dest, str, NULL);
}

/* Perform an AND operation between the bitsets,
 * up to the minimum size of both bitsets */
static inline void
M_F(bitset, and)(bitset_t dest, const bitset_t src)
{
  BITSETI_CONTRACT(dest);
  BITSETI_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] &= src->ptr[i];
  if (m) {
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    dest->ptr[n] = (dest->ptr[n] & src->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

/* Perform an OR operation between the bitsets,
 * up to the minimum size of both bitsets */
static inline void
M_F(bitset, or)(bitset_t dest, const bitset_t src)
{
  BITSETI_CONTRACT(dest);
  BITSETI_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] |= src->ptr[i];
  if (m) {
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    dest->ptr[n] = (dest->ptr[n] | src->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

/* Perform an XOR operation between the bitsets,
 * up to the minimum size of both bitsets */
static inline void
M_F(bitset, xor)(bitset_t dest, const bitset_t src)
{
  BITSETI_CONTRACT(dest);
  BITSETI_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] ^= src->ptr[i];
  if (m) {
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    dest->ptr[n] = (dest->ptr[n] ^ src->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

/* Perform a NOT operation of the bitset */
static inline void
M_F(bitset, not)(bitset_t dest)
{
  BITSETI_CONTRACT(dest);
  size_t s = dest->size;
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] = ~ (dest->ptr[i]);
  if (m) {
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    dest->ptr[n] = (~ dest->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

/* Copute a hash of the bitset */
static inline size_t
M_F(bitset, hash)(const bitset_t set)
{
  BITSETI_CONTRACT(set);
  size_t s = set->size;
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  M_HASH_DECL(hash);
  for(size_t i = 0 ; i < n; i++)
    M_HASH_UP(hash, set->ptr[i]);
  if (m) {
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    M_HASH_UP(hash, (set->ptr[n] & mask));
  }
  return M_HASH_FINAL (hash);
}

/* Count the number of leading zero */
static inline size_t
M_F(bitset, clz)(const bitset_t set)
{
  BITSETI_CONTRACT(set);
  size_t s = set->size;
  M_ASSERT_INDEX (0, s);                            // TBC: Special case to handle?
  size_t n = (s -1) / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  bitset_limb_ct limb = set->ptr[n];
  if (m) {
    bitset_limb_ct mask = (((bitset_limb_ct)1) << m) - 1;
    limb &= mask;
  } else {
    m = BITSET_LIMB_BIT;
  }
  s = 0;
  while (limb == 0 && n > 0) {
    s += m;
    limb = set->ptr[--n];
    m = BITSET_LIMB_BIT;
  }
  s += m_core_clz64(limb) - (BITSET_LIMB_BIT - m);
  return s;
}

/* Oplist for a bitset */
#define BITSET_OPLIST                                                        \
  (INIT(M_F(bitset, M_NAMING_INITIALIZE)),                                         \
   INIT_SET(M_F(bitset, M_NAMING_INIT_WITH)),                                 \
   INIT_WITH(API_1(M_INIT_VAI)),                                             \
   SET(M_F(bitset, M_NAMING_SET_AS)),                                           \
   CLEAR(M_F(bitset, M_NAMING_FINALIZE)),                                       \
   INIT_MOVE(M_F(bitset, init_move)),                                              \
   MOVE(M_F(bitset, move)),                                                        \
   SWAP(M_F(bitset, swap)),                                                        \
   TYPE(bitset_t),                                                           \
   SUBTYPE(bool),                                                            \
   TEST_EMPTY(M_F(bitset, M_NAMING_TEST_EMPTY)),                             \
   GET_SIZE(M_F(bitset, M_NAMING_GET_SIZE)),                                     \
   IT_TYPE(bitset_it_t),                                                     \
   IT_FIRST(M_F(bitset, M_NAMING_IT_FIRST)),                                 \
   IT_SET(M_F(bitset, M_NAMING_IT_SET)),                                     \
   IT_LAST(M_F(bitset, M_NAMING_IT_LAST)),                                   \
   IT_END(M_F(bitset, M_NAMING_IT_END)),                                     \
   IT_END_P(M_F(bitset, M_NAMING_IT_TEST_END)),                              \
   IT_LAST_P(M_F(bitset, M_NAMING_IT_TEST_LAST)),                            \
   IT_EQUAL_P(M_F(bitset, M_NAMING_IT_TEST_EQUAL)),                          \
   IT_NEXT(M_F(bitset, next)),                                                     \
   IT_PREVIOUS(M_F(bitset, previous)),                                             \
   IT_CREF(M_F(bitset, cref)),                                                     \
   CLEAN(M_F(bitset, M_NAMING_CLEAN)),                                       \
   PUSH(M_F(bitset, push, back)),                                                   \
   POP(M_F(bitset, pop, back)),                                                     \
   HASH(M_F(bitset, hash)),                                                  \
   GET_STR(M_F(bitset, get_str)),                                                  \
   OUT_STR(M_F(bitset, out_str)),                                                  \
   PARSE_CSTR(M_F(bitset, parse_cstr)),                                              \
   IN_STR(M_F(bitset, in_str)),                                                    \
   EQUAL(M_F(bitset, M_NAMING_TEST_EQUAL_TO)))

/* Register the OPLIST as a global one */
#define M_OPL_bitset_t() BITSET_OPLIST

// TODO: set_at2, insert_v, remove_v

#endif

// NOTE: Define this function only if m-string has been included
#if !defined(MSTARLIB_BITSET_STRING_H) && defined(MSTARLIB_STRING_H)
#define MSTARLIB_BITSET_STRING_H

/* Output to a string_t the formatted text representation of a bitset */
static inline void
M_F(bitset, get_str)(string_t str, const bitset_t set, bool append)
{
  BITSETI_CONTRACT (set);
  M_ASSERT(str != NULL);
  (append ? M_F(string, M_NAMING_CONCATENATE_WITH, cstr):
            M_F(string, set, cstr))(str, "[");
  for(size_t i = 0; i < set->size; i++) {
    const bool b = M_F(bitset, M_NAMING_GET)(set, i);
    const char c = b ? '1' : '0';
    M_F(string, push_back) (str, c);
  }
  M_F(string, push_back) (str, ']');
}

M_END_PROTECTED_CODE

#endif // MSTARLIB_BITSET_H
