/*
 * M*LIB - BITSET module
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
#ifndef __M_BITSET_H
#define __M_BITSET_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "m-core.h"
#include "m-string.h"   // Only for bitset_get_str function.

/********************************** INTERNAL ************************************/

// Define a limb of a bitset
typedef unsigned int bitset_limb;
#define BITSET_LIMB_BIT (sizeof(bitset_limb) * CHAR_BIT)

typedef struct bitset_s {
  size_t size;            // Size is the number of bits
  size_t alloc;           // Alloc is the number of allocated limbs
  bitset_limb *ptr;       // Pointer to the allocated limbs
} bitset_t[1];

typedef struct bitset_it_s {
  size_t index;           // index to the array of bit
  bool   value;           // value used for _ref & _cref to store the value
  struct bitset_s *set;   // the associated bitset
} bitset_it_t[1];

// bitset grow policy
#define BITSETI_INC_ALLOC_SIZE(n) ((n) < 2 ? 4 : (n) * 2)

// Compute the number of allocated limbs needed to handle 'n' bits.
#define BITSETI_TO_ALLOC(n)       (((n) + BITSET_LIMB_BIT - 1) / BITSET_LIMB_BIT)

// Compute the number of bits available from the allocated size
#define BITSETI_FROM_ALLOC(n)     ((n) * BITSET_LIMB_BIT)

#define BITSETI_CONTRACT(t) do {				\
    assert (t != NULL);						\
    assert (t->size <= BITSETI_FROM_ALLOC (t->alloc));		\
    assert (t->alloc <= ((size_t)-1) / BITSET_LIMB_BIT);	\
    assert (t->size == 0 || t->ptr != NULL);			\
  } while (0)


/********************************** EXTERNAL ************************************/

static inline void
bitset_init(bitset_t t)
{
  assert (t != NULL);
  assert (M_POWEROF2_P(BITSET_LIMB_BIT));
  t->size = t->alloc = 0;
  t->ptr = NULL;
  BITSETI_CONTRACT(t);
}

static inline void
bitset_clean(bitset_t t)
{
  BITSETI_CONTRACT(t);
  t->size = 0;
}

static inline void
bitset_clear(bitset_t t)
{
  bitset_clean(t);
  M_MEMORY_FREE(t->ptr);
  // This is not really needed, but is safer
  t->alloc = 0;
  t->ptr = NULL;
}

static inline void
bitset_set(bitset_t d, const bitset_t s)
{
  BITSETI_CONTRACT(d);
  BITSETI_CONTRACT(s);
  if (M_UNLIKELY (d == s)) return;
  const size_t needAlloc = BITSETI_TO_ALLOC (s->size);
  if (s->size > BITSETI_FROM_ALLOC (d->alloc)) {
    bitset_limb *ptr = M_MEMORY_REALLOC (bitset_limb, d->ptr, needAlloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(needAlloc);
      return ;
    }
    d->ptr = ptr;
    d->alloc = needAlloc;
  }
  memcpy (d->ptr, s->ptr, needAlloc * sizeof(bitset_limb) );
  d->size = s->size;
  BITSETI_CONTRACT(d);
}

static inline void
bitset_init_set(bitset_t d, const bitset_t s)
{
  assert (d != s);
  bitset_init(d);
  bitset_set(d, s);
}

static inline void
bitset_init_move(bitset_t d, bitset_t s)
{
  BITSETI_CONTRACT(s);
  d->size  = s->size;
  d->alloc = s->alloc;
  d->ptr   = s->ptr;
  s->alloc = 0;
  s->ptr = NULL;
  BITSETI_CONTRACT(d);  
}

static inline void
bitset_move(bitset_t d, bitset_t s)
{
  bitset_clear(d);
  bitset_init_move (d, s);
}

