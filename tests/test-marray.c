/*
 * Copyright (c) 2017-2018, Patrick Pelissier
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
#include <stdio.h>
#include <gmp.h>
#include "m-array.h"

static void my_mpz_out_str(FILE *f, const mpz_t z)
{
  mpz_out_str (f, 10, z);
}

static bool my_mpz_in_str(mpz_t z, FILE *f)
{
  return mpz_inp_str (z, f, 10) > 0;
}

static bool my_mpz_equal_p(const mpz_t z1, const mpz_t z2)
{
  return mpz_cmp (z1, z2) == 0;
}

#include "m-string.h"
static void my_mpz_str(string_t str, const mpz_t z, bool append)
{
  char *s = mpz_get_str(NULL, 10, z);
  assert (s != NULL);
  if (append) string_cat_str (str, s);
  else        string_set_str (str, s);
  free(s);
}

#include "coverage.h"
START_COVERAGE
ARRAY_DEF(array_uint, unsigned int)
ARRAY_DEF(array_mpz, mpz_t,
          (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), \
           OUT_STR(my_mpz_out_str), IN_STR(my_mpz_in_str), EQUAL(my_mpz_equal_p), \
           GET_STR(my_mpz_str) ))
END_COVERAGE
#define ARRAY_UINT_OPLIST ARRAY_OPLIST(array_uint)

static void test_uint(void)
{
  array_uint_t v;
  array_uint_init(v);

  assert (array_uint_empty_p(v) == true);

  array_uint_push_back(v, 2);
  assert (array_uint_empty_p(v) == false);
  unsigned int x;
  array_uint_pop_back(&x, v);
  assert( x == 2);

  for(unsigned int i = 0; i < 100; i++)
    array_uint_push_back(v, i);

  unsigned int s = 0;
  for(size_t i = 0; i < array_uint_size(v) ; i++)
    s += *array_uint_cget(v, i);
  assert( s == 100 * 99 /2);

  s = 0;
  for M_EACH(item, v, ARRAY_UINT_OPLIST) {
    s += *item;
  }
  assert( s == 100 * 99 /2);

  array_uint_pop_at(&s, v, 49);
  assert (s == 49);
  array_uint_push_at(v, 49, 49);
  
  array_uint_pop_at (NULL, v, 0);
  assert(array_uint_size(v) == 99);
  s= 0;
  for(size_t i = 0; i < array_uint_size(v) ; i++)
    s += *array_uint_cget(v, i);
  assert( s == 100 * 99 /2);

  unsigned int *p = array_uint_get(v, 1);
  *p += 999;
  s= 0;
  for(size_t i = 0; i < array_uint_size(v) ; i++)
    s += *array_uint_cget(v, i);
  assert( s == 100 * 99 /2 + 999);

  array_uint_swap_at(v, 2, 3);
  assert(*array_uint_cget(v, 2) == 4);
  assert(*array_uint_cget(v, 3) == 3);

  array_uint_reserve(v, 0);
  assert(array_uint_capacity(v) == array_uint_size(v));

  const unsigned int *src = array_uint_cget(v, 4);
  assert (*src == 5);

  array_uint_remove_v(v, 1, array_uint_size(v) -1);
  assert(array_uint_size(v) == 2);
  assert(*array_uint_cget(v, 0) == 1);
  assert(*array_uint_cget(v, 1) == 99);

  for(unsigned int i = 3; i < 100; i++)
    array_uint_push_back(v, i);

  array_uint_resize(v, 10);
  assert(array_uint_size(v) == 10);
  assert(*array_uint_cget(v, 9) == 10);
  array_uint_resize(v, 1000);
  assert(array_uint_size(v) == 1000);
  assert(*array_uint_cget(v, 999) == 0);

  array_uint_t v2, v3;
  array_uint_init_set(v2,v);
  array_uint_init(v3);
  array_uint_push_back(v3, 459320);
  array_uint_set(v3, v);
  assert (array_uint_equal_p(v3, v));
  array_uint_push_back(v3, 459320);  
  array_uint_set(v3, v);
  assert (array_uint_equal_p(v3, v));
  array_uint_clear(v);
  array_uint_clear(v3);
  assert(array_uint_size(v2) == 1000);
  assert(*array_uint_cget(v2, 999) == 0);
  array_uint_push_at (v2, 0, 1742);
  assert (array_uint_size(v2) == 1001);
  assert(*array_uint_cget(v2, 0) == 1742);
  assert(*array_uint_cget(v2, 1000) == 0);
  
  array_uint_insert_v (v2, 500, 4);
  assert (array_uint_size(v2) == 1005);
  assert(*array_uint_cget(v2, 0) == 1742);
  assert(*array_uint_cget(v2, 500) == 0);
  assert(array_uint_size(v2) == 1005);
  assert(*array_uint_cget(v2, 1004) == 0);

  array_uint_init(v);
  array_uint_swap(v,v2);
  assert(array_uint_size(v) == 1005);
  assert(array_uint_size(v2) == 0);

  array_uint_reserve(v, 100);
  assert(array_uint_size(v) == 1005);
  assert(array_uint_capacity(v) == 1005);
  array_uint_reserve(v, 10000);
  assert(array_uint_size(v) == 1005);
  assert(array_uint_capacity(v) == 10000);

  array_uint_set (v, v);
  array_uint_set (v2, v);
  assert(array_uint_size(v2) == 1005);

  array_uint_clean(v);
  assert (array_uint_capacity(v) > 0);
  array_uint_reserve(v, 0);
  assert (array_uint_capacity(v) == 0);
  
  array_uint_clear(v);
  array_uint_clear(v2);
}

static void test_mpz(void)
{
  array_mpz_t array1, array2;
  mpz_t z;
  
  array_mpz_init (array1);
  array_mpz_init (array2);
  mpz_init (z);

  // Test empty
  FILE *f = fopen ("a-marray.dat", "wt");
  if (!f) abort();
  array_mpz_out_str(f, array1);
  fclose (f);

  f = fopen ("a-marray.dat", "rt");
  if (!f) abort();
  bool b = array_mpz_in_str (array2, f);
  assert (b == true);
  assert (array_mpz_equal_p (array1, array2));
  fclose(f);
  
  // Test non empty
  for(int n = 0; n < 1000; n++) {
    mpz_set_ui (z, n);
    array_mpz_push_back (array1, z);
  }
  
  f = fopen ("a-marray.dat", "wt");
  if (!f) abort();
  array_mpz_out_str(f, array1);
  fclose (f);

  f = fopen ("a-marray.dat", "rt");
  if (!f) abort();
  b = array_mpz_in_str (array2, f);
  assert (b == true);
  assert (array_mpz_equal_p (array1, array2));
  fclose(f);

  array_mpz_clean(array1);

  M_LET(str, STRING_OPLIST) {
    array_mpz_get_str(str, array1, false);
    assert (string_cmp_str (str, "[]") == 0);
    mpz_set_ui (z, 17);
    array_mpz_push_back (array1, z);
    array_mpz_get_str(str, array1, false);
    assert (string_cmp_str (str, "[17]") == 0);
    mpz_set_ui (z, 42);
    array_mpz_push_back (array1, z);
    array_mpz_get_str(str, array1, true);
    assert (string_cmp_str (str, "[17][17,42]") == 0);
    // NOTE: Not the same order than list...
    }

  mpz_clear (z);
  array_mpz_clear(array2);
  array_mpz_clear(array1);
}

static void test_d(void)
{
  array_uint_t a1, a2;
  
  array_uint_init(a1);
  array_uint_init_move(a2, a1);
  assert (array_uint_empty_p (a2));
  array_uint_init(a1);
  array_uint_move(a1, a2);
  assert (array_uint_empty_p (a1));
  for(int i = 0; i < 10; i++)
    array_uint_push_back (a1, i);
  array_uint_set_at (a1, 0, 17);
  assert (*array_uint_get (a1, 0) == 17);
  assert (*array_uint_back(a1) == 9);
  unsigned int *p = array_uint_push_new(a1);
  assert (*p == 0);
  *p = 10;
  assert (*array_uint_back(a1) == 10);
  for(int i = 0; i < 10; i++)
    array_uint_push_at (a1, 9, i);
  assert (*array_uint_back(a1) == 10);
  for(int i = 9; i < 19; i++)
    assert (*array_uint_get (a1, i) == 18U-i);

  array_uint_set_at2(a1, 100, 100);
  assert (*array_uint_back(a1) == 100);
  assert (array_uint_size(a1) == 101);
  array_uint_pop_back (NULL, a1);
  assert (*array_uint_back(a1) == 0);
  assert (array_uint_size(a1) == 100);

  array_uint_insert_v (a1, 10, 200);
  assert (array_uint_size(a1) == 300);
  for(int i = 10; i < 210; i++) {
    assert (*array_uint_get (a1, i) == 0);
  }
  array_uint_it_t it;
  array_uint_it(it, a1);
  array_uint_remove (a1, it);
  assert (array_uint_size(a1) == 299);

  array_uint_it_last (it, a1);
  assert (array_uint_last_p(it));
  array_uint_it_end (it, a1);
  assert (array_uint_end_p(it));
  array_uint_previous (it);
  assert (array_uint_last_p(it));
  array_uint_it_t it2;
  array_uint_it_set (it2, it);
  assert (array_uint_it_equal_p(it2, it));
  assert (*array_uint_ref(it2) == 0);
  array_uint_insert(a1, it2, 17);
  assert (array_uint_size(a1) == 300);
  assert (*array_uint_ref(it2) == 0);
  array_uint_next(it2);
  assert (*array_uint_ref(it2) == 17);
  
  array_uint_init_set (a2, a1);
  assert (array_uint_equal_p (a2, a1));

  size_t h = array_uint_hash(a2);
  assert (h != 0);

  array_uint_clean(a1);
  array_uint_it(it, a1);
  assert (array_uint_end_p (it));
  assert (array_uint_last_p (it));
  
  array_uint_clear(a1);
  array_uint_clear(a2);
}

int main(void)
{
  test_uint();
  test_mpz();
  test_d();
  exit(0);
}
