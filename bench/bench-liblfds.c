#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "liblfds711.h"

#include "m-mutex.h"
#define MULTI_THREAD_MEASURE
#include "common.h"


/********************************************************************************************/

struct lfds711_queue_bmm_state g_buff;
struct lfds711_queue_bmm_state g_final;

static void final(void *arg)
{
  size_t *p_n = arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
  for(int i = 0; i < n;i++) {
    int rv;
    do {
      void *p;
      rv = lfds711_queue_bmm_dequeue(&g_final, &p, NULL);
      j = (uintptr_t)p;
    } while (rv == 0);
    s += j;
  }
  g_result = s;
}

static void conso(void *arg)
{
  unsigned int j;
  size_t *p_n = arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
  for(int i = 0; i < n;i++) {
    int rv;
    do {
      void *p; 
      rv = lfds711_queue_bmm_dequeue(&g_buff, &p, NULL);
      j = (uintptr_t) p;
    } while (rv == 0);
    s += j;
  }
  int rv;
  do {
    rv = lfds711_queue_bmm_enqueue(&g_final, (void*)(uintptr_t)s, NULL);
  } while (rv == 0); 
}

static void prod(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  size_t r = n;
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
  for(unsigned int i = 0; i < n;i++) {
    int rv;
    do {
      rv = lfds711_queue_bmm_enqueue(&g_buff, (void*)(uintptr_t)r, NULL);
    } while (rv == 0);
    r = r * 31421U + 6927U;
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
  // Init
  const size_t size = m_core_roundpow2(64*cpu_count);
  struct lfds711_queue_bmm_element g_buff_elements[size];
  struct lfds711_queue_bmm_element g_final_elements[size];
  lfds711_queue_bmm_init_valid_on_current_logical_core(&g_buff, g_buff_elements,
						       size, NULL);
  lfds711_queue_bmm_init_valid_on_current_logical_core(&g_final, g_final_elements,
						       size, NULL);
  
  // Create thread
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso, &n);
  }
  size_t n2 = conso_count;
  m_thread_create(idx_final, final, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  // Clear & quit
  lfds711_queue_bmm_cleanup(&g_final, NULL);
  lfds711_queue_bmm_cleanup(&g_buff, NULL);
}


/********************************************************************************************/

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  /*  if (n == 10)
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
    test_function("Sort   time", 10000000, test_sort);*/
  if (n == 60)
    test_function("Buffer time", 1000000, test_buffer);
  exit(0);
}