static inline void
bitset_set_at(bitset_t v, size_t i, bool x)
{
  BITSETI_CONTRACT(v);
  assert (i < v->size && v->ptr != NULL);
  size_t offset = i / BITSET_LIMB_BIT;
  size_t index  = i % BITSET_LIMB_BIT;
  // This is a branchless version as x can only be 0 or 1.
  v->ptr[offset] &= ~(1U<<index);
  v->ptr[offset] |= x<<index;
  BITSETI_CONTRACT (v);
}

static inline void
bitset_push_back (bitset_t v, bool x)
{
  BITSETI_CONTRACT (v);
  if (v->size >= BITSETI_FROM_ALLOC (v->alloc)) {
    const size_t needAlloc = BITSETI_INC_ALLOC_SIZE(v->alloc);
    // Check for integer overflow
    if (M_UNLIKELY (needAlloc <= v->alloc)) {
      M_MEMORY_FULL(needAlloc * sizeof(bitset_limb));
      return;
    }
    // Alloc memory
    bitset_limb *ptr = M_MEMORY_REALLOC (bitset_limb, v->ptr, needAlloc);
    // Check if success
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(needAlloc * sizeof(bitset_limb));
      return;
    }
    v->ptr = ptr;
    v->alloc = needAlloc;
  }
  assert(v->ptr != NULL);
  v->size ++;
  bitset_set_at (v, v->size - 1, x);
}

static inline void
bitset_resize (bitset_t v, size_t size)
{
  BITSETI_CONTRACT (v);
  size_t newAlloc = BITSETI_TO_ALLOC (size);
  if (newAlloc > v->alloc) {
    bitset_limb *ptr = M_MEMORY_REALLOC (bitset_limb, v->ptr, newAlloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(newAlloc * sizeof(bitset_limb));
      return;
    }
    v->ptr = ptr;
    v->alloc = newAlloc;
  }
  //FIXME: Last bit are not cleared. if new size is lower than org size
  //TODO: More efficient!
  size_t i = v->size;
  v->size = size;
  for( ; i < size; i++)
    bitset_set_at(v, i, 0);
  BITSETI_CONTRACT (v);
}

static inline void
bitset_reserve (bitset_t v, size_t size)
{
  BITSETI_CONTRACT (v);
  size_t oldAlloc = BITSETI_TO_ALLOC (v->size);
  size_t newAlloc = BITSETI_TO_ALLOC (size);
  if (oldAlloc > newAlloc) {
    newAlloc = oldAlloc;
  }
  if (M_UNLIKELY (newAlloc == 0)) {
    M_MEMORY_FREE (v->ptr);
    v->size = v->alloc = 0;
    v->ptr = NULL;
  } else {
    bitset_limb *ptr = M_MEMORY_REALLOC (bitset_limb, v->ptr, newAlloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(newAlloc * sizeof(bitset_limb));
      return;
    }
    v->ptr = ptr;
    v->alloc = newAlloc;
  }
  BITSETI_CONTRACT (v);
}

// NOTE: Interface is a little bit different:
// It doesn't return a pointer to the data, but the data itself.
static inline bool
bitset_get(const bitset_t v, size_t i)
{
  BITSETI_CONTRACT(v);
  assert (v->ptr != NULL && i < v->size);
  size_t offset = i / BITSET_LIMB_BIT;
  size_t index  = i % BITSET_LIMB_BIT;
  return ( v->ptr[offset] & (1 << index) ) != 0;
}
#define bitset_cget bitset_get

static inline void
bitset_pop_back(bool *dest, bitset_t v)
{
  BITSETI_CONTRACT (v);
  assert (v->size > 0);
  if (dest) {
    *dest = bitset_get (v, v->size - 1);
  }
  v->size--;
  BITSETI_CONTRACT (v);
}

static inline bool
bitset_back(bitset_t v)
{
  BITSETI_CONTRACT (v);
  assert(v->size > 0);
  return bitset_get(v, v->size-1);
}

static inline bool
bitset_empty_p(bitset_t v)
{
  BITSETI_CONTRACT (v);
  return v->size == 0;
}

static inline size_t
bitset_size(bitset_t v)
{
  BITSETI_CONTRACT (v);
  return v->size;
}

