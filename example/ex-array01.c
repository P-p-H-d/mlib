#include "m-array.h"

// Let's define an array of 'char *' named 'charp'
ARRAY_DEF (array_charp, char *)

int main(void)
{
  // Create an array list
  array_charp_t al;
  // Initialize it
  array_charp_init(al);
      
  // add elements to the array list
  array_charp_push_back (al,"C");
  array_charp_push_back (al,"A");
  array_charp_push_back (al,"E");
  array_charp_push_back (al,"B");
  array_charp_push_back (al,"D");
  array_charp_push_back (al,"F");

  // Use iterator to display contents of al
  printf("Original contents of al: ");
  array_charp_it_t itr;
  for (array_charp_it(itr, al) ; !array_charp_end_p (itr); array_charp_next(itr)) {
    char *element = *array_charp_ref(itr);
    printf ("%s ", element);
  }
  printf("\n");
      
  // Now, display the list backwards
  printf("Array backwards: ");
  for (array_charp_previous (itr) ; !array_charp_end_p (itr); array_charp_previous(itr)) {
    char *element = *array_charp_ref(itr);
    printf ("%s ", element);
  }
  printf("\n");

  // For array, we can also iterate over the integer
  printf("Array Direct: ");
  for(size_t i = 0 ; i < array_charp_size(al); ++i) {
    char *element = *array_charp_get(al, i);
    printf ("%s ", element);
  }
  printf("\n");

  // Clear the array
  array_charp_clear(al);
  return 0;
}
