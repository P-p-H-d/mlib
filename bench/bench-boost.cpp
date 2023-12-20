#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "m-thread.h"

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/lockfree/queue.hpp>

#include "common.h"

using namespace std;

/********************************************************************************************/

static void
test_dict2(size_t  n)
{
  boost::unordered_flat_map<unsigned long, unsigned long> dict;
  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    boost::unordered_flat_map<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it->second;
  }
  g_result = s;
}

static void
test_dict2_linear(size_t  n)
{
  boost::unordered_flat_map<unsigned long, unsigned long> dict;
  for (size_t i = 0; i < n; i++) {
    dict[i] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    boost::unordered_flat_map<unsigned long, unsigned long>::iterator it = dict.find(i);
    if (it != dict.end())
      s += it->second;
  }
  g_result = s;
}

/********************************************************************************************/
struct char_array_s {
  char a[256];
  char_array_s () { a[0] = 0 ; }
  char_array_s ( const char_array_s & other) { strcpy(a, other.a); }
  bool operator==(const char_array_s &other) const { return strcmp(a, other.a) == 0; }
};

inline std::size_t hash_value(const char_array_s &k) {
  size_t hash = 0;
  const char *s = k.a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

static void
test_dict_big(size_t  n)
{
  boost::unordered_flat_map<char_array_s, char_array_s> dict;

  for (size_t i = 0; i < n; i++) {
    char_array_s s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_s s1;
    sprintf(s1.a, "%u", rand_get());
    boost::unordered_flat_map<char_array_s, char_array_s>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/
static void
test_dict_str(size_t  n)
{
  boost::unordered_flat_map<string, string> dict;

  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    string s2 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    boost::unordered_flat_map<string, string>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/
boost::lockfree::queue<unsigned int, boost::lockfree::capacity<4*64>> g_buff;
boost::lockfree::queue<unsigned long long, boost::lockfree::capacity<4*64>> g_final;

static void final(void *arg)
{
  size_t *p_n = (size_t*) arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(unsigned int i = 0; i < n;i++) {
    while (!g_final.pop(j));
    s += j;
  }
  g_result = s;
}

static void conso(void *arg)
{
  unsigned int j;
  size_t *p_n = (size_t*)arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  for(unsigned int i = 0; i < n;i++) {
    while (!g_buff.pop(j));
    s += j;
  }
  while (!g_final.push(s));
}

static void prod(void *arg)
{
  size_t *p_n = (size_t*)arg;
  size_t n = *p_n;
  size_t r = n;
  for(unsigned int i = 0; i < n;i++) {
    while (!g_buff.push(r));
    r = r * 31421U + 6927U;
  }
}

static void test_queue(size_t n)
{
  const int cpu_count   = get_cpu_count();
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  if (cpu_count < 2) {
    fprintf(stderr, "WARNING: Can not measure Buffer performance.\n");
    return;
  }
  rand_get();
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

}


/********************************************************************************************/

const config_func_t table[] = {
  { 41,     "dictBig",   1000000, 0, test_dict_big, 0},
  { 42,    "dict",       1000000, 0, test_dict2, 0},
  { 43,    "DictStr",    1000000, 0, test_dict_str, 0},
  { 46,    "DictLinear", 1000000, 0, test_dict2_linear, 0},
  { 60,    "Queue MPMC", 1000000, 0, test_queue, 0}
};

int main(int argc, const char *argv[])
{
  test("BOOST", numberof(table), table, argc, argv);
  exit(0);
}
