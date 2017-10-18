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

static inline bool oor_equal_p(int k, unsigned char n)
{
  return k == (int)-n-1;
}
static inline void oor_set(int *k, unsigned char n)
{
  *k = (int)-n-1;
}

#include "coverage.h"
START_COVERAGE
DICT_DEF2(dict_str, string_t, STRING_OPLIST, string_t, STRING_OPLIST)
DICT_OA_DEF2(dict_oa_int, int, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)), int, M_DEFAULT_OPLIST)
END_COVERAGE

DICT_SET_DEF2(dict_setstr, string_t, STRING_OPLIST)

/* Helper structure */
ARRAY_DEF(array_string, string_t, STRING_OPLIST)
array_string_t v_str;

static void init_data(int data_size)
{
  uint32_t x = 11;
  string_t s;
  string_init (s);
  array_string_init(v_str);
  for (int i = 0; i < data_size; ++i) {
    int j = (unsigned)(data_size * ((double)x / UINT_MAX) / 4) * 271828183u;
    string_printf(s, "%x", j);
    array_string_push_back (v_str, s);
    x = 1664525L * x + 1013904223L;
  }
  string_clear(s);
}

static void clear_data(void)
{
  array_string_clear(v_str);
}

static void test_data(void)
{
  dict_str_t dict;
  int data_size = (int) array_string_size(v_str);
    
  dict_str_init (dict);
  for (int i = 0; i < data_size; ++i) {
    dict_str_set_at(dict, *array_string_cget(v_str, i), *array_string_cget(v_str, (i+1) % data_size));
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
    M_LET(dict, DICT_OPLIST(dict_str)) {
    dict_str_get_str(str, dict, false);
    assert (string_cmp_str (str, "{}") == 0);
    dict_str_set_at (dict, STRING_CTE("LICENCE"), STRING_CTE("BSD3"));
    dict_str_get_str(str, dict, false);
    assert (string_cmp_str (str, "{\"LICENCE\":\"BSD3\"}") == 0);
    dict_str_set_at (dict, STRING_CTE("AUTHOR"), STRING_CTE("PP"));
    dict_str_get_str(str, dict, false);
    //NOTE: order is dependent on the hash system.
    assert (string_cmp_str (str, "{\"LICENCE\":\"BSD3\",\"AUTHOR\":\"PP\"}") == 0 || string_cmp_str (str, "{\"AUTHOR\":\"PP\",\"LICENCE\":\"BSD3\"}") == 0);
    FILE *f = fopen ("a.dat", "wt");
    if (!f) abort();
    dict_str_out_str(f, dict);
    fclose (f);
    f = fopen ("a.dat", "rt");
    if (!f) abort();
    M_LET(dict2, DICT_OPLIST(dict_str)) {
      bool b = dict_str_in_str(dict2, f);
      assert (b == true);
      assert (dict_str_equal_p (dict, dict2));
    }
    fclose (f);
  }
}

static void test_set(void)
{
  M_LET(str, STRING_OPLIST)
    M_LET(set, DICT_SET_OPLIST(dict_setstr, STRING_OPLIST)) {
    for(int i = 0; i < 100; i++) {
      string_printf(str, "%d", i);
      dict_setstr_set_at(set, str);
    }
    for(int i = 0; i < 100; i++) {
      string_printf(str, "%d", i);
      string_t *p = dict_setstr_get(set, str);
      assert(string_equal_p (*p, str));
    }
  }
}

static void test_init(void)
{
  M_LET(str1, str2, STRING_OPLIST)
    M_LET(d1, d2, DICT_OPLIST(dict_str, STRING_OPLIST, STRING_OPLIST)){
    for(size_t i = 0; i < 100; i++) {
      string_printf(str1, "%d", 2*i);
      string_printf(str2, "%d", 2*i+1);
      dict_str_set_at (d1, str1, str2);
    }
    assert (dict_str_size (d1) == 100);
    dict_str_t d3;
    dict_str_init_set (d3, d1);
    assert (dict_str_equal_p (d3, d1));
    dict_str_set (d2, d1);
    assert (dict_str_equal_p (d2, d1));
    assert (dict_str_equal_p (d2, d3));
    dict_str_clear (d3);

    dict_str_set_at (d1, STRING_CTE("x"), STRING_CTE("y"));
    assert (dict_str_size (d1) == 101);
    assert (!dict_str_equal_p (d2, d1));
    bool b = dict_str_remove (d1, STRING_CTE("0"));
    assert (dict_str_size (d1) == 100);
    assert (b);
    assert (!dict_str_equal_p (d2, d1));

    for(size_t i = 1; i < 100; i++) {
      string_printf(str1, "%d", 2*i);
      b = dict_str_remove (d1, str1);
      assert (b);
    }
    assert (dict_str_size (d1) == 1);
    dict_str_swap (d1, d2);
    assert (dict_str_size (d1) == 100);
    assert (dict_str_size (d2) == 1);
    assert (string_equal_str_p (*dict_str_get (d2, STRING_CTE("x")), "y"));
    assert (dict_str_get (d2, STRING_CTE("y")) == NULL);

    dict_str_init_move (d3, d1);
    assert (dict_str_size (d3) == 100);
    for(size_t i = 0; i < 100; i++) {
      string_printf(str1, "%d", 2*i);
      string_printf(str2, "%d", 2*i+1);
      assert (string_equal_p (*dict_str_get (d3, str1), str2));
    }
    dict_str_init_set (d1, d3);
    assert (dict_str_size (d1) == 100);
    dict_str_move (d2, d3);
    assert (dict_str_size (d2) == 100);
    dict_str_clean (d2);
    assert (dict_str_size (d2) == 0);

    //assert (dict_str_hash (d2) != 0);
  }
}

// this test generates dict equal but with different physical representation
static void test_equal(void)
{
  for(int n = 10 ; n < 500; n+=2) {
    M_LET(str1, str2, string_t)
      M_LET(d1, d2, DICT_OPLIST(dict_str, STRING_OPLIST, STRING_OPLIST)){
      for(int i = 0 ; i < n; i++) {
        string_printf(str1, "%d", i);
        dict_str_set_at(d1, str1, str1);
      }
      for(int i = 0 ; i < n; i++) {
        string_printf(str1, "%d", i);
        dict_str_set_at(d2, str1, str1);
        string_printf(str1, "%d", n-1-i);
        dict_str_remove(d1, str1);
        assert(dict_str_equal_p(d1, d2) == ((i+1) == (n+1)/2));
        assert(dict_str_equal_p(d2, d1) == ((i+1) == (n+1)/2));
      }
    }
  }
}

static void test1(void)
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
  for M_EACH(r, dict, DICT_OPLIST(dict_str)) {
      if (string_cmp_str(r->key, "README") == 0)
      check1 = true;
    if (string_cmp_str(r->value, "BSD3") == 0)
      check2 = true;
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
}

static void test_oa(void)
{
  dict_oa_int_t d;
  dict_oa_int_init(d);

  for(size_t i = 0 ; i < 150; i+= 3)
    dict_oa_int_set_at(d, i, i*i);
  assert(dict_oa_int_size(d) == 50);
  for(size_t i = 0 ; i < 150; i++) {
    int *p = dict_oa_int_get(d, i);
    if ((i % 3) == 0) {
      assert (p != NULL);
      assert (*p == M_ASSIGN_CAST(int, i*i));
    } else {
      assert (p == NULL);
    }
  }

  dict_oa_int_clear(d);
}

static void test_init_oa(void)
{
  M_LET(d1, d2, DICT_OPLIST(dict_oa_int, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST)){
    for(size_t i = 0; i < 100; i++) {
      dict_oa_int_set_at (d1, 2*i, 2*i+1);
    }
    assert (dict_oa_int_size (d1) == 100);
    dict_oa_int_set_at (d1, 17, 42);
    dict_oa_int_remove (d1, 17);
    dict_oa_int_t d3;
    dict_oa_int_init_set (d3, d1);
    //assert (dict_oa_int_equal_p (d3, d1));
    dict_oa_int_set (d2, d1);

    assert (dict_oa_int_get (d2, -10) == NULL);
    assert (*dict_oa_int_get (d2, 10) == 11);
    //assert (dict_oa_int_equal_p (d2, d1));
    //assert (dict_oa_int_equal_p (d2, d3));
    dict_oa_int_clear (d3);

    dict_oa_int_set_at (d1, -10, -20);
    assert (dict_oa_int_size (d1) == 101);
    assert (*dict_oa_int_get (d1, -10) == -20);
    dict_oa_int_set_at (d1, -10, -22);
    assert (dict_oa_int_size (d1) == 101);
    assert (*dict_oa_int_get (d1, -10) == -22);

    //assert (!dict_oa_int_equal_p (d2, d1));
    bool b = dict_oa_int_remove (d1, 0);
    assert (dict_oa_int_size (d1) == 100);
    assert (b);
    //assert (!dict_oa_int_equal_p (d2, d1));

    for(size_t i = 1; i < 100; i++) {
      b = dict_oa_int_remove (d1, 2*i);
      assert (b);
    }
    assert (dict_oa_int_size (d1) == 1);
    dict_oa_int_swap (d1, d2);
    assert (dict_oa_int_size (d1) == 100);
    assert (dict_oa_int_size (d2) == 1);
    assert (*dict_oa_int_get (d2, -10) == -22);
    assert (dict_oa_int_get (d2, -20) == NULL);

    dict_oa_int_init_move (d3, d1);
    assert (dict_oa_int_size (d3) == 100);
    for(size_t i = 0; i < 100; i++) {
      assert (*dict_oa_int_get (d3, 2*i) == 2*(int)i+1);
    }
    dict_oa_int_init_set (d1, d3);
    assert (dict_oa_int_size (d1) == 100);
    dict_oa_int_move (d2, d3);
    assert (dict_oa_int_size (d2) == 100);
    dict_oa_int_clean (d2);
    assert (dict_oa_int_size (d2) == 0);

    //assert (dict_oa_int_hash (d2) != 0);
  }
}

static void test_it_oa(void)
{
  M_LET(d1, DICT_OPLIST(dict_oa_int, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST)){
    for(size_t i = 0; i < 100; i++) {
      dict_oa_int_set_at (d1, 2*i, 2*i+1);
    }
    assert (dict_oa_int_size (d1) == 100);

    dict_oa_int_it_t it;
    size_t s = 0;
    for(dict_oa_int_it(it, d1); !dict_oa_int_end_p(it); dict_oa_int_next(it)) {
      dict_oa_int_pair_t *pair = dict_oa_int_ref(it);
      assert (pair->key >= 0 && pair->key < 200);
      assert (pair->value == pair->key + 1);
      s++;
    }
    assert (s == 100);

    dict_oa_int_it_end(it, d1);
    assert (dict_oa_int_end_p(it));
    assert (dict_oa_int_last_p(it));
    dict_oa_int_previous(it);
    assert (!dict_oa_int_end_p(it));
    assert (dict_oa_int_last_p(it));
    dict_oa_int_it_t it2;
    dict_oa_int_it_last(it2, d1);
    assert (dict_oa_int_it_equal_p(it2, it));
    dict_oa_int_it(it2, d1);
    dict_oa_int_it_set(it2, it);
    assert (dict_oa_int_it_equal_p(it2, it));
    dict_oa_int_previous(it2);
    assert (!dict_oa_int_it_equal_p(it2, it));
    dict_oa_int_next(it2);
    assert (dict_oa_int_it_equal_p(it2, it));

    s = 0;
    for(dict_oa_int_it_last(it, d1); !dict_oa_int_end_p(it); dict_oa_int_previous(it)) {
      dict_oa_int_pair_t *pair = dict_oa_int_ref(it);
      assert (pair->key >= 0 && pair->key < 200);
      assert (pair->value == pair->key + 1);
      s++;
    }
    assert (s == 100);
  }
}

int main(void)
{
  test1();
  test_set();
  test_init();
  test_equal();
  test_oa();
  test_init_oa();
  test_it_oa();
  exit(0);
}
