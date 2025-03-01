/*
 * Define a test object that sometimes throw exception
 * Use only for tests
 * 
 * Copyright (c) 2017-2025, Patrick Pelissier
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
#ifndef TEST_OBJ_EXCEPT_T
#define TEST_OBJ_EXCEPT_T

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

// Overwrite malloc to throw exceptions sometimes
// Needs to be done before including M*LIB headers
// TODO: Not representative enough: it shall not throw an exception itself but return NULL
static inline void test_obj_except_trigger(void);
extern int test_obj_except__malloc_counter;
#ifdef __cplusplus
# include <cstdlib>
# define M_MEMORY_ALLOC(type) (test_obj_except_trigger(), test_obj_except__malloc_counter++, (type*)std::malloc (sizeof (type)))
# define M_MEMORY_DEL(ptr)  (test_obj_except__malloc_counter-=(ptr != NULL), std::free(ptr))
# define M_MEMORY_REALLOC(type, ptr, n)                                       \
  ((type*) (test_obj_except_trigger(), test_obj_except__malloc_counter+= (ptr == NULL), std::realloc ((ptr), (n)*sizeof (type))))
# define M_MEMORY_FREE(ptr) (test_obj_except__malloc_counter-=(ptr != NULL), std::free(ptr))
#else
# define M_MEMORY_ALLOC(type) (test_obj_except_trigger(), test_obj_except__malloc_counter++, malloc (sizeof (type)))
# define M_MEMORY_DEL(ptr)  (test_obj_except__malloc_counter-=(ptr != NULL), free(ptr))
# define M_MEMORY_REALLOC(type, ptr, n) (test_obj_except_trigger(), test_obj_except__malloc_counter+=(ptr == NULL), realloc ((ptr), (n)*sizeof (type)))
# define M_MEMORY_FREE(ptr) (test_obj_except__malloc_counter-=(ptr != NULL), free(ptr))
#endif

// Include M*LIB headers
#include "m-core.h"
#include "m-try.h"
#include "m-string.h"

M_BEGIN_PROTECTED_CODE

/* This is a trivial encapsulation of an opaque structure that triggers exception.
   Used for test purpose only.*/
typedef struct test_obj_except__s {
  unsigned int magic;
  unsigned int n;
  unsigned int *ptr;
} test_obj_except__t[1];

int test_obj_except__init_counter = 0;
int test_obj_except__malloc_counter = 0;
int test_obj_except__trigger_exception = 0;

// Trigger an exception if the counter reaches 0
static inline void test_obj_except_trigger(void)
{
    test_obj_except__trigger_exception --;
    if (test_obj_except__trigger_exception == 0) {
        M_THROW(M_ERROR_MEMORY);
    }
}

// All created testobj shall have been cleared at the end.
static inline void test_obj_except__final_check(void)
{
  assert(test_obj_except__init_counter == 0);
  assert(test_obj_except__malloc_counter == 0);
}

static inline void test_obj_except__init(test_obj_except__t z)
{
    test_obj_except_trigger();
    z->n = 1;
    z->ptr = (unsigned int*) calloc(1, sizeof(unsigned int));
    assert(z->ptr != NULL);
    z->ptr[0] = (unsigned int) rand();
    z->magic = 0xCAFE;
    test_obj_except__init_counter++;
}

static inline void test_obj_except__clear(test_obj_except__t z)
{
    assert (z->ptr != NULL);
    assert(z->magic == 0xCAFE);
    free(z->ptr);
    z->ptr = NULL;
    z->magic = 0xDEAD;
    test_obj_except__init_counter--;
    assert(test_obj_except__init_counter >= 0);
}

static inline void test_obj_except__init_set(test_obj_except__t d, const test_obj_except__t s)
{
    test_obj_except_trigger();
    d->n = s->n;
    d->ptr = (unsigned int *) calloc (s->n, sizeof(unsigned int));
    assert(d->ptr != NULL);
    d->magic = 0xCAFE;
    memcpy(d->ptr, s->ptr, sizeof(unsigned int) * s->n);
    test_obj_except__init_counter++;
}

static inline void test_obj_except__set(test_obj_except__t d, const test_obj_except__t s)
{
  if (d != s) {
    test_obj_except_trigger();
    test_obj_except__clear(d);
    // Don't trigger exception for init_set now
    test_obj_except__trigger_exception++;
    test_obj_except__init_set(d, s);
  }
}

static inline void test_obj_except__set_ui(test_obj_except__t d, unsigned int v)
{
  test_obj_except_trigger();
  d->n = 1;
  d->ptr[0] = v;
}

static inline void test_obj_except__init_set_ui(test_obj_except__t d, unsigned int v)
{
  // test_obj_except_trigger(); is already triggered by test_obj_except__init
  test_obj_except__init(d);
  // Don't trigger exception for test_obj_except__set_ui now
  test_obj_except__trigger_exception++;
  test_obj_except__set_ui(d, v);
}

