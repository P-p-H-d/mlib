/*
 * Copyright (c) 2017, Patrick Pelissier
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
#include <limits.h>
#include <time.h>
#include <stdint.h>
#include "m-dict.h"
#include "m-array.h"
#include "m-string.h"

START_COVERAGE
DICT_DEF2(str, string_t, STRING_OPLIST, string_t, STRING_OPLIST)
END_COVERAGE

ARRAY_DEF(string, string_t, STRING_OPLIST)

array_string_t v;

static void init_data(int data_size)
{
  uint32_t x = 11;
  string_t s;
  string_init (s);
  for (int i = 0; i < data_size; ++i) {
    int j = (unsigned)(data_size * ((double)x / UINT_MAX) / 4) * 271828183u;
    string_printf(s, "%x", j);
    array_string_push_back (v, s);
    x = 1664525L * x + 1013904223L;
  }
  string_clear(s);
}

static void clear_data(void)
{
  array_string_clear(v);
}

static void test_data(void)
{
  dict_str_t dict;
  int data_size = (int) array_string_size(v);
    
  dict_str_init (dict);
  for (int i = 0; i < data_size; ++i) {
    dict_str_set_at(dict, *array_string_cget(v, i), *array_string_cget(v, (i+1) % data_size));
  }
  assert(dict_str_size(dict) == 1227176);

  dict_str_t dict2;
  dict_str_init (dict2);
  dict_str_swap (dict, dict2);
  assert(dict_str_size(dict) == 0);
  assert(dict_str_size(dict2) == 1227176);
  dict_str_clear (dict2);

  dict_str_clear (dict);
}

static void check_io(void)
{
  M_LET(str, STRING_OPLIST)
    M_LET(dict, DICT_OPLIST(str)) {
    dict_str_get_str(str, dict, false);
    assert (string_cmp_str (str, "{}") == 0);
    dict_str_set_at (dict, STRING_CTE("LICENCE"), STRING_CTE("BSD3"));
    dict_str_get_str(str, dict, false);
    assert (string_cmp_str (str, "{\"LICENCE\":\"BSD3\"}") == 0);
    dict_str_set_at (dict, STRING_CTE("AUTHOR"), STRING_CTE("PP"));
    dict_str_get_str(str, dict, false);
    //NOTE: order is dependent on the hash system.
    assert (string_cmp_str (str, "{\"LICENCE\":\"BSD3\",\"AUTHOR\":\"PP\"}") == 0);
    FILE *f = fopen ("a.dat", "wt");
    if (!f) abort();
    dict_str_out_str(f, dict);
    fclose (f);
    f = fopen ("a.dat", "rt");
    if (!f) abort();
    M_LET(dict2, DICT_OPLIST(str)) {
      bool b = dict_str_in_str(dict2, f);
      assert (b == true);
      /* assert (dict_str_equal_p (dict, dict2)); */
    }
    fclose (f);
  }
}

int main(void)
{
  dict_str_t dict;
  string_t key, ref;
  
  dict_str_init(dict);
  string_init(key);
  string_init(ref);

  FILE *f = fopen("dict.txt", "rt");
  if (!f) abort();
  dict_str_set_at(dict, STRING_CTE("LICENCE"), STRING_CTE("BSD3"));
  while (!feof(f) && !ferror(f)) {
    string_fgets(key, f, STRING_READ_PURE_LINE);
    size_t idx = string_search_char(key, ':');
    string_set(ref, key);
    string_right(ref, idx+1);
    string_left(key, idx);
    dict_str_set_at (dict, key, ref);
  }
  fclose(f);
  assert(dict_str_size(dict) == 19);
  
  f = fopen("dict.txt", "rt");
  if (!f) abort();
  while (!feof(f) && !ferror(f)) {
    string_fgets(key, f, STRING_READ_PURE_LINE);
    size_t idx = string_search_char(key, ':');
    string_set(ref, key);
    string_right(ref, idx+1);
    string_left(key, idx);
    string_t *ptr = dict_str_get (dict, key);
    assert(ptr != NULL);
    assert(string_equal_p (*ptr, ref) || string_cmp_str(ref, "lisez-moi") == 0);
  }
  fclose(f);

  dict_str_set_at(dict, STRING_CTE("LICENCE"), STRING_CTE("BSD3"));
  assert(dict_str_size(dict) == 19);
  dict_str_set_at(dict, STRING_CTE("LICENCE"), STRING_CTE("BSD3"));
  assert(dict_str_size(dict) == 19);

  size_t s = 0;
  bool check1 = false, check2= false;
  for M_EACH(r, dict, DICT_OPLIST(str)) {
    if (string_cmp_str((*r)->key, "README") == 0)
      check1 = true;
    if (string_cmp_str((*r)->value, "BSD3") == 0)
      check2 = true;
    //printf ("key=%s value=%s\n", string_get_cstr((*r)->key), string_get_cstr((*r)->value));
    s++;
  }
  assert(s == 19);
  assert(check1);
  assert(check2);
  string_clear(key);
  string_clear(ref);
  dict_str_clear(dict);

  check_io();
  
  init_data(5000000);
  test_data();
  clear_data();
  exit(0);
}

/*
  [ key1: value1,
  key2: value2,
  key3: value3]

  a;b;"c;e";d;e;f

  TOKEN: 
  * separator ; : , AND .
  * ignore ' ' '\n'
  * remove blanck
  * handle " or ' or ( ) or [ ] 
  * recursive
 */
