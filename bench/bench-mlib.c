#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#define NDEBUG

#include "m-array.h"
#include "m-list.h"
#include "m-rbtree.h"
#include "m-dict.h"

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

ARRAY_DEF(uint, unsigned int)

static void test_array(size_t n)
{
  rand_init();
  M_LET(a1, a2, ARRAY_OPLIST(uint)) {
    for(size_t i = 0; i < n; i++) {
      array_uint_push_back(a1, rand_get() );
      array_uint_push_back(a2, rand_get() );
    }
    unsigned int s = 0;
    for(unsigned long i = 0; i < n; i++) {
      s += *array_uint_cget(a1, i ) * *array_uint_cget(a2, i );
    }
    g_result = s;
  }
}

/********************************************************************************************/

LIST_DEF(uint, unsigned int)

static void test_list (size_t n)
{
  rand_init();
  M_LET(a1, a2, LIST_OPLIST(uint)) {
    for(size_t i = 0; i < n; i++) {
      list_uint_push_back(a1, rand_get() );
      list_uint_push_back(a2, rand_get() );
    }
    unsigned int s = 0;
    list_it_uint_t it1, it2;
    for(list_uint_it(it1, a1), list_uint_it(it2,a2); !list_uint_end_p(it1); list_uint_next(it1), list_uint_next(it2)) {
      s += *list_uint_cref(it1) * *list_uint_cref(it2);
    }
    g_result = s;
  }
}

/********************************************************************************************/

RBTREE_DEF(ulong, unsigned long)

static void test_rbtree(size_t n)
{
  rand_init();
  M_LET(tree, RBTREE_OPLIST(ulong)) {
    for (size_t i = 0; i < n; i++) {
      rbtree_ulong_push(tree, rand_get());
    }
    
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = rbtree_ulong_get(tree, rand_get());
      if (p)
        s += *p;
    }
    g_result = s;
  }
}

/********************************************************************************************/

DICT_DEF2(ulong, unsigned long, M_DEFAULT_OPLIST, unsigned long, M_DEFAULT_OPLIST)

static void
test_dict(unsigned long  n)
{
  rand_init();
  M_LET(dict, DICT_OPLIST(ulong)) {
    for (size_t i = 0; i < n; i++) {
      dict_ulong_set_at(dict, rand_get(), rand_get() );
    }
    
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = dict_ulong_get(dict, rand_get());
      if (p)
        s += *p;
    }
    g_result = s;
  }
}

/********************************************************************************************/

typedef char char_array_t[256];

static void char_init (char_array_t a) { a[0] = 0; }
static void char_set (char_array_t a, const char_array_t b) { strcpy(a, b); }
static bool char_equal_p (const char_array_t a, const char_array_t b) { return strcmp(a,b)==0; }
static size_t char_hash(const char_array_t a) {
  M_HASH_DECL(hash);
  const char *s = a;
  while (*s) M_HASH_UP(hash, *s++);
  return hash;
}
// NOTE: Can't set OPLIST as a macro!
#define CHAR_OPLIST (INIT(char_init), INIT_SET(char_set), SET(char_set), CLEAR(char_init), HASH(char_hash), EQUAL(char_equal_p))

DICT_DEF2(char, char_array_t, CHAR_OPLIST, char_array_t, CHAR_OPLIST)

static void
test_dict_big(unsigned long  n)
{
  rand_init();
  M_LET(dict, DICT_OPLIST(char)) {
    for (size_t i = 0; i < n; i++) {
      char_array_t s1, s2;
      sprintf(s1, "%u", rand_get());
      sprintf(s2, "%u", rand_get());
      dict_char_set_at(dict, s1, s2);
    }
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      char_array_t s1;
      sprintf(s1, "%u", rand_get());
      char_array_t *p = dict_char_get(dict, s1);
      if (p)
        s ++;
    }
    g_result = s;
  }
}

/********************************************************************************************/

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  if (n == 1)
    test_function("List   time",10000000, test_list);
  if (n == 2)
    test_function("Array  time", 100000000, test_array);
  if (n == 3)
    test_function("Rbtree time", 1000000, test_rbtree);
  if (n == 4)
    test_function("Dict   time", 1000000, test_dict);
  if (n == 6)
    test_function("DictB  time", 1000000, test_dict_big);
}

