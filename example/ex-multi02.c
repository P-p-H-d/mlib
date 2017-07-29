#include <stdio.h>
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
/* Define the oplist associated to this variant */
#define INTEGER_OPLIST VARIANT_OPLIST(integer, M_DEFAULT_OPLIST, M_CLASSIC_OPLIST(mpz))

/* Define a dictionary of string --> integer named dict_my_t */
DICT_DEF2(dict_my,
          string_t, STRING_OPLIST,
          integer_t, INTEGER_OPLIST)
/* Define the oplist associated to this dictionary */
#define MY_DICT_OPLIST DICT_OPLIST(dict_my, STRING_OPLIST, INTEGER_OPLIST)

/* Define an array of such dictionary named array_my_t */
ARRAY_DEF(array_my, dict_my_t, MY_DICT_OPLIST)
/* Define the oplist associated to this array of dictionary of variant */
#define MY_ARRAY_OPLIST ARRAY_OPLIST(array_my, MY_DICT_OPLIST)

int main(void)
{
  M_LET(z, INTEGER_OPLIST)            /* Define & init & clear an instance of a variant of integer */
    M_LET(dict, MY_DICT_OPLIST)       /* Define & init & clear an instance of a dictionary of variant */
    M_LET(array, MY_ARRAY_OPLIST)     /* Define & init & clear an intance of an array of dictionary of variant */
    M_LET(gmp, M_CLASSIC_OPLIST(mpz)) /* M_LET allows even with non container type */
    {
      integer_set_n (z, 17LL);                   // Set z as the long long 17
      dict_my_set_at(dict, STRING_CTE("n1"), z); // Push it in the dictionary
      mpz_set_str(gmp, "25446846874687468746874687468746874686874", 10); // Set gmp to a very long integer.
      integer_set_z (z, gmp);                    // Set z as the mpz_t variable
      dict_my_set_at(dict, STRING_CTE("n2"), z); // Push it in the dictionary
      array_my_push_back (array, dict);          // Push the dictionary in the array

      // Iterate over the array container
      for M_EACH(item, array, MY_ARRAY_OPLIST) {
          // Iterate over the dictionary container
          for M_EACH(p, *item, MY_DICT_OPLIST) {
              // Dictionary iterator are pair (key,value)
              if (integer_n_p((*p)->value)) {
                printf ("It is a long long, value = %lld\n",
                        *integer_get_n((*p)->value));
              }
              if (integer_z_p((*p)->value)) {
                printf ("It is a mpz_t, value =");
                mpz_out_str(stdout, 10, *integer_get_z((*p)->value));
                printf("\n");
              }
            }
        }
    } /* All variables are automatically cleared beyond this point */
}
