#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "xxhash.h"

#include "common.h"

static unsigned long *g_p;

static void test_hash_prepare(size_t n)
{
  g_p = malloc (n * sizeof(unsigned long));
  if (g_p == NULL) abort();
  for(size_t i = 0; i < n ; i++)
    g_p[i] = ((rand_get()*RAND_MAX + rand_get())*RAND_MAX+rand_get())*RAND_MAX + rand_get();
}


static void test_hash_final(void)
{
  free(g_p);
}

static void test_hash(size_t n)
{
  g_result = XXH64(g_p, sizeof(unsigned long)*n, 0);
}

/********************************************************************************************/


int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  if (n == 70) {
    n = (argc > 2) ? atoi(argv[2]) : 100000000;
    test_hash_prepare(n);
    test_function("XXHASH64", n, test_hash);
    test_hash_final();
  }
  
  exit(0);
}

