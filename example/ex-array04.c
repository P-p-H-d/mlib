#include <gmp.h>

#include "m-array.h"
#include "m-algo.h"

ARRAY_DEF(mpz, mpz_t, M_CLASSIC_OPLIST(mpz))
ALGO_DEF(array_mpz, ARRAY_OPLIST(mpz, M_CLASSIC_OPLIST(mpz)))

static inline void my_mpz_inc(mpz_t *d, const mpz_t a){
  mpz_add(*d, *d, a);
}
static inline void my_mpz_sqr(mpz_t *d, const mpz_t a){
  mpz_mul(*d, a, a);
}

int main(void)
{
  mpz_t z;
  mpz_init_set_ui(z, 1);
  array_mpz_t a;
  array_mpz_init(a);
  array_mpz_push_back(a, z);
  for(size_t i = 2 ; i < 1000; i++) {
    mpz_mul_ui(z, z, i);
    array_mpz_push_back(a, z);    
  }
  /* z = sum (a[i]^2) */
  array_mpz_map_reduce (&z, a, my_mpz_inc, my_mpz_sqr);
  gmp_printf ("Z=%Zd\n", z);
  array_mpz_clear(a);
  mpz_clear(z);
}
