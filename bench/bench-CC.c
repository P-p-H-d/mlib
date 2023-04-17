#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"

#include "cc.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  vec( unsigned int ) a1, a2;
  init(&a1);
  init(&a2);
  
  for(size_t i = 0; i < n; i++) {
    push(&a1, rand_get() );
    push(&a2, rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *get(&a1, i ) * *get(&a2, i );
  }
  g_result = s;

  cleanup(&a1);
  cleanup(&a2);
}

/********************************************************************************************/

static void test_list(size_t n)
{
  list( unsigned int ) a1, a2;
  init(&a1);
  init(&a2);
  
  for(size_t i = 0; i < n; i++) {
    push(&a1, rand_get() );
    push(&a2, rand_get() );
  }
  unsigned int s = 0;
  for(unsigned *it1 = first( &a1 ), *it2 = first( &a2 ); it1 != end( &a1 ); it1 = next( &a1, it1 ), it2 = next(&a2, it2) ) {
    s += *it1 * *it2;
  }
  g_result = s;

  cleanup(&a1);
  cleanup(&a2);
}

/********************************************************************************************/

static void
test_dict(size_t  n)
{
  map( unsigned long, unsigned long ) dict;
  init(&dict);
  
  for (size_t i = 0; i < n; i++) {
    unsigned long value = rand_get();
    unsigned long key = rand_get();
    insert(&dict, key, value);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long key = rand_get();
    const unsigned long *val = get(&dict, key);
    if (val != NULL)
      s += *val;
  }
  g_result = s;
  
  cleanup(&dict);
}

/********************************************************************************************/
typedef struct {
  char a[256];
} char_array_t;
static inline void char_init (char_array_t *a) { a->a[0] = 0; }
static inline void char_set (char_array_t *a, const char_array_t b) { strcpy(a->a, b.a); }
static inline bool char_equal_p (const char_array_t *a, const char_array_t *b) { return strcmp(a->a,b->a)==0; }
static inline bool char_cmp (const char_array_t *a, const char_array_t *b) { return strcmp(a->a,b->a); }
static inline void char_clear (char_array_t *a) { (void)a; }
static inline size_t char_hash(const char_array_t *a) {
  size_t hash = 0;
  const char *s = a->a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}
#define CC_DTOR char_array_t, { char_clear(&val); }
#define CC_CMPR char_array_t, { return char_cmp(&val_1, &val_2); }
#define CC_HASH char_array_t, { return char_hash(&val); }
#include "cc.h"

static void
test_dict_big(size_t  n)
{
  map( char_array_t, char_array_t) dict;
  init(&dict);
  
  for (size_t i = 0; i < n; i++) {
    char_array_t s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    insert(&dict, s1, s2);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1.a, "%u", rand_get());
    const char_array_t *val = get(&dict, s1);
    if (val != NULL)
      s ++;
  }
  g_result = s;
  
  cleanup(&dict);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
};

int main(int argc, const char *argv[])
{
  test("CC", numberof(table), table, argc, argv);
  exit(0);
}
