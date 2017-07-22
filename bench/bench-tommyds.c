#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "tommyds/tommy.h"

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  tommy_array a1, a2;

  tommy_array_init(&a1);
  tommy_array_init(&a2);
  for(size_t i = 0; i < n; i++) {
    unsigned int v = rand_get();
    tommy_array_insert(&a1, (void*)(uintptr_t) v);
    v = rand_get();
    tommy_array_insert(&a2, (void*)(uintptr_t) v);
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += (uintptr_t) *tommy_array_ref(&a1, i) * (uintptr_t) *tommy_array_ref(&a2, i );
  }
  g_result = s;
  tommy_array_done(&a1);
  tommy_array_done(&a2);
}

/********************************************************************************************/

struct list_node_s {
  unsigned int value;
  tommy_node node;
};

static void test_list (size_t n)
{
  tommy_list a1, a2;

  tommy_list_init(&a1);
  tommy_list_init(&a2);
  for(size_t i = 0; i < n; i++) {
    struct list_node_s *obj;
    obj = malloc(sizeof(struct list_node_s));
    if (obj == NULL) abort();
    obj->value = rand_get();
    tommy_list_insert_tail(&a1, &obj->node, obj);
    obj = malloc(sizeof(struct list_node_s));
    if (obj == NULL) abort();
    obj->value = rand_get();
    tommy_list_insert_tail(&a2, &obj->node, obj);
  }
  unsigned int s = 0;
  for(tommy_node *it1 = tommy_list_head(&a1), *it2 = tommy_list_head(&a2)
	; it1 != NULL; it1 = it1->next, it2 = it2->next ) {
    struct list_node_s *obj1 = it1->data, *obj2 = it2->data;
    s += obj1->value * obj2->value;
    n--;
  }
  g_result = s;
  tommy_list_foreach(&a1, free);
  tommy_list_foreach(&a2, free);
}

/********************************************************************************************/

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  if (n == 10)
    test_function("List   time",10000000, test_list);
  if (n == 20)
    test_function("Array  time", 100000000, test_array);
  /*  if (n == 30)
    test_function("Rbtree time", 1000000, test_rbtree);
  if (n == 40)
    test_function("Dict   time", 1000000, test_dict);
  if (n == 41)
    test_function("DictB  time", 1000000, test_dict_big);
  if (n == 50)
  test_function("Sort   time", 10000000, test_sort);*/
  exit(0);
}
