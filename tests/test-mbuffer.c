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
#include "m-buffer.h"

// Define a fixed queue of unsigned int
BUFFER_DEF(uint, unsigned int, 10, BUFFER_QUEUE|BUFFER_BLOCKING)

// Define a variable stack of float
BUFFER_DEF(floats, float, 0, BUFFER_STACK|BUFFER_BLOCKING)

// Define a fixed stack of char
BUFFER_DEF(char, char, 10, BUFFER_STACK|BUFFER_UNBLOCKING)

// Define a fixed queue of long long
BUFFER_DEF(llong, long long, 16, BUFFER_QUEUE|BUFFER_THREAD_UNSAFE|BUFFER_UNBLOCKING)

#include <gmp.h>
BUFFER_DEF(mpz, mpz_t, 32, BUFFER_QUEUE, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)))

buffer_uint_t g_buff;

static void conso(void *arg)
{
  unsigned int j;
  assert (arg == NULL);
  for(int i = 0; i < 1000;i++) {
    buffer_uint_pop(&j, g_buff);
    assert (j < 1000);
  }
}

static void prod(void *arg)
{
  assert (arg == NULL);
  for(unsigned int i = 0; i < 1000;i++)
    buffer_uint_push(g_buff, i);
}

static void test_global(void)
{
  m_thread_t idx_p[100];
  m_thread_t idx_c[100];

  buffer_uint_init(g_buff, 10);
  
  for(int i = 0; i < 100; i++) {
    m_thread_create (idx_p[i], conso, NULL);
    m_thread_create (idx_c[i], prod, NULL);
  }
  for(int i = 0; i < 100;i++) {
    m_thread_join(idx_p[i]);
    m_thread_join(idx_c[i]);
  }

  buffer_uint_clear(g_buff);
}

static void test_stack(void)
{
  buffer_floats_t buff;
  buffer_floats_init(buff, 100);
  for(unsigned int i = 0; i < 100; i++) {
    float f = i;
    buffer_floats_push(buff, f);
    assert (buffer_floats_empty_p(buff) == false);
  }
  assert (buffer_floats_full_p(buff) == true);
  for(unsigned int i = 0; i < 100; i++) {
    float j;
    buffer_floats_pop(&j, buff);
    assert(j == 99-i);
    assert (buffer_floats_full_p(buff) == false);
  }
  assert (buffer_floats_empty_p(buff) == true);
  buffer_floats_clear(buff);
}

static void test_stack2(void)
{
  buffer_char_t buff;
  bool b;
  char c;
  buffer_char_init(buff, 10);
  for(int i = 0; i < 10; i++) {
    c = i;
    b = buffer_char_push(buff, c);
    assert (b == true);
    assert (buffer_char_empty_p(buff) == false);
  }
  b = buffer_char_push(buff, c);
  assert (b == false);
  assert (buffer_char_full_p(buff) == true);
  for(int i = 0; i < 10; i++) {
    buffer_char_pop(&c, buff);
    assert(c == 9-i);
    assert (buffer_char_full_p(buff) == false);
  }
  b = buffer_char_pop(&c, buff);
  assert (b == false);
  assert (buffer_char_empty_p(buff) == true);
  buffer_char_clear(buff);
}

static void test_no_thread(void)
{
  buffer_llong_t buff;
  bool b;
  long long c;
  buffer_llong_init(buff, 16);
  for(int i = 0; i < 16; i++) {
    c = i;
    b = buffer_llong_push(buff, c);
    assert (b == true);
    assert (buffer_llong_empty_p(buff) == false);
  }
  b = buffer_llong_push(buff, c);
  assert (b == false);
  assert (buffer_llong_full_p(buff) == true);
  for(int i = 0; i < 16; i++) {
    buffer_llong_pop(&c, buff);
    assert(c == i);
    assert (buffer_llong_full_p(buff) == false);
  }
  b = buffer_llong_pop(&c, buff);
  assert (b == false);
  assert (buffer_llong_empty_p(buff) == true);

  b = buffer_llong_push(buff, c);
  assert (b == true);
  buffer_llong_clean(buff);
  assert (buffer_llong_empty_p(buff) == true);
  
  buffer_llong_clear(buff);
}

