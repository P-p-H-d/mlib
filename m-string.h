/*
 * M*LIB - Dynamic Size String Module
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
#ifndef MSTARLIB_STRING_H
#define MSTARLIB_STRING_H

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "m-core.h"

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
    M_ASSUME (v->ptr != NULL);                                          \
    STRINGI_ASSUME (string_size(v) == strlen(v->ptr));                  \
    M_ASSUME (v->ptr[string_size(v)] == 0);                             \
    M_ASSUME (string_size(v) < string_capacity(v));                     \
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
typedef struct {
  size_t size;
  size_t alloc;
} str_heap_t;

typedef struct {
  char buffer[sizeof (str_heap_t)];
} str_stack_t;

typedef union {
  str_heap_t heap;
  str_stack_t stack;
} str_union_t;

typedef struct {
  str_union_t u;
  char *ptr;
} string_t[1];

typedef struct string_s *string_ptr;
typedef const struct string_s *string_srcptr;

/* Input option for the fgets function */
typedef enum string_fgets_s {
  STRING_READ_LINE = 0, STRING_READ_PURE_LINE = 1, STRING_READ_FILE = 2
} string_fgets_t;

/* Test if the string is stack based or heap based */
static inline bool
string_int_stack_p(const string_t s)
{
  return (s->ptr == &s->u.stack.buffer[0]);
}

static inline void
string_int_set_size(string_t s, size_t size)
{
  if (string_int_stack_p(s)) {
    assert (size < sizeof (str_heap_t) - 1);
    s->u.stack.buffer[sizeof (str_heap_t) - 1] = size;
  } else
    s->u.heap.size = size;
}

static inline size_t
string_size(const string_t s)
{
  const size_t s_stack = s->u.stack.buffer[sizeof (str_heap_t) - 1];
  const size_t s_heap  = s->u.heap.size;
  return string_int_stack_p(s) ?  s_stack : s_heap;
}

static inline size_t
string_capacity(const string_t s)
{
  const size_t c_stack = sizeof (str_heap_t) - 1;
  const size_t c_heap  = s->u.heap.alloc;
  return string_int_stack_p(s) ?  c_stack : c_heap;
}

static inline void
string_init(string_t s)
{
  s->ptr = &s->u.stack.buffer[0];
  s->ptr[0] = 0;
  string_int_set_size(s, 0);
  STRINGI_CONTRACT(s);
}

static inline void
string_clear(string_t v)
{
  STRINGI_CONTRACT(v);
  if (!string_int_stack_p(v)) {    
    M_MEMORY_FREE(v->ptr);
  }
  /* This is not needed but is safer to make
     the string invalid so that it can be detected. */
  v->ptr   = NULL;
}

/* NOTE: Internaly used by STRING_DECL_INIT */
static inline void stringi_clear2(string_t *v) { string_clear(*v); }

/* NOTE: The ownership of the data is transfered back to the caller
   and the returned pointer has to be released by M_MEMORY_FREE. */
static inline char *
string_clear_get_str(string_t v)
{
  STRINGI_CONTRACT(v);
  char *p = v->ptr;
  if (string_int_stack_p(v)) {
    size_t alloc = string_size(v)+1;
    char *ptr = M_MEMORY_REALLOC (char, NULL, alloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      return NULL;
    }
    memcpy(ptr, p, alloc);
  }
  v->ptr = NULL;
  return p;
}

static inline void
string_clean(string_t v)
{
  STRINGI_CONTRACT (v);
  string_int_set_size(v, 0);
  v->ptr[0] = 0;
  STRINGI_CONTRACT (v);
}

static inline char
string_get_char(const string_t v, size_t index)
{
  STRINGI_CONTRACT (v);
  M_ASSUME(index < string_size(v));
  return v->ptr[index];
}

static inline bool
string_empty_p(const string_t v)
{
  STRINGI_CONTRACT (v);
  return v->ptr[0] == 0;
}

// Size includes the final null char
static inline void
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
      return;
    }
    char *ptr = v->ptr;
    ptr = string_int_stack_p(v) ? NULL : ptr;
    ptr = M_MEMORY_REALLOC (char, ptr, alloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is broken...
      abort();
      return;
    }
    M_ASSUME(ptr != &v->u.stack.buffer[0]);
    if (string_int_stack_p(v)) {
      /* Copy the stack allocation into the heap allocation */
      memcpy(ptr, &v->u.stack.buffer[0], v->u.stack.buffer[sizeof (str_heap_t) - 1]+1);
    }
    v->ptr = ptr;
    v->u.heap.alloc = alloc;
  }
  assert(v->ptr != NULL);
}

