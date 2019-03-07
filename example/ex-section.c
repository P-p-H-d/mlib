#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#include <stdio.h>

#include "m-array.h"
#include "m-tuple.h"
#include "m-dict.h"
#include "m-string.h"

TUPLE_DEF2(symbol, (offset, long), (value, long))
#define M_OPL_symbol_t() TUPLE_OPLIST(symbol, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST)

ARRAY_DEF(array_symbol, symbol_t)
#define M_OPL_array_symbol_t() ARRAY_OPLIST(array_symbol, M_OPL_symbol_t())

DICT_DEF2(sections, string_t, array_symbol_t)
#define M_OPL_sections_t() DICT_OPLIST(sections, STRING_OPLIST, M_OPL_array_symbol_t())

int main(int argc, const char *argv[])
{
  if (argc < 2) abort();
  FILE *f = fopen(argv[1], "rt");
  if (!f) abort();
  M_LET(sc, sections_t) {
    sections_in_str(sc, f);
    array_symbol_t *a = sections_get(sc, STRING_CTE(".text"));
    if (a == NULL) {
      printf("There is no .text section.");
    } else {
      printf("Section .text is :");
      array_symbol_out_str(stdout, *a);
      printf("\n");
    }
  }
  return 0;
}

#else
int main(void) { return 0; }
#endif
