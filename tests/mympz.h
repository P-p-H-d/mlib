#ifndef MY_MPZ_T
#define MY_MPZ_T

#include "m-string.h"

/* This is a trivial encapsulation of an opaque structure.
   Used for test purpose only.*/
typedef struct my_mpz_s {
  unsigned int n;
  unsigned int a;
  unsigned int *ptr;
} my_mpz_t[1];

static void my_mpz_init(my_mpz_t z)
{
  z->n = 1;
  z->a = 1;
  z->ptr = (unsigned int*) calloc(1, sizeof(unsigned int));
  if (!z->ptr) abort();
}

static void my_mpz_clear(my_mpz_t z)
{
  free(z->ptr);
  z->ptr = NULL;
}

static void my_mpz_init_set(my_mpz_t d, const my_mpz_t s)
{
  d->n = s->n;
  d->a = s->n;
  d->ptr = (unsigned int *) calloc (s->n, sizeof(unsigned int));
  memcpy(d->ptr, s->ptr, sizeof(unsigned int) * s->n);
}

static void my_mpz_set(my_mpz_t d, const my_mpz_t s)
{
  my_mpz_clear(d);
  my_mpz_init_set(d, s);
}

static void my_mpz_set_ui(my_mpz_t d, unsigned int v)
{
  d->n = 1;
  d->ptr[0] = v;
}

static void my_mpz_init_set_ui(my_mpz_t d, unsigned int v)
{
  my_mpz_init(d);
  my_mpz_set_ui(d, v);
}

static unsigned int my_mpz_get_ui(const my_mpz_t z)
{
  return z->ptr[0];
}

static void my_mpz_add(my_mpz_t d, const my_mpz_t a, const my_mpz_t b)
{
  d->ptr[0] = a->ptr[0] + b->ptr[0];
}

static void my_mpz_out_str(FILE *f, const my_mpz_t z)
{
  assert(z->n == 1);
  fprintf(f, "%u", z->ptr[0]);
}

static bool my_mpz_in_str(my_mpz_t z, FILE *f)
{
  z->n = 1;
  return fscanf(f, "%u", &z->ptr[0]) == 1;
}

static bool my_mpz_parse_str(my_mpz_t z, const char str[], const char **endptr)
{
  z->n = 1;
  char *end;
  z->ptr[0] = strtol(str, &end, 0);
  if (endptr) { *endptr = (const char*) end; }
  return (uintptr_t) end != (uintptr_t) str;
}

static bool my_mpz_equal_p(const my_mpz_t z1, const my_mpz_t z2)
{
  if (z1->n != z2->n) return false;
  return memcmp(z1->ptr, z2->ptr, z1->n*sizeof(unsigned int)) == 0;
}

static int my_mpz_cmp(const my_mpz_t z1, const my_mpz_t z2)
{
  if (z1->n != z2->n) return z1-> n < z2->n ? -1 : 1;
  return memcmp(z1->ptr, z2->ptr, z1->n*sizeof(unsigned int));
}

static int my_mpz_cmp_ui(const my_mpz_t z1, unsigned int z2)
{
  if (z1->n != 1) return z1-> n < 1 ? -1 : 1;
  return memcmp(z1->ptr, &z2, sizeof(unsigned int));
}

static void my_mpz_str(string_t str, const my_mpz_t z, bool append)
{
  if (append) string_cat_printf (str, "%u", z->ptr[0]);
  else        string_printf (str, "%u", z->ptr[0]);
}

#define MY_MPZ_OPLIST							\
  (INIT(my_mpz_init), INIT_SET(my_mpz_init_set), SET(my_mpz_set), CLEAR(my_mpz_clear), \
   OUT_STR(my_mpz_out_str), IN_STR(my_mpz_in_str),                      \
   PARSE_STR(my_mpz_parse_str), GET_STR(my_mpz_str),                    \
   EQUAL(my_mpz_equal_p),                                               \
   )

#define MY_MPZ_CMP_OPLIST						\
  (INIT(my_mpz_init), INIT_SET(my_mpz_init_set), SET(my_mpz_set), CLEAR(my_mpz_clear), \
   OUT_STR(my_mpz_out_str), IN_STR(my_mpz_in_str), EQUAL(my_mpz_equal_p), \
   GET_STR(my_mpz_str), CMP(my_mpz_cmp) )

#endif
