#include <assert.h>

#include "m-shared.h"

SHARED_PTR_DEF(int, int)

#include <gmp.h>

SHARED_PTR_DEF(mpz, mpz_t, M_CLASSIC_OPLIST(mpz))

static int f(const shared_int_t p)
{
  return *shared_int_cref(p);
}

int main(void)
{
  shared_int_t p1, p2;
  
  shared_int_init2(p1, malloc(sizeof(int)));
  assert(shared_int_ref(p1) != NULL);

  *shared_int_ref(p1) = 12;

  shared_int_init_set(p2, p1);
  assert(f(p2) == 12);

  shared_int_clean(p1);
  assert(shared_int_NULL_p(p1));
  assert(f(p2) == 12);
  
  shared_int_set(p1, p2);

  shared_int_clean(p2);
  assert(shared_int_NULL_p(p2));

  assert(f(p1) == 12);
  shared_int_set(p2, p1);
  assert(f(p2) == 12);

  shared_int_clean(p2);
  shared_int_clean(p1);
  
  assert(shared_int_NULL_p(p1));
  assert(shared_int_NULL_p(p2));

  shared_int_clear(p2);
  shared_int_clear(p1);
  
  shared_int_init2(p1, NULL);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);
                  
  shared_int_init(p1);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);

  shared_int_init_new(p1);
  assert(!shared_int_NULL_p(p1));
  shared_int_clear(p1);
  
  exit(0);
}
