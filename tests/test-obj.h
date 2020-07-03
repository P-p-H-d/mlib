/*
 * Copyright 2020 - 2020, SP Vladislav Dmitrievich Turbanov
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
#ifndef TESTOBJ_T
#define TESTOBJ_T

#include <stdbool.h>
#include <assert.h>

#include "m-string.h"

M_BEGIN_PROTECTED_CODE

/* This is a trivial encapsulation of an opaque structure.
   Used for test purpose only.*/
typedef struct testobj_s {
  unsigned int n;
  unsigned int a;
  bool         allocated;
  unsigned int *ptr;
} testobj_t[1];

static inline void M_C(testobj, M_NAMING_INIT)(testobj_t z)
{
  z->n = 1;
  z->a = 1;
  z->ptr = (unsigned int*) calloc(1, sizeof(unsigned int));
  if (!z->ptr) abort();
  z->allocated = true;
}

static inline void testobj_clear(testobj_t z)
{
  assert (z->allocated);
  free(z->ptr);
  z->ptr = NULL;
  z->allocated = false;
}

static inline void M_C(testobj, M_NAMING_INIT_SET)(testobj_t d, const testobj_t s)
{
  d->n = s->n;
  d->a = s->n;
  d->ptr = (unsigned int *) calloc (s->n, sizeof(unsigned int));
  if (!d->ptr) abort();
  memcpy(d->ptr, s->ptr, sizeof(unsigned int) * s->n);
  d->allocated = true;
}

static inline void M_C(testobj, M_NAMING_SET)(testobj_t d, const testobj_t s)
{
  if (d != s) {
    testobj_clear(d);
    M_C(testobj, M_NAMING_INIT_SET)(d, s);
  }
}

static inline void testobj_set_ui(testobj_t d, unsigned int v)
{
  d->n = 1;
  d->ptr[0] = v;
}

static inline void testobj_init_set_ui(testobj_t d, unsigned int v)
{
  M_C(testobj, M_NAMING_INIT)(d);
  testobj_set_ui(d, v);
}

static inline unsigned int testobj_get_ui(const testobj_t z)
{
  return z->ptr[0];
}

static inline void testobj_add(testobj_t d, const testobj_t a, const testobj_t b)
{
  d->ptr[0] = a->ptr[0] + b->ptr[0];
}

static inline void testobj_out_str(FILE *f, const testobj_t z)
{
  assert(z->n == 1);
  fprintf(f, "%u", z->ptr[0]);
}

static inline bool testobj_in_str(testobj_t z, FILE *f)
{
  z->n = 1;
  return m_core_fscanf(f, "%u", &z->ptr[0]) == 1;
}

static inline bool testobj_parse_str(testobj_t z, const char str[], const char **endptr)
{
  z->n = 1;
  char *end;
  z->ptr[0] = (unsigned int) strtol(str, &end, 0);
  if (endptr) { *endptr = (const char*) end; }
  return (uintptr_t) end != (uintptr_t) str;
}

static inline bool M_C(testobj, M_NAMING_TEST_EQUAL)(const testobj_t z1, const testobj_t z2)
{
  if (z1->n != z2->n) return false;
  return memcmp(z1->ptr, z2->ptr, z1->n*sizeof(unsigned int)) == 0;
}

static inline int testobj_cmp(const testobj_t z1, const testobj_t z2)
{
  if (z1->n != z2->n) return z1-> n < z2->n ? -1 : 1;
  for(unsigned int i = 0; i < z1->n; i++) {
    if (z1->ptr[i] != z2->ptr[i])
      return z1->ptr[i] < z2->ptr[i] ? -1 : 1;
  }
  return 0;
}

static inline int testobj_cmp_ui(const testobj_t z1, unsigned int z2)
{
  if (z1->n != 1) return z1-> n < 1 ? -1 : 1;
  return z1->ptr[0] < z2 ? -1 : z1->ptr[0] > z2;
}

static inline void testobj_str(string_t str, const testobj_t z, bool append)
{
  if (append) string_cat_printf (str, "%u", z->ptr[0]);
  else        string_printf (str, "%u", z->ptr[0]);
}

#define TESTOBJ_OPLIST							                                    \
  (INIT(M_C(testobj, M_NAMING_INIT)),                                   \
   INIT_SET(M_C(testobj, M_NAMING_INIT_SET)),                           \
   SET(M_C(testobj, M_NAMING_SET)),                                     \
   CLEAR(M_C(testobj, M_NAMING_CLEAR)),                                 \
   TYPE(testobj_t),                                                     \
   OUT_STR(testobj_out_str),                                            \
   IN_STR(testobj_in_str),                                              \
   PARSE_STR(testobj_parse_str),                                        \
   GET_STR(testobj_str),                                                \
   EQUAL(M_C(testobj, M_NAMING_TEST_EQUAL)),                               \
   )

#define TESTOBJ_CMP_OPLIST						                                  \
  (INIT(M_C(testobj, M_NAMING_INIT)),                                   \
   INIT_SET(M_C(testobj, M_NAMING_INIT_SET)),                           \
   SET(M_C(testobj, M_NAMING_SET)),                                     \
   CLEAR(M_C(testobj, M_NAMING_CLEAR)),                                 \
   TYPE(testobj_t),                                                     \
   OUT_STR(testobj_out_str),                                            \
   IN_STR(testobj_in_str),                                              \
   PARSE_STR(testobj_parse_str),                                        \
   GET_STR(testobj_str),                                                \
   EQUAL(M_C(testobj, M_NAMING_TEST_EQUAL)),                               \
   CMP(testobj_cmp)                                                     \
   )

M_END_PROTECTED_CODE

#endif
