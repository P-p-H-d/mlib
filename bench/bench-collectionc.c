#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "array.h"
#include "slist.h"
#include "treetable.h"
#include "hashtable.h"

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
    void *key = (void*)(uintptr_t) rand_get();
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
static size_t hash_func (const void *key, int l, uint32_t seed)
{
  const unsigned long pa = (uintptr_t) key;
  (void)l;
  return pa ^ seed;
}

static int equal_func(const void *key1, const void *key2)
{
  const unsigned long pa = (uintptr_t) key1;
  const unsigned long pb = (uintptr_t) key2;
  return (pa < pb) ? -1 : pa > pb;
}

static void
test_dict(unsigned long  n)
{
  HashTable *dict;
  enum cc_stat stat;
  HashTableConf htc;
  hashtable_conf_init(&htc);
  htc.hash = hash_func;
  htc.key_compare = equal_func;
  stat = hashtable_new_conf(&htc, &dict);
  if (stat != CC_OK) abort();
  
  for (size_t i = 0; i < n; i++) {
    void *value = (void*)(uintptr_t) rand_get();
    void *key = (void*)(uintptr_t) rand_get();
    stat = hashtable_add(dict, key, value );
    if (stat != CC_OK) abort();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    void *key = (void*)(uintptr_t) rand_get();
    void *r;
    stat = hashtable_get(dict, key, &r);
    if (stat == CC_OK)
      s += (uintptr_t) r;
  }
  g_result = s;
  hashtable_destroy(dict);
}

/********************************************************************************************/

typedef char char_array_t[256];

static int char_equal (const void* a, const void * b)
{
  const char_array_t *pa = (const char_array_t *)a;
  const char_array_t *pb = (const char_array_t *)b;
  return strcmp(*pa,*pb);
}

static size_t char_hash(const void *a, int l, uint32_t seed)
{
  const char_array_t *pa = (const char_array_t *)a;
  const char *s = *pa;
  (void)l ;
  size_t hash = seed;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

static void
test_dict_big(unsigned long  n)
{
  HashTable *dict;
  enum cc_stat stat;
  HashTableConf htc;
  hashtable_conf_init(&htc);
  htc.hash = char_hash;
  htc.key_compare = char_equal;
  stat = hashtable_new_conf(&htc, &dict);
  if (stat != CC_OK) abort();

  char_array_t **tab = malloc (sizeof(char_array_t *)*2*n);
  if (!tab) abort();
  size_t cpt = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t *key = malloc (sizeof (char_array_t));
    char_array_t *value = malloc (sizeof (char_array_t));
    tab[cpt++] = key;
    tab[cpt++] = value;
    sprintf(*key, "%u", rand_get());
    sprintf(*value, "%u", rand_get());
    stat = hashtable_add(dict, key, value );
    if (stat != CC_OK) abort();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    void *r;
    sprintf(s1, "%u", rand_get());
    stat = hashtable_get(dict, s1, &r);
    if (stat == CC_OK)
      s ++;
  }
  g_result = s;
  hashtable_destroy(dict);
  if (cpt != 2*n) abort();
  for(size_t i = 0; i < 2*n;i++)
    free(tab[i]);
}

/********************************************************************************************/

static int cmp_float(const void * a, const void *b)
{
  const float *const*pa = a, *const*pb = b;
  return (**pa < **pb) ? -1 : (**pa > **pb);
}

static void test_sort(size_t n)
{
  Array *a1;
  enum cc_stat stat;

  stat = array_new(&a1);
  if (stat != CC_OK) abort();
  for(size_t i = 0; i < n; i++) {
    float *v = malloc(sizeof(float));
    *v = rand_get();
    stat = array_add(a1, v );
    if (stat != CC_OK) abort();
  }
  array_sort(a1, cmp_float);
  void *e;
  stat = array_get_at(a1, 0, &e);
  if (stat != CC_OK) abort();
  g_result = *(float*)e;
  array_destroy(a1);
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
  if (n == 40)
    test_function("Dict   time", 1000000, test_dict);
  if (n == 41)
    test_function("DictB  time", 1000000, test_dict_big);
  if (n == 50)
    test_function("Sort   time", 10000000, test_sort);
  exit(0);
}
