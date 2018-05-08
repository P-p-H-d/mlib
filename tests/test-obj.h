#ifndef TESTOBJ_T
#define TESTOBJ_T

#include "m-string.h"

/* This is a trivial encapsulation of an opaque structure.
   Used for test purpose only.*/
typedef struct testobj_s {
  unsigned int n;
  unsigned int a;
  unsigned int *ptr;
} testobj_t[1];

static void testobj_init(testobj_t z)
{
  z->n = 1;
  z->a = 1;
  z->ptr = (unsigned int*) calloc(1, sizeof(unsigned int));
  if (!z->ptr) abort();
}

static void testobj_clear(testobj_t z)
{
  free(z->ptr);
  z->ptr = NULL;
}

static void testobj_init_set(testobj_t d, const testobj_t s)
{
  d->n = s->n;
  d->a = s->n;
  d->ptr = (unsigned int *) calloc (s->n, sizeof(unsigned int));
  memcpy(d->ptr, s->ptr, sizeof(unsigned int) * s->n);
}

static void testobj_set(testobj_t d, const testobj_t s)
{
  testobj_clear(d);
  testobj_init_set(d, s);
}

static void testobj_set_ui(testobj_t d, unsigned int v)
{
  d->n = 1;
  d->ptr[0] = v;
}

static void testobj_init_set_ui(testobj_t d, unsigned int v)
{
  testobj_init(d);
  testobj_set_ui(d, v);
}

static unsigned int testobj_get_ui(const testobj_t z)
{
  return z->ptr[0];
}

static void testobj_add(testobj_t d, const testobj_t a, const testobj_t b)
{
  d->ptr[0] = a->ptr[0] + b->ptr[0];
}

static void testobj_out_str(FILE *f, const testobj_t z)
{
  assert(z->n == 1);
  fprintf(f, "%u", z->ptr[0]);
}

static bool testobj_in_str(testobj_t z, FILE *f)
{
  z->n = 1;
  return fscanf(f, "%u", &z->ptr[0]) == 1;
}

static bool testobj_parse_str(testobj_t z, const char str[], const char **endptr)
{
  z->n = 1;
  char *end;
  z->ptr[0] = strtol(str, &end, 0);
  if (endptr) { *endptr = (const char*) end; }
  return (uintptr_t) end != (uintptr_t) str;
}

static bool testobj_equal_p(const testobj_t z1, const testobj_t z2)
{
  if (z1->n != z2->n) return false;
  return memcmp(z1->ptr, z2->ptr, z1->n*sizeof(unsigned int)) == 0;
}

static int testobj_cmp(const testobj_t z1, const testobj_t z2)
{
  if (z1->n != z2->n) return z1-> n < z2->n ? -1 : 1;
  return memcmp(z1->ptr, z2->ptr, z1->n*sizeof(unsigned int));
}

static int testobj_cmp_ui(const testobj_t z1, unsigned int z2)
{
  if (z1->n != 1) return z1-> n < 1 ? -1 : 1;
  return memcmp(z1->ptr, &z2, sizeof(unsigned int));
}

static void testobj_str(string_t str, const testobj_t z, bool append)
{
  if (append) string_cat_printf (str, "%u", z->ptr[0]);
  else        string_printf (str, "%u", z->ptr[0]);
}

#define TESTOBJ_OPLIST							\
  (INIT(testobj_init),                                                  \
   INIT_SET(testobj_init_set),                                          \
   SET(testobj_set),                                                    \
   CLEAR(testobj_clear),                                                \
   TYPE(testobj_t),                                                     \
   OUT_STR(testobj_out_str),                                            \
   IN_STR(testobj_in_str),                                              \
   PARSE_STR(testobj_parse_str),                                        \
   GET_STR(testobj_str),                                                \
   EQUAL(testobj_equal_p),                                              \
   )

#define TESTOBJ_CMP_OPLIST						\
  (INIT(testobj_init),                                                  \
   INIT_SET(testobj_init_set),                                          \
   SET(testobj_set),                                                    \
   CLEAR(testobj_clear),                                                \
   TYPE(testobj_t),                                                     \
   OUT_STR(testobj_out_str),                                            \
   IN_STR(testobj_in_str),                                              \
   PARSE_STR(testobj_parse_str),                                        \
   GET_STR(testobj_str),                                                \
   EQUAL(testobj_equal_p),                                              \
   CMP(testobj_cmp)                                                     \
   )

#endif
