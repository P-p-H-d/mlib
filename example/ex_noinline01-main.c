#include "ex_noinline01.h"

int main(void)
{
  // Calls below use APIs declared in the header and defined in ex-noinline01-lib.c.
  array_str_t a;
  array_str_init(a);
  array_str_emplace_back(a, "Hello");
  array_str_emplace_back(a, "World");
  printf("a=");
  array_str_out_str(stdout, a);
  printf("\n");
  array_str_clear(a);
  return 0;
}