static inline void
string_reserve(string_t v, size_t alloc)
{
  STRINGI_CONTRACT (v);
  const size_t size = string_size(v);
  /* NOTE: Reserve below needed size, perform a shrink to fit */
  if (size + 1 > alloc) {
    alloc = size+1;
  }
  assert (alloc > 0);
  if (alloc < sizeof (str_heap_t)) {
    if (!string_int_stack_p(v)) {
      /* Transform Heap Allocate to Stack Allocate */
      char *ptr = &v->u.stack.buffer[0];
      memcpy(ptr, v->ptr, size+1);
      M_MEMORY_FREE(v->ptr);
      v->ptr = ptr;
      string_int_set_size(v, size);
    } else {
      /* Already a stack based alloc: nothing to do */
    }
  } else {
    assert(!string_int_stack_p(v));
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      return;
    }
    v->ptr = ptr; // Can be != ptr if v->ptr was NULL before.
    v->u.heap.alloc = alloc;
  }
  STRINGI_CONTRACT (v);
}

static inline void
string_set_str(string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  M_ASSUME(str != NULL);
  size_t size = strlen(str);
  stringi_fit2size(v, size+1);
  memcpy(v->ptr, str, size+1);
  string_int_set_size(v, size);
  STRINGI_CONTRACT (v);
}

static inline void
string_set_strn(string_t v, const char str[], size_t n)
{
  STRINGI_CONTRACT (v);
  M_ASSUME(str != NULL);
  size_t len  = strlen(str);
  size_t size = M_MIN (len, n);
  stringi_fit2size(v, size+1);
  memcpy(v->ptr, str, size);
  v->ptr[size] = 0;
  string_int_set_size(v, size);
  STRINGI_CONTRACT (v);
}

static inline const char*
string_get_cstr(const string_t v)
{
  STRINGI_CONTRACT (v);
  return v->ptr;
}

static inline void
string_set (string_t v1, const string_t v2)
{
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  if (M_LIKELY (v1 != v2)) {
    const size_t size = string_size(v2);
    stringi_fit2size(v1, size+1);
    memcpy(v1->ptr, v2->ptr, size+1);
    string_int_set_size(v1, size);
  }
  STRINGI_CONTRACT (v1);
}

static inline void
string_set_n(string_t v, const string_t ref, size_t offset, size_t length)
{
  STRINGI_CONTRACT (v);
  STRINGI_CONTRACT (ref);
  assert (offset <= string_size(ref));
  size_t size = M_MIN (string_size(ref) - offset, length);
  stringi_fit2size(v, size+1);
  memmove(v->ptr, ref->ptr + offset, size);
  v->ptr[size] = 0;
  string_int_set_size(v, size);
  STRINGI_CONTRACT (v);
}

static inline void
string_init_set(string_t v1, const string_t v2)
{
  string_init(v1);
  string_set(v1,v2);
}

static inline void
string_init_set_str(string_t v1, const char str[])
{
  string_init(v1);
  string_set_str(v1, str);
}

static inline void
string_init_move(string_t v1, string_t v2)
{
  STRINGI_CONTRACT (v2);
  memcpy(v1, v2, sizeof (string_t));
  if (string_int_stack_p(v2))
    v1->ptr = &v1->u.stack.buffer[0];
  // Note: nullify v2 to be safer
  v2->ptr   = NULL;
  STRINGI_CONTRACT (v1);
}

static inline void
string_swap(string_t v1, string_t v2)
{
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  M_SWAP (size_t, v1->u.heap.size,  v2->u.heap.size);
  M_SWAP (size_t, v1->u.heap.alloc, v2->u.heap.alloc);
  M_SWAP (char *, v1->ptr,   v2->ptr);
  bool s1 = string_int_stack_p(v1);
  bool s2 = string_int_stack_p(v1);
  if (s2)
    v1->ptr = &v1->u.stack.buffer[0];
  if (s1)
    v2->ptr = &v2->u.stack.buffer[0];
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
}

static inline void
string_move(string_t v1, string_t v2)
{
  string_clear(v1);
  string_init_move(v1,v2);
}

static inline void
string_push_back (string_t v, char c)
{
  STRINGI_CONTRACT (v);
  const size_t size = string_size(v);
  stringi_fit2size(v, size+2);
  v->ptr[size+0] = c;
  v->ptr[size+1] = 0;
  string_int_set_size(v, size+1);
  STRINGI_CONTRACT (v);
}

static inline void
string_cat_str(string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  M_ASSUME (str != NULL);
  const size_t old_size = string_size(v);
  const size_t size = strlen(str);
  stringi_fit2size(v, old_size + size + 1);
  memcpy(&v->ptr[old_size], str, size + 1);
  string_int_set_size(v, old_size + size);
  STRINGI_CONTRACT (v);
}

