#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "m-array.h"
#include "m-list.h"
#include "m-rbtree.h"
#include "m-dict.h"
#include "m-algo.h"
#include "m-mempool.h"
#include "m-string.h"
#include "m-buffer.h"
#include "m-mutex.h"

#include "common.h"

/********************************************************************************************/

ARRAY_DEF(uint, unsigned int)

static void test_array(size_t n)
{
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

// Two benched mode: with or without mempool
#ifdef USE_MEMPOOL
LIST_DEF(list_uint, unsigned int, (MEMPOOL( list_mpool), MEMPOOL_LINKAGE(static)))
#else
LIST_DEF(list_uint, unsigned int)
#endif

static void test_list (size_t n)
{
#ifdef USE_MEMPOOL
  mempool_list_uint_init(list_mpool);
#endif
  M_LET(a1, a2, LIST_OPLIST(list_uint)) {
    for(size_t i = 0; i < n; i++) {
      list_uint_push_back(a1, rand_get() );
      list_uint_push_back(a2, rand_get() );
    }
    unsigned int s = 0;
    list_uint_it_t it1, it2;
    for(list_uint_it(it1, a1), list_uint_it(it2,a2); !list_uint_end_p(it1); list_uint_next(it1), list_uint_next(it2)) {
      s += *list_uint_cref(it1) * *list_uint_cref(it2);
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  mempool_list_uint_clear(list_mpool);
#endif
}

/********************************************************************************************/

#ifdef USE_MEMPOOL
RBTREE_DEF(rbtree_ulong, unsigned long, (MEMPOOL( rbtree_mpool), MEMPOOL_LINKAGE(static)))
#else
RBTREE_DEF(rbtree_ulong, unsigned long)
#endif

static void test_rbtree(size_t n)
{
#ifdef USE_MEMPOOL
  mempool_rbtree_ulong_init(rbtree_mpool);
#endif
  M_LET(tree, RBTREE_OPLIST(rbtree_ulong)) {
    for (size_t i = 0; i < n; i++) {
      rbtree_ulong_push(tree, rand_get());
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = rbtree_ulong_get(tree, rand_get());
      if (p)
        s += *p;
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  mempool_rbtree_ulong_clear(rbtree_mpool);
#endif
}

/********************************************************************************************/

#ifdef USE_MEMPOOL
DICT_DEF2(ulong, unsigned long, M_OPEXTEND(M_DEFAULT_OPLIST, MEMPOOL(dict_mpool), MEMPOOL_LINKAGE(static)), unsigned long, M_DEFAULT_OPLIST)
#else
DICT_DEF2(ulong, unsigned long, M_DEFAULT_OPLIST, unsigned long, M_DEFAULT_OPLIST)
#endif

static void
test_dict(unsigned long  n)
{
#ifdef USE_MEMPOOL
  mempool_list_dict_pair_ulong_init(dict_mpool);
#endif
  M_LET(dict, DICT_OPLIST(ulong)) {
    for (size_t i = 0; i < n; i++) {
      unsigned long key = rand_get();
      dict_ulong_set_at(dict, key, rand_get() );
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = dict_ulong_get(dict, rand_get());
      if (p)
        s += *p;
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  mempool_list_dict_pair_ulong_clear(dict_mpool);
#endif
}

/********************************************************************************************/

static inline bool oor_equal_p(unsigned long k, unsigned char n)
{
  return k == (unsigned long)n;
}

static inline void oor_set(unsigned long *k, unsigned char n)
{
  *k = (unsigned long)n;
}

DICT_OA_DEF2(oa_ulong, unsigned long, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)), unsigned long, M_DEFAULT_OPLIST)

static void
test_dict_oa(unsigned long  n)
{
  M_LET(dict, DICT_OPLIST(oa_ulong)) {
    for (size_t i = 0; i < n; i++) {
      dict_oa_ulong_set_at(dict, rand_get(), rand_get() );
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = dict_oa_ulong_get(dict, rand_get());
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
static size_t char_hash(const char_array_t a) { return m_core_hash (a, strlen(a)); }

// NOTE: Can't use the name OPLIST as a macro!
#define CHAR_OPLIST (INIT(char_init), INIT_SET(char_set), SET(char_set), CLEAR(char_init), HASH(char_hash), EQUAL(char_equal_p))

#ifdef USE_MEMPOOL
DICT_DEF2(char, char_array_t, M_OPEXTEND(CHAR_OPLIST,MEMPOOL(dict_mpool2), MEMPOOL_LINKAGE(static)), char_array_t, CHAR_OPLIST)
#else
DICT_DEF2(char, char_array_t, CHAR_OPLIST, char_array_t, CHAR_OPLIST)
#endif

static void
test_dict_big(unsigned long  n)
{
#ifdef USE_MEMPOOL
  mempool_list_dict_pair_char_init(dict_mpool2);
#endif
  M_LET(dict, DICT_OPLIST(char)) {
    for (size_t i = 0; i < n; i++) {
      char_array_t s1, s2;
      sprintf(s1, "%u", rand_get());
      sprintf(s2, "%u", rand_get());
      dict_char_set_at(dict, s1, s2);
    }
    rand_init();
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
#ifdef USE_MEMPOOL
  mempool_list_dict_pair_char_clear(dict_mpool2);
#endif
}

/********************************************************************************************/

#ifdef USE_MEMPOOL
DICT_DEF2(str, string_t, M_OPEXTEND(STRING_OPLIST, MEMPOOL(dict_mpool3), MEMPOOL_LINKAGE(static)), string_t, STRING_OPLIST)
#else
DICT_DEF2(str, string_t, STRING_OPLIST, string_t, STRING_OPLIST)
#endif

static void
test_dict_str(unsigned long  n)
{
#ifdef USE_MEMPOOL
  mempool_list_dict_pair_str_init(dict_mpool3);
#endif
  M_LET(s1, s2, STRING_OPLIST)
  M_LET(dict, DICT_OPLIST(str)) {
    for (size_t i = 0; i < n; i++) {
      string_printf(s1, "%u", rand_get());
      string_printf(s2, "%u", rand_get());
      dict_str_set_at(dict, s1, s2);
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      string_printf(s1, "%u", rand_get());
      string_t *p = dict_str_get(dict, s1);
      if (p)
        s ++;
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  mempool_list_dict_pair_str_clear(dict_mpool3);
#endif
}

/********************************************************************************************/

ARRAY_DEF(float, float)
ALGO_DEF(afloat, ARRAY_OPLIST(float))

static void test_sort(size_t n)
{
  M_LET(a1, ARRAY_OPLIST(float)) {
    for(size_t i = 0; i < n; i++) {
      array_float_push_back(a1, rand_get() );
    }
    afloat_sort(a1);
    g_result = *array_float_get(a1, 0);
  }
}

/********************************************************************************************/

//NOTE: not sure this test is representative.

BUFFER_DEF(buffer_uint, unsigned int, 0, BUFFER_QUEUE|BUFFER_BLOCKING)
buffer_uint_t g_buff;

static void conso(void *arg)
{
  unsigned int j;
  size_t *p_n = arg;
  size_t n = *p_n;
  for(int i = 0; i < n;i++) {
    buffer_uint_pop(&j, g_buff);
    if (j >= n) abort();
  }
}

static void prod(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  for(unsigned int i = 0; i < n;i++) {
    buffer_uint_push(g_buff, i);
  }
}

static void test_buffer(size_t n)
{
  const int cpu_count   = get_cpu_count();
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  if (cpu_count < 2) {
    fprintf(stderr, "WARNING: Can not measure Buffer performance.\n");
    return;
  }
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  buffer_uint_init(g_buff, 64*cpu_count);
  
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso, &n);
  }
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }

  buffer_uint_clear(g_buff);
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
  if (n == 42)
    test_function("DictOA time", 1000000, test_dict_oa);
  if (n == 41)
    test_function("DictB  time", 1000000, test_dict_big);
  if (n == 43)
    test_function("DictS  time", 1000000, test_dict_str);
  if (n == 50)
    test_function("Sort   time", 10000000, test_sort);
  if (n == 60)
    test_function("Buffer time", 1000000, test_buffer);
  exit(0);
}

