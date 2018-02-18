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
#include <gmp.h> // For testing purpose only.

#include "m-snapshot.h"
#include "m-mutex.h"

#include "coverage.h"

// Long Structure with a simple CRC to check for proper passing
typedef struct {
  unsigned int n;
  unsigned long long spare1;
  unsigned int c;
  unsigned long long spare2;
  unsigned int p;
} data_t;

static void data_crc(data_t *p) { p->p = -p->n; p->c = ~p->n; }
static void data_init(data_t *p) { p->n = 0; p->spare1 = 0; p->spare2 = 0; data_crc(p); }
static void data_valid_p(const data_t *p) { assert (p->n == -p->p && p->n == ~p->c); }
static void data_clear(data_t *p) { (void) p;}
static void data_set(data_t *p, data_t o) { p->n = o.n; data_crc(p); }

#define DATA_OPLIST \
  (INIT(data_init M_IPTR), INIT_SET(data_set M_IPTR), SET(data_set M_IPTR), CLEAR(data_clear M_IPTR))

START_COVERAGE
SNAPSHOT_SRSW_DEF(snapshot_uint, unsigned int)
SNAPSHOT_SRSW_DEF(snapshot_mpz, mpz_t,
                  (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)))
SNAPSHOT_MRSW_DEF(snapshot_mrsw_data, data_t, DATA_OPLIST)
SNAPSHOT_MRMW_DEF(snapshot_mrmw_data, data_t, DATA_OPLIST)
END_COVERAGE
SNAPSHOT_DEF(snapshot_data, data_t, DATA_OPLIST )


static void test_uint(void)
{
  unsigned int *p_w;
  const unsigned int *p_r;
  snapshot_uint_t t;
  snapshot_uint_init(t);

  p_r = snapshot_uint_read(t);
  assert (p_r == snapshot_uint_get_read_buffer(t));
  assert(*p_r == 0);
  assert(snapshot_uint_read(t) == p_r);
  
  p_w = snapshot_uint_write(t);
  assert (snapshot_uint_updated_p (t) == true);
  assert (p_w != p_r);
  *p_w = 1;
  assert (*p_r == 0);
  assert (snapshot_uint_read(t) != p_r);
  assert (snapshot_uint_updated_p (t) == false);
  assert (*snapshot_uint_read(t) == 0);
  p_r = snapshot_uint_read(t);
  assert (*p_r == 0);
  
  p_w = snapshot_uint_write(t);
  *p_w = 2;
  assert (*snapshot_uint_read(t) == 1);
  
  p_w = snapshot_uint_write(t);
  assert (p_w == snapshot_uint_get_write_buffer(t));
  *p_w = 3;
  p_w = snapshot_uint_write(t);
  *p_w = 4;
  assert (*snapshot_uint_read(t) == 3);

  snapshot_uint_t t2;
  snapshot_uint_init_set (t2, t);
  snapshot_uint_clear(t);
  assert (*snapshot_uint_read(t) == 3);
  snapshot_uint_init(t);
  snapshot_uint_set (t, t2);
  assert (*snapshot_uint_read(t) == 3);
}

snapshot_data_t g_buff;

static void conso(void *arg)
{
  assert (arg == NULL);
  while (true) {
    const data_t *p = snapshot_data_read(g_buff);
    data_valid_p(p);
    if (p->n == 0)
      return;
  }
}

static void prod(void *arg)
{
  assert (arg == NULL);
  data_t *p = snapshot_data_get_write_buffer(g_buff);
  for(unsigned int i = 1; i < 100000;i++) {
    p->n = i * i;
    data_crc(p);
    p = snapshot_data_write(g_buff);
  }
  p->n = 0;
  data_crc(p);
  p = snapshot_data_write(g_buff);
  assert (p != NULL);
}

static void test_global(void)
{
  m_thread_t idx[2];

  snapshot_data_init(g_buff);
  data_t *p = snapshot_data_write(g_buff);
  assert (p != NULL);
  p->n = 42;
  data_crc(p);
  p = snapshot_data_write(g_buff);
  assert (p != NULL);

  m_thread_create (idx[0], conso, NULL);
  m_thread_create (idx[1], prod, NULL);

  m_thread_join(idx[0]);
  m_thread_join(idx[1]);

  snapshot_data_clear(g_buff);
}

