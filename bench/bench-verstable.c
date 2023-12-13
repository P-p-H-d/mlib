#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "m-core.h"
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

#ifndef CHAR_ARRAY_SIZE
#define CHAR_ARRAY_SIZE 256
#endif
typedef char char_array_t[CHAR_ARRAY_SIZE];
typedef struct  { char_array_t a; } char_encap;// Cannot use char_array_t directly
static bool char_equal_p (const char_encap a, const char_encap b) { return strcmp(a.a,b.a)==0; }
static size_t char_hash(const char_encap a) { return m_core_hash (a.a, strlen(a.a)); }

#define NAME umap_char_array
#define KEY_TY char_encap
#define VAL_TY char_encap
#define HASH_FN char_hash
#define CMPR_FN char_equal_p
#include "verstable.h"

static void
test_dict_big(size_t  n)
{
  umap_char_array dict;
  umap_char_array_init(&dict);

  for (size_t i = 0; i < n; i++) {
    char_encap s1, s2;
    
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    umap_char_array_insert(&dict, s1, s2);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_encap s1;
    sprintf(s1.a, "%u", rand_get());
    umap_char_array_itr it = umap_char_array_get(&dict, s1);
    if (!umap_char_array_is_end(it) )
      s ++;
  }
  g_result = s;

  umap_char_array_cleanup(&dict);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 40,    "dict",   1000000, 0, test_dict, 0},
  { 41, "dictBig",   1000000, 0, test_dict_big, 0},
};

int main(int argc, const char *argv[])
{
  test("CTL", numberof(table), table, argc, argv);
  exit(0);
}

