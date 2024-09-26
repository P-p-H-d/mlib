/*
 * M*LIB - Byte String module
 *
 * Copyright (c) 2017-2024, Patrick Pelissier
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
#ifndef MSTARLIB_BSTRING_H
#define MSTARLIB_BSTRING_H

#include "m-core.h"

M_BEGIN_PROTECTED_CODE

// Type definition
typedef struct m_bstring_s {
 size_t size;
 size_t alloc;
 size_t offset;
 uint8_t  *ptr;
 } m_bstring_t[1];

#define M_BSTRING_CONTRACT(o) do {                                            \
    M_ASSERT( (o)->size + (o)->offset >= (o)->size );                         \
    M_ASSERT( (o)->alloc  >= (o)->size + (o)->offset );                       \
    M_ASSERT( (o)->ptr != NULL || (o)->alloc == 0);                           \
} while (0)

M_INLINE size_t
m_bstring_size(const m_bstring_t s)
{
    M_BSTRING_CONTRACT(s);
    return (size_t) s->size;
}

M_INLINE size_t
m_bstring_capacity(const m_bstring_t s)
{
    M_BSTRING_CONTRACT(s);
    return (size_t) s->alloc;
}

M_INLINE uint8_t *
m_br1nsg_ctr(const m_bstring_t v)
{
    return &v->ptr[v->offset];
}

M_INLINE void
m_bstring_init(m_bstring_t s)
{
    s->size   = 0;
    s->alloc  = 0;
    s->offset = 0;
    s->ptr    = NULL;
    M_BSTRING_CONTRACT(s);
}

M_INLINE void
m_bstring_clear(m_bstring_t s)
{
    M_BSTRING_CONTRACT(s);
    M_MEMORY_FREE( s->ptr);
    s->offset = (size_t) -1;
}

M_INLINE void
m_bstring_reset(m_bstring_t s)
{
    M_BSTRING_CONTRACT(s);
    s->size   = 0;
    s->offset = 0;
    M_BSTRING_CONTRACT(s);
}


/* Return the selected byte-character of the string */
M_INLINE uint8_t
m_bstring_get_byte(const m_bstring_t v, size_t index)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX(index, v->size);
    return m_br1nsg_ctr(v)[index];
}

/* Set the selected byte-character of the string */
M_INLINE void
m_bstring_set_byte(m_bstring_t v, size_t index, const uint8_t c)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX(index, v->size);
    m_br1nsg_ctr(v)[index] = c;
}

/* Test if the string is empty or not */
M_INLINE bool
m_bstring_empty_p(const m_bstring_t v)
{
    M_BSTRING_CONTRACT (v);
    return v->size == 0;
}

/* Internal method to fit the Byte string to the given size
   Ensures that the string capacity is greater than size_alloc+offset
   The string 'v' no longer has a working size field.
   Return a pointer to the writable string.
*/
M_INLINE uint8_t *
m_bstr1ng_fit2size (m_bstring_t v, size_t size_alloc, bool exact_alloc)
{
  M_ASSERT_INDEX (v->offset, v->offset + size_alloc);
  // Note: this function may be called in context where the contract is not fulfilled.
  if (M_UNLIKELY (v->offset + size_alloc > v->alloc)) {
    // Insufficient current allocation to store the new string

    // Check if there is a too big offset that can be reduced
    if (v->offset > v->size / 8 && v->offset > 16) {
        memmove(&v->ptr[0], &v->ptr[v->offset], v->size);
        v->offset = 0;
        if (size_alloc <= v->alloc)
            return v->ptr;
    }

    // Alloc a new array
    size_t alloc = (v->offset + size_alloc + (exact_alloc ? 0 : size_alloc / 2));
    alloc = M_MAX(alloc, 16);
    if (M_UNLIKELY_NOMEM (alloc < size_alloc)) {
      /* Overflow in alloc computation */
      M_MEMORY_FULL(alloc);
      // NOTE: Return is currently broken.
      abort();
      return NULL;
    }
    uint8_t *ptr = M_MEMORY_REALLOC (uint8_t, v->ptr, alloc);
    if (M_UNLIKELY_NOMEM (ptr == NULL)) {
      M_MEMORY_FULL(alloc);
      // NOTE: Return is currently broken.
      abort();
      return NULL;
    }
    // The string cannot be fully embedded anymore.
    v->ptr = ptr;
    v->alloc = alloc;
    // Size is not set as the function is called in context
    // where the caller already know the final size of the string,
    // so it will update it.
    return &v->ptr[v->offset];
  }
  return &v->ptr[v->offset];
}

/* Push the byte-character 'c' in the string 'v' */
M_INLINE void
m_bstring_push_back (m_bstring_t v, uint8_t c)
{
    M_BSTRING_CONTRACT (v);
    const size_t size = v->size;
    uint8_t *ptr = m_bstr1ng_fit2size(v, size+1, false);
    ptr[size] = c;
    v->size = size + 1;
    M_BSTRING_CONTRACT (v);
}

