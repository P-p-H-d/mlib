#include "m-list.h"
#include "m-array.h"
#include "m-string.h"
#include "m-algo.h"

ARRAY_DEF(int, int)
LIST_DEF(int, int)
LIST_DEF(string, string_t, STRING_OPLIST)

START_COVERAGE
ALGO_DEF(algo_array, ARRAY_OPLIST(int))

ALGO_DEF(algo_list,  LIST_OPLIST(int))

ALGO_DEF(algo_string, LIST_OPLIST(string, STRING_OPLIST))
END_COVERAGE

static void test_list(void)
{
  list_int_t l;
  list_int_init(l);
  for(int i = 0; i < 100; i++)
    list_int_push_back (l, i);
  assert( algo_list_contains(l, 62) == true);
  assert( algo_list_contains(l, -1) == false);
  
  assert( algo_list_count(l, 1) == 1);
  list_int_push_back (l, 17);
  assert( algo_list_count(l, 17) == 2);
  assert( algo_list_count(l, -1) == 0);

#define f(x) assert((x) >= 0 && (x) < 100);
  ALGO_MAP(l, LIST_OPLIST(int), f);
#define g(y, x) assert((x) >= 0 && (x) < y);
  ALGO_MAP(l, LIST_OPLIST(int), g, 100);
  
  list_int_clear(l);
}

static void test_array(void)
{
  array_int_t l;
  array_int_init(l);
  for(int i = 0; i < 100; i++)
    array_int_push_back (l, i);
  assert( algo_array_contains(l, 62) == true);
  assert( algo_array_contains(l, -1) == false);

  assert( algo_array_count(l, 1) == 1);
  array_int_push_back (l, 17);
  assert( algo_array_count(l, 17) == 2);
  assert( algo_array_count(l, -1) == 0);

  array_int_clear(l);
}

static void test_string(void)
{
  list_string_t l;
  list_string_init(l);
  string_t s;
  string_init_set_str(s, "Hello, World, John");
  algo_string_split(l, s, ',');
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, "Hello"));
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, " World"));
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, " John"));
  assert (list_string_empty_p(l));
  string_clear(s);
  
  list_string_clear(l);
}

static void test_extract(void)
{
  list_int_t l;
  list_int_init (l);
  for(int i = -100; i < 100; i++)
    list_int_push_back (l, i);
  array_int_t a;
  array_int_init(a);
#define cond(d) ((d) > 0)
  ALGO_EXTRACT(a, ARRAY_OPLIST(int), l, LIST_OPLIST(int), cond);
  assert(array_int_size(a) == 99);
#define cond2(c, d) ((d) > (c))
  ALGO_EXTRACT(a, ARRAY_OPLIST(int), l, LIST_OPLIST(int), cond2, 10);
  assert(array_int_size(a) == 89);
  array_int_clear(a);
  list_int_clear(l);
}

int main(void)
{
  test_list();
  test_array();
  test_string();
  test_extract();
  exit(0);
}
