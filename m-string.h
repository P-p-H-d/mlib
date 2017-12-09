/*
 * M*LIB - Dynamic Size String Module
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
#ifndef __M_STRING_H
#define __M_STRING_H

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

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
#define STRING_CONTRACT(v) do {                                         \
    M_ASSUME (v != NULL);                                               \
    M_ASSUME (v->ptr != NULL || (v->size == 0 && v->alloc == 0));       \
    STRINGI_ASSUME (v->ptr == NULL || v->size == strlen(v->ptr));	\
    M_ASSUME (v->ptr == NULL || v->ptr[v->size] == 0);                  \
    M_ASSUME (v->ptr == NULL || v->size < v->alloc);                    \
  } while (0)


/********************************** EXTERNAL ************************************/

/* Index returned in case of error */
#define STRING_FAILURE ((size_t)-1)

typedef struct string_s {
  size_t size, alloc;
  char *ptr;
} string_t[1];

/* Input option for some functions */
typedef enum string_fgets_s {
  STRING_READ_LINE = 0, STRING_READ_PURE_LINE = 1, STRING_READ_FILE = 2
} string_fgets_t;

static inline void
string_init(string_t v)
{
  v->size = v->alloc = 0;
  v->ptr = NULL;
  STRING_CONTRACT(v);
}

static inline void
string_clear(string_t v)
{
  STRING_CONTRACT(v);
  M_MEMORY_FREE(v->ptr);
  v->size = v->alloc = 0;
  v->ptr = NULL;
}

/* NOTE: Internaly used by STRING_DECL_INIT */
static inline void stringi_clear2(string_t *v) { string_clear(*v); }

/* NOTE: Returned pointer has to be released by M_MEMORY_FREE. */
static inline char *
string_clear_get_str(string_t v)
{
  STRING_CONTRACT(v);
  char *p = v->ptr;
  v->size = v->alloc = 0;
  v->ptr = NULL;
  return p;
}

static inline void
string_clean(string_t v)
{
  STRING_CONTRACT (v);
  v->size = 0;
  if (M_LIKELY (v->ptr))
    v->ptr[0] = 0;
  STRING_CONTRACT (v);
}

static inline size_t
string_size(const string_t v)
{
  STRING_CONTRACT (v);
  return v->size;
}

static inline size_t
string_capacity(const string_t v)
{
  STRING_CONTRACT(v);
  return v->alloc;
}

static inline char
string_get_char(const string_t v, size_t index)
{
  STRING_CONTRACT (v);
  M_ASSUME( v->ptr != NULL && index < v->size);
  return v->ptr[index];
}

static inline bool
string_empty_p(const string_t v)
{
  STRING_CONTRACT (v);
  return v->size == 0;
}

static inline void
stringi_fit2size (string_t v, size_t size)
{
  // Note: this function may be called in context where the contract
  // is not fullfilled.
  if (size > v->alloc) {
    size_t alloc = size < 16 ? 16 : (size + size / 2);
    if (M_UNLIKELY (alloc <= v->alloc)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is broken...
      return;
    }
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is broken...
      return;
    }
    v->ptr = ptr;
    v->alloc = alloc;
  }
}

static inline void
string_reserve(string_t v, size_t alloc)
{
  STRING_CONTRACT (v);
  /* NOTE: Reserve below needed size, perform a shrink to fit */
  if (v->size + 1 > alloc) {
    alloc = v->size+1;
  }
  assert (alloc > 0);
  if (M_UNLIKELY (alloc == 1)) {
    // Only 1 byte reserved for the NUL char ==> free
    M_MEMORY_FREE(v->ptr);
    v->size = v->alloc = 0;
    v->ptr = NULL;
  } else {
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      return;
    }
    v->ptr = ptr; // Can be != ptr if v->ptr was NULL before.
    v->alloc = alloc;
  }
  STRING_CONTRACT (v);
}

