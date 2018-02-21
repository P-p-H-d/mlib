#ifndef MY_MPZ_T
#define MY_MPZ_T

#include <gmp.h>
#include "m-string.h"

static void my_mpz_out_str(FILE *f, const mpz_t z)
{
  mpz_out_str (f, 10, z);
}

static bool my_mpz_in_str(mpz_t z, FILE *f)
{
  return mpz_inp_str (z, f, 10) > 0;
}

static bool my_mpz_equal_p(const mpz_t z1, const mpz_t z2)
{
  return mpz_cmp (z1, z2) == 0;
}

static void my_mpz_str(string_t str, const mpz_t z, bool append)
{
  char *s = mpz_get_str(NULL, 10, z);
  assert (s != NULL);
  if (append) string_cat_str (str, s);
  else        string_set_str (str, s);
  free(s);
}

#define MYMPZ_OPLIST                                                    \
  (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), \
   OUT_STR(my_mpz_out_str), IN_STR(my_mpz_in_str), EQUAL(my_mpz_equal_p), \
   GET_STR(my_mpz_str) )

#endif
