#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "m-list.h"
#include "m-string.h"
#include "m-algo.h"

LIST_DEF(string, string_t, STRING_OPLIST)
#define ASTR_OPLIST LIST_OPLIST(string, STRING_OPLIST)
ALGO_DEF(astring, ASTR_OPLIST)

int main(void)
{
  // Let's create a variable named 'grow' which is a list of string_t
  M_LET (grow, ASTR_OPLIST)
    M_LET (tmpstr, STRING_OPLIST) { // And let's create a string_t

    // add string elements to the array
    list_string_push_back (grow, STRING_CTE("AB"));
    string_printf(tmpstr, "%d", 12);
    list_string_push_back (grow, tmpstr);
    list_string_push_back (grow, STRING_CTE("CD"));

    // get the chunk as a big string
    list_string_get_str (tmpstr, grow, false);

    // print out
    printf("Number of elements = %lu\n", (unsigned long) list_string_size(grow));
    printf("Final string = %s\n", string_get_cstr(tmpstr));

    // Another way to print the list
    for M_EACH (item, grow, ASTR_OPLIST) {
        printf ("Item = ");
        string_fputs (stdout, *item);
        printf ("\n");
      }

    // Get the min and the max of the list
    string_t *p = astring_min(grow);
    printf ("Min string is %s\n", string_get_cstr(*p));
    p = astring_max(grow);
    printf ("Max string is %s\n", string_get_cstr(*p));

    // We can even sort the list!
    astring_sort(grow);
    for M_EACH (item, grow, ASTR_OPLIST) {
        printf ("Sort Item = %s\n", string_get_cstr(*item) );
      }
    
    // Split a string into a list.
    string_set_str(tmpstr, "HELLO;JOHN;HOW;ARE;YOU");
    astring_split(grow, tmpstr, ';');
    for M_EACH (item, grow, ASTR_OPLIST) {
        printf ("Split Item = %s\n", string_get_cstr(*item) );
      }
    
  } // Everything is cleared after this point.
  
  return 0;
}
