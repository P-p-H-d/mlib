/*
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
#include <stdio.h>
#include "test-obj.h"

#include "m-string.h"
#include "m-deque.h"

#include "coverage.h"

START_COVERAGE
DEQUE_DEF(deque, int)
END_COVERAGE
DEQUE_DEF(deque_mpz, testobj_t, TESTOBJ_OPLIST)

// Deque with the minimum number of methods.
DEQUE_DEF(deque_min_z, testobj_t, (INIT_SET(testobj_init_set), SET(testobj_set), CLEAR(testobj_clear)))

#define OPL DEQUE_OPLIST(deque)

static void test_ti1(int n)
{
  deque_t d;

  deque_init(d);
  assert(deque_capacity(d) == DEQUEUI_DEFAULT_SIZE);
  
  for(int i =0; i < n ;i++) {
    deque_push_back(d, i);
    assert(*deque_back(d) == i);
    deque_push_front(d, i);
    assert(*deque_front(d) == i);
  }
  assert (!deque_empty_p(d));
  assert (deque_size(d) == (size_t)2*n);
  int s = 0;
  for(int i =0; i < 2*n ;i++) {
    int z;
    if ((i % 3) == 0) {
      deque_pop_front(&z, d);
      assert (z > *deque_front(d) );
    } else {
      deque_pop_back(&z, d);
    }
    s += z;
  }
  assert (s == n*(n-1) );
  assert (deque_empty_p(d));
  assert (deque_size(d) == 0);
  
  deque_clear(d);
}

static void test1(void)
{
  deque_t d;
  int j;

  deque_init(d);
  assert (deque_empty_p(d));
  assert (deque_size(d) == 0);
  deque_push_back_new(d);
  assert (!deque_empty_p(d));
  assert (deque_size(d) == 1);
  assert (*deque_back(d) == 0);
  *deque_back(d) = -1;
  deque_push_front_new(d);
  assert (!deque_empty_p(d));
  assert (deque_size(d) == 2);
  assert (*deque_front(d) == 0);
  *deque_front(d) = 1;
  assert (*deque_back(d) == -1);
  assert (*deque_cget(d, 0) == 1);
  assert (*deque_cget(d, 1) == -1);
  for(int i = -2; i > -100; i--)  {
    deque_push_back(d, i);
    assert (*deque_back(d) == i);
  }
  for(int i = 2; i < 100; i++) {
    deque_push_front(d, i);
    assert (*deque_front(d) == i);
  }
  assert (deque_size(d) == 99*2);
  for(int i = 0; i < 99*2; i++)
    assert (*deque_get(d, i) == (i < 99) ? i - 99 : i - 98);
  for(int i = 0; i < 98; i++)  {
    deque_pop_back(NULL, d);
    assert (*deque_back(d) == i - 98);
    deque_pop_front(NULL, d);
    assert (*deque_front(d) == 98 - i);
  }
  deque_pop_back(&j, d);
  assert (j == -1);
  deque_pop_front(&j, d);
  assert (j == 1);

  assert (deque_empty_p(d));
  assert (deque_size(d) == 0);
  deque_push_back(d, 1914);
  deque_push_front(d, 1918);
  assert (*deque_get(d, 0) == 1918);
  assert (*deque_get(d, 1) == 1914);

  deque_clear(d);
}

static void test_it(void)
{
  M_LET(d, OPL) {
    for(size_t i = 0; i < 1997; i ++) {
      deque_push_back(d, 1997+i);
      deque_push_front(d, 1996-i);
      deque_it_t it;
      deque_it_last (it, d);
      assert (deque_last_p(it));
      assert (*deque_cref (it) == (int)(1997+i));
      deque_it(it, d);
      assert (*deque_cref (it) == (int)(1996-i));
    }
    int s = 0;
    for M_EACH(it, d, OPL) {
	assert (*it == s++);
    }
    deque_it_t it;
    deque_it(it, d);
    assert (!deque_end_p(it));
    assert (!deque_last_p(it));
    assert (*deque_cref(it) == 0);
    deque_it_last(it, d);
    assert (!deque_end_p(it));
    assert (deque_last_p(it));
    assert (*deque_ref(it) == 1997+1996);
    deque_next(it);
    assert (deque_last_p(it));
    assert (deque_end_p(it));
    deque_it_last(it, d);
    while (!deque_end_p(it)) {
      deque_previous(it);
      assert (deque_end_p(it) || !deque_last_p(it));
    }
    deque_previous(it);
    assert (deque_end_p(it));
    deque_it_end(it, d);
    assert (deque_end_p(it));
    assert (deque_last_p(it));
    deque_it_t it2;
    deque_it_set(it2, it);
    assert (deque_end_p(it2));
    assert (deque_it_equal_p(it, it2));
    for(size_t i = 1996; i < 1997; i --) {
      int z;
      deque_pop_back(&z, d);
      assert (z == 1997+(int)i);
      deque_it_last (it, d);
      assert (deque_last_p(it));
      assert (deque_end_p(it) || *deque_cref (it) == (int)(1996+i));
    }
  }
  M_LET(d, OPL) {
    deque_it_t it;
    deque_it_last (it, d);
    assert(deque_last_p(it));
    assert(deque_end_p(it));
  }
}

static void test_set(void)
{
  M_LET(e, d, OPL) {
    assert (deque_equal_p(d, e));
    for(size_t i = 0; i < 1997; i ++)
      deque_push_back(d, i);
    assert (!deque_equal_p(d, e));
    deque_set(e, d);
    deque_set (e, e);
    assert (deque_size(e) == deque_size (d));
    deque_it_t it1, it2;
    for(deque_it(it1, e), deque_it (it2, d) ;
	!deque_end_p (it1) && !deque_end_p (it2) ;
	deque_next(it1) , deque_next (it2)) {
      assert (*deque_ref(it1) == *deque_ref(it2));
    }
    assert (deque_end_p (it1));
    assert (deque_end_p (it2));
    assert (deque_equal_p(d, e));
    assert (deque_hash(d) != 0);
    deque_clean (e);
    assert (deque_size (d) == 1997);
    assert (deque_size (e) == 0);
    deque_swap(d, e);
    assert (deque_size (d) == 0);
    assert (deque_size (e) == 1997);
    deque_set_at (e, 0, 2000);
    assert (*deque_front(e) == 2000);
    deque_set_at (e, deque_size(e)-1, 3000);
    assert (*deque_back(e) == 3000);
    deque_clear(d);
    deque_init_move (d, e);
    assert (*deque_back(d) == 3000);
    assert (*deque_front(d) == 2000);
    assert (deque_size (d) == 1997);
    deque_push_back(d, 10000);
    deque_push_front(d, -10000);
    assert (deque_size (d) == 1999);
    for(size_t i = 0; i < 1999; i ++)
      deque_pop_back(NULL, d);
    for(size_t i = 0; i < 3001; i ++)
      deque_push_back(d, i);
    deque_init (e);
    deque_move (e, d);
    deque_init (d);
    assert (*deque_back(e) == 3000);
  }
}

// This test showed a very bad behavior in the memory allocator
// deque will continously allocate a new space whereas at most only
// one item is in the container!
// This is still quite slow.
static void test_advance(void)
{
  deque_t d;
  int j;
  deque_init(d);
  for(int i = 0; i < 100000000; i ++) {
    deque_push_back(d, i);
    deque_pop_front(&j, d);
    assert (i == j);
  }
  deque_clear(d);
}

static void test_z(void)
{
  M_LET(d, DEQUE_OPLIST(deque_mpz, TESTOBJ_OPLIST))
  M_LET(z, TESTOBJ_OPLIST) {
    testobj_set_ui(z, 1742);
    deque_mpz_push_back_move(d, &z);
    assert (testobj_cmp_ui (*deque_mpz_back(d), 1742) == 0);
    testobj_init(z);
    testobj_set_ui(z, 4217);
    deque_mpz_push_front_move(d, &z);
    assert (testobj_cmp_ui (*deque_mpz_front(d), 4217) == 0);
    deque_mpz_pop_back_move(&z, d);
    assert (testobj_cmp_ui (z, 1742) == 0);
    testobj_clear(z);
    deque_mpz_pop_front_move(&z, d);
    assert (testobj_cmp_ui (z, 4217) == 0);
  }

}

static void test_io(void)
{
  M_LET(d1, d2, DEQUE_OPLIST(deque_mpz, TESTOBJ_OPLIST))
  M_LET(z, TESTOBJ_OPLIST)
  M_LET(str, STRING_OPLIST) {
    // Test empty
    FILE *f = fopen ("a-marray.dat", "wt");
    if (!f) abort();
    deque_mpz_out_str(f, d1);
    fclose (f);

    f = fopen ("a-marray.dat", "rt");
    if (!f) abort();
    bool b = deque_mpz_in_str (d2, f);
    assert (b == true);
    assert (deque_mpz_equal_p (d1, d2));
    fclose(f);
  
    // Test non empty
    for(int n = 0; n < 1000; n++) {
      testobj_set_ui (z, n);
      deque_mpz_push_back (d1, z);
    }
  
    f = fopen ("a-marray.dat", "wt");
    if (!f) abort();
    deque_mpz_out_str(f, d1);
    fclose (f);
    
    f = fopen ("a-marray.dat", "rt");
    if (!f) abort();
    b = deque_mpz_in_str (d2, f);
    assert (b == true);
    assert (deque_mpz_equal_p (d1, d2));
    fclose(f);
    
    deque_mpz_clean(d1);

    deque_mpz_get_str(str, d1, false);
    assert (string_cmp_str (str, "[]") == 0);
    const char *sp;
    b = deque_mpz_parse_str(d2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(deque_mpz_equal_p(d1, d2));

    testobj_set_ui (z, 17);
    deque_mpz_push_back (d1, z);
    deque_mpz_get_str(str, d1, false);
    assert (string_cmp_str (str, "[17]") == 0);
    b = deque_mpz_parse_str(d2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(deque_mpz_equal_p(d1, d2));

    testobj_set_ui (z, 42);
    deque_mpz_push_back (d1, z);
    deque_mpz_get_str(str, d1, true);
    assert (string_cmp_str (str, "[17][17,42]") == 0);
    b = deque_mpz_parse_str(d2, string_get_cstr(str), &sp);
    assert(b);
    assert(strcmp(sp, "[17,42]") == 0);
    assert(!deque_mpz_equal_p(d1, d2));
    b = deque_mpz_parse_str(d2, sp, &sp);
    assert(b);
    assert(strcmp(sp, "") == 0);
    assert(deque_mpz_equal_p(d1, d2));
  }
}

int main(void)
{
  test1();
  test_ti1(10);
  test_ti1(100);
  test_ti1(1000);
  test_ti1(10000);
  test_it();
  test_set();
  test_io();
  test_z();
  test_advance();
  exit(0);
}
