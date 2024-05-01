#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m-dict.h"
#include "m-string.h"

// Define a dictionary of string --> int
// named 'dict_str'
DICT_DEF2(dict_str, string_t, STRING_OPLIST, int, M_BASIC_OPLIST)

int main(void)
{
  // Declare the dictionary 'h'
  dict_str_t h;
  int max = 1;
  // Declare the string 'key'
  string_t key;

  // Initialize the dictionary & the string
  dict_str_init(h);
  string_init (key);
  
  // While there is some data in the stream
  while (true)
    {
      // Read a complete line from the stream and fill in the string 'key'
      string_fgets (key, stdin, STRING_READ_PURE_LINE);
      if (feof(stdin)) break;
      // If the 'key' exists in the dictionary
      int *v = dict_str_get (h, key);
      if (v != NULL) {
        // Increment the counter in the dictionary
        (*v)++;
        max = M_MAX(*v, max);
      } else {
        // Update the dictionary with h[key] = 1
        dict_str_set_at(h, key, 1);
      }
    }
  printf ("Max occurrence = %d\n", max);

  // Iterate on the dictionary
  dict_str_it_t it;
  for(dict_str_it(it, h); !dict_str_end_p(it); dict_str_next(it)) {
    // Get a constant reference on the pair stored in the dictionary
    const struct dict_str_pair_s *pair = dict_str_cref(it);
    printf ("Key=%s Value=%d\n",
            string_get_cstr(pair->key), pair->value);
  }

  // Clear variables
  string_clear(key);
  dict_str_clear(h);
  return 0;
}
