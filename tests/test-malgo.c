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

  int *p = algo_list_min(l);
  assert(p != NULL && *p == 0);
  p = algo_list_max(l);
  assert(p != NULL && *p == 99);
  int *p2;
  algo_list_minmax(&p, &p2, l);
  assert(p != NULL && *p == 0);
  assert(p2 != NULL && *p2 == 99);
  
  list_int_push_back (l, 3);
  list_it_int_t it1, it2;
  algo_list_find (it1, l, 3);
  assert (!list_int_end_p (it1));
  algo_list_find_last (it2, l, 3);
  assert (!list_int_end_p (it2));
  assert (!list_int_it_equal_p(it1, it2));
  
  for(int i = -100; i < 100; i+=2)
    list_int_push_back (l, i);

  assert (!algo_list_sort_p(l));
  algo_list_sort(l);
  assert (algo_list_sort_p(l));
  
  list_int_clear(l);

  ALGO_INIT_VA(l, LIST_OPLIST(int), 1, 2, 3, 4, 5);
  assert (list_int_size(l) == 5);
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

  ALGO_INIT_VA(l, ARRAY_OPLIST(int), 1, 2, 3, 4, 5);
  assert (array_int_size(l) == 5);
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

  list_string_push_back (l, STRING_CTE("John"));
  list_string_push_back (l, STRING_CTE("Who"));
  list_string_push_back (l, STRING_CTE("Is"));
  algo_string_join(s, l, STRING_CTE("-"));
  // NOTE: List is reverse... Is-it really ok?
  assert (string_equal_str_p(s, "Is-Who-John"));

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

  int dst = 0;
#define inc(d, c) (d) += (c)
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(int), inc);
  assert (dst == 100*99/2-10*11/2);
#define sqr(d, c) (d) = (c)*(c)
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(int), inc, sqr);
  assert (dst == 327965);
#define sqr2(d, f, c) (d) = (f) * (c)
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(int), inc, sqr2, 4);
  assert (dst == (100*99/2-10*11/2) *4 );

  ALGO_REDUCE(dst, a, ARRAY_OPLIST(int), sum);
  assert (dst == 100*99/2-10*11/2);
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(int), add);
  assert (dst == 100*99/2-10*11/2);
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(int), and);
  assert (dst == 0);
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(int), or);
  assert (dst == 127);

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
