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
#include "mympz.h"
#include "m-string.h"
#include "m-list.h"

#include "coverage.h"
START_COVERAGE
LIST_DEF(list_uint, unsigned int)
LIST_DEF(list_mpz, mpz_t, MYMPZ_OPLIST)
LIST_DUAL_PUSH_DEF(list2_double, double) 
END_COVERAGE
LIST_DUAL_PUSH_DEF(list2_mpz, mpz_t, MYMPZ_OPLIST)
#define LIST_UINT_OPLIST LIST_OPLIST(list_uint)

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

  list_uint_it_t u;
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
  list_uint_it_t u2;
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

  unsigned int *p = list_uint_push_new(v);
  assert (p != NULL);
  assert (*p == 0);
  *p=17421742;
  assert (*list_uint_back(v) == 17421742);

  size_t s1 = list_uint_size(v);
  size_t s2 = list_uint_size(v2);
  list_uint_swap(v, v2);
  assert (list_uint_size(v) == s2);
  assert (list_uint_size(v2) == s1);

  list_uint_it_end(u, v2);
  assert(list_uint_end_p(u));
  list_uint_it_set(u2, u);
  assert(list_uint_end_p(u2));
  assert(list_uint_it_equal_p(u, u2));
  assert(list_uint_last_p(u2));
  s1 = list_uint_size(v2);
  for(list_uint_it(u, v2), s2 = 0; !list_uint_last_p(u); list_uint_next(u)) {s2++;}
  assert(!list_uint_end_p(u));
  assert (!list_uint_it_equal_p(u, u2));
  list_uint_next(u);
  assert(list_uint_end_p(u));
  assert (s1 == s2 + 1);
  
  list_uint_clear(v);
  list_uint_init_set(v, v2);
  assert(list_uint_equal_p(v,v2));

  list_uint_move (v, v2);
  list_uint_init_move (v2, v);
  list_uint_init_set(v, v2);
  assert(list_uint_equal_p(v,v2));
  assert(list_uint_equal_p(v,v));

  list_uint_clean(v);
  list_uint_push_back (v2, 14562398);
  list_uint_it(u, v2);
  s2 = list_uint_size(v2);
  list_uint_splice_back(v, v2, u);
  assert (list_uint_size(v2) == s2 - 1);
  assert (list_uint_size(v) == 1);
  assert (*list_uint_back(v) == 14562398);
  list_uint_next(u);
  list_uint_splice_back(v, v2, u);
  assert (list_uint_size(v2) == s2 - 2);
  assert (list_uint_size(v) == 2);
  assert (!list_uint_equal_p(v,v2));

  assert (list_uint_hash(v) != 0);
  
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

  list_mpz_it_t u;
  mpz_set_ui (s, 0);
  for(list_mpz_it(u, v); !list_mpz_end_p(u); list_mpz_next(u)) {
    mpz_add (s, s, *list_mpz_cref(u));
  }
  assert (mpz_get_ui(s) == 1000*999/2);

  FILE *f = fopen ("a-mlist.dat", "wt");
  if (!f) abort();
  list_mpz_out_str(f, v);
  fclose (f);

  list_mpz_t list2;
  list_mpz_init(list2);
  f = fopen ("a-mlist.dat", "rt");
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

static void test_dual_push1(void)
{
  list2_double_t list;
  double d;
  
  list2_double_init(list);
  assert(list2_double_empty_p(list));
  assert(list2_double_size(list) == 0);

  list2_double_push_back(list, 2.0);
  assert(!list2_double_empty_p(list));
  assert (*list2_double_back(list) == 2.0);
  assert (*list2_double_front(list) == 2.0);
  list2_double_pop_back(&d, list);
  assert (d == 2.0);
  assert(list2_double_empty_p(list));
  assert(list2_double_size(list) == 0);

  list2_double_push_front(list, 3.0);
  assert (*list2_double_back(list) == 3.0);
  assert (*list2_double_front(list) == 3.0);
  assert(!list2_double_empty_p(list));
  list2_double_pop_back(&d, list);
  assert (d == 3.0);
  assert(list2_double_empty_p(list));
  assert(list2_double_size(list) == 0);
  
  list2_double_push_back(list, 2.0);
  list2_double_push_back(list, 3.0);
  list2_double_push_front(list, 1.0);
  list2_double_push_front(list, 0.0);
  assert (*list2_double_back(list) == 3.0);
  assert (*list2_double_front(list) == 0.0);
  list2_double_pop_back(&d, list);
  assert (d == 3.0);
  list2_double_pop_back(&d, list);
  assert (d == 2.0);
  list2_double_pop_back(&d, list);
  assert (d == 1.0);
  list2_double_pop_back(&d, list);
  assert (d == 0.0);
  assert(list2_double_empty_p(list));

  list2_double_push_back_new(list);
  list2_double_pop_back(&d, list);
  assert (d == 0.0);
  list2_double_push_front_new(list);
  list2_double_pop_back(&d, list);
  assert (d == 0.0);

  list2_double_push_back_new(list);
  list2_double_pop_back(NULL, list);
  assert(list2_double_empty_p(list));
  list2_double_push_front_new(list);
  list2_double_pop_back(NULL, list);
  assert(list2_double_empty_p(list));

  for(double e = 0.0; e < 1024.0; e += 1.0) {
    if (e < 1000.0)
      list2_double_push_front(list, e);
    if (e >= 24.0 && e < 1000.0) {
      assert (list2_double_size (list) == 25);
    }
    if (e >= 24.0) {
      list2_double_pop_back(&d, list);
      assert (d + 24.0 == e);
    }
  }

  list2_double_push_back(list, 15);
  list2_double_t list2;
  list2_double_init(list2);
  assert(list2_double_empty_p(list2));
  assert(!list2_double_empty_p(list));
  list2_double_swap(list, list2);
  assert(!list2_double_empty_p(list2));
  assert(list2_double_empty_p(list));
  list2_double_clear(list2);

  for(double e = 0.0; e < 1024.0; e += 1.0) {
    list2_double_push_back(list, e);
  }
  list2_double_init_set (list2, list);
  assert(list2_double_equal_p(list, list2));

  list2_double_reverse(list2);
  for(double e = 0.0; e < 1024.0 ; e += 1.0) {
    list2_double_pop_back(&d, list2);
    assert (d == e);
  }
  assert(list2_double_empty_p(list2));

  list2_double_splice(list2, list);
  assert(list2_double_size(list2) == 1024);
  assert(list2_double_size(list) == 0);
  list2_double_set(list, list2);
  assert(list2_double_size(list) == 1024);
  list2_double_splice(list2, list);
  assert(list2_double_size(list2) == 2048);
  assert(list2_double_size(list) == 0);
  
  list2_double_move(list, list2);
  list2_double_clear(list);
}


int main(void)
{
  test_uint();
  test_mpz();
  test_dual_push1();
  exit(0);
}

