#include <stdio.h>
#include "m-dict.h"

/*
 * Provide OOR methods for OA Hashmap.
 * See documentation for definition of OOR
 * OOR values are represented as: 
 * empty (0) is "EMPTY"
 * deleted (1) is "DELETED"
 */
static const char empty_str[] = "EMPTY";
static const char deleted_str[] = "DELETED";
static const char *const oor_table[] = { empty_str, deleted_str };

static inline bool oor_equal_p(const char *k, int n)
{
  return k == oor_table[n];
}

static inline const char *oor_set(int n)
{
  return oor_table[n];
}

/*
 * Provide a custom hash function (optional for OA)
 */
static inline size_t hash(const char *k)
{
  size_t r = 17;
  while (*k) {
    r = *k * 33 + r;
    k++;
  }
  return r;
}

/* Define an OA dictionnay which performs a hashmap between a CSTR to an int */
DICT_OA_DEF2(dict_oa,
	     const char *, M_OPEXTEND(M_CSTR_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(API_4(oor_set)), HASH(hash)),
	     int, M_BASIC_OPLIST)

int main(void)
{
  dict_oa_t d;
  dict_oa_init(d);

  dict_oa_set_at(d, "THIS", -17);
  dict_oa_set_at(d, "IS", -170);
  dict_oa_set_at(d, "AN", 7);
  dict_oa_set_at(d, "EXAMPLE", 77);
  
  printf("DICT[\"AN\"] = %d\n", *dict_oa_get(d, "AN"));

  dict_oa_it_t it;
  printf("DICT=");
  for(dict_oa_it(it, d); !dict_oa_end_p(it); dict_oa_next(it)) {
    printf("%s:%d, ", dict_oa_cref(it)->key, dict_oa_cref(it)->value);
  }
  printf("\n");
  
  dict_oa_clear(d);
  return 0;
}