static inline void
string_set_str(string_t v, const char str[])
{
  STRING_CONTRACT (v);
  M_ASSUME(str != NULL);
  size_t size = strlen(str);
  stringi_fit2size(v, size+1);
  memcpy(v->ptr, str, size+1);
  v->size = size;
  STRING_CONTRACT (v);
}

static inline void
string_set_strn(string_t v, const char str[], size_t n)
{
  STRING_CONTRACT (v);
  M_ASSUME(str != NULL);
  size_t len  = strlen(str);
  size_t size = M_MIN (len, n);
  stringi_fit2size(v, size+1);
  memcpy(v->ptr, str, size);
  v->ptr[size] = 0;
  v->size = size;
  STRING_CONTRACT (v);
}

static inline const char*
string_get_cstr(const string_t v)
{
  STRING_CONTRACT (v);
  return M_UNLIKELY (v->ptr == NULL) ? "" : v->ptr;
}

static inline void
string_set (string_t v1, const string_t v2)
{
  STRING_CONTRACT (v1);
  STRING_CONTRACT (v2);
  if (M_LIKELY (v1 != v2)) {
    size_t size = v2->size;
    stringi_fit2size(v1, size+1);
    if (M_LIKELY (v2->ptr != NULL)) {
      memcpy(v1->ptr, v2->ptr, size+1);
    } else {
      assert (size == 0);
      v1->ptr[0] = 0;
    }
    v1->size = size;
  }
  STRING_CONTRACT (v1);
}

static inline void
string_set_n(string_t v, const string_t ref, size_t offset, size_t length)
{
  STRING_CONTRACT (v);
  STRING_CONTRACT (ref);
  assert (offset <= ref->size);
  assert (v != ref); // Not supported yet
  if (M_UNLIKELY (ref->size == 0)) {
    v->size = 0;
    if (v->ptr) {
      v->ptr[0] = 0;
    }
  } else {
    assert (ref->ptr != NULL);
    string_set_strn(v, ref->ptr + offset, length);
  }
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
  STRING_CONTRACT (v2);
  v1->size  = v2->size;
  v1->alloc = v2->alloc;
  v1->ptr   = v2->ptr;
  // Note: nullify v2 to be safer
  v2->size  = 0;
  v2->alloc = 0;
  v2->ptr   = NULL;
  STRING_CONTRACT (v1);
}

static inline void
string_swap(string_t v1, string_t v2)
{
  STRING_CONTRACT (v1);
  STRING_CONTRACT (v2);
  M_SWAP (size_t, v1->size,  v2->size);
  M_SWAP (size_t, v1->alloc, v2->alloc);
  M_SWAP (char *, v1->ptr,   v2->ptr);
  STRING_CONTRACT (v1);
  STRING_CONTRACT (v2);
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
  STRING_CONTRACT (v);
  stringi_fit2size(v, v->size+2);
  v->ptr[v->size++] = c;
  v->ptr[v->size] = 0;
  STRING_CONTRACT (v);
}

static inline void
string_cat_str(string_t v, const char str[])
{
  STRING_CONTRACT (v);
  M_ASSUME (str != NULL);
  const size_t size = strlen(str);
  stringi_fit2size(v, v->size + size + 1);
  memcpy(&v->ptr[v->size], str, size + 1);
  v->size += size;
  STRING_CONTRACT (v);
}

static inline void
string_cat(string_t v, const string_t v2)
{
  STRING_CONTRACT (v2);
  STRING_CONTRACT (v);
  const size_t size = v2->size;
  if (M_LIKELY (size > 0)) {
    stringi_fit2size(v, v->size + size + 1);
    assert (v2->ptr != NULL);
    memcpy(&v->ptr[v->size], v2->ptr, size);
    v->size += size;
    v->ptr[v->size] = 0;
  }
  STRING_CONTRACT (v);
}

static inline int
string_cmp_str(const string_t v1, const char str[])
{
  STRING_CONTRACT (v1);
  M_ASSUME (str != NULL);
  return strcmp(string_get_cstr(v1), str);
}

static inline int
string_cmp(const string_t v1, const string_t v2)
{
  STRING_CONTRACT (v1);
  STRING_CONTRACT (v2);
  return strcmp(string_get_cstr(v1), string_get_cstr(v2));
}

