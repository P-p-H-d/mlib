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
// The list supports both forward and backward push and only one backward pop.
LIST_DUALB_DEF(list_string, string_t)

// Let's register the oplist globally so that other M*LIB macros
// can get the oplist without us giving it explicitly to them.
// An oplist is the association of operators to the provided methods
// so that generic code can use correctly an object of such type.
// LIST_OPLIST is a macro used to generate the oplist associated to the
// list itself. It needs the prefix used for generating the list (aka list_string here)
// and the oplist of the item in the list (aka the string oplist)
// LIST_OPLIST also works for LIST_DUALB_DEF but not for LIST_DUALF_DEF (See M_LIST_DUALF_OPLIST for this one).
#define M_OPL_list_string_t() LIST_OPLIST(list_string, STRING_OPLIST)

#define M_FPRINT_INT(f, i) fprintf(f, "%d", i)

// Let's create a list of int
// The list supports both forward and backward push and only one forward pop.
// We extend the basic oplist with an output function for int to be able to print the list of int.
// We won't need to define the output function for string since it is already defined in the string oplist.
// We won't need to define it if we build in C11 or later since the output function for int is already defined in the basic oplist of int in this case.
LIST_DUALF_DEF(list_int, int, M_OPEXTEND(M_BASIC_OPLIST, OUT_STR(M_FPRINT_INT)))

// Let's register the oplist globally so that other M*LIB macros
#define M_OPL_list_int_t() LIST_DUALF_OPLIST(list_int, M_OPEXTEND(M_BASIC_OPLIST, OUT_STR(M_FPRINT_INT)))

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

    // Another way to add element is to push at the front of the list
    list_string_push_front (list, STRING_CTE("EF"));
    // We can also emplace at the front of the list
    list_string_emplace_front (list, "GH");

    // Serialize the list of string into a big string.
    list_string_get_str (tmpstr, list, false);

    // print it
    printf("Number of elements = %zu\n", list_string_size(list));
    printf("Final string = %s\n", string_get_cstr(tmpstr));

    // Let's pop one element but we can do it only at the back
    list_string_pop_back (&tmpstr, list);

    // Let's iterate over each element of the list
    // using the macro for iteration over a container.
    printf ("Using macro EACH after popping back element:\n");
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

  } // Everything is cleared after this point.

  // Let's create another list of int and init it.
  list_int_t list;
  list_int_init(list);
  // Let's put some elements in it:
  list_int_push_back (list, 1);
  list_int_push_back (list, 2);
  list_int_push_back (list, 3);
  list_int_push_back (list, 4);

  list_int_push_front (list, 10);
  list_int_push_front (list, 20);
  list_int_push_front (list, 30);
  list_int_push_front (list, 40);

  // Let's display the int by serializing the elements of the list:
  printf ("List of int = ");
  list_int_out_str(stdout, list);
  printf ("\n");

  // Let's pop one element but we can do it only at the front
  int j;
  list_int_pop_front (&j, list);
  printf("Pop element = %d\n", j);

  // Don't forget to clear the list before leaving
  list_int_clear(list);
  
  return 0;
}
