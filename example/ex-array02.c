#include "m-array.h"

// Let's define an array of 'char *' named 'charp'
ARRAY_DEF (array_charp, char *)
// Let's register globaly the oplist of array_charp
#define M_OPL_array_charp_t() ARRAY_OPLIST(array_charp)


int main(void)
{
  // Create an array list 'al' of type array_charp_t
  M_LET (al, array_charp_t) {
    // add elements to the array list
    array_charp_push_back (al,"C");
    array_charp_push_back (al,"A");
    array_charp_push_back (al,"E");
    array_charp_push_back (al,"B");
    array_charp_push_back (al,"D");
    array_charp_push_back (al,"F");

    // Display contents of al
    printf("Original contents of al: ");
    // For each element of the array al
    for M_EACH(element, al, array_charp_t) {
        printf ("%s ", *element);
    }
    printf("\n");
  } // After this point al is automatically cleared
  return 0;
}
