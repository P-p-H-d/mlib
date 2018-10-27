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
#include <assert.h>
#include "test-obj.h"
#include "m-prioqueue.h"

#include "coverage.h"
START_COVERAGE
PRIOQUEUE_DEF(int_pqueue, int)
END_COVERAGE
PRIOQUEUE_DEF(mpz_pqueue, testobj_t, TESTOBJ_CMP_OPLIST)

static void test1(void)
{
  int x;
  int_pqueue_t p;
  int_pqueue_init(p);
  assert (int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 0);
  int_pqueue_push(p, 10);
  assert (!int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 1);
  int_pqueue_push(p, 60);
  int_pqueue_push(p, 40);
  int_pqueue_push(p, 5);
  int_pqueue_push(p, 30);
  assert (int_pqueue_size(p) == 5);
  assert (*int_pqueue_front(p) == 5);
  int_pqueue_pop(&x, p);
  assert (int_pqueue_size(p) == 4);
  assert (x == 5);
  int_pqueue_pop(&x, p);
  assert (int_pqueue_size(p) == 3);
  assert (x == 10);
  int_pqueue_pop(&x, p);
  assert (x == 30);
  int_pqueue_pop(&x, p);
  assert (x == 40);
  int_pqueue_pop(&x, p);
  assert (x == 60);
  assert (int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 0);
  int_pqueue_push(p, 10);
  int_pqueue_push(p, 30);
  int_pqueue_pop(&x, p);
  assert (x == 10);
  int_pqueue_push(p, 5);
  int_pqueue_pop(&x, p);
  assert (x == 5);
  int_pqueue_push(p, 17);
  bool b = int_pqueue_erase(p, 17);
  assert(b);
  b = int_pqueue_erase(p, 1742);
  assert(!b);
  int_pqueue_clear(p);
}

static void test2(void)
{
  int_pqueue_t p;
  int_pqueue_init(p);
  for(int i = -100; i < 100; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 200);
  for(int i = -200; i < -100; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 300);
  for(int i = 100; i < 200; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 400);
  for(int i = -200; i < 200; i++) {
    int x;
    int_pqueue_pop(&x, p);
    assert (x == i);
  }
  
  int_pqueue_clear(p);
}

int main(void)
{
  test1();
  test2();
  exit(0);
}
