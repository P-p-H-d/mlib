#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "dynamic.h"

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  vector a1, a2;

  vector_construct(&a1, sizeof(unsigned int));
  vector_construct(&a2, sizeof(unsigned int));
  for(size_t i = 0; i < n; i++) {
    unsigned int v = rand_get();
    vector_push_back(&a1, &v);
    v = rand_get();
    vector_push_back(&a2, &v);
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *(unsigned int*) vector_at(&a1, i) * *(unsigned int*) vector_at(&a2, i);
  }
  g_result = s;
  vector_destruct(&a1, NULL);
  vector_destruct(&a2, NULL);
}


/********************************************************************************************/

static size_t hash_func (void *a)
{
  return hash_data(a, sizeof (unsigned long));
}

static int equal_func(void *a, void *b)
{
  const unsigned long *pa = a;
  const unsigned long *pb = b;
  return *pa == *pb;
}

static void set_func(void *a, void *b)
{
  unsigned long *pa = a;
  const unsigned long *pb = b;
  *pa = *pb;
}

typedef struct map_element map_element;
struct map_element
{
  unsigned long key;
  unsigned long value;
};

static void
test_dict(size_t  n)
{
  map dict;
  map_construct(&dict, sizeof(map_element), (map_element[]) {{.key = -1}}, set_func);

  for (size_t i = 0; i < n; i++) {
    map_insert(&dict, (map_element[]) {{.key = rand_get(), .value = rand_get()}}, hash_func, equal_func, set_func, NULL);
  }    
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    map_element *p = map_at(&dict, (map_element[]){{.key = rand_get()}}, hash_func, equal_func);
    if (p)
      s += p->value;
  }
  g_result = s;
  map_destruct(&dict, NULL, NULL);
}

/********************************************************************************************/

typedef char char_array_t[256];

static int char_equal (void *a, void *b)
{
  const char_array_t *pa = (const char_array_t *)a;
  const char_array_t *pb = (const char_array_t *)b;
  return strcmp(*pa,*pb)==0;
}

static size_t char_hash(void *a)
{  
  char_array_t *pa = (char_array_t *)a;
  return hash_string(*pa);
}

static void char_set(void *a, void *b)
{
  char_array_t *pa = (char_array_t *)a;
  const char_array_t *pb = (const char_array_t *)b;
  strcpy(*pa,*pb);
}

typedef struct big_map_element big_map_element;
struct big_map_element
{
  char_array_t key;
  char_array_t value;
};

static void
test_dict_big(size_t  n)
{
  map dict;
  big_map_element empty;
  strcpy(empty.key, "");
  strcpy(empty.value, "");
  map_construct(&dict, sizeof(big_map_element), &empty, char_set);
  
  for (size_t i = 0; i < n; i++) {
    big_map_element el;
    sprintf(el.key, "%u", rand_get());
    sprintf(el.value, "%u", rand_get());
    map_insert(&dict, &el, char_hash, char_equal, char_set, NULL);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    big_map_element el;
    sprintf(el.key, "%u", rand_get());
    big_map_element *p = map_at(&dict, &el, char_hash, char_equal);
    if (p)
      s ++;
  }
  g_result = s;
  map_destruct(&dict, NULL, NULL);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
};

int main(int argc, const char *argv[])
{
  test("LIBDYNAMIC", numberof(table), table, argc, argv);
  exit(0);
 }