static inline bool
string_equal_str_p(const string_t v1, const char str[])
{
  STRING_CONTRACT(v1);
  M_ASSUME (str != NULL);
  return string_cmp_str(v1, str) == 0;
}

static inline bool
string_equal_p(const string_t v1, const string_t v2)
{
  STRING_CONTRACT(v1);
  STRING_CONTRACT(v2);
  return v1->size == v2->size && string_cmp(v1, v2) == 0;
}

// Note: doesn't work with UTF-8 strings...
static inline int
string_cmpi_str(const string_t v1, const char p2[])
{
  STRING_CONTRACT (v1);
  M_ASSUME (p2 != NULL);
  // strcasecmp is POSIX
  const char *p1 = string_get_cstr(v1);
  int c1, c2;
  do {
    // To avoid locale without 1 per 1 mapping.
    c1 = tolower((unsigned char) toupper((unsigned char) *p1++));
    c2 = tolower((unsigned char) toupper((unsigned char) *p2++));
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
  STRING_CONTRACT (v);
  assert (start <= v->size);
  const char *p = M_ASSIGN_CAST(const char*,
				strchr(string_get_cstr(v)+start, c));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search_rchar (const string_t v, char c, size_t start)
{
  STRING_CONTRACT (v);
  assert (start <= v->size);
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
  STRING_CONTRACT (v);
  assert (start <= v->size);
  M_ASSUME (str != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
				strstr(string_get_cstr(v)+start, str));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search (const string_t v1, const string_t v2, size_t start)
{
  STRING_CONTRACT (v2);
  assert (start <= v1->size);
  return string_search_str(v1, string_get_cstr(v2), start);
}

static inline size_t
string_search_pbrk(const string_t v1, const char first_of[], size_t start)
{
  STRING_CONTRACT (v1);
  assert (start <= v1->size);
  M_ASSUME (first_of != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
				strpbrk(string_get_cstr(v1)+start, first_of));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v1));
}

static inline int
string_strcoll_str(const string_t v, const char str[])
{
  STRING_CONTRACT (v);
  return strcoll(string_get_cstr(v), str);
}

static inline int
string_strcoll (const string_t v1, const string_t v2)
{
  STRING_CONTRACT (v2);
  return string_strcoll_str(v1, string_get_cstr(v2));
}

static inline size_t
string_spn(const string_t v1, const char accept[])
{
  STRING_CONTRACT (v1);
  M_ASSUME (accept != NULL);
  return strspn(string_get_cstr(v1), accept);
}

static inline size_t
string_cspn(const string_t v1, const char accept[])
{
  STRING_CONTRACT (v1);
  M_ASSUME (accept != NULL);
  return strcspn(string_get_cstr(v1), accept);
}

static inline void
string_left(string_t v, size_t index)
{
  STRING_CONTRACT (v);
  if (index >= v->size)
    return;
  M_ASSUME (v->ptr != NULL);
  v->ptr[index] = 0;
  v->size = index;
  STRING_CONTRACT (v);
}

// Note: this is an index, not the size to keep from the right !
static inline void
string_right(string_t v, size_t index)
{
  STRING_CONTRACT (v);
  if (index >= v->size) {
    if (v->ptr != NULL) {
      v->ptr[0] = 0;
      v->size = 0;
    }
    STRING_CONTRACT (v);
    return;
  }
  M_ASSUME (v->ptr != NULL);
  size_t s2 = v->size - index;
  memmove (&v->ptr[0], &v->ptr[index], s2+1);
  v->size = s2;
  STRING_CONTRACT (v);
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
  STRING_CONTRACT (v);
  M_ASSUME (str1 != NULL && str2 != NULL);
  size_t i = string_search_str(v, str1, start);
  if (i != STRING_FAILURE) {
    size_t str1_l = strlen(str1);
    size_t str2_l = strlen(str2);
    stringi_fit2size (v, v->size + str2_l - str1_l + 1);
    if (str1_l != str2_l)
      memmove(&v->ptr[i+str2_l], &v->ptr[i+str1_l], v->size - i - str1_l + 1);
    memcpy (&v->ptr[i], str2, str2_l);
    v->size += str2_l - str1_l;
    STRING_CONTRACT (v);
  }
  return i;
}

static inline size_t
string_replace (string_t v, const string_t v1, const string_t v2, size_t start)
{
  STRING_CONTRACT (v);
  STRING_CONTRACT (v1);
  STRING_CONTRACT (v2);
  return string_replace_str(v, string_get_cstr(v1), string_get_cstr(v2), start);
}

static inline void
string_replace_at (string_t v, size_t pos, size_t len, const char str2[])
{
  STRING_CONTRACT (v);
  M_ASSUME (pos+len < v->size && str2 != NULL);
  const size_t str1_l = len;
  const size_t str2_l = strlen(str2);
  stringi_fit2size (v, v->size + str2_l - str1_l + 1);
  if (str1_l != str2_l)
    memmove(&v->ptr[pos+str2_l], &v->ptr[pos+str1_l], v->size - pos - str1_l + 1);
  memcpy (&v->ptr[pos], str2, str2_l);
  v->size += str2_l - str1_l;
  STRING_CONTRACT (v);
}

static inline int
string_printf (string_t v, const char format[], ...)
{
  STRING_CONTRACT (v);
  M_ASSUME (format != NULL);
  va_list args;
  int size;
  va_start (args, format);
  size = vsnprintf (v->ptr, v->alloc, format, args);
  if (size > 0 && ((size_t) size+1 >= v->alloc) ) {
    // We have to realloc our string to fit the needed size
    stringi_fit2size (v, (size_t) size + 1);
    // and redo the parsing.
    va_end (args);
    va_start (args, format);
    size = vsnprintf (v->ptr, v->alloc, format, args);
    assert (size > 0 && (size_t)size < v->alloc);
  }
  if (size >= 0) {
    v->size = (size_t) size;
  }
  va_end (args);
  STRING_CONTRACT (v);
  return size;
}

static inline int
string_cat_printf (string_t v, const char format[], ...)
{
  STRING_CONTRACT (v);
  M_ASSUME (format != NULL);
  va_list args;
  int size;
  va_start (args, format);
  size = vsnprintf (&v->ptr[v->size], v->alloc - v->size, format, args);
  if (size > 0 && (v->size+(size_t)size+1 >= v->alloc) ) {
    // We have to realloc our string to fit the needed size
    stringi_fit2size (v, v->size + (size_t) size + 1);
    // and redo the parsing.
    va_end (args);
    va_start (args, format);
    size = vsnprintf (&v->ptr[v->size], v->alloc - v->size, format, args);
  }
  if (size >= 0) {
    v->size += (size_t) size;
  }
  va_end (args);
  STRING_CONTRACT (v);
  return size;
}

static inline bool
string_fgets(string_t v, FILE *f, string_fgets_t arg)
{
  STRING_CONTRACT(v);
  stringi_fit2size (v, 100);
  M_ASSUME(v->ptr != NULL);
  v->size = 0;
  v->ptr[0] = 0;
  bool retcode = false; /* Nothing has been read yet */
  while (fgets(&v->ptr[v->size], v->alloc - v->size, f) != NULL) {
    retcode = true; /* Something has been read */
    char *p = strchr(&v->ptr[v->size], '\n');
    v->size += strlen(&v->ptr[v->size]);
    STRING_CONTRACT(v);
    if (arg != STRING_READ_FILE && p != NULL) {
      if (arg == STRING_READ_PURE_LINE) {
        /* Remove EOL */
        *p = 0;
        v->size --;
      }
      STRING_CONTRACT(v);
      return retcode; /* Normal terminaison */
    } else if (p == NULL && !feof(f)) {
      /* The string buffer is not big enough:
         increase it and continue reading */
      stringi_fit2size (v, v->alloc + v->alloc/2);
    }
  }
  STRING_CONTRACT (v);
  return retcode; /* Abnormal terminaison */
}

static inline bool
string_fget_word (string_t v, const char separator[], FILE *f)
{
  char buffer[128];
  char c = 0;
  STRING_CONTRACT(v);
  assert (1+20+2+strlen(separator)+3 < sizeof buffer);
  stringi_fit2size (v, 10);
  v->size = 0;
  v->ptr[0] = 0;
  bool retcode = false;
  /* NOTE: We generate a buffer which we give to scanf to parse the string,
     that it is to say, we generate the format dynamically!
     The format is like " %49[^ \t.\n]%c"
     So in this case, we parse up to 49 chars, up to the separators char,
     and we read the next char. If the next char is a separator, we successful
     read a word, otherwise we have to continue parsing.
     The user shall give a constant string as the separator argument,
     as a control over this argument may give an attacker
     an opportunity for stack overflow */
  while (snprintf(buffer, sizeof buffer -1, " %%%lu[^%s]%%c", (unsigned long) v->alloc-1-v->size, separator) > 0
         && fscanf(f, buffer, &v->ptr[v->size], &c) == 2) {
    retcode = true;
    v->size += strlen(&v->ptr[v->size]);
    STRING_CONTRACT(v);
    if (strchr(separator, c) != NULL)
      return retcode;
    /* Next char is not a separator: continue parsing */
    stringi_fit2size (v, v->alloc + v->alloc/2);
    assert (v->alloc > v->size + 1);
    v->ptr[v->size++] = c;
    v->ptr[v->size] = 0;
  }
  return retcode;
}

static inline void
string_fputs(FILE *f, const string_t v)
{
  STRING_CONTRACT (v);
  fputs(string_get_cstr(v), f);
}

static inline bool
string_start_with_str_p(const string_t v, const char str[])
{
  STRING_CONTRACT (v);
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
  STRING_CONTRACT (v2);
  return string_start_with_str_p (v, string_get_cstr(v2));
}

static inline size_t
string_hash(const string_t v)
{
  STRING_CONTRACT (v);
  return m_core_hash(v->ptr, v->size);
}

static inline void
string_strim(string_t v)
{
  STRING_CONTRACT (v);
  char *b = v->ptr;
  size_t size = v->size;
  while (size > 0 && (b[size-1] == ' ' || b[size-1] == '\n' || b[size-1] == '\r' || b[size-1] == '\t'))
    size --;
  if (size == 0) {
    v->ptr[0] = 0;
    v->size = size;
    return;
  }
  while (*b == ' ' || *b == '\n' || *b == '\r' || *b == '\t')
    b++;
  M_ASSUME (b >= v->ptr &&  size >= (size_t) (b - v->ptr) );
  size -= (b - v->ptr);
  memmove (v->ptr, b, size);
  v->ptr[size] = 0;
  v->size = size;
  STRING_CONTRACT (v);
}

/* I/O */
static inline void
string_get_str(string_t v, const string_t v2, bool append)
{
  /* Output: "string" with quote around
     Replace " by \" within the string */
  STRING_CONTRACT(v2);
  STRING_CONTRACT(v);
  M_ASSUME (v != v2); // Limitation
  if(append==false) v->size = 0;
  size_t targetSize = v->size + v2->size + 3;
  stringi_fit2size(v, targetSize);
  v->ptr[v->size ++] = '"';
  for(size_t i = 0 ; i < v2->size; i++) {
    const char c = v2->ptr[i];
    if (c == '"') {
      targetSize ++ ;
      stringi_fit2size(v, targetSize);
      v->ptr[v->size ++] = '\\';
    }
    v->ptr[v->size ++] = c;
  }
  v->ptr[v->size ++] = '"';
  v->ptr[v->size] = 0;
  M_ASSUME (v->size <= targetSize);
  STRING_CONTRACT (v);
}

static inline void
string_out_str(FILE *f, const string_t v)
{
  /* Output: "string" with quote around
     Replace " by \" within the string */
  STRING_CONTRACT(v);
  M_ASSUME (f != NULL);
  fputc('"', f);
  for(size_t i = 0 ; i < v->size; i++) {
    const char c = v->ptr[i];
    if (c == '"') {
      fputc('\\', f);
    }
    fputc(c, f);
  }
  fputc('"', f);
}

static inline bool
string_in_str(string_t v, FILE *f)
{
  STRING_CONTRACT(v);
  M_ASSUME (f != NULL);
  int c = fgetc(f);
  if (c != '"') return false;
  string_clean(v);
  c = fgetc(f);
  while (c != '"' && c != EOF) {
    if (c == '\\') {
      c = fgetc(f);
      if (c != '"') string_push_back (v, '\\');
    }
    string_push_back (v, c);
    c = fgetc(f);
  }
  return c == '"';
}

/* UTF8 Handling */
typedef enum {
  STRINGI_UTF8_STARTING = 0,
  STRINGI_UTF8_DECODING_1 = 1,
  STRINGI_UTF8_DECODING_2 = 2,
  STRINGI_UTF8_DOCODING_3 = 3,
  STRINGI_UTF8_ERROR = 4  
} stringi_utf8_state_e;

typedef unsigned int string_unicode_t;

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
#define STRINGI_UTF8_STATE_TAB                                          \
  "\000\004\001\002\003\004\004\004"                                    \
  "\004\000\004\004\004\004\004\004"                                    \
  "\004\001\004\004\004\004\004\004"                                    \
  "\004\002\004\004\004\004\004\004"                                    \
  "\004\004\004\004\004\004\004\004"

static inline void stringi_utf8_decode(char car, stringi_utf8_state_e *state,
                                       string_unicode_t *unicode)
{
  const int type = m_core_clz((unsigned char)~car) - (sizeof(unsigned long) - 1) * CHAR_BIT;
  const string_unicode_t mask1 = -(string_unicode_t)(*state != STRINGI_UTF8_STARTING);
  const string_unicode_t mask2 = (0xFFU >> type);
  *unicode = ((*unicode << 6) & mask1) | (car & mask2);
  *state = STRINGI_UTF8_STATE_TAB[*state * 8 + type];
}

/* NOTE: Non-canonical representation are not rejected */
static inline bool stringi_utf8_valid_str_p(const char str[])
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

static inline size_t stringi_utf8_length(const char str[])
{
  size_t size = 0;
  stringi_utf8_state_e s = STRINGI_UTF8_STARTING;
  string_unicode_t u = 0;
  while (*str) {
    stringi_utf8_decode(*str, &s, &u);
    if (s == STRINGI_UTF8_ERROR) return -1;
    if (s == STRINGI_UTF8_STARTING) size++;
    str++;
  }
  return size;
}

static inline void stringi_utf8_encode(char buffer[5], string_unicode_t u)
{
  if (M_LIKELY (u <= 0x7F)) {
    buffer[0] = u;
    buffer[1] = 0;
  } else if (u <= 0x7FF) {
    buffer[0] = 0xC0 | (u >> 6);
    buffer[1] = 0x80 | (u & 0x3F);
    buffer[2] = 0;
  } else if (u <= 0xFFFF) {
    buffer[0] = 0xE0 | (u >> 12);
    buffer[1] = 0x80 | ((u >> 6) & 0x3F);
    buffer[2] = 0x80 | (u & 0x3F);
    buffer[3] = 0;
  } else {
    buffer[0] = 0xF0 | (u >> 18);
    buffer[1] = 0x80 | ((u >> 12) & 0x3F);
    buffer[2] = 0x80 | ((u >> 6) & 0x3F);
    buffer[3] = 0x80 | (u & 0x3F);
    buffer[4] = 0;
  }
}

typedef struct {
  string_unicode_t u;
  const char *ptr;
  const char *next_ptr;
} string_it_t[1];

static inline void
string_it(string_it_t it, const string_t str)
{
  STRING_CONTRACT(str);
  it->ptr    = str->ptr;
  it->next_ptr  = str->ptr;
  it->u      = 0;
}

static inline bool
string_end_p (string_it_t it)
{
  assert (it != NULL);
  if (*it->ptr == 0)
    return true;
  stringi_utf8_state_e state =  STRINGI_UTF8_STARTING;
  string_unicode_t u = 0;
  const char *str = it->ptr;
  do {
    stringi_utf8_decode(*str, &state, &u);
    str++;
  } while (state != STRINGI_UTF8_STARTING && state != STRINGI_UTF8_ERROR && *str != 0);
  it->next_ptr = str;
  it->u = M_UNLIKELY (state == STRINGI_UTF8_ERROR) ? -1U : u;
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
  STRING_CONTRACT(str);
  char buffer[4+1];
  stringi_utf8_encode(buffer, u);
  string_cat_str(str, buffer);
}

/* Compute the length in UTF8 characters in the string */
static inline size_t
string_length_u(string_t str)
{
  STRING_CONTRACT(str);
  return stringi_utf8_length(str->ptr);
}

static inline bool
string_utf8_p(string_t str)
{
  STRING_CONTRACT(str);
  return stringi_utf8_valid_str_p(str->ptr);
}


#define STRING_SPLIT(name, oplist, type_oplist)                         \
  static inline void M_C(name, _split)(M_GET_TYPE oplist cont,          \
                                   const string_t str, const char sep)  \
  {                                                                     \
    size_t begin = 0;                                                   \
    string_t tmp;                                                       \
    string_init(tmp);                                                   \
    M_GET_CLEAN oplist (cont);                                          \
    for(size_t i = 0 ; i < string_size(str); i++) {			\
      char c = string_get_char(str, i);                                 \
      if (c == sep) {                                                   \
        string_set_strn(tmp, &str->ptr[begin], i - begin);              \
        M_GET_PUSH oplist (cont, tmp);                                  \
        begin = i + 1;                                                  \
      }                                                                 \
    }                                                                   \
    string_set_strn(tmp, &str->ptr[begin], string_size(str) - begin);	\
    M_GET_PUSH oplist (cont, tmp);                                      \
    /* HACK: if method reverse is defined, it is likely that we have */ \
    /* inserted the items in the wrong order (aka for a list) */        \
    M_IF_METHOD(REVERSE, oplist) (M_GET_REVERSE oplist (cont);, )       \
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
  ((const string_t){{.size = sizeof(s)-1, .alloc = sizeof(s),           \
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
        this->string->size = N -1;
        this->string->alloc = N;
        memcpy (this->str, lstr, N);
        this->string->ptr = this->str;
      }
    };
}
#define STRING_CTE(s)                                                   \
  m_string::m_aligned_string<sizeof (s)>(s).string
#endif

#define STRING_INIT_PRINTF(v, format, ...) do {                         \
  string_init (v);                                                      \
  string_printf (v, format, __VA_ARGS__) ;                              \
  } while (0)

/* NOTE: Use GCC extension */
#define STRING_DECL_INIT(v)                                             \
  string_t v __attribute__((cleanup(stringi_clear2))) = {{ 0, 0, NULL}}

#define STRING_DECL_INIT_PRINTF(v, format, ...)                         \
  STRING_DECL_INIT(v);                                                  \
  string_printf (v, format, __VA_ARGS__)


/* Define the OPLIST of a STRING */
#define STRING_OPLIST                                                   \
  (INIT(string_init),INIT_SET(string_init_set), SET(string_set),        \
   INIT_MOVE(string_init_move), MOVE(string_move),                      \
   SWAP(string_swap),                                                   \
   CLEAR(string_clear), HASH(string_hash), EQUAL(string_equal_p),       \
   CMP(string_cmp), TYPE(string_t), GET_STR(string_get_str),            \
   OUT_STR(string_out_str), IN_STR(string_in_str),                      \
   EXT_ALGO(STRING_SPLIT)                                               \
   )
/* Register the OPLIST as a global one */
#define M_OPL_string_t() STRING_OPLIST


/* Macro encapsulation to give default value for start offset */
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

#endif

#endif
