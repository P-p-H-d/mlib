/*
 * Copyright (c) 2017-2020, Patrick Pelissier
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

static inline bool testobj_equal2_p(const testobj_t z1, const testobj_t z2)
{
  return z1->a == z2->a;
}

PRIOQUEUE_DEF(obj_pqueue, testobj_t, M_OPEXTEND(TESTOBJ_CMP_OPLIST, EQUAL(testobj_equal2_p) ))

PRIOQUEUE_DEF_AS(PrioDouble, PrioDouble, PrioDoubleIt, double)
#define M_OPL_PrioDouble() PRIOQUEUE_OPLIST(PrioDouble, M_DEFAULT_OPLIST)

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
  
  assert (int_pqueue_size(p) == 0);
  for(int i = -100; i < 100; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 200);
  for(int i = -100; i < 100; i++) {
    bool b = int_pqueue_erase(p, i);
    assert (b);
  }
  assert (int_pqueue_size(p) == 0);

  int_pqueue_clear(p);
}

static void test_update(void)
{
  obj_pqueue_t p;
  obj_pqueue_init(p);
  const testobj_t *front;
  M_LET( x, y, z, TESTOBJ_OPLIST) {
    testobj_set_ui(x, 7);
    testobj_set_id(x, 1);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 30);
    testobj_set_id(x, 2);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 40);
    testobj_set_id(x, 3);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 10);
    testobj_set_id(x, 4);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 20);
    testobj_set_id(x, 5);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 3);
    testobj_set_id(x, 6);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 35);
    testobj_set_id(x, 7);
    obj_pqueue_push(p, x);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 6);

    testobj_set_ui(y, 8);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 6);

    testobj_set_ui(y, 1);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 5);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 6);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 6);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 5);

    testobj_set_ui(y, 100);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 2);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 3);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 4);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 1);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 7);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 7);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 5);
  }
  obj_pqueue_clear(p);
}

static void test_double(void)
{
  M_LET( (tab, 0.0, 1.0, 2.0, 3.0), PrioDouble) {
    double ref = 0.0;
    // NOTE: The binary heap is sorted in the same natural order that the sorted array
    for M_EACH(i, tab, PrioDouble) {
      assert (*i == ref);
      ref += 1.0;
    }
  }
}

int main(void)
{
  test1();
  test2();
  test_update();
  test_double();
  exit(0);
}
