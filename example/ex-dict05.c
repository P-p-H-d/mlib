#include <stdio.h>
#include "m-dict.h"

/*
 * Provide OOR methods for OA Hashmap.
 * See documentation for definition of OOR
 * OOR values are represented as: 
 * empty (0) is INT_MIN
 * deleted (1) is INT_MIN + 1
 * So valid range for integers are [INT_MIN+2, INT_MAX]
 */
static inline bool oor_equal_p(int k, char n)
{
  return k == INT_MIN + n;
}

static inline int  oor_set(char n)
{
  return INT_MIN + n;
}

/*
 * Provide a custom hash function (optional for OA)
 */
static inline size_t hash(int n)
{
  return (size_t) n; // Identity hash!
}

/* Define an OA dictionnay which performs a hashmap between an int to an int */
DICT_OA_DEF2(dict_oa,
	     int, M_OPEXTEND(M_BASIC_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(API_4(oor_set)), HASH(hash)),
	     int, M_BASIC_OPLIST)

int main(void)
{
  dict_oa_t d;
  dict_oa_init(d);

  for(int i = 0; i < 10; i++) {
    dict_oa_set_at(d, i*i, 4*i);
  }
  dict_oa_set_at(d, 3, -17);
  
  printf("DICT[3] = %d\n", *dict_oa_get(d, 3));

  dict_oa_it_t it;
  printf("DICT=");
  for(dict_oa_it(it, d); !dict_oa_end_p(it); dict_oa_next(it)) {
    printf("%d:%d, ", dict_oa_cref(it)->key, dict_oa_cref(it)->value);
  }
  printf("\n");
  
  dict_oa_clear(d);
  return 0;
}
