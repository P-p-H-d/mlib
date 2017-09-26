#include "gmp.h"
#include "m-array.h"
#include "m-list.h"
#include "m-register.h"

ARRAY_DEF (mpz, mpz_t, (INIT(mpz_init), CLEAR(mpz_clear), SET(mpz_set), INIT_SET(mpz_init_set)))
LIST_DEF (uint, unsigned int, M_DEFAULT_OPLIST)
          
// Register new class: mpz_t
#define TYPE  mpz_t
#define OPLIST (INIT(mpz_init), CLEAR(mpz_clear), SET(mpz_set), INIT_SET(mpz_init_set))
#include M_REGISTER_TYPE

void mpq_init_set(mpq_t q, const mpq_t p)
{
  mpq_init(q);
  mpq_set(q, p);
}

// Register new class: mpq_t
#define TYPE mpq_t
#define OPLIST (INIT(mpq_init), CLEAR(mpq_clear), SET(mpq_set), INIT_SET(mpq_init_set))
#include M_REGISTER_TYPE

#define TYPE  array_mpz_t
#define OPLIST ARRAY_OPLIST(mpz)
//#include M_REGISTER_TYPE

//#define TYPE  list_uint_t
//#define OPLIST LIST_OPLIST(unsigned int)
//#include M_REGISTER_TYPE

bool is42(array_mpz_t array)
{
  bool retval = false;
  mpz_t *z = new(mpz_t);
  mpz_set_ui (*z, 42);
  for_each (item, array) {
    if (equal(*item, *z)) {
      retval = true;
      break;
    }
  }
  delete(z);
  return retval;
}

void example(void)
{
  list_uint_t list;
  init(list);
  push(list, 42);
  push(list, 17);
  for_each (item, list) {
    print(*item, "\n");
  }
  clear(list);
}
