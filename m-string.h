/*
 * M*LIB - Dynamic Size String Module
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

#ifndef MSTARLIB_STRING_H
#define MSTARLIB_STRING_H

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "m-core.h"

M_BEGIN_PROTECTED_CODE

/********************************** INTERNAL ************************************/

/* If within the tests, perform additional checks */
#ifdef STRING_WITHIN_TEST
# define STRINGI_ASSUME(n) M_ASSUME(n)
#else
# define STRINGI_ASSUME(n) (void) 0
#endif

// This macro defines the contract of a string.
// Note: A ==> B is represented as not(A) or B
// Note: use of strlen can slow down a lot the program in some cases.
#define STRINGI_CONTRACT(v) do {                                        \
    M_ASSUME (v != NULL);                                               \
    STRINGI_ASSUME (M_C(string, M_NAMING_SIZE)(v) == strlen(string_get_cstr(v)));      \
    M_ASSUME (string_get_cstr(v)[M_C(string, M_NAMING_SIZE)(v)] == 0);                 \
    M_ASSUME (M_C(string, M_NAMING_SIZE)(v) < string_capacity(v));                     \
    M_ASSUME (string_capacity(v) < sizeof (string_heap_t) || !stringi_stack_p(v)); \
  } while(0)


/****************************** EXTERNAL *******************************/

/***********************************************************************/
/*                                                                     */
/*                           DYNAMIC     STRING                        */
/*                                                                     */
/***********************************************************************/

/* Index returned in case of error instead of the position within the string */
#define STRING_FAILURE ((size_t)-1)

/* This is the main structure of this module. */

// string if it is heap allocated
typedef struct string_heap_s {
  size_t size;
  size_t alloc;
} string_heap_t;
// string if it is stack allocated
typedef struct string_stack_s {
  char buffer[sizeof (string_heap_t)];
} string_stack_t;
// both cases of string are possible
typedef union string_union_u {
  string_heap_t heap;
  string_stack_t stack;
} string_union_t;

// Dynamic string
typedef struct string_s {
  string_union_t u;
  char *ptr;
} string_t[1];

// Pointer to a Dynamic string
typedef struct string_s *string_ptr;

// Constant pointer to a Dynamic string
typedef const struct string_s *string_srcptr;

/* Input option for string_fgets 
   STRING_READ_LINE  (read line), 
   STRING_READ_PURE_LINE (read line and remove CR and LF)
   STRING_READ_FILE (read all file)
*/
typedef enum string_fgets_s {
  STRING_READ_LINE = 0, STRING_READ_PURE_LINE = 1, STRING_READ_FILE = 2
} string_fgets_t;

/* Internal method to test if the string is stack based or heap based */
static inline bool
stringi_stack_p(const string_t s)
{
  // Function can be called when contract is not fullfilled
  return (s->ptr == NULL);
}

/* Set the size of the string */
static inline void
stringi_set_size(string_t s, size_t size)
{
  // Function can be called when contract is not fullfilled
  if (stringi_stack_p(s)) {
    assert (size < sizeof (string_heap_t) - 1);
    s->u.stack.buffer[sizeof (string_heap_t) - 1] = (char) size;
  } else
    s->u.heap.size = size;
}

/* Return the number of characters of the string */
static inline size_t
M_C(string, M_NAMING_SIZE)(const string_t s)
{
  // Function can be called when contract is not fullfilled
  // Reading both values before calling the '?' operator allows compiler to generate branchless code
  const size_t s_stack = (size_t) s->u.stack.buffer[sizeof (string_heap_t) - 1];
  const size_t s_heap  = s->u.heap.size;
  return stringi_stack_p(s) ?  s_stack : s_heap;
}

/* Return the capacity of the string */
static inline size_t
string_capacity(const string_t s)
{
  // Function can be called when contract is not fullfilled
  // Reading both values before calling the '?' operator allows compiler to generate branchless code
  const size_t c_stack = sizeof (string_heap_t) - 1;
  const size_t c_heap  = s->u.heap.alloc;
  return stringi_stack_p(s) ?  c_stack : c_heap;
}

/* Return a writable pointer to the array of char of the string */
static inline char*
stringi_get_str(string_t v)
{
  // Function can be called when contract is not fullfilled
  char *const ptr_stack = &v->u.stack.buffer[0];
  char *const ptr_heap  = v->ptr;
  return stringi_stack_p(v) ?  ptr_stack : ptr_heap;
}

/* Return the string view a classic C string (const char *) */
static inline const char*
string_get_cstr(const string_t v)
{
  // Function cannot be called when contract is not fullfilled
  // but it is called by contract (so no contract check to avoid infinite recursion).
  const char *const ptr_stack = &v->u.stack.buffer[0];
  const char *const ptr_heap  = v->ptr;
  return stringi_stack_p(v) ?  ptr_stack : ptr_heap;
}

/* Initialize the dynamic string (constructor) 
  and make it empty */
static inline void
M_C(string, M_NAMING_INIT)(string_t s)
{
  s->ptr = NULL;
  s->u.stack.buffer[0] = 0;
  stringi_set_size(s, 0);
  STRINGI_CONTRACT(s);
}

/* Clear the Dynamic string (destructor) */
static inline void
M_C(string, M_NAMING_CLEAR)(string_t v)
{
  STRINGI_CONTRACT(v);
  if (!stringi_stack_p(v)) {    
    M_MEMORY_FREE(v->ptr);
    v->ptr   = NULL;
  }
  /* This is not needed but is safer to make
     the string invalid so that it can be detected. */
  v->u.stack.buffer[sizeof (string_heap_t) - 1] = CHAR_MAX;
}

/* NOTE: Internaly used by STRING_DECL_INIT */
static inline void stringi_clear2(string_t *v) { M_C(string, M_NAMING_CLEAR)(*v); }

/* Clear the Dynamic string (destructor)
  and return a heap pointer to the string.
  The ownership of the data is transfered back to the caller
  and the returned pointer has to be released by M_MEMORY_FREE. */
static inline char *
string_clear_get_str(string_t v)
{
  STRINGI_CONTRACT(v);
  char *p = v->ptr;
  if (stringi_stack_p(v)) {
    // The string was stack allocated.
    p = v->u.stack.buffer;
    // Need to allocate a heap string to return the copy.
    size_t alloc = M_C(string, M_NAMING_SIZE)(v)+1;
    char *ptr = M_MEMORY_REALLOC (char, NULL, alloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      return NULL;
    }
    assert(ptr != NULL && p != NULL);
    memcpy(ptr, p, alloc);
    p = ptr;
  }
  v->ptr = NULL;
  v->u.stack.buffer[sizeof (string_heap_t) - 1] = CHAR_MAX;
  return p;
}

/* Make the string empty */
static inline void
string_clean(string_t v)
{
  STRINGI_CONTRACT (v);
  stringi_set_size(v, 0);
  stringi_get_str(v)[0] = 0;
  STRINGI_CONTRACT (v);
}

/* Return the selected character of the string */
static inline char
string_get_char(const string_t v, size_t index)
{
  STRINGI_CONTRACT (v);
  M_ASSUME(index < M_C(string, M_NAMING_SIZE)(v));
  return string_get_cstr(v)[index];
}

/* Test if the string is empty or not */
static inline bool
M_C(string, M_NAMING_TEST_EMPTY)(const string_t v)
{
  STRINGI_CONTRACT (v);
  return M_C(string, M_NAMING_SIZE)(v) == 0;
}

/* Ensures that the string capacity is greater than size_alloc
   (size_alloc shall include the final null char).
   It may move the string from stack based to heap based.
   Return a pointer to the writable string.
*/
static inline char *
stringi_fit2size (string_t v, size_t size_alloc)
{
  assert (size_alloc > 0);
  // Note: this function may be called in context where the contract
  // is not fullfilled.
  const size_t old_alloc = string_capacity(v);
  if (M_UNLIKELY (size_alloc > old_alloc)) {
    size_t alloc = size_alloc + size_alloc / 2;
    if (M_UNLIKELY (alloc <= old_alloc)) {
      /* Overflow in alloc computation */
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is broken...
      abort();
      return NULL;
    }
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is currently broken.
      abort();
      return NULL;
    }
    M_ASSUME(ptr != &v->u.stack.buffer[0]);
    if (stringi_stack_p(v)) {
      /* Copy the stack allocation into the heap allocation */
      memcpy(ptr, &v->u.stack.buffer[0], 
              (size_t) v->u.stack.buffer[sizeof (string_heap_t) - 1] + 1U);
    }
    v->ptr = ptr;
    v->u.heap.alloc = alloc;
    return ptr;
  }
  return stringi_get_str(v);
}

/* Modify the string capacity to be able to handle at least 'alloc'
   characters (including final nul char) */