static inline void
string_cat(string_t v, const string_t v2)
{
  STRINGI_CONTRACT (v2);
  STRINGI_CONTRACT (v);
  const size_t size = string_size(v2);
  if (M_LIKELY (size > 0)) {
    const size_t old_size = string_size(v);
    stringi_fit2size(v, old_size + size + 1);
    memcpy(&v->ptr[old_size], v2->ptr, size);
    v->ptr[old_size + size] = 0;
    string_int_set_size(v, old_size + size);
  }
  STRINGI_CONTRACT (v);
}

static inline int
string_cmp_str(const string_t v1, const char str[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (str != NULL);
  return strcmp(string_get_cstr(v1), str);
}

static inline int
string_cmp(const string_t v1, const string_t v2)
{
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  return strcmp(string_get_cstr(v1), string_get_cstr(v2));
}

static inline bool
string_equal_str_p(const string_t v1, const char str[])
{
  STRINGI_CONTRACT(v1);
  M_ASSUME (str != NULL);
  return string_cmp_str(v1, str) == 0;
}

static inline bool
string_equal_p(const string_t v1, const string_t v2)
{
  /* string_equal_p can be called with one string which is an OOR value.
     In case of OOR value, .ptr is NULL and .size is maximum.
     It will detect a heap based string, and read size from heap structure.
  */
  assert(v1 != NULL);
  assert(v2 != NULL);
  return string_size(v1) == string_size(v2) && string_cmp(v1, v2) == 0;
}

// Note: doesn't work with UTF-8 strings...
static inline int
string_cmpi_str(const string_t v1, const char p2[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (p2 != NULL);
  // strcasecmp is POSIX
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

static inline int
string_cmpi(const string_t v1, const string_t v2)
{
  return string_cmpi_str(v1, string_get_cstr(v2));
}

static inline size_t
string_search_char (const string_t v, char c, size_t start)
{
  STRINGI_CONTRACT (v);
  assert (start <= string_size(v));
  const char *p = M_ASSIGN_CAST(const char*,
				strchr(string_get_cstr(v)+start, c));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search_rchar (const string_t v, char c, size_t start)
{
  STRINGI_CONTRACT (v);
  assert (start <= string_size(v));
  // NOTE: Can implement it in a faster way than the libc function
  // by scanning backward from the bottom of the string (which is
  // possible since we know the size)
  const char *p = M_ASSIGN_CAST(const char*,
				strrchr(string_get_cstr(v)+start, c));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search_str (const string_t v, const char str[], size_t start)
{
  STRINGI_CONTRACT (v);
  assert (start <= string_size(v));
  M_ASSUME (str != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
				strstr(string_get_cstr(v)+start, str));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search (const string_t v1, const string_t v2, size_t start)
{
  STRINGI_CONTRACT (v2);
  assert (start <= string_size(v1));
  return string_search_str(v1, string_get_cstr(v2), start);
}

static inline size_t
string_search_pbrk(const string_t v1, const char first_of[], size_t start)
{
  STRINGI_CONTRACT (v1);
  assert (start <= string_size(v1));
  M_ASSUME (first_of != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
				strpbrk(string_get_cstr(v1)+start, first_of));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v1));
}

static inline int
string_strcoll_str(const string_t v, const char str[])
{
  STRINGI_CONTRACT (v);
  return strcoll(string_get_cstr(v), str);
}

static inline int
string_strcoll (const string_t v1, const string_t v2)
{
  STRINGI_CONTRACT (v2);
  return string_strcoll_str(v1, string_get_cstr(v2));
}

static inline size_t
string_spn(const string_t v1, const char accept[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (accept != NULL);
  return strspn(string_get_cstr(v1), accept);
}

static inline size_t
string_cspn(const string_t v1, const char reject[])
{
  STRINGI_CONTRACT (v1);
  M_ASSUME (reject != NULL);
  return strcspn(string_get_cstr(v1), reject);
}

static inline void
string_left(string_t v, size_t index)
{
  STRINGI_CONTRACT (v);
  const size_t size = string_size(v);
  if (index >= size)
    return;
  M_ASSUME (v->ptr != NULL);
  v->ptr[index] = 0;
  string_int_set_size(v,index);
  STRINGI_CONTRACT (v);
}

// Note: this is an index, not the size to keep from the right !
static inline void
string_right(string_t v, size_t index)
{
  STRINGI_CONTRACT (v);
  const size_t size = string_size(v);
  if (index >= size) {
    v->ptr[0] = 0;
    string_int_set_size(v, 0);
    STRINGI_CONTRACT (v);
    return;
  }
  size_t s2 = size - index;
  memmove (&v->ptr[0], &v->ptr[index], s2+1);
  string_int_set_size(v, s2);
  STRINGI_CONTRACT (v);
}

static inline void
string_mid (string_t v, size_t index, size_t size)
{
  string_right(v, index);
  string_left(v, size);
}

static inline size_t
string_replace_str (string_t v, const char str1[], const char str2[], size_t start)
{
  STRINGI_CONTRACT (v);
  M_ASSUME (str1 != NULL && str2 != NULL);
  size_t i = string_search_str(v, str1, start);
  if (i != STRING_FAILURE) {
    const size_t str1_l = strlen(str1);
    const size_t str2_l = strlen(str2);
    const size_t size   = string_size(v);
    assert(size + 1 + str2_l > str1_l);
    stringi_fit2size (v, size + str2_l - str1_l + 1);
    if (str1_l != str2_l) {
      memmove(&v->ptr[i+str2_l], &v->ptr[i+str1_l], size - i - str1_l + 1);
      string_int_set_size(v, size + str2_l - str1_l);
    }
    memcpy (&v->ptr[i], str2, str2_l);
    STRINGI_CONTRACT (v);
  }
  return i;
}

static inline size_t
string_replace (string_t v, const string_t v1, const string_t v2, size_t start)
{
  STRINGI_CONTRACT (v);
  STRINGI_CONTRACT (v1);
  STRINGI_CONTRACT (v2);
  return string_replace_str(v, string_get_cstr(v1), string_get_cstr(v2), start);
}

static inline void
string_replace_at (string_t v, size_t pos, size_t len, const char str2[])
{
  STRINGI_CONTRACT (v);
  M_ASSUME (pos+len < string_size(v) && str2 != NULL);
  const size_t str1_l = len;
  const size_t str2_l = strlen(str2);
  const size_t size   = string_size(v);
  stringi_fit2size (v, size + str2_l - str1_l + 1);
  if (str1_l != str2_l) {
    memmove(&v->ptr[pos+str2_l], &v->ptr[pos+str1_l], size - pos - str1_l + 1);
    string_int_set_size(v, size + str2_l - str1_l);
  }
  memcpy (&v->ptr[pos], str2, str2_l);
  STRINGI_CONTRACT (v);
}

static inline int
string_printf (string_t v, const char format[], ...)
{
  STRINGI_CONTRACT (v);
  M_ASSUME (format != NULL);
  va_list args;
  int size;
  size_t alloc = string_capacity(v);
  va_start (args, format);
  size = vsnprintf (v->ptr, alloc, format, args);
  if (size > 0 && ((size_t) size+1 >= alloc) ) {
    // We have to realloc our string to fit the needed size
    stringi_fit2size (v, (size_t) size + 1);
    // and redo the parsing.
    va_end (args);
    va_start (args, format);
    alloc = string_capacity(v);
    size = vsnprintf (v->ptr, alloc, format, args);
    assert (size > 0 && (size_t)size < alloc);
  }
  if (size >= 0) {
    string_int_set_size(v, (size_t) size);
  }
  va_end (args);
  STRINGI_CONTRACT (v);
  return size;
}

static inline int
string_cat_printf (string_t v, const char format[], ...)
{
  STRINGI_CONTRACT (v);
  M_ASSUME (format != NULL);
  va_list args;
  int size;
  size_t old_size = string_size(v);
  size_t alloc = string_capacity(v);
  va_start (args, format);
  size = vsnprintf (&v->ptr[old_size], alloc - old_size, format, args);
  if (size > 0 && (old_size+(size_t)size+1 >= alloc) ) {
    // We have to realloc our string to fit the needed size
    stringi_fit2size (v, old_size + (size_t) size + 1);
    alloc = string_capacity(v);
    // and redo the parsing.
    va_end (args);
    va_start (args, format);
    size = vsnprintf (&v->ptr[old_size], alloc - old_size, format, args);
    assert (size >= 0);
  }
  if (size >= 0) {
    string_int_set_size(v, old_size + (size_t) size);
  } else {
    // vsnprintf may have output some characters before returning an error.
    // Undo this to have a clean state
    v->ptr[old_size] = 0;
  }
  va_end (args);
  STRINGI_CONTRACT (v);
  return size;
}

static inline bool
string_fgets(string_t v, FILE *f, string_fgets_t arg)
{
  STRINGI_CONTRACT(v);
  assert (f != NULL);
  stringi_fit2size (v, 100);
  size_t size = 0;
  size_t alloc = string_capacity(v);
  v->ptr[0] = 0;
  bool retcode = false; /* Nothing has been read yet */
  while (fgets(&v->ptr[size], alloc - size, f) != NULL) {
    retcode = true; /* Something has been read */
    size += strlen(&v->ptr[size]);
    if (arg != STRING_READ_FILE && v->ptr[size-1] == '\n') {
      if (arg == STRING_READ_PURE_LINE) {
        size --;
        v->ptr[size] = 0;         /* Remove EOL */
      }
      string_int_set_size(v, size);
      STRINGI_CONTRACT(v);
      return retcode; /* Normal terminaison */
    } else if (v->ptr[size-1] != '\n' && !feof(f)) {
      /* The string buffer is not big enough:
         increase it and continue reading */
      /* v cannot be stack alloc */
      stringi_fit2size (v, alloc + alloc/2);
      alloc = string_capacity(v);
    }
  }
  string_int_set_size(v, size);
  STRINGI_CONTRACT (v);
  return retcode; /* Abnormal terminaison */
}

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
  v->ptr[0] = 0;

  /* NOTE: We generate a buffer which we give to scanf to parse the string,
     that it is to say, we generate the format dynamically!
     The format is like " %49[^ \t.\n]%c"
     So in this case, we parse up to 49 chars, up to the separators char,
     and we read the next char. If the next char is a separator, we successful
     read a word, otherwise we have to continue parsing.
     The user shall give a constant string as the separator argument,
     as a control over this argument may give an attacker
     an opportunity for stack overflow */
  size_t alloc = string_capacity(v);
  while (snprintf(buffer, sizeof buffer -1, " %%%zu[^%s]%%c",
                  (size_t) alloc-1-size, separator) > 0
         && fscanf(f, buffer, &v->ptr[size], &c) == 2) {
    retcode = true;
    size += strlen(&v->ptr[size]);
    if (strchr(separator, c) != NULL)
      break;
    /* Next char is not a separator: continue parsing */
    string_int_set_size(v, size);
    stringi_fit2size (v, alloc + alloc/2);
    alloc = string_capacity(v);
    assert (alloc > size + 1);
    v->ptr[size++] = c;
    v->ptr[size] = 0;
  }
  string_int_set_size(v, size);
  STRINGI_CONTRACT(v);  
  return retcode;
}

static inline bool
string_fputs(FILE *f, const string_t v)
{
  STRINGI_CONTRACT (v);
  assert (f != NULL);
  return fputs(string_get_cstr(v), f) >= 0;
}

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

static inline bool
string_start_with_string_p(const string_t v, const string_t v2)
{
  STRINGI_CONTRACT (v2);
  return string_start_with_str_p (v, string_get_cstr(v2));
}

static inline size_t
string_hash(const string_t v)
{
  STRINGI_CONTRACT (v);
  // TODO: alignemnt incorrect if stack based & string_t is 32-bits aligned ?...
  return m_core_hash(v->ptr, string_size(v));
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

static inline void
string_strim(string_t v, const char charac[])
{
  STRINGI_CONTRACT (v);
  char *b = v->ptr;
  size_t size = string_size(v);
  while (size > 0 && stringi_strim_char(b[size-1], charac))
    size --;
  if (size > 0) {
    while (stringi_strim_char(*b, charac))
      b++;
    M_ASSUME (b >= v->ptr &&  size >= (size_t) (b - v->ptr) );
    size -= (b - v->ptr);
    memmove (v->ptr, b, size);
  }
  v->ptr[size] = 0;
  string_int_set_size(v, size);
  STRINGI_CONTRACT (v);
}

static inline bool
string_oor_equal_p(const string_t s, unsigned char n)
{
  return (s->ptr == NULL) & (s->u.heap.size == ~(size_t)n);
}

static inline void
string_oor_set(string_t s, unsigned char n)
{
  s->ptr = NULL;
  s->u.heap.size = ~(size_t)n;
}

/* I/O */
/* Output: "string" with quote around
   Replace " by \" within the string (and \ to \\)
   \n, \t & \r by their standard representation
   and other not printable character with \0xx */
static inline void
string_get_str(string_t v, const string_t v2, bool append)
{
  STRINGI_CONTRACT(v2);
  STRINGI_CONTRACT(v);
  M_ASSUME (v != v2); // Limitation
  size_t size = append ? string_size(v) : 0;
  size_t v2_size = string_size(v2);
  size_t targetSize = size + v2_size + 3;
  stringi_fit2size(v, targetSize);
  v->ptr[size ++] = '"';
  for(size_t i = 0 ; i < v2_size; i++) {
    const char c = v2->ptr[i];
    switch (c) {
    case '\\':
    case '"':
    case '\n':
    case '\t':
    case '\r':
      // Special characters which can be displayed in a short form.
      string_int_set_size(v, size);
      stringi_fit2size(v, ++targetSize);
      v->ptr[size ++] = '\\';
      // This string acts as a perfect hashmap which supposes an ASCII mapping
      // and (c^(c>>5)) is the hash function
      v->ptr[size ++] = " tn\" r\\"[(c ^ (c >>5)) & 0x07];
      break;
    default:
      if (M_UNLIKELY (!isprint(c))) {
        targetSize += 3;
        string_int_set_size(v, size);
        stringi_fit2size(v, targetSize);
        int d1 = c & 0x07, d2 = (c>>3) & 0x07, d3 = (c>>6) & 0x07;
        v->ptr[size ++] = '\\';
        v->ptr[size ++] = '0' + d3;
        v->ptr[size ++] = '0' + d2;
        v->ptr[size ++] = '0' + d1;
      } else {
        v->ptr[size ++] = c;
      }
      break;
    }
  }
  v->ptr[size ++] = '"';
  v->ptr[size] = 0;
  string_int_set_size(v, size);
  assert (size <= targetSize);
  STRINGI_CONTRACT (v);
}

static inline void
string_out_str(FILE *f, const string_t v)
{
  STRINGI_CONTRACT(v);
  M_ASSUME (f != NULL);
  fputc('"', f);
  size_t size = string_size(v);
  for(size_t i = 0 ; i < size; i++) {
    const char c = v->ptr[i];
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

/* UTF8 Handling */
typedef enum {
  STRINGI_UTF8_STARTING = 0,
  STRINGI_UTF8_DECODING_1 = 8,
  STRINGI_UTF8_DECODING_2 = 16,
  STRINGI_UTF8_DOCODING_3 = 24,
  STRINGI_UTF8_ERROR = 32
} stringi_utf8_state_e;

typedef unsigned int string_unicode_t;
#define STRING_UNICODE_ERROR (-1U)

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
/* The use of a string allows the compiler/linker to factorize it. */
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
  const int type = m_core_clz32((unsigned char)~c) - (sizeof(uint32_t) - 1) * CHAR_BIT;
  const string_unicode_t mask1 = -(string_unicode_t)(*state != STRINGI_UTF8_STARTING);
  const string_unicode_t mask2 = (0xFFU >> type);
  *unicode = ((*unicode << 6) & mask1) | (c & mask2);
  *state = M_ASSIGN_CAST(stringi_utf8_state_e,
                         STRINGI_UTF8_STATE_TAB[*state + type]);
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

/* Computer the number of unicode characters are represented in the UTF8 stream */
static inline size_t
stringi_utf8_length(const char str[])
{
  size_t size = 0;
  stringi_utf8_state_e s = STRINGI_UTF8_STARTING;
  string_unicode_t u = 0;
  while (*str) {
    stringi_utf8_decode(*str, &s, &u);
    if (M_UNLIKELY (s == STRINGI_UTF8_ERROR)) return -1;
    size += (s == STRINGI_UTF8_STARTING);
    str++;
  }
  return size;
}

/* Encode an unicode into an UTF8 stream of characters */
static inline int
stringi_utf8_encode(char buffer[5], string_unicode_t u)
{
  if (M_LIKELY (u <= 0x7F)) {
    buffer[0] = u;
    buffer[1] = 0;
    return 1;
  } else if (u <= 0x7FF) {
    buffer[0] = 0xC0 | (u >> 6);
    buffer[1] = 0x80 | (u & 0x3F);
    buffer[2] = 0;
    return 2;
  } else if (u <= 0xFFFF) {
    buffer[0] = 0xE0 | (u >> 12);
    buffer[1] = 0x80 | ((u >> 6) & 0x3F);
    buffer[2] = 0x80 | (u & 0x3F);
    buffer[3] = 0;
    return 3;
  } else {
    buffer[0] = 0xF0 | (u >> 18);
    buffer[1] = 0x80 | ((u >> 12) & 0x3F);
    buffer[2] = 0x80 | ((u >> 6) & 0x3F);
    buffer[3] = 0x80 | (u & 0x3F);
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

static inline bool
string_end_p (string_it_t it)
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

static inline void
string_next (string_it_t it)
{
  assert (it != NULL);
  it->ptr = it->next_ptr;
}

static inline string_unicode_t
string_get_cref (const string_it_t it)
{
  assert (it != NULL);
  return it->u;
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
    size_t size = string_size(str);                                     \
    string_init(tmp);                                                   \
    M_CALL_CLEAN(oplist, cont);                                         \
    for(size_t i = 0 ; i < size; i++) {                                 \
      char c = string_get_char(str, i);                                 \
      if (c == sep) {                                                   \
        string_set_strn(tmp, &str->ptr[begin], i - begin);              \
        /* If push move method is available, use it */                  \
        M_IF_METHOD(PUSH_MOVE,oplist)(                                  \
                                      M_CALL_PUSH_MOVE(oplist, cont, &tmp); \
                                      string_init(tmp);                 \
                                      ,                                 \
                                      M_CALL_PUSH(oplist, cont, tmp);   \
                                                                        ) \
          begin = i + 1;                                                \
      }                                                                 \
    }                                                                   \
    string_set_strn(tmp, &str->ptr[begin], size - begin);               \
    M_CALL_PUSH(oplist, cont, tmp);                                     \
    /* HACK: if method reverse is defined, it is likely that we have */ \
    /* inserted the items in the wrong order (aka for a list) */        \
    M_IF_METHOD(REVERSE, oplist) (M_CALL_REVERSE(oplist, cont);, )      \
    string_clear(tmp);                                                  \
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
   is a needed asumption by string_hash).
   Otherwise it could have been :
   #define STRING_CTE(s)                                          \
     ((const string_t){{.size = sizeof(s)-1, .alloc = sizeof(s),  \
     .ptr = s}})
   which produces faster code.
   Note: This code doesn't work with C++ (use of c99 feature
   of recursive struct definition). As such, there is a separate
   C++ definition.
*/
#ifndef __cplusplus
# define STRING_CTE(s)                                                  \
  ((const string_t){{.u.heap = { .size = sizeof(s)-1, .alloc = sizeof(s) } , \
        .ptr = ((struct { long long _n; char _d[sizeof (s)]; }){ 0, s })._d }})
#else
namespace m_string {
  template <int N>
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
#define STRING_CTE(s)                                                   \
  m_string::m_aligned_string<sizeof (s)>(s).string
#endif

#define STRING_INIT_PRINTF(v, ...)                      \
  (string_init (v),  string_printf (v, __VA_ARGS__) ) 

/* NOTE: Use GCC extension FIXME: To keep? */
#define STRING_DECL_INIT(v)                                             \
  string_t v __attribute__((cleanup(stringi_clear2))) = {{ 0, 0, NULL}}

#define STRING_DECL_INIT_PRINTF(v, format, ...)                         \
  STRING_DECL_INIT(v);                                                  \
  string_printf (v, format, __VA_ARGS__)


/* Define the OPLIST of a STRING */
#define STRING_OPLIST                                                   \
  (INIT(string_init),INIT_SET(string_init_set), SET(string_set),        \
   INIT_WITH(STRING_INIT_PRINTF),                                       \
   INIT_MOVE(string_init_move), MOVE(string_move),                      \
   SWAP(string_swap), CLEAN(string_clean),                              \
   TEST_EMPTY(string_empty_p),                                          \
   CLEAR(string_clear), HASH(string_hash), EQUAL(string_equal_p),       \
   CMP(string_cmp), TYPE(string_t),                                     \
   PARSE_STR(string_parse_str), GET_STR(string_get_str),                \
   OUT_STR(string_out_str), IN_STR(string_in_str),                      \
   EXT_ALGO(STRING_SPLIT),                                              \
   OOR_EQUAL(string_oor_equal_p), OOR_SET(string_oor_set)               \
   )

/* Register the OPLIST as a global one */
#define M_OPL_string_t() STRING_OPLIST


/* Macro encapsulation to give a default value of 0 for start offset */
#define string_search_char(v, ...)					\
  M_APPLY(string_search_char, v, M_IF_DEFAULT1(0, __VA_ARGS__))

#define string_search_rchar(v, ...)					\
  M_APPLY(string_search_rchar, v, M_IF_DEFAULT1(0, __VA_ARGS__))

#define string_search_str(v, ...)					\
  M_APPLY(string_search_str, v, M_IF_DEFAULT1(0, __VA_ARGS__))

#define string_search(v, ...)						\
  M_APPLY(string_search, v, M_IF_DEFAULT1(0, __VA_ARGS__))

#define string_search_pbrk(v, ...)					\
  M_APPLY(string_search_pbrk, v, M_IF_DEFAULT1(0, __VA_ARGS__))

#define string_replace_str(v, s1, ...)					\
  M_APPLY(string_replace_str, v, s1, M_IF_DEFAULT1(0, __VA_ARGS__))

#define string_replace(v, s1, ...)					\
  M_APPLY(string_replace, v, s1, M_IF_DEFAULT1(0, __VA_ARGS__))

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

#define string_set(a,b) STRINGI_SELECT2(string_set, string_set_str, a, b)
#define string_init_set(a,b) STRINGI_SELECT2(string_init_set, string_init_set_str, a, b)
#define string_set(a,b) STRINGI_SELECT2(string_set, string_set_str, a, b)
#define string_cat(a,b) STRINGI_SELECT2(string_cat, string_cat_str, a, b)
#define string_cmp(a,b) STRINGI_SELECT2(string_cmp, string_cmp_str, a, b)
#define string_equal_p(a,b) STRINGI_SELECT2(string_equal_p, string_equal_str_p, a, b)
#define string_strcoll(a,b) STRINGI_SELECT2(string_strcoll, string_strcoll_str, a, b)

#undef string_search
#define string_search(v, ...)						\
  M_APPLY(STRINGI_SELECT3, string_search, string_search_str,		\
	  v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Provide GET_STR method to default type */
#undef M_GET_STR_METHOD_FOR_DEFAULT_TYPE
#define M_GET_STR_METHOD_FOR_DEFAULT_TYPE GET_STR(M_GET_STRING_ARG)

#endif


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
  M_C(name, _init)(M_C(name,_t) s)                                      \
  {                                                                     \
    assert(s != NULL);                                                  \
    assert(max_size >= 1);                                              \
    s->s[0] = 0;                                                        \
    s->s[max_size] = 0;                                                 \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _clear)(M_C(name,_t) s)                                     \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    /* nothing to do */                                                 \
    (void) s;                                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _clean)(M_C(name,_t) s)                                     \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    s->s[0] = 0;                                                        \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name, _size)(const M_C(name,_t) s)                                \
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
  M_C(name, _empty_p)(const M_C(name,_t) s)                             \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    return s->s[0] == 0;                                                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _set_str)(M_C(name,_t) s, const char str[])                 \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    strncpy(s->s, str, max_size);                                       \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _set_strn)(M_C(name,_t) s, const char str[], size_t n)      \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert(str != NULL);                                                \
    size_t len = M_MIN(max_size, n);                                    \
    strncpy(s->s, str, len);                                            \
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
  M_C(name, _set)(M_C(name,_t) s, const M_C(name,_t) str)               \
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
  M_C(name, _init_set)(M_C(name,_t) s, const M_C(name,_t) str)          \
  {                                                                     \
    M_C(name,_init)(s);                                                 \
    M_C(name,_set)(s, str);                                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init_set_str)(M_C(name,_t) s, const char str[])            \
  {                                                                     \
    M_C(name,_init)(s);                                                 \
    M_C(name,_set_str)(s, str);                                         \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _cat_str)(M_C(name,_t) s, const char str[])                 \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(s, max_size);                              \
    assert (str != NULL);                                               \
    assert (strlen(s->s) <= max_size);                                  \
    strncat(s->s, str, max_size-strlen(s->s));                          \
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
  M_C(name, _equal_p)(const M_C(name,_t) s, const M_C(name,_t) str)     \
  {                                                                     \
    /* _equal_p may be called in context OOR. So contract cannot be verified */ \
    return (s->s[max_size] == str->s[max_size]) & (strcmp(s->s, str->s) == 0); \
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
    while (*str) M_HASH_UP(hash, *str++);                               \
    return M_HASH_FINAL(hash);                                          \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _oor_equal_p)(const M_C(name,_t) s, unsigned char n)        \
  {                                                                     \
    /* s may be invalid contract */                                     \
    assert (s != NULL);                                                 \
    return s->s[max_size] == n+1;                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _oor_set)(M_C(name,_t) s, unsigned char n)                  \
  {                                                                     \
    /* s may be invalid contract */                                     \
    assert (s != NULL);                                                 \
    s->s[max_size] = n+1;                                               \
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
    string_init(v2);                                                    \
    bool ret = string_in_str(v2, f);                                    \
    strncpy(v->s, v2->ptr, max_size);                                   \
    string_clear(v2);                                                   \
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _parse_str)(M_C(name,_t) v, const char str[], const char **endptr) \
  {                                                                     \
    BOUNDED_STRINGI_CONTRACT(v, max_size);                              \
    assert(str != NULL);                                                \
    string_t v2;                                                        \
    string_init(v2);                                                    \
    bool ret = string_parse_str(v2, str, endptr);                       \
    strncpy(v->s, v2->ptr, max_size);                                   \
    string_clear(v2);                                                   \
    return ret;                                                         \
  }                                                                     \


/* Define the OPLIST of a BOUNDED_STRING */
#define BOUNDED_STRING_OPLIST(name)                                     \
  (INIT(M_C(name,_init)), INIT_SET(M_C(name,_init_set)),                \
   SET(M_C(name,_set)), CLEAR(M_C(name,_clear)), HASH(M_C(name,_hash)), \
   EQUAL(M_C(name,_equal_p)), CMP(M_C(name,_cmp)), TYPE(M_C(name,_t)),  \
   OOR_EQUAL(M_C(name,_oor_equal_p)), OOR_SET(M_C(name, _oor_set))      \
   PARSE_STR(M_C(name,_parse_str)), GET_STR(M_C(name,_get_str)),        \
   OUT_STR(M_C(name,_out_str)), IN_STR(M_C(name,_in_str)),              \
   )
   
/* Init a constant bounded string.
   Try to do a clean cast */
/* Use of Compound Literals to init a constant string.
   See above */
#ifndef __cplusplus
#define BOUNDED_STRING_CTE(name, string)                                \
  ((const struct M_C(name, _s) *)((M_C(name, _array_t)){string}))
#else
namespace m_string {
  template <int N>
    struct m_bounded_string {
      char s[N];
      inline m_bounded_string(const char lstr[])
      {
        memset(this->s, 0, N);
        strncpy(this->s, lstr, N-1);
      }
    };
}
#define BOUNDED_STRING_CTE(name, string)                \
  ((const struct M_C(name, _s) *)(m_string::m_bounded_string<sizeof (M_C(name, _t))>(string).s))
#endif


#endif