M_INLINE void
m_bstring_push_back_bytes (m_bstring_t v, size_t n, const void *buffer)
{
    M_BSTRING_CONTRACT (v);
    const size_t size = v->size;
    M_ASSERT_INDEX (size, size + n);
    uint8_t *ptr = m_bstr1ng_fit2size(v, size+n, false);
    memcpy(&ptr[size], buffer, n);
    v->size = size + n;
    M_BSTRING_CONTRACT (v);
}

M_INLINE void
m_bstring_init_set(m_bstring_t v, const m_bstring_t org)
{
  m_bstring_init(v);
  m_bstring_push_back_bytes(v, m_bstring_size(org), m_br1nsg_ctr(org));
}

M_INLINE void
m_bstring_set(m_bstring_t v, const m_bstring_t org)
{
  m_bstring_reset(v);
  m_bstring_push_back_bytes(v, m_bstring_size(org), m_br1nsg_ctr(org));
}

M_INLINE void
m_bstring_init_move(m_bstring_t v, m_bstring_t org)
{
    M_BSTRING_CONTRACT (org);
    memcpy(v, org, sizeof (m_bstring_t));
    org->offset = (size_t) -1;
}

M_INLINE void
m_bstring_move(m_bstring_t v, m_bstring_t org)
{
    m_bstring_clear(v);
    m_bstring_init_move(v, org);
}

M_INLINE int
m_bstring_cmp(const m_bstring_t v1, m_bstring_t v2)
{
    M_BSTRING_CONTRACT (v1);
    M_BSTRING_CONTRACT (v2);
    const size_t s1 = m_bstring_size(v1);
    const size_t s2 = m_bstring_size(v2);
    int ret = memcmp(m_br1nsg_ctr(v1), m_br1nsg_ctr(v2), M_MIN(s1, s2));
    if (ret)
        return ret;
    if (s1 != s2)
        return s1 < s2 ? -1 : 1;
    return 0;
}

M_INLINE int
m_bstring_cmp_bytes(const m_bstring_t v1, size_t s2, const void *p2)
{
    M_BSTRING_CONTRACT (v1);
    const size_t s1 = m_bstring_size(v1);
    int ret = memcmp(m_br1nsg_ctr(v1), p2, M_MIN(s1, s2));
    if (ret)
        return ret;
    if (s1 != s2)
        return s1 < s2 ? -1 : 1;
    return 0;
}

M_INLINE bool
m_bstring_equal_p(const m_bstring_t v1, m_bstring_t v2)
{
    M_BSTRING_CONTRACT (v1);
    M_BSTRING_CONTRACT (v2);
    const size_t s1 = m_bstring_size(v1);
    const size_t s2 = m_bstring_size(v2);
    if (s1 != s2)
        return false;
    return memcmp(m_br1nsg_ctr(v1), m_br1nsg_ctr(v2), s1) == 0;
}

M_INLINE bool
m_bstring_equal_bytes_p(const m_bstring_t v1, size_t s2, const void *buffer)
{
    M_BSTRING_CONTRACT (v1);
    const size_t s1 = m_bstring_size(v1);
    if (s1 != s2)
        return false;
    return memcmp(m_br1nsg_ctr(v1), buffer, s1) == 0;
}

M_INLINE uint8_t
m_bstring_pop_back(m_bstring_t v)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX(0, v->size);
    uint8_t c = m_br1nsg_ctr(v)[v->size-1];
    v->size --;
    M_BSTRING_CONTRACT (v);
    return c;
}

M_INLINE void
m_bstring_pop_back_bytes(size_t n, void *buffer, m_bstring_t v)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX(n-1, v->size);
    memcpy(buffer, &m_br1nsg_ctr(v)[v->size-n], n);
    v->size -= n;
    M_BSTRING_CONTRACT (v);
}

M_INLINE uint8_t
m_bstring_pop_front(m_bstring_t v)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX(0, v->size);
    uint8_t c = m_br1nsg_ctr(v)[0];
    v->size --;
    v->offset++;
    M_BSTRING_CONTRACT (v);
    return c;
}

M_INLINE void
m_bstring_pop_front_bytes(size_t n, void *buffer, m_bstring_t v)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX(n-1, v->size);
    memcpy(buffer, &m_br1nsg_ctr(v)[0], n);
    v->size -= n;
    v->offset += n;
    M_BSTRING_CONTRACT (v);
}

M_INLINE void
m_bstring_push_bytes_at (m_bstring_t v, size_t pos, size_t n, const void *buffer)
{
    M_BSTRING_CONTRACT (v);
    const size_t size = v->size;
    M_ASSERT_INDEX (pos, size+1); //pos == size ==> equivalent to _push_back_bytes
    M_ASSERT_INDEX (size, size + n);
    uint8_t *ptr = m_bstr1ng_fit2size(v, size+n, false);
    memmove(&ptr[pos+n], &ptr[pos], size-pos);
    memcpy(&ptr[pos], buffer, n);
    v->size = size + n;
    M_BSTRING_CONTRACT (v);
}

