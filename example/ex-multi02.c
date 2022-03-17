#if HAVE_GMP

#include <stdio.h>
#include <gmp.h>

#include "m-variant.h"
#include "m-string.h"
#include "m-dict.h"
#include "m-array.h"

/* Define the OPLIST of a mpz_t and register it globally */
#define M_OPL_mpz_t()                                                   \
  (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), \
   EQUAL( API( mpz_cmp, EQ(0), ARG1, ARG2)),                            \
   CMP(mpz_cmp),                                                        \
   OUT_STR( API( mpz_out_str, VOID, ARG1, 10, ARG2)),                   \
   IN_STR( API( mpz_inp_str, GT(0), ARG1, ARG2, 10)),                   \
   TYPE(mpz_t) )

/* Define a variant over two integers named integer_t
   Notice than mpz_t has special init & clear functions
   mpz_init & mpz_clear.
   This is handle by the library.
*/
VARIANT_DEF2(my_integer,
             (n, long long),
             (z, mpz_t))
/* Define the oplist associated to this variant and register it */
#define M_OPL_my_integer_t() VARIANT_OPLIST(my_integer, M_DEFAULT_OPLIST, M_OPL_mpz_t() )

/* Define a dictionary of string --> integer named dict_my_t */
DICT_DEF2(my_dict, string_t, my_integer_t)
/* Define the oplist associated to this dictionary and register it */
#define M_OPL_my_dict_t() DICT_OPLIST(my_dict, STRING_OPLIST, M_OPL_my_integer_t() )

/* Define an array of such dictionary named array_my_t */
ARRAY_DEF(my_array, my_dict_t)
/* Define the oplist associated to this array of dictionary of variant and register it */
#define M_OPL_my_array_t() ARRAY_OPLIST(my_array, M_OPL_my_dict_t())

int main(void)
{
  M_LET(z, my_integer_t)              /* Define & init & clear an instance of a variant of integer */
    M_LET(dict, my_dict_t)            /* Define & init & clear an instance of a dictionary of variant */
    M_LET(array, my_array_t)          /* Define & init & clear an intance of an array of dictionary of variant */
    M_LET(gmp, mpz_t)                 /* M_LET allows even with non container type */
    {
      my_integer_set_n (z, 17LL);                // Set z as the long long 17
      my_dict_set_at(dict, STRING_CTE("n1"), z); // Push it in the dictionary
      mpz_set_str(gmp, "25446846874687468746874687468746874686874", 10); // Set gmp to a very long integer.
      my_integer_set_z (z, gmp);                 // Set z as the mpz_t variable
      my_dict_set_at(dict, STRING_CTE("n2"), z); // Push it in the dictionary
      my_array_push_back (array, dict);          // Push the dictionary in the array

      // Iterate over the array container
      for M_EACH(item, array, my_array_t) {
          // Iterate over the dictionary container
          for M_EACH(p, *item, M_OPL_my_dict_t()) {
              // Dictionary iterator are pair (key,value)
              if (my_integer_n_p(p->value)) {
                printf ("It is a long long, value = %lld\n",
                        *my_integer_cget_n(p->value));
              }
              if (my_integer_z_p(p->value)) {
                printf ("It is a mpz_t, value =");
                mpz_out_str(stdout, 10, *my_integer_cget_z(p->value));
                printf("\n");
              }
            }
        }
    } /* All variables are automatically cleared beyond this point */
}

#else
// Nothing to do
int main(void) { return 0; }
#endif