/* Test intrusive shared pointer + buffer */
#include "m-i-shared.h"
// Tiny test structure
typedef struct test_s {
  ISHARED_PTR_INTERFACE(itest, struct test_s);
  char buffer[52];
  char bigbuffer[1000000];
} test_t;

static void test_init(test_t *p)  { memset(p->buffer, 0x00, 52); }
static void test_clear(test_t *p) { memset(p->buffer, 0xFF, 52); }

ISHARED_PTR_DEF(itest, test_t,
                (INIT(test_init M_IPTR), CLEAR(test_clear M_IPTR), DEL(free)))

static test_t *test_new(void)
{
  test_t *p = M_ASSIGN_CAST(test_t*, malloc (sizeof (test_t)));
  assert (p != NULL);
  test_init (p);
  return ishared_itest_init(p);
}

BUFFER_DEF(itest, test_t *, 16, BUFFER_PUSH_INIT_POP_MOVE,
           ISHARED_PTR_OPLIST(itest))

static buffer_itest_t comm1;
static buffer_itest_t comm2;

static void test_conso1(void *arg)
{
  test_t *p;
  assert (arg == NULL);
  for(int i = 0; i < 10;i++) {
    buffer_itest_pop(&p, comm1);
    for(int j = 0; j < 52; j++)
      assert (p->buffer[j] == (char) ((j * j * 17) + j * 42 + 1));
    ishared_itest_clear(p);
  }
}

static void test_conso2(void *arg)
{
  test_t *p;
  assert (arg == NULL);
  for(int i = 0; i < 10;i++) {
    buffer_itest_pop(&p, comm2);
    for(int j = 0; j < 52; j++)
      assert (p->buffer[j] == (char) ((j * j * 17) + j * 42 + 1));
    ishared_itest_clear(p);
  }
}

static void test_prod(void *arg)
{
  assert (arg == NULL);
  for(unsigned int i = 0; i < 10;i++) {
    test_t *p = test_new();
    for(int j = 0; j < 52; j++)
      p->buffer[j] = (j * j * 17) + j * 42 + 1;
    buffer_itest_push(comm1, p);
    buffer_itest_push(comm2, p);
    ishared_itest_clear(p);
  }
}

static void test_global_ishared(void)
{
  m_thread_t idx_p[100];
  m_thread_t idx_c1[100];
  m_thread_t idx_c2[100];

  buffer_itest_init(comm1, 16);
  buffer_itest_init(comm2, 16);
  
  for(int i = 0; i < 100; i++) {
    m_thread_create (idx_c1[i], test_conso1, NULL);
    m_thread_create (idx_c2[i], test_conso2, NULL);
    m_thread_create (idx_p[i], test_prod, NULL);
  }
  for(int i = 0; i < 100;i++) {
    m_thread_join(idx_p[i]);
    m_thread_join(idx_c1[i]);
    m_thread_join(idx_c2[i]);
  }

  buffer_itest_clear(comm1);
  buffer_itest_clear(comm2);
}

int main(void)
{
  unsigned int x, y;
  buffer_uint_t v;
  buffer_uint_init(v, 10);

  assert (buffer_uint_empty_p(v) == true);
  assert (buffer_uint_full_p(v) == false);

  x = 10;
  buffer_uint_push (v, x);

  assert (buffer_uint_empty_p(v) == false);
  assert (buffer_uint_full_p(v) == false);

  buffer_uint_pop (&y, v);

  assert (buffer_uint_empty_p(v) == true);
  assert (buffer_uint_full_p(v) == false);
  assert (y == 10);

  buffer_uint_clear(v);

  test_global();
  test_stack();
  test_stack2();
  test_no_thread();
  test_global_ishared();
  exit(0);
}
