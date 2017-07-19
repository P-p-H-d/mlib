#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "array.h"

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  Array *a1, *a2;
  enum cc_stat stat;

  stat = array_new(&a1);
  if (stat != CC_OK) abort();
  stat = array_new(&a2);
  if (stat != CC_OK) abort();
  
  for(size_t i = 0; i < n; i++) {
    stat = array_add(a1, (void*) (uintptr_t)rand_get() );
    if (stat != CC_OK) abort();
    stat = array_add(a2, (void*) (uintptr_t)rand_get() );
    if (stat != CC_OK) abort();
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    void *e, *f;
    stat = array_get_at(a1, i, &e);
    if (stat != CC_OK) abort();
    stat = array_get_at(a2, i, &f);
    if (stat != CC_OK) abort();
    s += ((unsigned int)(uintptr_t) e) * ((unsigned int)(uintptr_t) f);
  }
  g_result = s;

  array_destroy(a1);
  array_destroy(a2);
}


/********************************************************************************************/

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  if (n == 20)
    test_function("Array  time", 100000000, test_array);
  exit(0);
}

