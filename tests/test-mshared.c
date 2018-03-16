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
#include <gmp.h>
#include <assert.h>

#include "m-shared.h"

#include "coverage.h"
START_COVERAGE
SHARED_PTR_DEF(shared_int, int)
SHARED_RESOURCE_DEF(shared_ressource, mpz_t, M_CLASSIC_OPLIST(mpz))
END_COVERAGE
SHARED_PTR_DEF(shared_mpz, mpz_t, M_CLASSIC_OPLIST(mpz))
SHARED_PTR_RELAXED_DEF(shared_relaxed_int, int)

static int f(const shared_int_t p)
{
  return *shared_int_cref(p);
}

static void test1(void)
{
  shared_int_t p1, p2;

  int *p = M_ASSIGN_CAST(int*, malloc(sizeof(int)));
  assert (p != NULL);
  
  shared_int_init2(p1, p);
  assert(!shared_int_NULL_p(p1));
  assert(shared_int_ref(p1) != NULL);

  *shared_int_ref(p1) = 12;

  shared_int_init_set(p2, p1);
  assert(f(p2) == 12);

  shared_int_clean(p1);
  assert(shared_int_NULL_p(p1));
  assert(!shared_int_NULL_p(p2));
  assert(f(p2) == 12);
  
  shared_int_set(p1, p2);

  shared_int_clean(p2);
  assert(!shared_int_NULL_p(p1));  
  assert(shared_int_NULL_p(p2));
  assert(f(p1) == 12);
  shared_int_set(p2, p1);
  assert(!shared_int_NULL_p(p1));  
  assert(!shared_int_NULL_p(p2));
  assert(f(p2) == 12);

  shared_int_clean(p2);
  shared_int_clean(p1);
  
  assert(shared_int_NULL_p(p1));
  assert(shared_int_NULL_p(p2));

  shared_int_clear(p2);
  shared_int_clear(p1);
  
  shared_int_init2(p1, NULL);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);
                  
  shared_int_init(p1);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);

  shared_int_init_new(p1);
  assert(!shared_int_NULL_p(p1));
  *shared_int_ref(p1) = 1;
  shared_int_init_new(p2);
  assert(!shared_int_NULL_p(p2));
  *shared_int_ref(p2) = 2;
  assert(!shared_int_equal_p(p1, p2));
  assert (*shared_int_cref(p1) == 1);
  assert (*shared_int_cref(p2) == 2);
  shared_int_swap(p1, p2);
  assert (*shared_int_cref(p1) == 2);
  assert (*shared_int_cref(p2) == 1);
  shared_int_set(p1, p2);
  assert(shared_int_equal_p(p1, p2));
  assert (*shared_int_cref(p1) == 1);
  assert (*shared_int_cref(p2) == 1);
  shared_int_clear(p1);
  shared_int_init_move(p1, p2);
  assert (*shared_int_cref(p1) == 1);
  shared_int_init_new(p2);
  assert (*shared_int_cref(p2) == 0);
  shared_int_set(p2, p1);
  shared_int_move(p2, p1);
  assert (*shared_int_cref(p2) == 1);
  shared_int_clear(p2);
}

static void test2(void)
{
  shared_int_t p1, p2;

  shared_int_init_new (p1);
  *shared_int_ref(p1) = 1;
  shared_int_init_new (p2);
  *shared_int_ref(p2) = 2;
  shared_int_set(p1, p2);
  assert (*shared_int_ref(p1) == 2);
  assert (*shared_int_ref(p2) == 2);
  shared_int_clear(p2);
  assert (*shared_int_ref(p1) == 2);
  shared_int_clear(p1);
}

#define MAX_RESSOURCE 10

static void test_ressource(int n)
{
  shared_ressource_t ressource;
  shared_ressource_it_t it[MAX_RESSOURCE];
  shared_ressource_it_t copy[MAX_RESSOURCE];
  shared_ressource_it_t onemore;
  
  shared_ressource_init (ressource, n);
  for(int i = 0; i < n ; i++) {
    shared_ressource_it (it[i], ressource);
    assert (!shared_ressource_end_p(it[i]));
    mpz_t *z = shared_ressource_ref(it[i]);
    assert (z != NULL);
    mpz_set_ui(*z, i);
  }
  shared_ressource_it (onemore, ressource);
  assert (shared_ressource_end_p(onemore));
  for(int i = 0; i < n ; i++) {
    shared_ressource_it_set (copy[i], it[i]);
    assert (!shared_ressource_end_p(copy[i]));
    const mpz_t *z = shared_ressource_cref(copy[i]);
    assert (z != NULL);
    assert (mpz_cmp_ui(*z, i) == 0);
  }
  shared_ressource_it (onemore, ressource);
  assert (shared_ressource_end_p(onemore));
  for(int i = 0; i < n ; i++) {
    shared_ressource_end (it[i], ressource);
  }
  shared_ressource_it (onemore, ressource);
  assert (shared_ressource_end_p(onemore));
  for(int i = n-1; i >= 0 ; i--) {
    const mpz_t *z = shared_ressource_cref(copy[i]);
    assert (z != NULL);
    shared_ressource_end (copy[i], ressource);
    assert (shared_ressource_end_p(copy[i]));
    shared_ressource_it (onemore, ressource);
    assert (!shared_ressource_end_p(onemore));
    shared_ressource_end (onemore, ressource);
    shared_ressource_end (copy[i], ressource);
  }
  
  shared_ressource_clear(ressource);
}


int main(void)
{
  test1();
  test2();
  for(int i = 1; i < MAX_RESSOURCE; i++)
    test_ressource(i);  
  exit(0);
}
