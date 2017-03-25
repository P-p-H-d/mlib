#define NDEBUG

#include <QLinkedList>
#include <QVector>
#include <QSet>
#include <QMap>
#include <QHash>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

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

static void test_list(size_t n)
{
  rand_init();
  QLinkedList<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for (QLinkedList<unsigned int>::const_iterator ci1 = a1.begin(), ci2 = a2.begin (); ci1 != a1.end(); ++ci1, ++ci2) {
    s += *ci1 * *ci2;
  }
}

/********************************************************************************************/

static void test_array(size_t n)
{
  rand_init();
  QVector<unsigned int> a1, a2;
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

/* NOTE: QSet doesn't perform full ordering. I can't find any container
   which is a set with total ordering within Qt. */
#if 0
static void test_rbtree(size_t n)
{
  rand_init();
  QSet<unsigned long> tree;

  for (size_t i = 0; i < n; i++) {
    tree.insert(rand_get());
  }
    
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    QSet<unsigned long>::iterator it = tree.find(rand_get());
    if (it != tree.end())
      s += *it;
  }
}
#endif

/********************************************************************************************/

static void
test_dict1(unsigned long  n)
{
  rand_init();
  QMap<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
    
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    QMap<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it.value();
  }
}

/********************************************************************************************/

static void
test_dict2(unsigned long  n)
{
  rand_init();
  QHash<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
    
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    QHash<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it.value();
  }
}

/********************************************************************************************/
struct char_array_s {
  char a[256];
  char_array_s () { a[0] = 0 ; }
  char_array_s ( const char_array_s & other) { strcpy(a, other.a); }
  bool operator==(const char_array_s &other) const { return strcmp(a, other.a) == 0; }
};

uint qHash(const char_array_s &k)
{
  size_t hash = 0;
  const char *s = k.a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
};

static void
test_dict_big(unsigned long  n)
{
  rand_init();
  QHash<char_array_s, char_array_s> dict;

  for (size_t i = 0; i < n; i++) {
    char_array_s s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    dict[s1] = s2;
  }

  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_s s1;
    sprintf(s1.a, "%u", rand_get());
    QHash<char_array_s, char_array_s>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
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
  /* if (n == 3)
     test_function("Rbtree time", 1000000, test_rbtree); */
  if (n == 4)
    test_function("Dict(m)time", 1000000, test_dict1);
  if (n == 5)
    test_function("Dict(u)time", 1000000, test_dict2);
  if (n == 6)
    test_function("DictB  time", 1000000, test_dict_big);
}