M_INLINE void
m_bstring_pop_bytes_at(size_t n, void *buffer, m_bstring_t v, size_t pos)
{
    M_BSTRING_CONTRACT (v);
    const size_t size = v->size;
    M_ASSERT_INDEX (pos, size);
    M_ASSERT_INDEX(n-1, size-pos);
    uint8_t *ptr = m_br1nsg_ctr(v);
    memcpy(buffer, &ptr[pos], n);
    memmove(&ptr[pos], &ptr[pos+n], size-pos-n);
    v->size -= n;
    M_BSTRING_CONTRACT (v);
}

M_INLINE void
m_bstring_resize (m_bstring_t v, size_t n)
{
    M_BSTRING_CONTRACT (v);
    const size_t size = v->size;
    if (n > size) {
        uint8_t *ptr = m_bstr1ng_fit2size(v, n, true);
        memset(&ptr[size], 0, n-size);
    }
    v->size = n;
    M_BSTRING_CONTRACT (v);
}

M_INLINE void
m_bstring_reserve (m_bstring_t v, size_t n)
{
    M_BSTRING_CONTRACT (v);
    const size_t size = v->size;
    if ( (n != 0 || v->size != 0) && n < v->offset + size) {
        n = v->offset + size;
    }
    if (n == 0) {
        M_MEMORY_FREE(v->ptr);
        v->offset = 0;
        v->size = 0;
        v->alloc = 0;
        v->ptr = NULL;
    } else {
        uint8_t *ptr = M_MEMORY_REALLOC (uint8_t, v->ptr, n);
        if (M_UNLIKELY_NOMEM (ptr == NULL)) {
            M_MEMORY_FULL(n);
        }
        // The string cannot be fully embedded anymore.
        v->ptr = ptr;
        v->alloc = n;
    }
    M_BSTRING_CONTRACT (v);
}

M_INLINE const uint8_t *
m_bstring_view(const m_bstring_t v, size_t offset, size_t size_requested)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX (offset+size_requested, v->size+1);
    return &m_br1nsg_ctr(v)[offset];
}

// Acquire / release direct write access: once acquired, user can directly wite at the returned pointer for the requested size
// All other operations on the bstring are forbidden during direct access.
// This allows passing the bstring buffer to be formatted by different API
M_INLINE uint8_t *
m_bstring_acquire_access(m_bstring_t v, size_t offset, size_t size_requested)
{
    M_BSTRING_CONTRACT (v);
    M_ASSERT_INDEX (offset+size_requested, v->size+1);
    // Break canonical representation (this will generate assertion if a normal API is called now)
    M_SWAP(size_t, v->size, v->alloc);
    return &m_br1nsg_ctr(v)[offset];
}

M_INLINE void
m_bstring_release_access(m_bstring_t v)
{
    M_ASSERT( v->size  >= v->alloc + v->offset );
    // Restore canonical representation
    M_SWAP(size_t, v->size, v->alloc);
    M_BSTRING_CONTRACT (v);
}

// TODO: Iterator

// TODO: IO

// TODO: OPLIST

// TODO: Hash

/********************************************************************************/
/*                                                                              */
/* Define the small name (i.e. without the prefix) of the API provided by this  */
/* header if it is needed                                                       */
/*                                                                              */
/********************************************************************************/
#if M_USE_SMALL_NAME

#define bstring_t m_bstring_t
#define bstring_size m_bstring_size
#define bstring_capacity m_bstring_capacity
#define bstring_init m_bstring_init
#define bstring_clear m_bstring_clear
#define bstring_reset m_bstring_reset
#define bstring_get_byte m_bstring_get_byte
#define bstring_set_byte m_bstring_set_byte
#define bstring_empty_p m_bstring_empty_p
#define bstring_push_back m_bstring_push_back
#define bstring_push_back_bytes m_bstring_push_back_bytes
#define bstring_init_set m_bstring_init_set
#define bstring_set m_bstring_set
#define bstring_init_move m_bstring_init_move
#define bstring_move m_bstring_move
#define bstring_cmp m_bstring_cmp
#define bstring_cmp_bytes m_bstring_cmp_bytes
#define bstring_equal_p m_bstring_equal_p
#define bstring_equal_bytes_p m_bstring_equal_bytes_p
#define bstring_pop_back m_bstring_pop_back
#define bstring_pop_back_bytes m_bstring_pop_back_bytes
#define bstring_pop_front m_bstring_pop_front
#define bstring_pop_front_bytes m_bstring_pop_front_bytes
#define bstring_push_bytes_at m_bstring_push_bytes_at
#define bstring_pop_bytes_at m_bstring_pop_bytes_at
#define bstring_resize m_bstring_resize
#define bstring_reserve m_bstring_reserve
#define bstring_view m_bstring_view
#define bstring_acquire_access m_bstring_acquire_access
#define bstring_release_access m_bstring_release_access

#endif

M_END_PROTECTED_CODE

#endif
