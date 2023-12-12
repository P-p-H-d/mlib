#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"

#define NAME umap_ulong
#define KEY_TY unsigned long
#define VAL_TY unsigned long
#include "verstable.h"

static void
test_dict(size_t  n)
{
  umap_ulong dict;
  umap_ulong_init(&dict);
  
  for (size_t i = 0; i < n; i++) {
    unsigned long value = rand_get();
    unsigned long key = rand_get();
    umap_ulong_insert(&dict, key, value);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    umap_ulong_itr it = umap_ulong_get(&dict, rand_get());
    if (!umap_ulong_is_end(it) )
      s += it.data->val;
  }
  g_result = s;

  umap_ulong_cleanup(&dict);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 40,    "dict",   1000000, 0, test_dict, 0},
};

int main(int argc, const char *argv[])
{
  test("CTL", numberof(table), table, argc, argv);
  exit(0);
}

