#include "ex_noinline02.h"

int main(void)
{
  // string_* calls use m-string's default linkage in this configuration.
  string_t s;
  string_init_set_str(s, "a=");
  string_fputs(stdout, s);
  string_clear(s);

  // array_str_* calls were declared in the header and are implemented
  // out-of-line in ex-noinline02-lib.c via M_USE_DEF.
  array_str_t a;
  array_str_init(a);
  array_str_emplace_back(a, "Hello");
  array_str_emplace_back(a, "World");
  array_str_out_str(stdout, a);
  printf("\n");
  array_str_clear(a);
  return 0;
}
