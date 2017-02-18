#include <stdlib.h>
#include "m-array.h"

ARRAY_DEF(int, int)

const int n = 100000;

int main(void)
{
  array_int_t x;
  array_int_init(x);
  
  for (int i = 0; i < n; i++) {
    array_int_push_back(x, rand());
  }
  
  for (int i = 0; i < n; i++) {
    array_int_push_at (x, rand() % n, rand());
  }
  
  for (int i = 0; i < n; i++) {
    array_int_pop_at (NULL, x, rand() % n);
  }
  
  array_int_clear(x);
  return 0;
}
