/*
 * M*LIB - Test for Concurrent memory pool allocator
 *
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

#include "m-c-mempool.h"
#include "m-mutex.h"

C_MEMPOOL_DEF(lf_mempool, int)

/**********************************************************************/

#define MAX_VALUE 100
#define MAX_THREAD 4

lf_mempool_t g;

typedef struct {
  int step;
  int *ptr[MAX_VALUE];
  int value[MAX_VALUE];
} p_t;

p_t g_tab[MAX_THREAD];

static void do_stuff(lf_mempool_tid_t id)
{
  // Mix of local and more global allocation
  int *d[MAX_VALUE];
  for(int i = 0; i < MAX_VALUE; i++) {
    d[i] = lf_mempool_new(g, id);
    *d[i] = i*i;
  }
  switch (g_tab[id].step)
    {
    case 0:
      for(int i = 0; i < 50; i++) {
        g_tab[id].value[i] += i;
        g_tab[id].ptr[i] = lf_mempool_new(g, id);
        *g_tab[id].ptr[i] = g_tab[id].value[i];
      }
      g_tab[id].step = 1;
      break;
    case 1:
      for(int i = 50; i < MAX_VALUE; i++) {
        g_tab[id].value[i] += i;
        g_tab[id].ptr[i] = lf_mempool_new(g, id);
        *g_tab[id].ptr[i] = g_tab[id].value[i];
      }
      g_tab[id].step = 2;
      break;
    case 2:
      for(int i = 0; i < 50; i++) {
        assert(g_tab[id].ptr[i] != NULL);
        assert(*g_tab[id].ptr[i] == g_tab[id].value[i] );
        lf_mempool_del(g, g_tab[id].ptr[i], id);
        g_tab[id].ptr[i] = NULL;
      }
      g_tab[id].step = 3;
      break;
    case 3:
      for(int i = 50; i < MAX_VALUE; i++) {
        assert(g_tab[id].ptr[i] != NULL);
        assert(*g_tab[id].ptr[i] == g_tab[id].value[i] );
        lf_mempool_del(g, g_tab[id].ptr[i], id);
        g_tab[id].ptr[i] = NULL;
      }
      g_tab[id].step = 0;
      break;
    default:
      break;
    }
  for(int i = 0; i < MAX_VALUE; i++) {
    assert (*d[i] == i*i);
    lf_mempool_del(g, d[i], id);
  }
}

static void thread1(void *arg)
{
  (void)arg;
  lf_mempool_tid_t id = lf_mempool_attach_thread(g);
  for(int n = 0; n < 100000; n ++) {
    lf_mempool_awake(g, id);
    do_stuff(id);
    lf_mempool_sleep(g, id);
  }
  lf_mempool_detach_thread(g, id);
}

static void thread2(void *arg)
{
  (void)arg;
  lf_mempool_tid_t id = lf_mempool_attach_thread(g);
  for(int n = 0; n < 10000; n ++) {
    lf_mempool_awake(g, id);
    do_stuff(id);
    m_thread_sleep(1);
    lf_mempool_sleep(g, id);
  }
  lf_mempool_detach_thread(g, id);
}

static void test(void)
{
  lf_mempool_init(g, MAX_THREAD, 256, MAX_THREAD);

  m_thread_t id[MAX_THREAD];
  for(int i = 0; i < MAX_THREAD; i+=2) {
    m_thread_create(id[i], thread1, NULL);
    m_thread_create(id[i+1], thread2, NULL);
  }
  for(int i = 0; i < MAX_THREAD; i++) {
    m_thread_join(id[i]);
  }
  lf_mempool_clear(g);
}

int main(void)
{
  test();
  exit(0);
}

