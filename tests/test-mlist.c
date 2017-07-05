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
#include <stdio.h>
#include <gmp.h> // For testing purpose only.
#include "m-string.h"
#include "m-list.h"

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

static void my_mpz_str(string_t str, const mpz_t z, bool append)
{
  char *s = mpz_get_str(NULL, 10, z);
  if (append) string_cat_str (str, s);
  else        string_set_str (str, s);
  free(s);
}

START_COVERAGE
LIST_DEF(uint, unsigned int)
LIST_DEF(mpz, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), \
                      OUT_STR(my_mpz_out_str), IN_STR(my_mpz_in_str), EQUAL(my_mpz_equal_p), \
                      GET_STR(my_mpz_str)))
END_COVERAGE
#define LIST_UINT_OPLIST LIST_OPLIST(uint)

static void test_uint(void)
{
  list_uint_t v;
  list_uint_init(v);

  assert (list_uint_empty_p(v) == true);

  list_uint_push_back(v, 2);
  assert (list_uint_empty_p(v) == false);
  unsigned int x ;
  list_uint_pop_back(&x, v);
  assert( x == 2);
  assert (list_uint_empty_p(v) == true);
  assert (list_uint_size(v) == 0);

  for(unsigned int i = 0; i < 1000; i ++) {
    list_uint_push_back(v, i);
  }
  assert (list_uint_size(v) == 1000);
  assert (*list_uint_cget(v, 500) == 500);
  assert (*list_uint_cget(v, 999) == 999);

  list_it_uint_t u;
  unsigned int s = 0;
  for(list_uint_it(u, v); !list_uint_end_p(u); list_uint_next(u)) {
    s += *list_uint_cref(u);
  }
  assert (s == 1000*999/2);
  s = 0;
  for M_EACH(item, v, LIST_UINT_OPLIST) {
    s += *item;
  }
  assert (s == 1000*999/2);

  list_uint_insert(v, u, 1996); // Insert in back since u is empty
  assert (list_uint_size(v) == 1001);
  list_uint_it(u, v);
  list_uint_insert(v, u, 1997); // Insert in second place
  assert (list_uint_size(v) == 1002);
  list_uint_pop_back(&x, v);
  assert (x == 1996);
  list_uint_pop_back(&x, v);
  assert (x == 1997);
  assert (list_uint_size(v) == 1000);

  list_uint_push_back (v, x);
  list_uint_pop_back (NULL, v);
  
  list_uint_t v2;
  list_it_uint_t u2;
  list_uint_init(v2);
  list_uint_set(v, v);
  list_uint_set(v2,v);
  for(list_uint_it(u, v), list_uint_it(u2, v2); !list_uint_end_p(u); list_uint_next(u), list_uint_next(u2)) {
    assert(list_uint_end_p(u2) == false);
    assert(*list_uint_cref(u) == *list_uint_cref(u2));
  }

  list_uint_it(u2, v2);   //v2 is the first el
  assert (*list_uint_cref(u2) == 999);
  list_uint_next(u2);  //v2 is the second el
  assert (*list_uint_cref(u2) == 998);
  list_uint_remove(v2, u2);// the second el is deleted
  list_uint_it(u2, v2);   //v2 is the first el
  assert (*list_uint_cref(u2) == 999);
  list_uint_next(u2);  //v2 is the second el
  assert (*list_uint_cref(u2) == 997);
  
  list_uint_it(u2, v2);   //v2 is the first el
  list_uint_remove(v2, u2);// the second el is deleted
  list_uint_it(u2, v2);   //v2 is the first el
  assert (*list_uint_cref(u2) == 997);

  list_uint_clean(v);
  assert (list_uint_empty_p(v) == true);
  assert (list_uint_empty_p(v2) == false);
  assert (list_uint_size(v2) == 998);

  list_uint_set (v, v2);
  list_uint_splice (v, v2);
  assert (list_uint_empty_p(v) == false);
  assert (list_uint_empty_p(v2) == true);
  assert (list_uint_size(v) == 998*2);

  list_uint_reverse(v2);
  assert (list_uint_empty_p(v2) == true);
  list_uint_push_back(v2, 12);
  list_uint_push_back(v2, 17);
  list_uint_reverse(v2);
  list_uint_pop_back(&x, v2);
  assert (x == 12);
  list_uint_pop_back(&x, v2);
  assert (x == 17);
  assert (list_uint_empty_p(v2) == true);

  list_uint_clear(v);
  list_uint_clear(v2);
}

static void test_mpz(void)
{
  list_mpz_t v;
  mpz_t z, x, s;

  mpz_init (z);
  mpz_init (x);
  mpz_init (s);

  list_mpz_init(v);
  assert (list_mpz_empty_p(v) == true);

  mpz_set_ui (z, 2);
  list_mpz_push_back(v, z);
  assert (list_mpz_empty_p(v) == false);

  list_mpz_pop_back(&x, v);
  assert( mpz_cmp (x, z) == 0);
  assert (list_mpz_empty_p(v) == true);
  assert (list_mpz_size(v) == 0);

  for(unsigned int i = 0; i < 1000; i ++) {
    mpz_set_ui (z, i);
    list_mpz_push_back(v, z);
  }
  assert (list_mpz_size(v) == 1000);

  list_it_mpz_t u;
  mpz_set_ui (s, 0);
  for(list_mpz_it(u, v); !list_mpz_end_p(u); list_mpz_next(u)) {
    mpz_add (s, s, *list_mpz_cref(u));
  }
  assert (mpz_get_ui(s) == 1000*999/2);

  FILE *f = fopen ("a.dat", "wt");
  if (!f) abort();
  list_mpz_out_str(f, v);
  fclose (f);

  list_mpz_t list2;
  list_mpz_init(list2);
  f = fopen ("a.dat", "rt");
  bool b = list_mpz_in_str (list2, f);
  assert (b == true);
  fclose(f);
  assert (list_mpz_equal_p (v, list2));
  list_mpz_clear(list2);

  list_mpz_clean(v);
  M_LET(str, STRING_OPLIST) {
    list_mpz_get_str(str, v, false);
    assert (string_cmp_str (str, "[]") == 0);
    mpz_set_ui (z, 17);
    list_mpz_push_back (v, z);
    list_mpz_get_str(str, v, false);
    assert (string_cmp_str (str, "[17]") == 0);
    mpz_set_ui (z, 42);
    list_mpz_push_back (v, z);
    list_mpz_get_str(str, v, true);
    assert (string_cmp_str (str, "[17][42,17]") == 0);
  }
  
  list_mpz_clear(v);

  mpz_clear (z);
  mpz_clear (x);
  mpz_clear (s);
}

int main(void)
{
  test_uint();
  test_mpz();
  exit(0);
}
