#include "exn-header.h"

int main(void)
{
  string_t s;
  string_init_set_str(s, "a=");
  string_fputs(stdout, s);
  string_clear(s);
  array_str_t a;
  array_str_init(a);
  array_str_emplace_back(a, "Hello");
  array_str_emplace_back(a, "World");
  //printf("a=");
  array_str_out_str(stdout, a);
  printf("\n");
  array_str_clear(a);
  //main2();
  return 0;
}
