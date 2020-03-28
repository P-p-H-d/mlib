#include <stdio.h>
#include "m-array.h"
#include "m-algo.h"

/* Define a dynamic array of int */ 
ARRAY_DEF(vector_int, int)
#define M_OPL_vector_int_t() ARRAY_OPLIST(vector_int)
ALGO_DEF(vector_int, vector_int_t)

/* Define a dynamic array of bool */ 
ARRAY_DEF(vector_bool, bool)
#define M_OPL_vector_bool_t() ARRAY_OPLIST(vector_bool)
ALGO_DEF(vector_bool, vector_bool_t)

static bool id(bool x) { return x ; }

int main(void)
{
  M_LET(myvector, tmp, vector_int_t) {
    vector_int_fill_n(myvector, 3, 20);
    vector_int_fill_n(tmp, 3, 33);
    vector_int_splice(myvector, tmp);
    vector_int_fill_n(tmp, 2, 10);
    vector_int_splice(myvector, tmp);
    printf("myvector contains:");
    // In C11, the for loop can be simplified as :
    // ALGO_FOR_EACH(myvector, vector_int_t, M_PRINT, " ");
    for M_EACH(it, myvector, vector_int_t) {
        printf(" %d", *it);
      }
    printf("\n");
  }
  
  M_LET(myvector, tmp, vector_bool_t) {
    vector_bool_fill_n(myvector, 3, false);
    vector_bool_fill_n(tmp, 3, true);
    vector_bool_splice(myvector, tmp);
    vector_bool_fill_n(tmp, 2, false);
    vector_bool_splice(myvector, tmp);
    printf("myvector contains:");
    // In C11, the for loop can be simplified as :
    // ALGO_FOR_EACH(myvector, vector_bool_t, M_PRINT, " ");
    for M_EACH(it, myvector, vector_bool_t) {
        printf(" %d", (int)*it);
      }
    printf("\n");
    printf ("ALL: %d ANY: %d NONE: %d\n",
            vector_bool_all_of_p(myvector, id),
            vector_bool_any_of_p(myvector, id),
            vector_bool_none_of_p(myvector, id) );
  }
  return 0;
}
