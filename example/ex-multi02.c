#include <stdio.h>
#include <gmp.h>

#include "m-variant.h"
#include "m-string.h"
#include "m-dict.h"
#include "m-array.h"

/* Define a variant over two integers named integer_t
   Notice than mpz_t has special init & clear functions
   mpz_init & mpz_clear.
   This is handle by the library.
*/
VARIANT_DEF2(my_integer,
             (n, long long, M_DEFAULT_OPLIST),
             (z, mpz_t, M_CLASSIC_OPLIST(mpz)))
/* Define the oplist associated to this variant */
#define MY_INTEGER_OPLIST VARIANT_OPLIST(my_integer, M_DEFAULT_OPLIST, M_CLASSIC_OPLIST(mpz))

/* Define a dictionary of string --> integer named dict_my_t */
DICT_DEF2(my_dict,
          string_t, STRING_OPLIST,
          my_integer_t, MY_INTEGER_OPLIST)
/* Define the oplist associated to this dictionary */
#define MY_DICT_OPLIST DICT_OPLIST(my_dict, STRING_OPLIST, MY_INTEGER_OPLIST)

/* Define an array of such dictionary named array_my_t */
ARRAY_DEF(my_array, my_dict_t, MY_DICT_OPLIST)
/* Define the oplist associated to this array of dictionary of variant */
#define MY_ARRAY_OPLIST ARRAY_OPLIST(my_array, MY_DICT_OPLIST)

int main(void)
{
  M_LET(z, MY_INTEGER_OPLIST)         /* Define & init & clear an instance of a variant of integer */
    M_LET(dict, MY_DICT_OPLIST)       /* Define & init & clear an instance of a dictionary of variant */
    M_LET(array, MY_ARRAY_OPLIST)     /* Define & init & clear an intance of an array of dictionary of variant */
    M_LET(gmp, M_CLASSIC_OPLIST(mpz)) /* M_LET allows even with non container type */
    {
      my_integer_set_n (z, 17LL);                // Set z as the long long 17
      my_dict_set_at(dict, STRING_CTE("n1"), z); // Push it in the dictionary
      mpz_set_str(gmp, "25446846874687468746874687468746874686874", 10); // Set gmp to a very long integer.
      my_integer_set_z (z, gmp);                 // Set z as the mpz_t variable
      my_dict_set_at(dict, STRING_CTE("n2"), z); // Push it in the dictionary
      my_array_push_back (array, dict);          // Push the dictionary in the array

      // Iterate over the array container
      for M_EACH(item, array, MY_ARRAY_OPLIST) {
          // Iterate over the dictionary container
          for M_EACH(p, *item, MY_DICT_OPLIST) {
              // Dictionary iterator are pair (key,value)
              if (my_integer_n_p(p->value)) {
                printf ("It is a long long, value = %lld\n",
                        *my_integer_get_n(p->value));
              }
              if (my_integer_z_p(p->value)) {
                printf ("It is a mpz_t, value =");
                mpz_out_str(stdout, 10, *my_integer_get_z(p->value));
                printf("\n");
              }
            }
        }
    } /* All variables are automatically cleared beyond this point */
}