static inline void
string_reserve(string_t v, size_t alloc)
{
  STRINGI_CONTRACT (v);
  const size_t size = M_C(string, M_NAMING_SIZE)(v);
  /* NOTE: Reserve below needed size, perform a shrink to fit */
  if (size + 1 > alloc) {
    alloc = size+1;
  }
  assert (alloc > 0);
  if (alloc < sizeof (string_heap_t)) {
    // Allocation can fit in the stack space
    if (!stringi_stack_p(v)) {
      /* Transform Heap Allocate to Stack Allocate */
      char *ptr = &v->u.stack.buffer[0];
      memcpy(ptr, v->ptr, size+1);
      M_MEMORY_FREE(v->ptr);
      v->ptr = NULL;
      stringi_set_size(v, size);
    } else {
      /* Already a stack based alloc: nothing to do */
    }
  } else {
    // Allocation cannot fit in the stack space
    // Need to allocate in heap space
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      return;
    }
    if (stringi_stack_p(v)) {
      // Copy from stack space to heap space the string
      char *ptr_stack = &v->u.stack.buffer[0];
      memcpy(ptr, ptr_stack, size+1);
      v->u.heap.size = size;
    }
    v->ptr = ptr;
    v->u.heap.alloc = alloc;
  }
  STRINGI_CONTRACT (v);
}

/* Set the string to the C string str */
static inline void
string_set_str(string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  M_ASSUME(str != NULL);
  size_t size = strlen(str);
  char *ptr = stringi_fit2size(v, size+1);
  memcpy(ptr, str, size+1);
  stringi_set_size(v, size);
  STRINGI_CONTRACT (v);
}

/* Set the string to the n first characters of the C string str */
static inline void
string_set_strn(string_t v, const char str[], size_t n)
{
  STRINGI_CONTRACT (v);
  M_ASSUME(str != NULL);
  size_t len  = strlen(str);
  size_t size = M_MIN (len, n);
  char *ptr = stringi_fit2size(v, size+1);
  memcpy(ptr, str, size);
  ptr[size] = 0;
  stringi_set_size(v, size);
  STRINGI_CONTRACT (v);
}

/* Set the string to the other one */
static inline void
string_set (string_t v1, const string_t v2)
{
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  if (M_LIKELY (v1 != v2)) {
    const size_t size = M_C(string, M_NAMING_SIZE)(v2);
    char *ptr = stringi_fit2size(v1, size+1);
    memcpy(ptr, string_get_cstr(v2), size+1);
    stringi_set_size(v1, size);
  }
  STRINGI_CONTRACT (v1);
}

/* Set the string to the n first characters of other one */
static inline void
string_set_n(string_t v, const string_t ref, size_t offset, size_t length)
{
  STRINGI_CONTRACT (v);
  STRINGI_CONTRACT (ref);
  assert (offset <= M_C(string, M_NAMING_SIZE)(ref));
  size_t size = M_MIN (M_C(string, M_NAMING_SIZE)(ref) - offset, length);
  char *ptr = stringi_fit2size(v, size+1);
  memmove(ptr, string_get_cstr(ref) + offset, size);
  ptr[size] = 0;
  stringi_set_size(v, size);
  STRINGI_CONTRACT (v);
}

/* Initialize the string and set it to the other one 
   (constructor) */
static inline void
M_C(string, M_NAMING_INIT_SET)(string_t v1, const string_t v2)
{
  M_C(string, M_NAMING_INIT)(v1);
  string_set(v1,v2);
}

/* Initialize the string and set it to the C string
   (constructor) */
static inline void
string_init_set_str(string_t v1, const char str[])
{
  M_C(string, M_NAMING_INIT)(v1);
  string_set_str(v1, str);
}

/* Initialize the string, set it to the other one,
   and destroy the other one.
   (constructor & destructor) */
static inline void
string_init_move(string_t v1, string_t v2)
{
  STRINGI_CONTRACT (v2);
  memcpy(v1, v2, sizeof (string_t));
  // Note: nullify v2 to be safer
  v2->ptr   = NULL;
  STRINGI_CONTRACT (v1);
}

/* Swap the string */
static inline void
string_swap(string_t v1, string_t v2)
{
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  M_SWAP (size_t, v1->u.heap.size,  v2->u.heap.size);
  M_SWAP (size_t, v1->u.heap.alloc, v2->u.heap.alloc);
  M_SWAP (char *, v1->ptr,   v2->ptr);
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
}

/* Set the string to the other one,
   and destroy the other one.
   (destructor) */
static inline void
string_move(string_t v1, string_t v2)
{
  M_C(string, M_NAMING_CLEAR)(v1);
  string_init_move(v1,v2);
}

/* Push a character in a string */
static inline void
string_push_back (string_t v, char c)
{
  STRINGI_CONTRACT (v);
  const size_t size = M_C(string, M_NAMING_SIZE)(v);
  char *ptr = stringi_fit2size(v, size+2);
  ptr[size+0] = c;
  ptr[size+1] = 0;
  stringi_set_size(v, size+1);
  STRINGI_CONTRACT (v);
}

/* Concatene the string with the C string */
static inline void
string_cat_str(string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  M_ASSUME (str != NULL);
  const size_t old_size = M_C(string, M_NAMING_SIZE)(v);
  const size_t size = strlen(str);
  char *ptr = stringi_fit2size(v, old_size + size + 1);
  memcpy(&ptr[old_size], str, size + 1);
  stringi_set_size(v, old_size + size);
  STRINGI_CONTRACT (v);
}

/* Concatene the string with the other string */
static inline void
string_cat(string_t v, const string_t v2)
{
  STRINGI_CONTRACT (v2);
  STRINGI_CONTRACT (v);
  const size_t size = M_C(string, M_NAMING_SIZE)(v2);
  if (M_LIKELY (size > 0)) {
    const size_t old_size = M_C(string, M_NAMING_SIZE)(v);
    char *ptr = stringi_fit2size(v, old_size + size + 1);
    memcpy(&ptr[old_size], string_get_cstr(v2), size);
    ptr[old_size + size] = 0;
    stringi_set_size(v, old_size + size);
  }
  STRINGI_CONTRACT (v);
}

/* Compare the string to the C string and
  return the sort order (-1 if less, 0 if equal, 1 if greater) */