static inline size_t
bitset_capacity(bitset_t v)
{
  BITSETI_CONTRACT (v);
  return BITSETI_FROM_ALLOC (v->alloc);
}

static inline void
bitset_swap_at (bitset_t v, size_t i, size_t j)
{
  BITSETI_CONTRACT (v);
  assert (i < v->size && j < v->size);
  bool i_val = bitset_get(v, i);
  bool j_val = bitset_get(v, j);
  bitset_set_at (v, i, j_val);
  bitset_set_at (v, j, i_val);
  BITSETI_CONTRACT (v);
}

static inline void
bitset_swap (bitset_t v1, bitset_t v2)
{
  BITSETI_CONTRACT (v1);
  BITSETI_CONTRACT (v2);
  M_SWAP (size_t, v1->size, v2->size);
  M_SWAP (size_t, v1->alloc, v2->alloc);
  M_SWAP (bitset_limb *, v1->ptr, v2->ptr);
}

static inline bitset_limb
bitseti_lshift(bitset_limb ptr[], size_t n, bitset_limb carry)
{
  for(size_t i = 0; i < n; i++) {
    bitset_limb v = ptr[i];
    ptr[i] = (v << 1) | carry;
    carry = (v >> (BITSET_LIMB_BIT-1) );
  }
  return carry;
}

static inline bitset_limb
bitseti_rshift(bitset_limb ptr[], size_t n, bitset_limb carry)
{
  for(size_t i = n - 1; i < n; i--) {
    bitset_limb v = ptr[i];
    v = (v >> 1) | (carry << (BITSET_LIMB_BIT-1) );
    carry = v & 1;
  }
  return carry;
}

static inline void
bitset_push_at(bitset_t set, size_t key, bool value)
{
  BITSETI_CONTRACT (set);
  // First push another value to extend the array to the right size
  bitset_push_back(set, true);
  assert (set->ptr != NULL && key < set->size);
  // Then shift it
  size_t offset = key / BITSET_LIMB_BIT;
  size_t index  = key % BITSET_LIMB_BIT;
  // BEFORE: 76543210 FEDBCA98
  // if key=5 ==>
  // AFTER: 65X43210 EDBCA987 000000F
  bitset_limb v = set->ptr[offset];
  bitset_limb mask = (1<<index)-1;
  bitset_limb carry = (v >> (BITSET_LIMB_BIT-1) );
  v = (v & mask) | (value << index) | ((v & ~mask) << 1);
  set->ptr[offset] = v;
  size_t size = (set->size + BITSET_LIMB_BIT - 1) / BITSET_LIMB_BIT;
  v = bitseti_lshift(&set->ptr[offset+1], size - offset - 1, carry);
  BITSETI_CONTRACT (set);
}

static inline void
bitset_pop_at(bool *dest, bitset_t set, size_t key)
{
   BITSETI_CONTRACT (set);
   assert (set->ptr != NULL && key < set->size);
   if (dest) {
     *dest = bitset_get (set, key);
   }
  // Shift it
   size_t offset = key / BITSET_LIMB_BIT;
   size_t index  = key % BITSET_LIMB_BIT;
  // BEFORE: 76543210 FEDBCA98
  // if key=5 ==>
  // AFTER: 8643210 XFEDBCA9
   size_t size = (set->size + BITSET_LIMB_BIT - 1) / BITSET_LIMB_BIT;
   bitset_limb v, mask, carry;
   carry = bitseti_rshift(&set->ptr[offset+1], size - offset - 1, false);
   v = set->ptr[offset];
   mask = (1<<index)-1;
   v = (v & mask) | ((v>>1) & ~mask) | (carry << (BITSET_LIMB_BIT-1)) ;
   set->ptr[offset] = v;
   // Decrease size
   set->size --;
   BITSETI_CONTRACT (set);
}

