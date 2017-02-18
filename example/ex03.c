#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m-dict.h"
#include "m-string.h"

DICT_DEF2(str, string_t, STRING_OPLIST, int, M_DEFAULT_OPLIST)

int main(void)
{
  dict_str_t h;
  int max = 1;
  string_t key;

  dict_str_init(h);
  string_init (key);
  while (!feof(stdin))
    {
      string_fgets (key, stdin, STRING_READ_PURE_LINE);
      int *v = dict_str_get (h, key);
      if (v != NULL) {
        (*v)++;
        max = M_MAX(*v, max);
      } else {
        dict_str_set_at(h, key, 1);
      }
    }
  string_clear(key);
  dict_str_clear(h);
  return 0;
}
