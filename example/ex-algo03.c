#include "m-dict.h"
#include "m-array.h"
#include "m-algo.h"
#include "m-string.h"

DICT_DEF2(dict, string_t, int)
#define M_OPL_dict_t() DICT_OPLIST(dict, STRING_OPLIST, M_DEFAULT_OPLIST)

ARRAY_DEF(vector_int, int)
#define M_OPL_vector_int_t() ARRAY_OPLIST(vector_int)

#define start_with(pattern, item)                \
  string_start_with_str_p((item).key, (pattern))
#define get_value(out, item) ((out) = (item).value)

int main(void)
{
  int s;
  M_LET(keys, vector_int_t)
    M_LET( (m, (STRING_CTE("foo"), 1), (STRING_CTE("bar"), 42), (STRING_CTE("bluez"), 7), (STRING_CTE("stop"), 789) ), tmp, dict_t) {
 
    /* Extract all elements of 'm' that starts with 'b' */
    ALGO_EXTRACT(tmp, dict_t, m, dict_t, start_with, "b");
    /* Extract the values of theses elements */
    ALGO_TRANSFORM(keys, vector_int_t, tmp, dict_t, get_value);
    /* Sum theses values */
    ALGO_REDUCE(s, keys, vector_int_t, sum);
    printf("Sum of elements starting with 'b' is: %d\n", s);
  }
  return 0;
}
