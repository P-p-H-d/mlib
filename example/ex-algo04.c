#include "m-algo.h"
#include "m-array.h"
#include "m-string.h"

/* Define an array of int */
ARRAY_DEF(vector_int, int)
/* Register the oplist of this array of int globally */
#define M_OPL_vector_int_t() ARRAY_OPLIST(vector_int)

/* Concat two strings by insertion a '-' within them */
#define str_cat(s1, s2) (string_cat_str(s1, "-"), string_cat(s1, s2))

int main(void)
{
  // Let 's' be a string_t and initialize it.
  M_LET(s, string_t)
    // Let 'v' be a vector of integer and initialize it.
    M_LET( (v, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10), vector_int_t) {
    int sum = 0, product = 0;
    
    // Compute the sum of theses integers.
    // 'add' is a registered keyword, recognized by M*LIB to perform a sum.
    ALGO_REDUCE(sum, v, vector_int_t, add);

    // Compute the product of theses integers.
    // 'mul' is a registered keyword, recognized by M*LIB to perform a product.
    ALGO_REDUCE(product, v, vector_int_t, mul);

    /* Example of reduction with a transformation of the type from int to string_t
       By default, the result ("s") should be of the same type as the base type
       of the container. This is not the case here, we want to transform the
       type. As such, we add the precision of the type 'string_t' to 's'.
       The transformation from string to integer is done by string_set_si */
    ALGO_REDUCE( (s, string_t), v, vector_int_t, str_cat, string_set_si);

    printf ("sum: %d\n"
            "product: %d\n"
            "dash str: %s\n", sum, product, string_get_cstr(s));
  }
  return 0;
}
