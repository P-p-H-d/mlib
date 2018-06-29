#include <stdio.h>
#include "m-string.h"
#include "m-array.h"
#include "m-dict.h"

ARRAY_DEF(array_size, size_t)
#define M_OPL_array_size_t() ARRAY_OPLIST(array_size)
DICT_DEF2(map_pos, string_t, STRING_OPLIST, array_size_t, ARRAY_OPLIST(array_size))
#define M_OPL_map_pos_t() DICT_OPLIST(map_pos, STRING_OPLIST, ARRAY_OPLIST(array_size))

int main(int argc, const char *argv[])
{
   if (argc == 2) {
     M_LET(word, string_t)
     M_LET(positions, map_pos_t) {
        FILE *f = fopen(argv[1], "rt");
        if (!f) return 1;
        while (string_fget_word(word, " \t\n\r\"(),=", f)) {
          array_size_t *array = map_pos_get_at(positions, word);
          array_size_push_back(*array, (size_t)ftell(f) - string_size(word));
        }
        fclose(f);

        for M_EACH(pair, positions, map_pos_t) {
         string_fputs(stdout, pair->key);
         for M_EACH(pos, pair->value, array_size_t)
            printf(" %zu", *pos);
         printf("\n");
        }
     }
   }
}
