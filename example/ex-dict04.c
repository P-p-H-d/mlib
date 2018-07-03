#include <stdio.h>
#include "m-string.h"
#include "m-dict.h"

/* program that reads text from standard-input, 
   and prints the total number of distinct words found in the text. 
*/
DICT_SET_DEF2(dict_str, string_t, STRING_OPLIST)

int main(void)
{
  M_LET(word, STRING_OPLIST)
    M_LET(wordcount, DICT_SET_OPLIST(dict_str, STRING_OPLIST)) {
    while (string_fget_word(word, " \t\n,.!;:?", stdin))
      dict_str_push(wordcount, word);
    printf ("Words: %zu\n", dict_str_size(wordcount));
  }
}
