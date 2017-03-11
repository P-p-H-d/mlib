#include <gmp.h>

#include "m-variant.h"
#include "m-string.h"
#include "m-dict.h"
#include "m-array.h"

/* Define a variant over two integers named integer_t
   Notice than mpz_t has special init & clear functions.
*/
VARIANT_DEF2(integer,
             (n, long long, M_DEFAULT_OPLIST),
             (z, mpz_t, M_CLASSIC_OPLIST(mpz)))
#define INTEGER_OPLIST VARIANT_OPLIST(integer, M_DEFAULT_OPLIST, M_CLASSIC_OPLIST(mpz))

/* Define a dictionary of string --> integer named dict_my_t */
DICT_DEF2(my,
          string_t, STRING_OPLIST,
          integer_t, INTEGER_OPLIST)
#define MY_DICT_OPLIST DICT_OPLIST(my, STRING_OPLIST, INTEGER_OPLIST)

/* Define an array of such dictionary named array_my_t */
ARRAY_DEF(my, dict_my_t, MY_DICT_OPLIST)
#define MY_ARRAY_OPLIST ARRAY_OPLIST(my, MY_DICT_OPLIST)

int main()
{
  M_LET(z, INTEGER_OPLIST)
    M_LET(dict, MY_DICT_OPLIST)
    M_LET(array, MY_ARRAY_OPLIST)
    {
      mpz_t gmp;
      mpz_init_set_str(gmp, "25446846874687468746874687468746874686874", 10);
      integer_set_z (z, gmp); // Since we didn't define MOVE operators for mpz_t, we can't use integer_move_z
      mpz_clear(gmp);
      dict_my_set_at(dict, STRING_CTE("n"), z);
      array_my_push_back (array, dict);
    }
}
