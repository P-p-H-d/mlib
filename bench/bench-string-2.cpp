/*
 * M*LIB - String Bench
 *
 * Copyright (c) 2017-2019, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string>
#include <sstream>
#include <string>
#include <vector>

#include "m-string.h"
#include "common.h"

#ifdef BENCH_CAN_USE_SDS
extern "C" {
#include "sds.h"
};
#endif

#ifdef BENCH_CAN_USE_RAPIDSTRING
#include "rapidstring.h"
#endif

/*
 * BENCH
 * 2000000
 * 1. Generate N strings of random numbers 32 bits.
 * 2. Concat all strings in a string usigned a random permutation of N.
 * 3. Replace "1234" in the string into "WELL" until nothing remains.
 * 4. Replace "56789" in the string into "DONE" until nothing remains.
 * Return the final string length.
 */

unsigned *permutation_tab = NULL;

void random_permutation(unsigned n)
{
  if (permutation_tab) free(permutation_tab);
  permutation_tab = (unsigned *) malloc(n * sizeof(unsigned));
  if (permutation_tab == NULL) abort();
  for(unsigned i = 0; i < n; i++)
    permutation_tab[i] = i;
  for(unsigned i = 0; i < n; i++) {
    unsigned j = rand_get() % n;
    unsigned k = rand_get() % n;
    unsigned l = permutation_tab[j];
    permutation_tab[j] = permutation_tab[k];
    permutation_tab[k] = l;
  }
}

using namespace std;

size_t bench_stl(unsigned n)
{
  vector<string> tab;
  tab.resize(n);
  for(unsigned i = 0; i < n; i++)
    tab[i] = to_string(rand_get());

  string str;
  for(unsigned i = 0; i < n; i++) {
    str += tab[permutation_tab[i]];
  }

  size_t pos = 0;
  do {
    pos = str.find("1234", pos);
    if (pos != string::npos) {
      str.replace (pos, 4, "WELL");
    }
  } while (pos != string::npos);

  pos = 0;
  do {
    pos = str.find("56789", pos);
    if (pos != string::npos) {
      str.replace (pos, 5, "DONE");
    }
  } while (pos != string::npos);

  return str.length();
}
                        
size_t bench_mlib(unsigned n)
{
  string_t *tab = (string_t*) malloc (n * sizeof (string_t));
  assert (tab != 0);
  // P1
  for(unsigned i = 0; i < n; i++) {
    string_init(tab[i]);
    string_printf(tab[i], "%u", rand_get());
  }
  // P2
  string_t str;
  string_init(str);
  for(unsigned i = 0; i < n; i++) {
    string_cat(str, tab[permutation_tab[i]]);
  }
  // P3
  size_t i = 0;
  do {
    i = string_replace_str(str, "1234", "WELL", i);
  } while (i != STRING_FAILURE);
  // P4
  i = 0;
  do {
    i = string_replace_str(str, "56789", "DONE", i);
  } while (i != STRING_FAILURE);
  size_t length = string_size(str);
  // Clean
  string_clear(str);
  for(unsigned i = 0; i < n; i++) {
    string_clear(tab[i]);
  }
  free(tab);
  return length;
}

#ifdef BENCH_CAN_USE_SDS
sds SDS_replace_at(sds str, size_t pos, size_t len, const char str2[])
{
  // simple implementation as replace is not available
  sds a = sdsnewlen((const void*) str, pos);
  a = sdscat(a, str2);
  a = sdscat(a, &str[pos+len]);
  sdsfree(str);
  return a;
}

size_t bench_sds(unsigned n)
{
  sds *tab = (sds*) malloc (n * sizeof (sds));
  assert (tab != 0);
  // P1
  for(unsigned i = 0; i < n; i++) {
    tab[i] =  sdsfromlonglong(rand_get());
  }
  // P2
  sds str = sdsempty();
  for(unsigned i = 0; i < n; i++) {
    str = sdscat(str, tab[permutation_tab[i]]);
  }
  // P3
  size_t pos = 0;
  do {
    char *p = strstr(&str[pos], "1234");
    pos = p == NULL ? -1U : (p - str);
    if (pos != -1U) {
      str = SDS_replace_at(str, pos, 4, "WELL");
    }
  } while (pos != -1U);
  // P4
  pos = 0;
  do {
    char *p = strstr(&str[pos], "56789");
    pos = p == NULL ? -1U : (p - str);
    if (pos != -1U) {
      str = SDS_replace_at(str, pos, 5, "DONE");
    }
  } while (pos != -1U);
  size_t length = sdslen(str);
  // Clean
  sdsfree(str);
  for(unsigned i = 0; i < n; i++) {
    sdsfree(tab[i]);
  }
  free(tab);
  return length;
}
#endif

#ifdef BENCH_CAN_USE_RAPIDSTRING
void RAPIDSTRING_replace_at(rapidstring *str, size_t pos, size_t len, const char str2[])
{
  // simple implementation as replace is not available
  rapidstring a;
  
  rs_init_w_n(&a, rs_data(str), pos);
  rs_cat(&a, str2);
  rs_cat(&a, &rs_data(str)[pos+len] );
  rs_free(str);
  *str = a;
}

size_t bench_rapidstring(unsigned n)
{
  rapidstring *tab = (rapidstring*) malloc (n * sizeof (rapidstring));
  assert (tab != 0);
  // P1
  for(unsigned i = 0; i < n; i++) {
    char tmp[10];
    sprintf(tmp, "%u", rand_get());
    rs_init_w(&tab[i], tmp);
  }
  // P2
  rapidstring str;
  rs_init(&str);
  for(unsigned i = 0; i < n; i++) {
    rs_cat_rs(&str, &tab[permutation_tab[i]]);
  }
  // P3
  size_t pos = 0;
  do {
    char *p = strstr(&rs_data(&str)[pos], "1234");
    pos = p == NULL ? -1U : (p - rs_data(&str));
    if (pos != -1U) {
      RAPIDSTRING_replace_at(&str, pos, 4, "WELL");
    }
  } while (pos != -1U);
  // P4
  pos = 0;
  do {
    char *p = strstr(&rs_data(&str)[pos], "56789");
    pos = p == NULL ? -1U : (p - rs_data(&str));
    if (pos != -1U) {
      RAPIDSTRING_replace_at(&str, pos, 5, "DONE");
    }
  } while (pos != -1U);
  size_t length = rs_len(&str);
  // Clean
  rs_free(&str);
  for(unsigned i = 0; i < n; i++) {
    rs_free(&tab[i]);
  }
  free(tab);
  return length;
}
#endif

int main(int argc, const char *argv[])
{
  unsigned length, n = atoi(argv[1]);
  unsigned select = atoi(argv[2]);
  const char *name;
  random_permutation(n);
  rand_init();
  unsigned long long t0 = cputime();

  switch (select) {
  case 0:
    length = bench_mlib(n);
    name = "MLIB";
    break;
  case 1:
    length = bench_stl(n);
    name = "STL";
    break;
#ifdef BENCH_CAN_USE_SDS
  case 2:
    length = bench_sds(n);
    name = "SDS";
    break;
#endif
#ifdef BENCH_CAN_USE_RAPIDSTRING
  case 3:
    length = bench_rapidstring(n);
    name = "RAPID";
    break;
#endif
  default:
    exit(0);
  }
  unsigned long long t1 = cputime();
  printf("%5.5s LENGTH=%u T= %f s\n", name, length, (t1-t0) / 1000000.0 );

  return 0;
}