static inline bool
bitset_equal_p (const bitset_t set1, const bitset_t set2)
{
  BITSETI_CONTRACT (set1);
  BITSETI_CONTRACT (set2);
  if (set1->size != set2->size)
    return false;
  /* We won't compare each bit individualy,
     but instead compare them per limb */
  const size_t limbSize = (set1->size) / BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < limbSize;i++)
    if (set1->ptr[i] != set2->ptr[i])
      return false;
  /* Compare the last limb if needed */
  const size_t index = set1->size % BITSET_LIMB_BIT;
  if (index > 0) {
    const size_t mask = (1 << index) - 1;
    if ((set1->ptr[limbSize] & mask) != (set2->ptr[limbSize] & mask))
      return false;
  }
  return true;
}

static inline void
bitset_it(bitset_it_t it, bitset_t set)
{
   BITSETI_CONTRACT (set);
   it->index = 0;
   it->set = set;
}

static inline void
bitset_it_set(bitset_it_t it, const bitset_it_t itorg)
{
  assert (it != NULL && itorg != NULL);
  it->index = itorg->index;
  it->set = itorg->set;
}

static inline void
bitset_it_last(bitset_it_t it, bitset_t set)
{
   BITSETI_CONTRACT (set);
   it->index = set->size-1;
   it->set = set;
}

static inline void
bitset_it_end(bitset_it_t it, bitset_t set)
{
   BITSETI_CONTRACT (set);
   it->index = set->size;
   it->set = set;
}

static inline bool
bitset_end_p(const bitset_it_t it)
{
  assert (it != NULL && it->set != NULL);
  return (it->index) >= (it->set->size);
}

static inline bool
bitset_last_p(const bitset_it_t it)
{
  assert (it != NULL && it->set != NULL);
  return (it->index) >= (it->set->size)-1;
}

static inline bool
bitset_it_equal_p(const bitset_it_t it1, const bitset_it_t it2)
{
  assert (it1 != NULL && it2 != NULL);
  return it1->index == it2->index && it1->set == it2->set;
}

static inline void
bitset_next(bitset_it_t it)
{
  assert (it != NULL && it->set != NULL);
  it->index++;
}

static inline void
bitset_previous(bitset_it_t it)
{
  assert (it != NULL && it->set != NULL);
  it->index--;
}

static inline const bool *
bitset_cref(bitset_it_t it)
{
  assert (it != NULL && it->set != NULL);
  it->value = bitset_get(it->set, it->index);
  return &it->value;
}

static inline void
bitset_out_str(FILE *file, const bitset_t set)
{
  BITSETI_CONTRACT (set);
  assert(file != NULL);
  fputc ('[', file);
  for(size_t i = 0; i < set->size; i++) {
    const bool b = bitset_get (set, i);
    const char c = b ? '1' : '0';
    fputc (c, file);
  }
  fputc (']', file);
}

static inline bool
bitset_in_str(bitset_t set, FILE *file)
{
  BITSETI_CONTRACT (set);
  assert(file != NULL);
  bitset_clean(set);
  char c = fgetc(file);
  if (c != '[') return false;
  c = fgetc(file);
  do {
    if (c != '0' && c != '1') break;
    const bool b = (c == '1');
    bitset_push_back (set, b);
    c = fgetc(file);
  } while (c != ']' && !feof(file) && !ferror(file));
  BITSETI_CONTRACT (set);
  return c == ']';
}

static inline bool
bitset_set_str(bitset_t set, const char str[])
{
  BITSETI_CONTRACT (set);
  assert(str != NULL);
  bitset_clean(set);
  char c = *str++;
  if (c != '[') return false;
  c = *str++;
  do {
    if (c != '0' && c != '1') return false;
    const bool b = (c == '1');
    bitset_push_back (set, b);
    c = *str++;
  } while (c != ']' && c != 0);
  BITSETI_CONTRACT (set);
  return c == ']';
}

