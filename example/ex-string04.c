#include <stdio.h>
#include "m-string.h"
#include "m-dict.h"
#include "m-array.h"

/* Definition of a bounded string of 16 characters.
   Such a string can hold at most 15 characters plus the final '\0' character.
*/
BOUNDED_STRING_DEF(string16, 16)
#define M_OPL_string16_t() BOUNDED_STRING_OPLIST(string16)

/* Definition of an associative map string16_t --> size_t */
DICT_DEF2(dict_str, string16_t, size_t)
#define M_OPL_dict_str_t() DICT_OPLIST(dict_str, M_OPL_string16_t(), M_BASIC_OPLIST)

/* Definition of an array of string16_t */
ARRAY_DEF(vector_str, string16_t)
#define M_OPL_vector_str_t() ARRAY_OPLIST(vector_str, M_OPL_string16_t())

int main(void)
{
    // Construct an array of string16_t, performing a conversion of the C const char *
    // into a proper string16_t at real time and push then into a dynamic array
    // that is declared, initialized and cleared.
    // So there is two levels of conversion: from const char * to string16_t 
    // and all string16_t are pushed back in the array.
    M_LET( (words, ("This"), ("is"), ("a"), ("useless"), ("sentence"), ("."),
                    ("It"), ("is"), ("used"), ("a"), ("bit"), ("to"), ("count"), ("words"), (".") ),
                    vector_str_t) {
        // Print the arrays.
        printf("The words are: ");
        vector_str_out_str(stdout, words);
        printf("\n");

        // Count the words.
        M_LET(map, dict_str_t) {
            // Count the words into the dictionary 'map'
            for M_EACH(w, words, vector_str_t) {
                (*dict_str_safe_get(map, *w)) ++;
            }

            // Print the count:
            for M_EACH(p, map, dict_str_t) {
                // In C11 version we can use the M_PRINT macro that simplifies formatting.
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)            
                M_PRINT(p->value, " occurrences of ", p->key, "\n");
#else
                printf ("%zu occurrences of %s\n", p->value, string16_get_cstr(p->key));
#endif
            }
        }
    }
}

