#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "dynamic.h"

unsigned long g_result;

static unsigned long long
cputime (void)
{
  struct rusage rus;
  getrusage (0, &rus);
  return rus.ru_utime.tv_sec * 1000000ULL + rus.ru_utime.tv_usec;
}

static unsigned int randValue = 0;
static void rand_init(void)
{
  randValue = 0;
}
static unsigned int rand_get(void)
{
  randValue = randValue * 31421U + 6927U;
  return randValue;
}

static void test_function(const char str[], size_t n, void (*func)(size_t))
{
  unsigned long long start, end;
  //  (*func)(n);
  start = cputime();
  (*func)(n);
  end = cputime();
  end = (end - start) / 1000U;
  printf ("%s %Lu ms for n = %lu\n", str, end, n);
}

/********************************************************************************************/

static void test_array(size_t n)
{
  rand_init();
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
  vector_destruct(&a1);
  vector_destruct(&a2);
}


/********************************************************************************************/

static size_t hash_func (map *m, void *a)
{
  return hash_data(a, sizeof (unsigned long));
}

static int equal_func(map *m, void *a, void *b)
{
  const unsigned long *pa = a;
  const unsigned long *pb = b;
  return *pa == *pb;
}

static void set_func(map *m, void *a, void *b)
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
test_dict(unsigned long  n)
{
  rand_init();
  map dict;
  map_construct(&dict, sizeof(map_element), (map_element[]) {{.key = -1}}, set_func);

  for (size_t i = 0; i < n; i++) {
    map_insert(&dict, (map_element[]) {{.key = rand_get(), .value = rand_get()}}, hash_func, equal_func, set_func, NULL);
  }    
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

static int char_equal (map *m, void *a, void *b)
{
  const char_array_t *pa = (const char_array_t *)a;
  const char_array_t *pb = (const char_array_t *)b;
  return strcmp(*pa,*pb)==0;
}

static size_t char_hash(map *m, void *a)
{  
  char_array_t *pa = (char_array_t *)a;
  return hash_string(*pa);
}

static void char_set(map *m, void *a, void *b)
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
test_dict_big(unsigned long  n)
{
  rand_init();
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

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  if (n == 2)
    test_function("Array  time", 100000000, test_array);
  if (n == 4)
    test_function("Dict   time", 1000000, test_dict);
  if (n == 6)
    test_function("Dict B time", 1000000, test_dict_big);
 }
