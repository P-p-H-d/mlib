#include "m-array.h"

// Let's define an array of 'char *' named 'charp'
ARRAY_DEF (array_charp, char *)

int main(void)
{
  // Create an array list 'al'
  M_LET (al, ARRAY_OPLIST(array_charp)) {
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
    for M_EACH(element, al, ARRAY_OPLIST(array_charp)) {
        printf ("%s ", *element);
    }
    printf("\n");
  } // After this point al is automatically cleared
  return 0;
}