static inline void
bitset_and(bitset_t dest, const bitset_t src)
{
  BITSETI_CONTRACT(dest);
  BITSETI_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] &= src->ptr[i];
  if (m) {
    size_t mask = (1UL << m) - 1;
    dest->ptr[n] = (dest->ptr[n] & src->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

static inline void
bitset_or(bitset_t dest, const bitset_t src)
{
  BITSETI_CONTRACT(dest);
  BITSETI_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] |= src->ptr[i];
  if (m) {
    size_t mask = (1UL << m) - 1;
    dest->ptr[n] = (dest->ptr[n] | src->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

static inline void
bitset_xor(bitset_t dest, const bitset_t src)
{
  BITSETI_CONTRACT(dest);
  BITSETI_CONTRACT(src);
  size_t s = M_MIN(dest->size, src->size);
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] ^= src->ptr[i];
  if (m) {
    size_t mask = (1UL << m) - 1;
    dest->ptr[n] = (dest->ptr[n] ^ src->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

static inline void
bitset_not(bitset_t dest)
{
  BITSETI_CONTRACT(dest);
  size_t s = dest->size;
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  for(size_t i = 0 ; i < n; i++)
    dest->ptr[i] = ~ (dest->ptr[i]);
  if (m) {
    size_t mask = (1UL << m) - 1;
    dest->ptr[n] = (~ dest->ptr[n]) & mask;
  }
  dest->size = s;
  BITSETI_CONTRACT(dest);
}

static inline size_t
bitset_hash(const bitset_t dest)
{
  BITSETI_CONTRACT(dest);
  size_t s = dest->size;
  size_t n = s / BITSET_LIMB_BIT;
  size_t m = s % BITSET_LIMB_BIT;
  M_HASH_DECL(hash);
  for(size_t i = 0 ; i < n; i++)
    M_HASH_UP(hash, dest->ptr[i]);
  if (m) {
    size_t mask = (1UL << m) - 1;
    M_HASH_UP(hash, (dest->ptr[n] & mask));
  }
  return hash;
}

#define BITSET_OPLIST                                                   \
  (INIT(bitset_init)                                                    \
   ,INIT_SET(bitset_init_set)                                           \
   ,SET(bitset_set)                                                     \
   ,CLEAR(bitset_clear)                                                 \
   ,INIT_MOVE(bitset_init_move)                                         \
   ,MOVE(bitset_move)                                                   \
   ,SWAP(bitset_swap)                                                   \
   ,TYPE(bitset_t)                                                      \
   ,SUBTYPE(bool)                                                       \
   ,IT_TYPE(bitset_it_t)                                                \
   ,IT_FIRST(bitset_it)                                                 \
   ,IT_SET(bitset_it_set)                                               \
   ,IT_LAST(bitset_it_last)                                             \
   ,IT_END(bitset_it_end)                                               \
   ,IT_END_P(bitset_end_p)                                              \
   ,IT_LAST_P(bitset_last_p)                                            \
   ,IT_EQUAL_P(bitset_it_equal_p)                                       \
   ,IT_NEXT(bitset_next)                                                \
   ,IT_PREVIOUS(bitset_previous)                                        \
   ,IT_CREF(bitset_cref)                                                \
   ,CLEAN(bitset_clean)                                                 \
   ,PUSH(bitset_push_back)                                              \
   ,POP(bitset_pop_back)                                                \
   ,HASH(bitset_hash)                                                   \
   ,GET_STR(bitset_get_str)                                             \
   ,OUT_STR(bitset_out_str)                                             \
   ,IN_STR(bitset_in_str)                                               \
   ,EQUAL(bitset_equal_p)                                               \
   )

// TODO: set_at2, insert_v, remove_v

#endif

// NOTE: Define this function only if m-string has been included
#ifndef __M_BITSET_STRING_H
#ifdef  __M_STRING_H
#define __M_BITSET_STRING_H

static inline void
bitset_get_str(string_t str, const bitset_t set, bool append)
{
  BITSETI_CONTRACT (set);
  assert(str != NULL);
  (append ? string_cat_str : string_set_str) (str, "[");
  for(size_t i = 0; i < set->size; i++) {
    const bool b = bitset_get (set, i);
    const char c = b ? '1' : '0';
    string_push_back (str, c);
  }
  string_push_back (str, ']');
}

#endif
#endif
