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

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "m-core.h"

/********************************** INTERNAL ************************************/

// This macro defines the contract of a string.
// Note: A ==> B is represented as not(A) or B
// Note: use of strlen can slow down a lot the program in some cases.
#define STRING_CONTRACT(v) do {                                         \
    M_ASSUME (v != NULL);                                               \
    M_ASSUME (v->ptr != NULL || (v->size == 0 && v->alloc == 0));       \
    M_ASSUME (v->ptr == NULL || strlen(v->ptr) == v->size);		\
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
string_get_length(const string_t v)
{
  STRING_CONTRACT (v);
  return v->size;
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
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (ptr == NULL) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is broken...
      return;
    }
    v->ptr = ptr;
    v->alloc = alloc;
  }
}

static inline void
string_shrink2fit(string_t v)
{
  STRING_CONTRACT (v);
  char *ptr = M_MEMORY_REALLOC (char, v->ptr, v->size+1);
  M_ASSUME (ptr != NULL);
  v->ptr = ptr; // Can be != ptr if v->ptr was NULL before.
  v->alloc = v->size+1;
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
      v1->ptr[0] = 0;
    }
    v1->size = size;
  }
  STRING_CONTRACT (v1);
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
string_search_char (const string_t v, char c)
{
  STRING_CONTRACT (v);
  const char *p = M_ASSIGN_CAST(const char*, strchr(string_get_cstr(v), c));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search_rchar (const string_t v, char c)
{
  STRING_CONTRACT (v);
  // NOTE: Can implement it in a faster way than the libc function
  // by scanning backward from the bottom of the string (which is
  // possible since we know the size)
  const char *p = M_ASSIGN_CAST(const char*, strrchr(string_get_cstr(v), c));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search_str (const string_t v, const char str[])
{
  STRING_CONTRACT (v);
  M_ASSUME (str != NULL);
  const char *p = M_ASSIGN_CAST(const char*, strstr(string_get_cstr(v), str));
  return p == NULL ? STRING_FAILURE : (size_t) (p-string_get_cstr(v));
}

static inline size_t
string_search (const string_t v1, const string_t v2)
{
  STRING_CONTRACT (v2);
  return string_search_str(v1, string_get_cstr(v2));
}

static inline int
string_strcoll_str(const string_t v, const char str[])
{
  STRING_CONTRACT (v);
  M_ASSUME (str != NULL);
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

static inline bool
string_replace_str (string_t v, const char str1[], const char str2[])
{
  STRING_CONTRACT (v);
  M_ASSUME (str1 != NULL && str2 != NULL);
  size_t i = string_search_str(v, str1);
  if (i != STRING_FAILURE) {
    size_t str1_l = strlen(str1);
    size_t str2_l = strlen(str2);
    stringi_fit2size (v, v->size + str2_l - str1_l + 1);
    if (str1_l != str2_l)
      memmove(&v->ptr[i+str2_l], &v->ptr[i+str1_l], v->size - i - str1_l + 1);
    memcpy (&v->ptr[i], str2, str2_l);
    v->size += str2_l - str1_l;
    STRING_CONTRACT (v);
    return true;
  }
  return false;
}

static inline bool
string_replace (string_t v, const string_t v1, const string_t v2)
{
  STRING_CONTRACT (v);
  STRING_CONTRACT (v1);
  STRING_CONTRACT (v2);
  return string_replace_str(v, string_get_cstr(v1), string_get_cstr(v2));
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

static inline void
string_fgets(string_t v, FILE *f, string_fgets_t arg)
{
  STRING_CONTRACT(v);
  stringi_fit2size (v, 100);
  M_ASSUME(v->ptr != NULL);
  v->size = 0;
  v->ptr[0] = 0;
  while (!feof (f) && !ferror(f)) {
    fgets(&v->ptr[v->size], v->alloc - v->size, f);
    char *p = strchr(&v->ptr[v->size], '\n');
    v->size += strlen(&v->ptr[v->size]);
    STRING_CONTRACT(v);
    if (arg != STRING_READ_FILE && p != NULL) {
      if (arg == STRING_READ_PURE_LINE) {
        *p = 0;
        v->size --;
      }
      STRING_CONTRACT(v);
      return;
    } else if (p == NULL)
      stringi_fit2size (v, v->alloc + v->alloc/2);
  }
  STRING_CONTRACT (v);
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
  char c = fgetc(f);
  if (c != '"') return false;
  string_clean(v);
  c = fgetc(f);
  while (c != '"' && !feof(f) && !ferror(f)) {
    if (c == '\\') {
      c = getc(f);
      if (c != '"') string_push_back (v, '\\');
    }
    string_push_back (v, c);
    c = fgetc(f);
  }
  return c == '"';
}

#define STRING_SPLIT(name, oplist, type_oplist)                         \
  static inline void M_C(name, _split)(M_GET_TYPE oplist cont,          \
                                   const string_t str, const char sep)  \
  {                                                                     \
    size_t begin = 0;                                                   \
    string_t tmp;                                                       \
    string_init(tmp);                                                   \
    M_GET_CLEAN oplist (cont);                                          \
    for(size_t i = 0 ; i < string_get_length(str); i++) {               \
      char c = string_get_char(str, i);                                 \
      if (c == sep) {                                                   \
        string_set_strn(tmp, &str->ptr[begin], i - begin);              \
        M_GET_PUSH oplist (cont, tmp);                                  \
        begin = i + 1;                                                  \
      }                                                                 \
    }                                                                   \
    string_set_strn(tmp, &str->ptr[begin], string_get_length(str) - begin); \
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
   of recursive struct definition)
*/
#define STRING_CTE(s)                                                   \
  ((const string_t){{.size = sizeof(s)-1, .alloc = sizeof(s),           \
        .ptr = ((struct { int _n; char _d[sizeof (s)]; }){ 0, s })._d }})

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

#define STRING_OPLIST                                                   \
  (INIT(string_init),INIT_SET(string_init_set), SET(string_set),        \
   INIT_MOVE(string_init_move), MOVE(string_move),                      \
   SWAP(string_swap),                                                   \
   CLEAR(string_clear), HASH(string_hash), EQUAL(string_equal_p),       \
   CMP(string_cmp), TYPE(string_t), GET_STR(string_get_str),            \
   OUT_STR(string_out_str), IN_STR(string_in_str),                      \
   EXT_ALGO(STRING_SPLIT)                                               \
   )

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

#define string_set(a,b) STRINGI_SELECT2(string_set, string_set_str, a, b)
#define string_init_set(a,b) STRINGI_SELECT2(string_init_set, string_init_set_str, a, b)
#define string_set(a,b) STRINGI_SELECT2(string_set, string_set_str, a, b)
#define string_cat(a,b) STRINGI_SELECT2(string_cat, string_cat_str, a, b)
#define string_cmp(a,b) STRINGI_SELECT2(string_cmp, string_cmp_str, a, b)
#define string_equal_p(a,b) STRINGI_SELECT2(string_equal_p, string_equal_str_p, a, b)
#define string_search(a,b)  STRINGI_SELECT2(string_search, string_search_str, a, b)
#define string_strcoll(a,b) STRINGI_SELECT2(string_strcoll, string_strcoll_str, a, b)

#endif

#endif
