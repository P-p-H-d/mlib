#include <vector>
#include <list>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#define NDEBUG

using namespace std;

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
  return (int) randValue;
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

static void test_list(size_t n)
{
  rand_init();
  list<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for (list<unsigned int>::const_iterator ci1 = a1.begin(), ci2 = a2.begin (); ci1 != a1.end(); ++ci1, ++ci2) {
    s += *ci1 * *ci2;
  }
}

/********************************************************************************************/

static void test_array(size_t n)
{
  rand_init();
  vector<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += a1[i] * a2[i];
  }
}

/********************************************************************************************/

static void test_rbtree(size_t n)
{
  rand_init();
  set<unsigned long> tree;

  for (size_t i = 0; i < n; i++) {
    tree.insert(rand_get());
  }
    
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    set<unsigned long>::iterator it = tree.find(rand_get());
    if (it != tree.end())
      s += *it;
  }
}

/********************************************************************************************/

static void
test_dict1(unsigned long  n)
{
  rand_init();
  map<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
    
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    map<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it->second;
  }
}

/********************************************************************************************/

static void
test_dict2(unsigned long  n)
{
  rand_init();
  unordered_map<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
    
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unordered_map<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it->second;
  }
}

/********************************************************************************************/

int main(int argc, const char *argv[])
{
  test_function("List   time",10000000, test_list);
  test_function("Array  time", 100000000, test_array);
  test_function("Rbtree time", 1000000, test_rbtree);
  test_function("Dict(m)time", 1000000, test_dict1);
  test_function("Dict(u)time", 1000000, test_dict2);
}