static void test_mrsw_int1(void)
{
  snapshot_mrsw_int_t idx;
  snapshot_mrsw_int_init(idx, 1);

  assert (snapshot_mrsw_int_get_write_idx(idx) == 1);

  assert (snapshot_mrsw_int_write(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 1);
  assert (snapshot_mrsw_int_write(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 1);

  assert (snapshot_mrsw_int_read_start(idx) == 0);
  snapshot_mrsw_int_read_end(idx, 0);
  assert (snapshot_mrsw_int_read_start(idx) == 0);
  snapshot_mrsw_int_read_end(idx, 0);
  
  assert (snapshot_mrsw_int_write(idx) == 0);

  assert (snapshot_mrsw_int_read_start(idx) == 1);
  assert (snapshot_mrsw_int_write(idx) == 2);
  assert (snapshot_mrsw_int_write(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 2);
  snapshot_mrsw_int_read_end(idx, 1);
  assert (snapshot_mrsw_int_read_start(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 1);

  snapshot_mrsw_int_clear(idx);
}

static void test_mrsw_int2(void)
{
  snapshot_mrsw_int_t idx;
  snapshot_mrsw_int_init(idx, 2);

  assert (snapshot_mrsw_int_get_write_idx(idx) == 1);

  assert (snapshot_mrsw_int_write(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 1);
  assert (snapshot_mrsw_int_write(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 1);

  assert (snapshot_mrsw_int_read_start(idx) == 0);
  snapshot_mrsw_int_read_end(idx, 0);
  assert (snapshot_mrsw_int_read_start(idx) == 0);
  snapshot_mrsw_int_read_end(idx, 0);
  
  assert (snapshot_mrsw_int_write(idx) == 0);

  assert (snapshot_mrsw_int_read_start(idx) == 1);
  assert (snapshot_mrsw_int_write(idx) == 2);
  assert (snapshot_mrsw_int_write(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 2);
  assert (snapshot_mrsw_int_read_start(idx) == 0);
  assert (snapshot_mrsw_int_write(idx) == 3);
  snapshot_mrsw_int_read_end(idx, 1);
  assert (snapshot_mrsw_int_read_start(idx) == 2);
  assert (snapshot_mrsw_int_write(idx) == 1);
  snapshot_mrsw_int_read_end(idx, 0);

  snapshot_mrsw_int_clear(idx);
}

snapshot_mrsw_data_t global_mrsw;

static void conso2(void *arg)
{
  assert (arg == NULL);
  while (true) {
    const data_t *p = snapshot_mrsw_data_read_start(global_mrsw);
    assert (p != NULL);
    data_valid_p(p);
    if (p->n == 0)
      return;
    snapshot_mrsw_data_read_end(global_mrsw, p);
  }
}

static void prod2(void *arg)
{
  assert (arg == NULL);
  data_t *p = snapshot_mrsw_data_get_write_buffer(global_mrsw);
  for(unsigned int i = 1; i < 200000;i++) {
    assert (p != NULL);
    p->n = i * i;
    data_crc(p);
    p = snapshot_mrsw_data_write(global_mrsw);
  }
  p->n = 0;
  data_crc(p);
  p = snapshot_mrsw_data_write(global_mrsw);
  assert (p != NULL);
}

static void test_mrsw_global(int reader)
{
  m_thread_t idx_w;
  m_thread_t idx[reader];

  snapshot_mrsw_data_init(global_mrsw, reader);
  data_t *p = snapshot_mrsw_data_write(global_mrsw);
  assert (p != NULL);
  p->n = 42;
  data_crc(p);
  p = snapshot_mrsw_data_write(global_mrsw);
  assert (p != NULL);

  for(int i = 0 ; i < reader; i++)
    m_thread_create (idx[i], conso2, NULL);
  m_thread_create (idx_w, prod2, NULL);

  m_thread_join(idx_w);
  for(int i = 0 ; i < reader; i++)
    m_thread_join(idx[i]);

  snapshot_mrsw_data_clear(global_mrsw);
}

snapshot_mrmw_data_t global_mrmw;

static void conso3(void *arg)
{
  assert (arg == NULL);
  while (true) {
    const data_t *p = snapshot_mrmw_data_read_start(global_mrmw);
    assert (p != NULL);
    data_valid_p(p);
    if (p->n == 0)
      return;
    snapshot_mrmw_data_read_end(global_mrmw, p);
  }
}

static void prod3(void *arg)
{
  assert (arg == NULL);
  for(unsigned int i = 1; i < 20000000;i++) {
    data_t *p = snapshot_mrmw_data_write_start(global_mrmw);
    assert (p != NULL);
    p->n = i * i;
    data_crc(p);
    snapshot_mrmw_data_write_end(global_mrmw, p);
  }
  data_t *p = snapshot_mrmw_data_write_start(global_mrmw);
  p->n = 0;
  data_crc(p);
  snapshot_mrmw_data_write_end(global_mrmw, p);
}

static void test_mrmw_global(int reader, int writer)
{
  m_thread_t idx_w[writer];
  m_thread_t idx[reader];

  snapshot_mrmw_data_init(global_mrmw, reader, writer);
  data_t *p = snapshot_mrmw_data_write_start(global_mrmw);
  assert (p != NULL);
  p->n = 42;
  data_crc(p);
  snapshot_mrmw_data_write_end(global_mrmw, p);

  for(int i = 0 ; i < reader; i++)
    m_thread_create (idx[i], conso3, NULL);
  for(int i = 0 ; i < writer; i++)
    m_thread_create (idx_w[i], prod3, NULL);

  for(int i = 0 ; i < writer; i++)
    m_thread_join(idx_w[i]);
  for(int i = 0 ; i < reader; i++)
    m_thread_join(idx[i]);

  snapshot_mrmw_data_clear(global_mrmw);
}

int main(void)
{
  test_uint();
  test_global();
  test_mrsw_int1();
  test_mrsw_int2();
  test_mrsw_global(1);
  test_mrsw_global(2);
  test_mrsw_global(4);
  test_mrsw_global(8);
  exit(0);
}

