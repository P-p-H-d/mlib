#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "m-list.h"
#include "m-string.h"
#include "m-algo.h"

// Let's create a list of string by using the M*LIB dynamic string 'string_t'
// The prefix used for generating functions will be 'list_string'
LIST_DEF(list_string, string_t)

// Let's register the oplist globally so that other M*LIB macros
// can get the oplist without us giving it explictly to them.
// An oplist is the association of operators to the provided methods
// so that generic code can use correctly an object of such type.
// LIST_OPLIST is a macro used to generate the oplist associated to the
// list itself. It needs the prefix used for generating the list (aka list_string here)
// and the oplist of the item in the list (aka the string oplist)
#define M_OPL_list_string_t() LIST_OPLIST(list_string, STRING_OPLIST)

// Let's define some basic algorithms over this list of string.
// We reuse the prefix of the list to simplify usage, but algorithms can use separate namespace.
ALGO_DEF(list_string, M_OPL_list_string_t())

int main(void)
{
  // Let's create a variable named 'list' which is a list of string_t
  // This variable is only defined within the associated '{' '}' block.
  // The constructor/destructor of the variable will be called within this block
  // You can also define the variable (list_string_t) and use explicitly
  // its constructor (list_string_init) / destructor (list_string_clear).
  M_LET (list, list_string_t)
    M_LET (tmpstr, string_t) { // And let's create also a string_t

    // add string elements to the list in different ways
    // First is pushing a const string.
    // The macro STRING_CTE creates a const string_t based on the given const char * string.
    list_string_push_back (list, STRING_CTE("AB"));
    // Second is creating a separate string_t and then pushing it in the list
    string_printf(tmpstr, "%d", 12);
    list_string_push_back (list, tmpstr);
    // Third is to emplace directly in the list by constructing a new string_t based on the given const char * 
    list_string_emplace_back (list, "CD");

    // Serialize the list of string into a big string.
    list_string_get_str (tmpstr, list, false);

    // print it
    printf("Number of elements = %zu\n", list_string_size(list));
    printf("Final string = %s\n", string_get_cstr(tmpstr));

    // Let's iterate over each element of the list
    // using the macro for iteration over a container.
    printf ("Using macro EACH:\n");
    for M_EACH (item, list, list_string_t) {
        printf ("Item = ");
        string_fputs (stdout, *item);
        printf ("\n");
      }

    // You can also use classic iterator:
    list_string_it_t it;
    printf ("Using iterators:\n");
    for(list_string_it(it, list) ; !list_string_end_p(it) ; list_string_next(it)) {
      const string_t *item = list_string_cref (it);
      printf ("Item = ");
      string_fputs (stdout, *item);
      printf ("\n");
    }

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
    // Another way: we map each element to the M_PRINT function.
    printf ("Using macro ALGO_FOR_EACH:");
    ALGO_FOR_EACH(list, list_string_t, M_PRINT, "\nItem = ");
    printf ("\n");
#endif

    // Get the min and the max of the list
    string_t *p = list_string_min(list);
    printf ("Min string is %s\n", string_get_cstr(*p));
    p = list_string_max(list);
    printf ("Max string is %s\n", string_get_cstr(*p));

    // We can even sort the list.
    list_string_sort(list);
    for M_EACH (item, list, list_string_t) {
        printf ("Sort Item = %s\n", string_get_cstr(*item) );
      }
    
    // Split a string into a list.
    string_set_str(tmpstr, "HELLO;JOHN;HOW;ARE;YOU");
    list_string_split(list, tmpstr, ';');
    // Iterate over the splitted list
    for M_EACH (item, list, list_string_t) {
        printf ("Split Item = %s\n", string_get_cstr(*item) );
      }
    
  } // Everything is cleared after this point.

  // Let's create another list of string and init it.
  list_string_t list;
  list_string_init(list);
  // Let's put some elements in it:
  list_string_emplace_back (list, "A");
  list_string_emplace_back (list, "B");
  list_string_emplace_back (list, "C");
  list_string_emplace_back (list, "D");
  // Reverse it
  list_string_reverse(list);
  // Let's display the string by serializing the elements of the list:
  printf ("List of string = ");
  list_string_out_str(stdout, list);
  printf ("\n");
  // Don't forget to clear the list before leaving
  list_string_clear(list);
  
  return 0;
}
