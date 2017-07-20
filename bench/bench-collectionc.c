#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "array.h"
#include "slist.h"
#include "treetable.h"

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

static void test_list(size_t n)
{
  SList *a1, *a2;
  enum cc_stat stat;

  stat = slist_new(&a1);
  if (stat != CC_OK) abort();
  stat = slist_new(&a2);
  if (stat != CC_OK) abort();
  
  for(size_t i = 0; i < n; i++) {
    stat = slist_add(a1, (void*) (uintptr_t)rand_get() );
    if (stat != CC_OK) abort();
    stat = slist_add(a2, (void*) (uintptr_t)rand_get() );
    if (stat != CC_OK) abort();
  }
  unsigned int s = 0;
  SListIter iter1, iter2;
    void *e, *f;
  slist_iter_init(&iter1, a1);
  slist_iter_init(&iter2, a2);
  while (slist_iter_next(&iter1, &e) == CC_OK && slist_iter_next(&iter2, &f) == CC_OK) {
    s += ((unsigned int)(uintptr_t) e) * ((unsigned int)(uintptr_t) f);
  }
  g_result = s;

  slist_destroy(a1);
  slist_destroy(a2);
}

/********************************************************************************************/

static int compare(const void *a, const void *b)
{
  const uintptr_t pa = (uintptr_t) a;
  const uintptr_t pb = (uintptr_t) b;
  return (pa < pb) ? -1 : (pa > pb);
}

static void test_rbtree(size_t n)
{
  TreeTable *tree;
  enum cc_stat stat;

  stat = treetable_new(compare, &tree);
  if (stat != CC_OK) abort();
  for (size_t i = 0; i < n; i++) {
    void *key = (void*)(uintptr_t) i;
    stat = treetable_add(tree, key, key);
    if (stat != CC_OK) abort();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    void *e;
    void *key = (void*) (uintptr_t) rand_get();
    stat = treetable_get(tree, key, &e);
    if (stat == CC_OK)
      s += (uintptr_t)e;
  }
  g_result = s;
  treetable_destroy(tree);
}

/********************************************************************************************/

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  if (n == 10)
    test_function("List   time",10000000, test_list);
  if (n == 20)
    test_function("Array  time", 100000000, test_array);
  if (n == 30)
    test_function("Rbtree time", 1000000, test_rbtree);
  exit(0);
}

