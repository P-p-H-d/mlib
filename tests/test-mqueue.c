/*
 * Copyright (c) 2017-2025, Patrick Pelissier
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
#include "test-obj.h"
#include "m-queue.h"
#include "coverage.h"

START_COVERAGE
STACK_DEF(stack_obj, testobj_t, 0, TESTOBJ_OPLIST)
END_COVERAGE

M_QUEUE_DEF(queue_obj, testobj_t, 10, TESTOBJ_OPLIST)
M_STACK_DEF(stack, int, 15)
QUEUE_DEF(buffer_llong, long long, 0)

static void test_obj(void)
{
  stack_obj_t b1, b2;
  testobj_t o;
  bool b;

  testobj_init(o);
  stack_obj_init(b1, 24);
  assert( stack_obj_empty_p(b1) == true);
  assert( stack_obj_full_p(b1) == false);
  assert( stack_obj_size(b1) == 0);
  assert( stack_obj_capacity(b1) == 24);

  stack_obj_reset(b1);
  assert( stack_obj_empty_p(b1) == true);
  assert( stack_obj_full_p(b1) == false);
  assert( stack_obj_size(b1) == 0);

  for(unsigned i = 0; i < 24; i++) {
    testobj_set_ui(o, i);
    assert( stack_obj_full_p(b1) == false);
    b = stack_obj_push(b1, o);
    assert(b);
    assert( stack_obj_empty_p(b1) == false);
    assert( stack_obj_size(b1) == i+1);
  }

  assert( stack_obj_full_p(b1) == true);
  assert( stack_obj_size(b1) == 24);
  b = stack_obj_push(b1, o);
  assert(!b);
  assert( stack_obj_size(b1) == 24);

  for(unsigned i = 23; i < 24; i--) {
    testobj_set_ui(o, i);
    assert( stack_obj_empty_p(b1) == false);
    b = stack_obj_pop(&o, b1);
    assert(b);
    assert( stack_obj_full_p(b1) == false);
    assert( stack_obj_size(b1) == i);
    assert( testobj_cmp_ui(o, i) == 0);
  }

  assert( stack_obj_empty_p(b1) == true);
  assert( stack_obj_size(b1) == 0);
  b = stack_obj_pop(&o, b1);
  assert(!b);
  assert( stack_obj_size(b1) == 0);

  for(unsigned i = 0; i < 24; i++) {
    testobj_set_ui(o, i);
    assert( stack_obj_full_p(b1) == false);
    b = stack_obj_push(b1, o);
    assert(b);
    assert( stack_obj_empty_p(b1) == false);
    assert( stack_obj_size(b1) == i+1);
  }

  stack_obj_init_set(b2, b1);
  stack_obj_reset(b1);
  assert( stack_obj_empty_p(b1) == true);
  assert( stack_obj_size(b1) == 0);
  b = stack_obj_pop(&o, b1);
  assert(!b);
  assert( stack_obj_size(b1) == 0);

  assert( stack_obj_full_p(b2) == true);
  assert( stack_obj_size(b2) == 24);
  b = stack_obj_push(b2, o);
  assert(!b);
  assert( stack_obj_size(b2) == 24);

  stack_obj_set(b1, b2);
  assert( stack_obj_size(b1) == 24);

  stack_obj_clear(b1);
  stack_obj_clear(b2);
  testobj_clear(o);
}

static void test_queue_llong(void)
{
  buffer_llong_t buff;
  bool b;
  long long c;
  buffer_llong_init(buff, 16);
  assert (buffer_llong_empty_p(buff) == true);
  for(int i = 0; i < 16; i++) {
    assert (buffer_llong_full_p(buff) == false);
    c = i;
    b = buffer_llong_push(buff, c);
    assert (b == true);
    assert (buffer_llong_empty_p(buff) == false);
  }
  assert (buffer_llong_full_p(buff) == true);
  b = buffer_llong_push(buff, c);
  assert (b == false);
  for(int i = 0; i < 16; i++) {
    assert (buffer_llong_empty_p(buff) == false);
    buffer_llong_pop(&c, buff);
    assert(c == i);
    assert (buffer_llong_full_p(buff) == false);
  }
  assert (buffer_llong_empty_p(buff) == true);
  b = buffer_llong_pop(&c, buff);
  assert (b == false);

  b = buffer_llong_push(buff, c);
  assert (b == true);
  assert (buffer_llong_empty_p(buff) == false);
  buffer_llong_reset(buff);
  assert (buffer_llong_empty_p(buff) == true);
  
  buffer_llong_clear(buff);
}

static void test_emplace(void)
{
  stack_obj_t b;
  stack_obj_init(b, 32);
  stack_obj_emplace_str(b, "56");
  testobj_t o;
  testobj_init(o);
  stack_obj_pop(&o, b);
  assert( testobj_cmp_ui(o, 56) == 0 );
  testobj_clear(o);
  stack_obj_clear(b);
}

int main(void)
{
  test_obj();
  test_queue_llong();
  test_emplace();
  testobj_final_check();
  exit(0);
}