static inline void test_obj_except__init_set_str(test_obj_except__t d, const char v[])
{
  // test_obj_except_trigger(); is already triggered by test_obj_except__init
  test_obj_except__init(d);
  // Don't trigger exception for test_obj_except__set_ui now
  test_obj_except__trigger_exception++;
  test_obj_except__set_ui(d, (unsigned int) atoi(v) );
}

static inline unsigned int test_obj_except__get_ui(const test_obj_except__t z)
{
  return z->ptr[0];
}

static inline void test_obj_except__add(test_obj_except__t d, const test_obj_except__t a, const test_obj_except__t b)
{
  test_obj_except_trigger();
  d->ptr[0] = a->ptr[0] + b->ptr[0];
}

static inline void test_obj_except__out_str(FILE *f, const test_obj_except__t z)
{
  test_obj_except_trigger();
  assert(z->n == 1);
  fprintf(f, "%u", z->ptr[0]);
}

static inline bool test_obj_except__in_str(test_obj_except__t z, FILE *f)
{
  test_obj_except_trigger();
  z->n = 1;
  return m_core_fscanf(f, "%u", &z->ptr[0]) == 1;
}

static inline bool test_obj_except__parse_str(test_obj_except__t z, const char str[], const char **endptr)
{
  test_obj_except_trigger();
  z->n = 1;
  char *end;
  z->ptr[0] = (unsigned int) strtol(str, &end, 0);
  if (endptr) { *endptr = (const char*) end; }
  return (uintptr_t) end != (uintptr_t) str;
}

static inline bool test_obj_except__equal_p(const test_obj_except__t z1, const test_obj_except__t z2)
{
  if (z1->n != z2->n) return false;
  return memcmp(z1->ptr, z2->ptr, z1->n*sizeof(unsigned int)) == 0;
}

static inline int test_obj_except__cmp(const test_obj_except__t z1, const test_obj_except__t z2)
{
  int ret = 0;
  if (z1->n != z2->n) {
    ret = z1-> n < z2->n ? -1 : 1;
  } else {
    for(unsigned int i = 0; i < z1->n; i++) {
      if (z1->ptr[i] != z2->ptr[i]) {
        ret = z1->ptr[i] < z2->ptr[i] ? -1 : 1;
        break;
      }
    }
  }
  return ret;
}

static inline int test_obj_except__cmp_ui(const test_obj_except__t z1, unsigned int z2)
{
  if (z1->n != 1) return z1-> n < 1 ? -1 : 1;
  return z1->ptr[0] < z2 ? -1 : z1->ptr[0] > z2;
}

static inline void test_obj_except__str(string_t str, const test_obj_except__t z, bool append)
{
  if (append) string_cat_printf (str, "%u", z->ptr[0]);
  else        string_printf (str, "%u", z->ptr[0]);
}

static inline size_t test_obj_except__hash(const test_obj_except__t z)
{
  M_HASH_DECL(hash);
  for(unsigned int i = 0; i < z->n; i++) {
    M_HASH_UP(hash, z->ptr[i]);
  }
  return M_HASH_FINAL(hash);
}

#define TEST_OBJ_EXCEPT_OPLIST						                             \
  (INIT(test_obj_except__init),                                                  \
   INIT_SET(test_obj_except__init_set),                                          \
   SET(test_obj_except__set),                                                    \
   CLEAR(test_obj_except__clear),                                                \
   TYPE(test_obj_except__t),                                                     \
   ADD(test_obj_except__add),                                                    \
   OUT_STR(test_obj_except__out_str),                                            \
   IN_STR(test_obj_except__in_str),                                              \
   PARSE_STR(test_obj_except__parse_str),                                        \
   GET_STR(test_obj_except__str),                                                \
   EQUAL(test_obj_except__equal_p),                                              \
   CMP(test_obj_except__cmp),                                                    \
   INIT_WITH(test_obj_except__init_set_ui),                                      \
   HASh(test_obj_except__hash),                                                  \
   EMPLACE_TYPE( LIST( (_ui, test_obj_except__init_set_ui, unsigned int), (_str, test_obj_except__init_set_str, const char *), ( /*empty*/, test_obj_except__init_set, test_obj_except__t) ) ) \
   )

#define M_OPL_test_obj_except__t() TEST_OBJ_EXCEPT_OPLIST

// Write some values in the stack to avoid having a stack set to zero
// A zero stack may hide some issues.
static void reset_stack(void)
{
  volatile char table[16*1024];
  for(unsigned i = 0 ; i < sizeof table; i++)
    table[i] = (char) i;
}

static void do_test1(void (*test)(unsigned))
{
    reset_stack();
    // Run once to get the number of exceptions point existing in the test service
    test_obj_except__trigger_exception = 0;
    test(10);
    test_obj_except__final_check();
    int count = -test_obj_except__trigger_exception;
    assert(count > 0);
    // Run once again the test service, and for each registered exception point, throw an exception
    for(int i = 1; i <= count; i++) {
        test_obj_except__trigger_exception = i;
        test(10);
        // Check there is no memory leak
        test_obj_except__final_check();
    }
}

M_END_PROTECTED_CODE

#endif
