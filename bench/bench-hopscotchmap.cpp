#define NDEBUG

#include <vector>
#include <list>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "src/hopscotch_map.h"

using namespace tsl;
using namespace std;

#include "common.h"

/********************************************************************************************/

static void
test_dict2(size_t  n)
{
  tsl::hopscotch_map<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    hopscotch_map<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
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

namespace std {
  template <> struct hash<char_array_s> {
    std::size_t operator()(const char_array_s &k) const {
      size_t hash = 0;
      const char *s = k.a;
      while (*s) hash = hash * 31421 + (*s++) + 6927;
      return hash;
    };
  };
}

static void
test_dict_big(size_t  n)
{
  hopscotch_map<char_array_s, char_array_s> dict;

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
    hopscotch_map<char_array_s, char_array_s>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/
static void
test_dict_str(size_t  n)
{
  hopscotch_map<string, string> dict;

  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    string s2 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    hopscotch_map<string, string>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/

int main(int argc, const char *argv[])
{
  int n = (argc > 1) ? atoi(argv[1]) : 0;
  if (n == 42)
    test_function("Dict(u)time", 1000000, test_dict2);
  if (n == 41)
    test_function("DictB  time", 1000000, test_dict_big);
  if (n == 43)
    test_function("DictS(u)  time", 1000000, test_dict_str);
  exit(0);
}
