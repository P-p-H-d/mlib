#include "m-algo.h"
#include "m-array.h"
#include "m-string.h"

ARRAY_DEF(vector_int, int)
#define M_OPL_vector_int_t() ARRAY_OPLIST(vector_int)

#define str_ui(s, i) string_printf(s, "%d", i)
#define str_cat(s1, s2) (string_cat_str(s1, "-"), string_cat(s1, s2))

int main(void)
{
  M_LET(s, string_t)
    M_LET( (v, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10), vector_int_t) {
    int sum, product;
    
    ALGO_REDUCE(sum, v, vector_int_t, add);
    ALGO_REDUCE(product, v, vector_int_t, mul);
    /* Example of reduction with a transformation of the type from int to string_t */
    ALGO_REDUCE( (s, string_t), v, vector_int_t, str_cat, str_ui);

    printf ("sum: %d\n"
            "product: %d\n"
            "dash str: %s\n", sum, product, string_get_cstr(s));
  }
  return 0;
}