static inline int
string_cmp_str(const string_t v1, const char str[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (str != NULL);
  return strcmp(string_get_cstr(v1), str);
}

/* Compare the string to the other string and
  return the sort order (-1 if less, 0 if equal, 1 if greater) */
static inline int
string_cmp(const string_t v1, const string_t v2)
{
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  return strcmp(string_get_cstr(v1), string_get_cstr(v2));
}

/* Test if the string is equal to the given C string */
static inline bool
string_equal_str_p(const string_t v1, const char str[])
{
  STRINGI_CONTRACT(v1);
  M_ASSUME (str != NULL);
  return string_cmp_str(v1, str) == 0;
}

/* Test if the string is equal to the other string */
static inline bool
M_C(string, M_NAMING_TEST_EQUAL)(const string_t v1, const string_t v2)
{
  /* string_equal_p can be called with one string which is an OOR value.
     In case of OOR value, .ptr is NULL and .size is maximum.
     It will detect a heap based string, and read size from heap structure.
  */
  assert(v1 != NULL);
  assert(v2 != NULL);
  /* Optimization: both strings shall have at least the same size */
  return M_C(string, M_NAMING_SIZE)(v1) == M_C(string, M_NAMING_SIZE)(v2) && string_cmp(v1, v2) == 0;
}

/* Test if the string is equal to the C string 
   (case-insensitive according to the current locale)
   @remark Doesn't work with UTF-8 strings.
*/
static inline int
string_cmpi_str(const string_t v1, const char p2[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (p2 != NULL);
  // strcasecmp is POSIX only
  const char *p1 = string_get_cstr(v1);
  int c1, c2;
  do {
    // To avoid locale without 1 per 1 mapping.
    c1 = toupper((unsigned char) *p1++);
    c2 = toupper((unsigned char) *p2++);
    c1 = tolower((unsigned char) c1);
    c2 = tolower((unsigned char) c2);
  } while (c1 == c2 && c1 != 0);
  return c1 - c2;
}

/* Test if the string is equal to the other string 
   (case-insensitive according to the current locale)
   @remark Doesn't work with UTF-8 strings.
*/
static inline int
string_cmpi(const string_t v1, const string_t v2)
{
  return string_cmpi_str(v1, string_get_cstr(v2));
}

/* Search for the position of the character c
   from the position 'start' (include)  in the string 
   Return STRING_FAILURE if not found.
   By default, start is zero.
*/
static inline size_t
string_search_char (const string_t v, char c, size_t start)
{
  STRINGI_CONTRACT (v);
  assert (start <= M_C(string, M_NAMING_SIZE)(v));
  const char *p = M_ASSIGN_CAST(const char*,
				strchr(string_get_cstr(v)+start, c));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

/* Reverse Search for the position of the character c
   from the position 'start' (include)  in the string 
   Return STRING_FAILURE if not found.
   By default, start is zero.
*/
static inline size_t
string_search_rchar (const string_t v, char c, size_t start)
{
  STRINGI_CONTRACT (v);
  assert (start <= M_C(string, M_NAMING_SIZE)(v));
  // NOTE: Can implement it in a faster way than the libc function
  // by scanning backward from the bottom of the string (which is
  // possible since we know the size)
  const char *p = M_ASSIGN_CAST(const char*,
				strrchr(string_get_cstr(v)+start, c));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

/* Search for the sub C string in the string from the position start
   Return STRING_FAILURE if not found.
   By default, start is zero. */
static inline size_t
string_search_str (const string_t v, const char str[], size_t start)
{
  STRINGI_CONTRACT (v);
  assert (start <= M_C(string, M_NAMING_SIZE)(v));
  M_ASSUME (str != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
				strstr(string_get_cstr(v)+start, str));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

/* Search for the sub other string v2 in the string v1 from the position start
   Return STRING_FAILURE if not found.
   By default, start is zero. */
static inline size_t
string_search (const string_t v1, const string_t v2, size_t start)
{
  STRINGI_CONTRACT (v2);
  assert (start <= M_C(string, M_NAMING_SIZE)(v1));
  return string_search_str(v1, string_get_cstr(v2), start);
}

/* Search for the first matching character in the given C string 
   in the string v1 from the position start
   Return STRING_FAILURE if not found.
   By default, start is zero. */
static inline size_t
string_search_pbrk(const string_t v1, const char first_of[], size_t start)
{
  STRINGI_CONTRACT (v1);
  assert (start <= M_C(string, M_NAMING_SIZE)(v1));
  M_ASSUME (first_of != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
				strpbrk(string_get_cstr(v1)+start, first_of));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v1));
}

/* Compare the string to the C string using strcoll */
static inline int
string_strcoll_str(const string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  return strcoll(string_get_cstr(v), str);
}

/* Compare the string to the other string using strcoll */
static inline int
string_strcoll (const string_t v1, const string_t v2)
{
  STRINGI_CONTRACT (v2);
  return string_strcoll_str(v1, string_get_cstr(v2));
}

/* Return the number of bytes of the segment of s
   that consists entirely of bytes in accept */
static inline size_t
string_spn(const string_t v1, const char accept[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (accept != NULL);
  return strspn(string_get_cstr(v1), accept);
}

/* Return the number of bytes of the segment of s
   that consists entirely of bytes not in reject */
static inline size_t
string_cspn(const string_t v1, const char reject[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (reject != NULL);
  return strcspn(string_get_cstr(v1), reject);
}

/* Return the string left truncated to the first 'index' bytes */
static inline void
string_left(string_t v, size_t index)
{
  STRINGI_CONTRACT (v);
  const size_t size = M_C(string, M_NAMING_SIZE)(v);
  if (index >= size)
    return;
  stringi_get_str(v)[index] = 0;
  stringi_set_size(v,index);
  STRINGI_CONTRACT (v);
}

/* Return the string right truncated from the 'index' position to the last position */
static inline void
string_right(string_t v, size_t index)
{
  STRINGI_CONTRACT (v);
  char *ptr = stringi_get_str(v);
  const size_t size = M_C(string, M_NAMING_SIZE)(v);
  if (index >= size) {
    ptr[0] = 0;
    stringi_set_size(v, 0);
    STRINGI_CONTRACT (v);
    return;
  }
  size_t s2 = size - index;
  memmove (&ptr[0], &ptr[index], s2+1);
  stringi_set_size(v, s2);
  STRINGI_CONTRACT (v);
}

/* Return the string from position index to size bytes */
static inline void
string_mid (string_t v, size_t index, size_t size)
{
  string_right(v, index);
  string_left(v, size);
}

/* Return in the string the C string str1 into the C string str2 from start
   By default, start is zero.
*/
static inline size_t
string_replace_str (string_t v, const char str1[], const char str2[], size_t start)
{
  STRINGI_CONTRACT (v);
  M_ASSUME (str1 != NULL && str2 != NULL);
  size_t i = string_search_str(v, str1, start);
  if (i != STRING_FAILURE) {
    const size_t str1_l = strlen(str1);
    const size_t str2_l = strlen(str2);
    const size_t size   = M_C(string, M_NAMING_SIZE)(v);
    assert(size + 1 + str2_l > str1_l);
    char *ptr = stringi_fit2size (v, size + str2_l - str1_l + 1);
    if (str1_l != str2_l) {
      memmove(&ptr[i+str2_l], &ptr[i+str1_l], size - i - str1_l + 1);
      stringi_set_size(v, size + str2_l - str1_l);
    }
    memcpy (&ptr[i], str2, str2_l);
    STRINGI_CONTRACT (v);
  }
  return i;
}

/* Return in the string the string str1 into the string str2 from start
   By default, start is zero.
*/
static inline size_t
string_replace (string_t v, const string_t v1, const string_t v2, size_t start)
{
  STRINGI_CONTRACT (v);
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  return string_replace_str(v, string_get_cstr(v1), string_get_cstr(v2), start);
}

/* Replace tin the string the sub-string at position 'pos' for 'len' bytes
   into the C string str2. */
static inline void
string_replace_at (string_t v, size_t pos, size_t len, const char str2[])
{
  STRINGI_CONTRACT (v);
  assert(str2 != NULL);
  const size_t str1_l = len;
  const size_t str2_l = strlen(str2);
  const size_t size   = M_C(string, M_NAMING_SIZE)(v);
  char *ptr;
  if (str1_l != str2_l) {
    M_ASSUME (size + str2_l + 1 > str1_l);
    ptr = stringi_fit2size (v, size + str2_l - str1_l + 1);
    M_ASSUME (pos + str1_l < size + 1);
    memmove(&ptr[pos+str2_l], &ptr[pos+str1_l], size - pos - str1_l + 1);
    stringi_set_size(v, size + str2_l - str1_l);
  } else {
    ptr = stringi_get_str(v);
  }
  memcpy (&ptr[pos], str2, str2_l);
  STRINGI_CONTRACT (v);
}

/* Format in the string the given printf format */
static inline int
string_printf (string_t v, const char format[], ...)
{
  STRINGI_CONTRACT (v);
  M_ASSUME (format != NULL);
  va_list args;
  int size;
  char *ptr = stringi_get_str(v);
  size_t alloc = string_capacity(v);
  va_start (args, format);
  size = vsnprintf (ptr, alloc, format, args);
  if (size > 0 && ((size_t) size+1 >= alloc) ) {
    // We have to realloc our string to fit the needed size
    ptr = stringi_fit2size (v, (size_t) size + 1);
    alloc = string_capacity(v);
    // and redo the parsing.
    va_end (args);
    va_start (args, format);
    size = vsnprintf (ptr, alloc, format, args);
    assert (size > 0 && (size_t)size < alloc);
  }
  if (size >= 0) {
    stringi_set_size(v, (size_t) size);
  }
  va_end (args);
  STRINGI_CONTRACT (v);
  return size;
}

/* Append to the string the formatted string of the given printf format */
static inline int
string_cat_printf (string_t v, const char format[], ...)
{
  STRINGI_CONTRACT (v);
  M_ASSUME (format != NULL);
  va_list args;
  int size;
  size_t old_size = M_C(string, M_NAMING_SIZE)(v);
  char  *ptr      = stringi_get_str(v);
  size_t alloc    = string_capacity(v);
  va_start (args, format);
  size = vsnprintf (&ptr[old_size], alloc - old_size, format, args);
  if (size > 0 && (old_size+(size_t)size+1 >= alloc) ) {
    // We have to realloc our string to fit the needed size
    ptr = stringi_fit2size (v, old_size + (size_t) size + 1);
    alloc = string_capacity(v);
    // and redo the parsing.
    va_end (args);
    va_start (args, format);
    size = vsnprintf (&ptr[old_size], alloc - old_size, format, args);
    assert (size >= 0);
  }
  if (size >= 0) {
    stringi_set_size(v, old_size + (size_t) size);
  } else {
    // vsnprintf may have output some characters before returning an error.
    // Undo this to have a clean state
    ptr[old_size] = 0;
  }
  va_end (args);
  STRINGI_CONTRACT (v);
  return size;
}

/* Get a line/pureline/file from the FILE and store it in the string */
static inline bool
string_fgets(string_t v, FILE *f, string_fgets_t arg)
{
  STRINGI_CONTRACT(v);
  assert (f != NULL);
  char *ptr = stringi_fit2size (v, 100);
  size_t size = 0;
  size_t alloc = string_capacity(v);
  ptr[0] = 0;
  bool retcode = false; /* Nothing has been read yet */
  /* alloc - size is very unlikely to be bigger than INT_MAX
    but fgets accepts an int as the size argument */
  while (fgets(&ptr[size], (int) M_MIN( (alloc - size), (size_t) INT_MAX ), f) != NULL) {
    retcode = true; /* Something has been read */
    // fgets doesn't return the number of characters read, so we need to count.
    size += strlen(&ptr[size]);
    if (arg != STRING_READ_FILE && ptr[size-1] == '\n') {
      if (arg == STRING_READ_PURE_LINE) {
        size --;
        ptr[size] = 0;         /* Remove EOL */
      }
      stringi_set_size(v, size);
      STRINGI_CONTRACT(v);
      return retcode; /* Normal terminaison */
    } else if (ptr[size-1] != '\n' && !feof(f)) {
      /* The string buffer is not big enough:
         increase it and continue reading */
      /* v cannot be stack alloc */
      ptr   = stringi_fit2size (v, alloc + alloc/2);
      alloc = string_capacity(v);
    }
  }
  stringi_set_size(v, size);
  STRINGI_CONTRACT (v);
  return retcode; /* Abnormal terminaison */
}

/* Get a word from the FILE and store it in the string.
   Words are supposed to be separated each other by the given list of separator
   separator shall be a CONSTANT C array.
 */
static inline bool
string_fget_word (string_t v, const char separator[], FILE *f)
{
  char buffer[128];
  char c = 0;
  int d;
  STRINGI_CONTRACT(v);
  assert (f != NULL);
  assert (1+20+2+strlen(separator)+3 < sizeof buffer);
  size_t size = 0;
  bool retcode = false;

  /* Skip separator first */
  do {
    d = fgetc(f);
    if (d == EOF) {
      return false;
    }
  } while (strchr(separator, d) != NULL);
  ungetc(d, f);

  size_t alloc = string_capacity(v);
  char *ptr    = stringi_get_str(v);
  ptr[0] = 0;

  /* NOTE: We generate a buffer which we give to scanf to parse the string,
     that it is to say, we generate the format dynamically!
     The format is like " %49[^ \t.\n]%c"
     So in this case, we parse up to 49 chars, up to the separators char,
     and we read the next char. If the next char is a separator, we successful
     read a word, otherwise we have to continue parsing.
     The user shall give a constant string as the separator argument,
     as a control over this argument may give an attacker
     an opportunity for stack overflow */
  while (snprintf(buffer, sizeof buffer -1, " %%%zu[^%s]%%c", (size_t) alloc-1-size, separator) > 0
         /* We may read one or two argument(s) */
         && m_core_fscanf(f, buffer, m_core_arg_size(&ptr[size], alloc-size), &c) >= 1) {
    retcode = true;
    size += strlen(&ptr[size]);
    /* If we read only one argument 
       or if the final read character is a separator */
    if (c == 0 || strchr(separator, c) != NULL)
      break;
    /* Next char is not a separator: continue parsing */
    stringi_set_size(v, size);
    ptr = stringi_fit2size (v, alloc + alloc/2);
    alloc = string_capacity(v);
    assert (alloc > size + 1);
    ptr[size++] = c;
    ptr[size] = 0;
    // Invalid c character for next iteration
    c= 0;
  }
  stringi_set_size(v, size);
  STRINGI_CONTRACT(v);  
  return retcode;
}

/* Put the string in the given FILE without formatting */
static inline bool
string_fputs(FILE *f, const string_t v)
{
  STRINGI_CONTRACT (v);
  assert (f != NULL);
  return fputs(string_get_cstr(v), f) >= 0;
}

/* Test if the string starts with the given C string */
static inline bool
string_start_with_str_p(const string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  M_ASSUME (str != NULL);
  const char *v_str = string_get_cstr(v);
  while (*str){
    if (*str != *v_str)
      return false;
    str++;
    v_str++;
  }
  return true;
}

/* Test if the string starts with the other string */
static inline bool
string_start_with_string_p(const string_t v, const string_t v2)
{
  STRINGI_CONTRACT (v2);
  return string_start_with_str_p (v, string_get_cstr(v2));
}

/* Test if the string ends with the C string */
static inline bool
string_end_with_str_p(const string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  M_ASSUME (str != NULL);
  const char *v_str  = string_get_cstr(v);
  const size_t v_l   = M_C(string, M_NAMING_SIZE)(v);
  const size_t str_l = strlen(str);
  if (v_l < str_l)
    return false;
  for(size_t i = 0; i < str_l; i++) {
    if (str[i] != v_str[v_l - str_l + i])
      return false;
  }
  return true;
}

/* Test if the string ends with the other string */
static inline bool
string_end_with_string_p(const string_t v, const string_t v2)
{
  STRINGI_CONTRACT (v2);
  return string_end_with_str_p (v, string_get_cstr(v2));
}

/* Compute a hash for the string */
static inline size_t
string_hash(const string_t v)
{
  STRINGI_CONTRACT (v);
  return m_core_hash(string_get_cstr(v), M_C(string, M_NAMING_SIZE)(v));
}

// Return true if c is a character from charac
static bool
stringi_strim_char(char c, const char charac[])
{
  for(const char *s = charac; *s; s++) {
    if (c == *s)
      return true;
  }
  return false;
}

/* Remove any characters from charac that are present 
   in the begining of the string and the end of the string. */
static inline void
string_strim(string_t v, const char charac[])
{
  STRINGI_CONTRACT (v);
  char *ptr = stringi_get_str(v);
  char *b   = ptr;
  size_t size = M_C(string, M_NAMING_SIZE)(v);
  while (size > 0 && stringi_strim_char(b[size-1], charac))
    size --;
  if (size > 0) {
    while (stringi_strim_char(*b, charac))
      b++;
    M_ASSUME (b >= ptr &&  size >= (size_t) (b - ptr) );
    size -= (size_t) (b - ptr);
    memmove (ptr, b, size);
  }
  ptr[size] = 0;
  stringi_set_size(v, size);
  STRINGI_CONTRACT (v);
}

/* Test if the string is equal to the OOR value */
static inline bool
string_oor_equal_p(const string_t s, unsigned char n)
{
  return (s->ptr == NULL) & (s->u.heap.alloc == ~(size_t)n);
}

/* Set the unitialized string to the OOR value */
static inline void
string_oor_set(string_t s, unsigned char n)
{
  s->ptr = NULL;
  s->u.heap.alloc = ~(size_t)n;
}

/* I/O */
/* Output: "string" with quote around
   Replace " by \" within the string (and \ to \\)
   \n, \t & \r by their standard representation
   and other not printable character with \0xx */

/* Transform the string 'v2' into a formatted string
   and set it to (or append in) the string 'v'. */
static inline void
string_get_str(string_t v, const string_t v2, bool append)
{
  STRINGI_CONTRACT(v2);
  STRINGI_CONTRACT(v);
  M_ASSUME (v != v2); // Limitation
  size_t size = append ? M_C(string, M_NAMING_SIZE)(v) : 0;
  size_t v2_size = M_C(string, M_NAMING_SIZE)(v2);
  size_t targetSize = size + v2_size + 3;
  char *ptr = stringi_fit2size(v, targetSize);
  ptr[size ++] = '"';
  for(size_t i = 0 ; i < v2_size; i++) {
    const char c = string_get_char(v2,i);
    switch (c) {
    case '\\':
    case '"':
    case '\n':
    case '\t':
    case '\r':
      // Special characters which can be displayed in a short form.
      stringi_set_size(v, size);
      ptr = stringi_fit2size(v, ++targetSize);
      ptr[size ++] = '\\';
      // This string acts as a perfect hashmap which supposes an ASCII mapping
      // and (c^(c>>5)) is the hash function
      ptr[size ++] = " tn\" r\\"[(c ^ (c >>5)) & 0x07];
      break;
    default:
      if (M_UNLIKELY (!isprint(c))) {
        targetSize += 3;
        stringi_set_size(v, size);
        ptr = stringi_fit2size(v, targetSize);
        int d1 = c & 0x07, d2 = (c>>3) & 0x07, d3 = (c>>6) & 0x07;
        ptr[size ++] = '\\';
        ptr[size ++] = '0' + d3;
        ptr[size ++] = '0' + d2;
        ptr[size ++] = '0' + d1;
      } else {
        ptr[size ++] = c;
      }
      break;
    }
  }
  ptr[size ++] = '"';
  ptr[size] = 0;
  stringi_set_size(v, size);
  assert (size <= targetSize);
  STRINGI_CONTRACT (v);
}

/* Transform the string 'v2' into a formatted string
   and output it in the given FILE */
static inline void
string_out_str(FILE *f, const string_t v)
{
  STRINGI_CONTRACT(v);
  M_ASSUME (f != NULL);
  fputc('"', f);
  size_t size = M_C(string, M_NAMING_SIZE)(v);
  for(size_t i = 0 ; i < size; i++) {
    const char c = string_get_char(v, i);
    switch (c) {
    case '\\':
    case '"':
    case '\n':
    case '\t':
    case '\r':
      fputc('\\', f);
      fputc(" tn\" r\\"[(c ^ c >>5) & 0x07], f);
      break;
    default:
      if (M_UNLIKELY (!isprint(c))) {
        int d1 = c & 0x07, d2 = (c>>3) & 0x07, d3 = (c>>6) & 0x07;
        fputc('\\', f);
        fputc('0' + d3, f);
        fputc('0' + d2, f);
        fputc('0' + d1, f);
      } else {
        fputc(c, f);
      }
      break;
    }
  }
  fputc('"', f);
}

/* Read the formatted string from the FILE
   and set the converted value in the string 'v'.
   Return true in case of success */
static inline bool
string_in_str(string_t v, FILE *f)
{
  STRINGI_CONTRACT(v);
  M_ASSUME (f != NULL);
  int c = fgetc(f);
  if (c != '"') return false;
  string_clean(v);
  c = fgetc(f);
  while (c != '"' && c != EOF) {
    if (M_UNLIKELY (c == '\\')) {
      c = fgetc(f);
      switch (c) {
      case 'n':
      case 't':
      case 'r':
      case '\\':
      case '\"':
        // This string acts as a perfect hashmap which supposes an ASCII mapping
        // and (c^(c>>5)) is the hash function
        c = " \r \" \n\\\t"[(c^(c>>5))& 0x07];
        break;
      default:
        if (!(c >= '0' && c <= '7'))
          return false;
        int d1 = c - '0';
        c = fgetc(f);
        if (!(c >= '0' && c <= '7'))
          return false;
        int d2 = c - '0';
        c = fgetc(f);
        if (!(c >= '0' && c <= '7'))
          return false;
        int d3 = c - '0';
        c = (d1 << 6) + (d2 << 3) + d3;
        break;
      }
    }
    string_push_back (v, c);
    c = fgetc(f);
  }
  return c == '"';
}

/* Read the formatted string from the C string
   and set the converted value in the string 'v'.
   Return true in case of success
   If endptr is not null, update the position of the parsing.
*/
static inline bool
string_parse_str(string_t v, const char str[], const char **endptr)
{
  STRINGI_CONTRACT(v);
  M_ASSUME (str != NULL);
  bool success = false;
  int c = *str++;
  if (c != '"') goto exit;
  string_clean(v);
  c = *str++;
  while (c != '"' && c != 0) {
    if (M_UNLIKELY (c == '\\')) {
      c = *str++;
      switch (c) {
      case 'n':
      case 't':
      case 'r':
      case '\\':
      case '\"':
        // This string acts as a perfect hashmap which supposes an ASCII mapping
        // and (c^(c>>5)) is the hash function
        c = " \r \" \n\\\t"[(c^(c>>5))& 0x07];
        break;
      default:
        if (!(c >= '0' && c <= '7'))
          goto exit;
        int d1 = c - '0';
        c = *str++;
        if (!(c >= '0' && c <= '7'))
          goto exit;
        int d2 = c - '0';
        c = *str++;
        if (!(c >= '0' && c <= '7'))
          goto exit;
        int d3 = c - '0';
        c = (d1 << 6) + (d2 << 3) + d3;
        break;
      }
    }
    string_push_back (v, c);
    c = *str++;
  }
  success = (c == '"');
 exit:
  if (endptr != NULL) *endptr = str;
  return success;
}

/* Transform the string 'v2' into a formatted string
   and output it in the given serializer
   See serialization for return code.
*/
static inline m_serial_return_code_t
string_out_serial(m_serial_write_t serial, const string_t v)
{
  assert (serial != NULL && serial->m_interface != NULL);
  return serial->m_interface->write_string(serial, string_get_cstr(v), M_C(string, M_NAMING_SIZE)(v) );
}

/* Read the formatted string from the serializer
   and set the converted value in the string 'v'.
   See serialization for return code.
*/
static inline m_serial_return_code_t
string_in_serial(string_t v, m_serial_read_t serial)
{
  assert (serial != NULL && serial->m_interface != NULL);
  return serial->m_interface->read_string(serial, v);
}

/* State of the UTF8 decoding machine state */
typedef enum {
  STRINGI_UTF8_STARTING = 0,
  STRINGI_UTF8_DECODING_1 = 8,
  STRINGI_UTF8_DECODING_2 = 16,
  STRINGI_UTF8_DOCODING_3 = 24,
  STRINGI_UTF8_ERROR = 32
} stringi_utf8_state_e;

/* An unicode value */
typedef unsigned int string_unicode_t;

/* Error in case of decoding */
#define STRING_UNICODE_ERROR (UINT_MAX)

/* UTF8 character classification:
 * 
 * 0*       --> type 1 byte  A
 * 10*      --> chained byte B
 * 110*     --> type 2 byte  C
 * 1110*    --> type 3 byte  D
 * 11110*   --> type 4 byte  E
 * 111110*  --> invalid      I
 */
/* UTF8 State Transition table:
 *    ABCDEI
 *   +------
 *  S|SI123III
 *  1|ISIIIIII
 *  2|I1IIIIII
 *  3|I2IIIIII
 *  I|IIIIIIII
 */

/* The use of a string enables the compiler/linker to factorize it. */
#define STRINGI_UTF8_STATE_TAB                                          \
  "\000\040\010\020\030\040\040\040"                                    \
  "\040\000\040\040\040\040\040\040"                                    \
  "\040\010\040\040\040\040\040\040"                                    \
  "\040\020\040\040\040\040\040\040"                                    \
  "\040\040\040\040\040\040\040\040"

/* Main generic UTF8 decoder
   It shall be (nearly) branchless on any CPU.
   It takes a character, and the previous state and the previous value of the unicode value.
   It updates the state and the decoded unicode value.
   A decoded unicoded value is valid only when the state is STARTING.
 */
static inline void
stringi_utf8_decode(char c, stringi_utf8_state_e *state,
                    string_unicode_t *unicode)
{
  const unsigned int type = m_core_clz32((unsigned char)~c) - (sizeof(uint32_t) - 1) * CHAR_BIT;
  const string_unicode_t mask1 = (UINT_MAX - (string_unicode_t)(*state != STRINGI_UTF8_STARTING) + 1);
  const string_unicode_t mask2 = (0xFFU >> type);
  *unicode = ((*unicode << 6) & mask1) | ((unsigned int) c & mask2);
  *state = (stringi_utf8_state_e) STRINGI_UTF8_STATE_TAB[(unsigned int) *state + type];
}

/* Check if the given array of characters is a valid UTF8 stream */
/* NOTE: Non-canonical representation are not rejected */
static inline bool
stringi_utf8_valid_str_p(const char str[])
{
  stringi_utf8_state_e s = STRINGI_UTF8_STARTING;
  string_unicode_t u = 0;
  while (*str) {
    stringi_utf8_decode(*str, &s, &u);
    if ((s == STRINGI_UTF8_ERROR)
        ||(s == STRINGI_UTF8_STARTING
           &&(u > 0x10FFFF /* out of range */
              ||(u >= 0xD800 && u <= 0xDFFF) /* surrogate halves */)))
      return false;
    str++;
  }
  return true;
}

/* Computer the number of unicode characters are represented in the UTF8 stream
   Return SIZE_MAX (aka -1) in case of error
 */
static inline size_t
stringi_utf8_length(const char str[])
{
  size_t size = 0;
  stringi_utf8_state_e s = STRINGI_UTF8_STARTING;
  string_unicode_t u = 0;
  while (*str) {
    stringi_utf8_decode(*str, &s, &u);
    if (M_UNLIKELY (s == STRINGI_UTF8_ERROR)) return SIZE_MAX;
    size += (s == STRINGI_UTF8_STARTING);
    str++;
  }
  return size;
}

/* Encode an unicode into an UTF8 stream of characters */
static inline int
stringi_utf8_encode(char buffer[5], string_unicode_t u)
{
  if (M_LIKELY (u <= 0x7Fu)) {
    buffer[0] = (char) u;
    buffer[1] = 0;
    return 1;
  } else if (u <= 0x7FFu) {
    buffer[0] = (char) (0xC0u | (u >> 6));
    buffer[1] = (char) (0x80 | (u & 0x3Fu));
    buffer[2] = 0;
    return 2;
  } else if (u <= 0xFFFFu) {
    buffer[0] = (char) (0xE0u | (u >> 12));
    buffer[1] = (char) (0x80u | ((u >> 6) & 0x3Fu));
    buffer[2] = (char) (0x80u | (u & 0x3Fu));
    buffer[3] = 0;
    return 3;
  } else {
    buffer[0] = (char) (0xF0u | (u >> 18));
    buffer[1] = (char) (0x80u | ((u >> 12) & 0x3Fu));
    buffer[2] = (char) (0x80u | ((u >> 6) & 0x3Fu));
    buffer[3] = (char) (0x80u | (u & 0x3F));
    buffer[4] = 0;
    return 4;
  }
}

/* Iterator on a string over UTF8 encoded characters */
typedef struct string_it_s {
  string_unicode_t u;
  const char *ptr;
  const char *next_ptr;
} string_it_t[1];

/* Start iteration over the UTF8 encoded unicode value */
static inline void
string_it(string_it_t it, const string_t str)
{
  STRINGI_CONTRACT(str);
  assert(it != NULL);
  it->ptr      = string_get_cstr(str);
  it->next_ptr = it->ptr;
  it->u        = 0;
}

/* Set the iterator to the end of string 
   The iterator references therefore nothong.
*/
static inline void
M_C(string, M_NAMING_IT_END)(string_it_t it, const string_t str)
{
  STRINGI_CONTRACT(str);
  assert(it != NULL);
  it->ptr      = &string_get_cstr(str)[M_C(string, M_NAMING_SIZE)(str)];
  it->next_ptr = 0;
  it->u        = 0;
}

/* Set the iterator to the same position than the other one */
static inline void
M_C(string, M_NAMING_IT_SET)(string_it_t it, const string_it_t itsrc)
{
  assert(it != NULL && itsrc != NULL);
  it->ptr      = itsrc->ptr;
  it->next_ptr = itsrc->next_ptr;
  it->u        = itsrc->u;
}

/* Test if the iterator has reached the end of the string */
static inline bool
M_C(string, M_NAMING_IT_TEST_END)(string_it_t it)
{
  assert (it != NULL);
  if (M_UNLIKELY (*it->ptr == 0))
    return true;
  stringi_utf8_state_e state =  STRINGI_UTF8_STARTING;
  string_unicode_t u = 0;
  const char *str = it->ptr;
  do {
    stringi_utf8_decode(*str, &state, &u);
    str++;
  } while (state != STRINGI_UTF8_STARTING && state != STRINGI_UTF8_ERROR && *str != 0);
  it->next_ptr = str;
  it->u = M_UNLIKELY (state == STRINGI_UTF8_ERROR) ? STRING_UNICODE_ERROR : u;
  return false;
}

/* Test if the iterator is equal to the other one */
static inline bool
M_C(string, M_NAMING_IT_TEST_EQUAL)(const string_it_t it1, const string_it_t it2)
{
  assert(it1 != NULL && it2 != NULL);
  // IT1.ptr == IT2.ptr ==> IT1 == IT2 ==> All fields are equal
  assert(it1->ptr != it2->ptr || (it1->next_ptr == it2->next_ptr && it1->u == it2->u));
  return it1->ptr == it2->ptr;
}

/* Advance the iterator to the next UTF8 unicode character */
static inline void
string_next (string_it_t it)
{
  assert (it != NULL);
  it->ptr = it->next_ptr;
}

/* Return the unicode value associated to the iterator */
static inline string_unicode_t
string_get_cref (const string_it_t it)
{
  assert (it != NULL);
  return it->u;
}

/* Return the unicode value associated to the iterator */
static inline const string_unicode_t *
string_cref (const string_it_t it)
{
  assert (it != NULL);
  return &it->u;
}

/* Push unicode into string, encoding it in UTF8 */
static inline void
string_push_u (string_t str, string_unicode_t u)
{
  STRINGI_CONTRACT(str);
  char buffer[4+1];
  stringi_utf8_encode(buffer, u);
  string_cat_str(str, buffer);
}

/* Compute the length in UTF8 characters in the string */
static inline size_t
string_length_u(string_t str)
{
  STRINGI_CONTRACT(str);
  return stringi_utf8_length(string_get_cstr(str));
}

/* Check if a string is a valid UTF8 encoded stream */
static inline bool
string_utf8_p(string_t str)
{
  STRINGI_CONTRACT(str);
  return stringi_utf8_valid_str_p(string_get_cstr(str));
}


/* Define the split & the join functions 
   in case of usage with the algorithm module */
#define STRING_SPLIT(name, oplist, type_oplist)                         \
  static inline void M_C(name, _split)(M_GET_TYPE oplist cont,          \
                                   const string_t str, const char sep)  \
  {                                                                     \
    size_t begin = 0;                                                   \
    string_t tmp;                                                       \
    size_t size = M_C(string, M_NAMING_SIZE)(str);                                     \
    M_C(string, M_NAMING_INIT)(tmp);                                                   \
    M_CALL_CLEAN(oplist, cont);                                         \
    for(size_t i = 0 ; i < size; i++) {                                 \
      char c = string_get_char(str, i);                                 \
      if (c == sep) {                                                   \
        string_set_strn(tmp, &string_get_cstr(str)[begin], i - begin);  \
        /* If push move method is available, use it */                  \
        M_IF_METHOD(PUSH_MOVE,oplist)(                                  \
                                      M_CALL_PUSH_MOVE(oplist, cont, &tmp); \
                                      M_C(string, M_NAMING_INIT)(tmp);                 \
                                      ,                                 \
                                      M_CALL_PUSH(oplist, cont, tmp);   \
                                                                        ) \
          begin = i + 1;                                                \
      }                                                                 \
    }                                                                   \
    string_set_strn(tmp, &string_get_cstr(str)[begin], size - begin);   \
    M_CALL_PUSH(oplist, cont, tmp);                                     \
    /* HACK: if method reverse is defined, it is likely that we have */ \
    /* inserted the items in the wrong order (aka for a list) */        \
    M_IF_METHOD(REVERSE, oplist) (M_CALL_REVERSE(oplist, cont);, )      \
    M_C(string, M_NAMING_CLEAR)(tmp);                                   \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _join)(string_t dst, M_GET_TYPE oplist cont, \
                                      const string_t str)               \
  {                                                                     \
    bool init_done = false;                                             \
    string_clean (dst);                                                 \
    for M_EACH(item, cont, oplist) {                                    \
        if (init_done) {                                                \
          string_cat(dst, str);                                         \
        }                                                               \
        string_cat (dst, *item);                                        \
        init_done = true;                                               \
    }                                                                   \
  }                                                                     \


/* Use of Compound Literals to init a constant string.
   NOTE: The use of the additional structure layer is to ensure
   that the pointer to char is properly aligned to an int (this
   is a needed assumption by string_hash).
   Otherwise it could have been :
   #define STRING_CTE(s)                                          \
     ((const string_t){{.size = sizeof(s)-1, .alloc = sizeof(s),  \
     .ptr = s}})
   which produces faster code.
   Note: This code doesn't work with C++ (use of c99 feature
   of recursive struct definition and compound literal). 
   As such, there is a separate C++ definition.
*/
#ifndef __cplusplus
/* Initialize a constant string with the given C string */
# define STRING_CTE(s)                                                       \
  ((const string_t){{.u.heap = { .size = sizeof(s)-1, .alloc = sizeof(s) } , \
        .ptr = ((struct { long long _n; char _d[sizeof (s)]; }){ 0, s })._d }})
#else
namespace m_string {
  template <unsigned int N>
    struct m_aligned_string {
      string_t string;
      union  {
        char str[N];
        long long i;
      };
    inline m_aligned_string(const char lstr[])
      {
        this->string->u.heap.size = N -1;
        this->string->u.heap.alloc = N;
        memcpy (this->str, lstr, N);
        this->string->ptr = this->str;
      }
    };
}
/* Initialize a constant string with the given C string (C++ mode) */
#define STRING_CTE(s)                                                   \
  m_string::m_aligned_string<sizeof (s)>(s).string
#endif

/* Initialize and set a string to the given formatted value. */
#define STRINGI_INIT_PRINTF(v, ...)                      \
  (M_C(string, M_NAMING_INIT) (v),  string_printf (v, __VA_ARGS__) ) 

/* Initialize a string with the given list of arguments.
   Check if it is a formatted input or not by counting the number of arguments.
   If there is only one argument, it can only be a set to C string.
   It is much faster in this case to call string_init_set_str.
   In C11 mode, it uses the fact that string_init_set is overloaded to handle both
   C string and string. */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
#define STRINGI_INIT_WITH(v, ...)                                       \
  M_IF_NARGS_EQ1(__VA_ARGS__)(M_C(string, M_NAMING_INIT_SET), STRINGI_INIT_PRINTF)(v, __VA_ARGS__)
#else
#define STRINGI_INIT_WITH(v, ...)                                       \
  M_IF_NARGS_EQ1(__VA_ARGS__)(string_init_set_str, STRINGI_INIT_PRINTF)(v, __VA_ARGS__)
#endif

/* NOTE: Use GCC extension (OBSOLETE) */
#define STRING_DECL_INIT(v)                                             \
  string_t v __attribute__((cleanup(stringi_clear2))) = {{ 0, 0, NULL}}

/* NOTE: Use GCC extension (OBSOLETE) */
#define STRING_DECL_INIT_PRINTF(v, format, ...)                         \
  STRING_DECL_INIT(v);                                                  \
  string_printf (v, format, __VA_ARGS__)


/* Define the OPLIST of a STRING */
#define STRING_OPLIST                                                   \
  (INIT(M_C(string, M_NAMING_INIT)),                                    \
   INIT_SET(M_C(string, M_NAMING_INIT_SET)),                            \
   SET(M_C(string, M_NAMING_SET)),                                      \
   INIT_WITH(STRINGI_INIT_WITH),                                        \
   INIT_MOVE(string_init_move), MOVE(string_move),                      \
   SWAP(string_swap), CLEAN(M_C(string, M_NAMING_CLEAN)),               \
   TEST_EMPTY(M_C(string, M_NAMING_TEST_EMPTY)),                        \
   CLEAR(M_C(string, M_NAMING_CLEAR)),                                  \
   HASH(string_hash), EQUAL(M_C(string, M_NAMING_TEST_EQUAL)),          \
   CMP(string_cmp), TYPE(string_t),                                     \
   PARSE_STR(string_parse_str), GET_STR(string_get_str),                \
   OUT_STR(string_out_str), IN_STR(string_in_str),                      \
   OUT_SERIAL(string_out_serial), IN_SERIAL(string_in_serial),          \
   EXT_ALGO(STRING_SPLIT),                                              \
   OOR_EQUAL(string_oor_equal_p), OOR_SET(string_oor_set)               \
   ,SUBTYPE(string_unicode_t)                                           \
   ,IT_TYPE(string_it_t)						                                    \
   ,IT_FIRST(M_C(string, M_NAMING_IT_FIRST))                            \
   ,IT_END(M_C(string, M_NAMING_IT_END))                                \
   ,IT_SET(M_C(string, M_NAMING_IT_SET))                                \
   ,IT_END_P(M_C(string, M_NAMING_IT_TEST_END))	                        \
   ,IT_EQUAL_P(M_C(string, M_NAMING_IT_TEST_EQUAL))	                    \
   ,IT_NEXT(string_next)						                                    \
   ,IT_CREF(string_cref)						                                    \
   )

/* Register the OPLIST as a global one */
#define M_OPL_string_t() STRING_OPLIST


/* Macro encapsulation to give a default value of 0 for start offset */

/* Search for a character in a string (string, character[, start=0]) */
#define string_search_char(v, ...)					\
  M_APPLY(string_search_char, v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Reverse Search for a character in a string (string, character[, start=0]) */
#define string_search_rchar(v, ...)					\
  M_APPLY(string_search_rchar, v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Search for a C string in a string (string, c_string[, start=0]) */
#define string_search_str(v, ...)					\
  M_APPLY(string_search_str, v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Search for a string in a string (string, string[, start=0]) */
#define string_search(v, ...)						\
  M_APPLY(string_search, v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* PBRK for a string in a string (string, string[, start=0]) */
#define string_search_pbrk(v, ...)					\
  M_APPLY(string_search_pbrk, v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Replace a C string to another C string in a string (string, c_src_string, c_dst_string, [, start=0]) */
#define string_replace_str(v, s1, ...)					\
  M_APPLY(string_replace_str, v, s1, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Replace a string to another string in a string (string, src_string, dst_string, [, start=0]) */
#define string_replace(v, s1, ...)					\
  M_APPLY(string_replace, v, s1, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Strim a string from the given set of characters (default is " \n\r\t") */
#define string_strim(...)                                               \
  M_APPLY(string_strim, M_IF_DEFAULT1("  \n\r\t", __VA_ARGS__))

/* Macro encapsulation for C11 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

/* Select either the string function or the str function depending on
   the b operade to the function.
   func1 is the string function / func2 is the str function. */
# define STRINGI_SELECT2(func1,func2,a,b)       \
  _Generic((b)+0,                               \
           char*: func2,                        \
           const char *: func2,                 \
           default : func1                      \
           )(a,b)
# define STRINGI_SELECT3(func1,func2,a,b,c)	\
  _Generic((b)+0,                               \
           char*: func2,                        \
           const char *: func2,                 \
           default : func1                      \
           )(a,b,c)

/* Init & Set the string a to the string (or C string) b (constructor) */
#define string_init_set(a,b) STRINGI_SELECT2(M_C(string, M_NAMING_INIT_SET), string_init_set_str, a, b)

/* Set the string a to the string (or C string) b */
#define string_set(a,b) STRINGI_SELECT2(M_C(string, M_NAMING_SET), string_set_str, a, b)

/* Concatene the string (or C string) b to the string a */
#define string_cat(a,b) STRINGI_SELECT2(string_cat, string_cat_str, a, b)

/* Compare the string a to the string (or C string) b and return the sort order */
#define string_cmp(a,b) STRINGI_SELECT2(string_cmp, string_cmp_str, a, b)

/* Check if a string is equal to another string (or C-string). */
#define string_equal_p(a,b) STRINGI_SELECT2(M_C(string, M_NAMING_TEST_EQUAL), string_equal_str_p, a, b)

/* 'strcoll' a string with another string (or C-string). */
#define string_strcoll(a,b) STRINGI_SELECT2(string_strcoll, string_strcoll_str, a, b)

#undef string_search
/* Search for a string in a string (or C string) (string, string[, start=0]) */
#define string_search(v, ...)						\
  M_APPLY(STRINGI_SELECT3, string_search, string_search_str,		\
	  v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Internal Macro: Provide GET_STR method to default type */
#undef M_GET_STR_METHOD_FOR_DEFAULT_TYPE
#define M_GET_STR_METHOD_FOR_DEFAULT_TYPE GET_STR(M_GET_STRING_ARG)

#endif

/* Internal Macro: Provide GET_STR method to enum type */
#undef M_GET_STR_METHOD_FOR_ENUM_TYPE
#define M_GET_STR_METHOD_FOR_ENUM_TYPE GET_STR(M_ENUM_GET_STR)

/***********************************************************************/
/*                                                                     */
/*                BOUNDED STRING, aka char[N+1]                        */
/*                                                                     */
/***********************************************************************/

#define BOUNDED_STRINGI_CONTRACT(var, max_size) do {                    \
    assert(var != NULL);                                                \
    assert(var->s[max_size] == 0);                                      \
  } while (0)

#define BOUNDED_STRING_DEF(name, max_size)                              \
                                                                        \
  typedef char M_C(name, _array_t)[max_size+1];                         \
                                                                        \
  typedef struct M_C(name, _s) {                                        \
    char s[max_size+1];                                                 \
  } M_C(name,_t)[1];                                                    \
                                                                        \
  static inline void                                                    \
  M_C(name, M_NAMING_INIT)(M_C(name,_t) s)                                      \
  {                                                                     \
    assert(s != NULL);                                                  \
    assert(max_size >= 1);                                              \
    s->s[0] = 0;                                                        \
    s->s[max_size] = 0;                                                 \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, M_NAMING_CLEAR)(M_C(name,_t) s)                                     \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    /* nothing to do */                                                 \
    (void) s;                                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, M_NAMING_CLEAN)(M_C(name,_t) s)                                     \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    s->s[0] = 0;                                                        \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, M_NAMING_SIZE)(const M_C(name,_t) s)                                \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    return strlen(s->s);                                                \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _capacity)(const M_C(name,_t) s)                            \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    (void)s; /* unused */                                               \
    return max_size+1;                                                  \
  }                                                                     \
                                                                        \
  static inline char                                                    \
  M_C(name, _get_char)(const M_C(name,_t) s, size_t index)              \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(index < max_size);                                           \
    return s->s[index];                                                 \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, M_NAMING_TEST_EMPTY)(const M_C(name,_t) s)                             \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    return s->s[0] == 0;                                                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _set_str)(M_C(name,_t) s, const char str[])                 \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    m_core_strncpy(s->s, str, max_size);                                \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _set_strn)(M_C(name,_t) s, const char str[], size_t n)      \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(str != NULL);                                                \
    size_t len = M_MIN(max_size, n);                                    \
    m_core_strncpy(s->s, str, len);                                     \
    s->s[len] = 0;                                                      \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
  }                                                                     \
                                                                        \
  static inline const char *                                            \
  M_C(name, _get_cstr)(const M_C(name,_t) s)                            \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    return s->s;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, M_NAMING_SET)(M_C(name,_t) s, const M_C(name,_t) str)       \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    BOUNDED_STRINGI_CONTRACT(str, max_size);                            \
    M_C(name, _set_str)(s, str->s);                                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _set_n)(M_C(name,_t) s, const M_C(name,_t) str,             \
                    size_t offset, size_t length)                       \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    BOUNDED_STRINGI_CONTRACT(str, max_size);                            \
    assert (offset <= max_size);                                        \
    M_C(name, _set_strn)(s, str->s+offset, length);                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, M_NAMING_INIT_SET)(M_C(name,_t) s, const M_C(name,_t) str)  \
  {                                                                     \
    M_C(name,M_NAMING_INIT)(s);                                         \
    M_C(name,_set)(s, str);                                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init_set_str)(M_C(name,_t) s, const char str[])            \
  {                                                                     \
    M_C(name, M_NAMING_INIT)(s);                                        \
    M_C(name, _set_str)(s, str);                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _cat_str)(M_C(name,_t) s, const char str[])                 \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert (str != NULL);                                               \
    assert (strlen(s->s) <= max_size);                                  \
    m_core_strncat(s->s, str, max_size-strlen(s->s));                   \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _cat)(M_C(name,_t) s, const M_C(name,_t)  str)              \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(str, max_size);                            \
    M_C(name, _cat_str)(s, str->s);                                     \
  }                                                                     \
                                                                        \
  static inline int                                                     \
  M_C(name, _cmp_str)(const M_C(name,_t) s, const char str[])           \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(str != NULL);                                                \
    return strcmp(s->s, str);                                           \
  }                                                                     \
                                                                        \
  static inline int                                                     \
  M_C(name, _cmp)(const M_C(name,_t) s, const M_C(name,_t) str)         \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    BOUNDED_STRINGI_CONTRACT(str, max_size);                            \
    return strcmp(s->s, str->s);                                        \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _equal_str_p)(const M_C(name,_t) s, const char str[])       \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(str != NULL);                                                \
    return strcmp(s->s, str) == 0;                                      \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, M_NAMING_TEST_EQUAL)                                        \
    (const M_C(name,_t) s, const M_C(name,_t) str)                      \
  {                                                                     \
    /* _equal_p may be called in context OOR.                           \
       So contract cannot be verified */                                \
    return (s->s[max_size] == str->s[max_size]) &                       \
           (strcmp(s->s, str->s) == 0);                                 \
  }                                                                     \
                                                                        \
  static inline int                                                     \
  M_C(name, _printf)(M_C(name,_t) s, const char format[], ...)          \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(format != NULL);                                             \
    va_list args;                                                       \
    int ret;                                                            \
    va_start (args, format);                                            \
    ret = vsnprintf (s->s, max_size+1, format, args);                   \
    va_end (args);                                                      \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline int                                                     \
  M_C(name, _cat_printf)(M_C(name,_t) s, const char format[], ...)      \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(format != NULL);                                             \
    va_list args;                                                       \
    int ret;                                                            \
    va_start (args, format);                                            \
    size_t length = strlen(s->s);                                       \
    assert(length <= max_size);                                         \
    ret = vsnprintf (&s->s[length], max_size+1-length, format, args);   \
    va_end (args);                                                      \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _fgets)(M_C(name,_t)s, FILE *f, string_fgets_t arg)         \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(f != NULL);                                                  \
    assert(arg != STRING_READ_FILE);                                    \
    char *ret = fgets(s->s, max_size+1, f);                             \
    s->s[max_size] = 0;                                                 \
    if (ret != NULL && arg == STRING_READ_PURE_LINE) {                  \
      size_t length = strlen(s->s);                                     \
      if (length > 0 && s->s[length-1] == '\n')                         \
        s->s[length-1] = 0;                                             \
    }                                                                   \
    return ret != NULL;                                                 \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _fputs)(FILE *f, const M_C(name,_t) s)                      \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(f != NULL);                                                  \
    return fputs(s->s, f) >= 0;                                         \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _hash)(const M_C(name,_t) s)                                \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    /* Cannot use m_core_hash: alignment not sufficent */               \
    M_HASH_DECL(hash);                                                  \
    const char *str = s->s;                                             \
    while (*str) {                                                      \
      size_t h = (size_t) *str++;                                       \
      M_HASH_UP(hash, h);                                               \
    }                                                                   \
    return M_HASH_FINAL(hash);                                          \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _oor_equal_p)(const M_C(name,_t) s, unsigned char n)        \
  {                                                                     \
    /* s may be invalid contract */                                     \
    assert (s != NULL);                                                 \
    assert ( (n == 0) || (n == 1));                                     \
    return s->s[max_size] == (char) (n+1);                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _oor_set)(M_C(name,_t) s, unsigned char n)                  \
  {                                                                     \
    /* s may be invalid contract */                                     \
    assert (s != NULL);                                                 \
    assert ( (n == 0) || (n == 1));                                     \
    s->s[max_size] = (char) (n+1);                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _get_str)(string_t v, const M_C(name,_t) s, bool append)    \
  {                                                                     \
    STRINGI_CONTRACT(v);                                                \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    /* Build dummy string to reuse string_get_str */                    \
    uintptr_t ptr = (uintptr_t) &s->s[0];                               \
    string_t v2;                                                        \
    v2->u.heap.size = strlen(s->s);                                     \
    v2->u.heap.alloc = v2->u.heap.size + 1;                             \
    v2->ptr = (char*)ptr;                                               \
    string_get_str(v, v2, append);                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *f, const M_C(name,_t) s)                    \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(f != NULL);                                                  \
    /* Build dummy string to reuse string_get_str */                    \
    uintptr_t ptr = (uintptr_t) &s->s[0];                               \
    string_t v2;                                                        \
    v2->u.heap.size = strlen(s->s);                                     \
    v2->u.heap.alloc = v2->u.heap.size + 1;                             \
    v2->ptr = (char*)ptr;                                               \
    string_out_str(f, v2);                                              \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _in_str)(M_C(name,_t) v, FILE *f)                           \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(v, max_size);                              \
    assert(f != NULL);                                                  \
    string_t v2;                                                        \
    M_C(string, M_NAMING_INIT)(v2);                                     \
    bool ret = string_in_str(v2, f);                                    \
    m_core_strncpy(v->s, string_get_cstr(v2), max_size);                \
    M_C(string, M_NAMING_CLEAR)(v2);                                    \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _parse_str)                                                 \
    (M_C(name,_t) v, const char str[], const char **endptr)             \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(v, max_size);                              \
    assert(str != NULL);                                                \
    string_t v2;                                                        \
    M_C(string, M_NAMING_INIT)(v2);                                     \
    bool ret = string_parse_str(v2, str, endptr);                       \
    m_core_strncpy(v->s, string_get_cstr(v2), max_size);                \
    M_C(string, M_NAMING_CLEAR)(v2);                                    \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline m_serial_return_code_t                                  \
  M_C(name, _out_serial)(m_serial_write_t serial, const M_C(name,_t) v) \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(v, max_size);                              \
    assert (serial != NULL && serial->m_interface != NULL);             \
    return serial->m_interface->write_string(serial,                    \
                                             v->s, strlen(v->s));       \
  }                                                                     \
                                                                        \
  static inline m_serial_return_code_t                                  \
  M_C(name, _in_serial)(M_C(name,_t) v, m_serial_read_t serial)         \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(v, max_size);                              \
    assert (serial != NULL && serial->m_interface != NULL);             \
    string_t tmp;                                                       \
    /* TODO: Not optimum */                                             \
    M_C(string, M_NAMING_INIT)(tmp);                                    \
    m_serial_return_code_t r =                                          \
      serial->m_interface->read_string(serial, tmp);                    \
    m_core_strncpy(v->s, string_get_cstr(tmp), max_size);               \
    M_C(string, M_NAMING_CLEAR)(tmp);                                   \
    BOUNDED_STRINGI_CONTRACT(v, max_size);                              \
    return r;                                                           \
  }


/* Define the OPLIST of a BOUNDED_STRING */
#define BOUNDED_STRING_OPLIST(name)                                            \
    (INIT(M_C(name, M_NAMING_INIT)), INIT_SET(M_C(name, M_NAMING_INIT_SET)),   \
     SET(M_C(name, M_NAMING_SET)), CLEAR(M_C(name, M_NAMING_CLEAR)),           \
     HASH(M_C(name, _hash)), EQUAL(M_C(name, M_NAMING_TEST_EQUAL)),            \
     CMP(M_C(name, _cmp)), TYPE(M_C(name, _t)),                                \
     OOR_EQUAL(M_C3(name, _, M_NAMING_MAKE_PREDICATE(oor_equal))),             \
     OOR_SET(M_C(name, _oor_set)) PARSE_STR(M_C(name, _parse_str)),            \
     GET_STR(M_C(name, _get_str)), OUT_STR(M_C(name, _out_str)),               \
     IN_STR(M_C(name, _in_str)), OUT_SERIAL(M_C(name, _out_serial)),           \
     IN_SERIAL(M_C(name, _in_serial)), )

/* Init a constant bounded string.
   Try to do a clean cast */
/* Use of Compound Literals to init a constant string.
   See above */
#ifndef __cplusplus
#define BOUNDED_STRING_CTE(name, string)                                \
  ((const struct M_C(name, _s) *)((M_C(name, _array_t)){string}))
#else
namespace m_string {
  template <unsigned int N>
    struct m_bounded_string {
      char s[N];
      inline m_bounded_string(const char lstr[])
      {
        memset(this->s, 0, N);
        m_core_strncpy(this->s, lstr, N-1);
      }
    };
}
#define BOUNDED_STRING_CTE(name, string)                \
  ((const struct M_C(name, _s) *)(m_string::m_bounded_string<sizeof (M_C(name, _t))>(string).s))
#endif

M_END_PROTECTED_CODE

#endif
