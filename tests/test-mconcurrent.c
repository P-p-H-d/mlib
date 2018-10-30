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
#include "test-obj.h"
#include "coverage.h"

#include "m-tuple.h"
#include "m-array.h"
#include "m-dict.h"
#include "m-list.h"
#include "m-deque.h"
#include "m-bptree.h"
#include "m-rbtree.h"
#include "m-concurrent.h"

TUPLE_DEF2(point, (x, int), (y, int))
#define M_OPL_point_t() TUPLE_OPLIST(point, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST)
CONCURRENT_DEF(ppoint, point_t)

ARRAY_DEF(array1, int)
CONCURRENT_DEF(parray1, array1_t, ARRAY_OPLIST(array1))

DICT_DEF2(dict1, int, int)
CONCURRENT_DEF(pdict1, dict1_t, DICT_OPLIST(dict1))

DICT_SET_DEF(dict2, int)
CONCURRENT_DEF(pdict2, dict2_t, DICT_SET_OPLIST(dict2))

LIST_DEF(list1, int)
CONCURRENT_DEF(plist1, list1_t, LIST_OPLIST(list1))

LIST_DUAL_PUSH_DEF(list2, int)
CONCURRENT_DEF(plist2, list2_t, LIST_OPLIST(list2))

DEQUE_DEF(deque1, int)
CONCURRENT_DEF(pdeque1, deque1_t, DEQUE_OPLIST(deque1))

BPTREE_DEF2(bptree1, 10, int, int)
CONCURRENT_DEF(pbtree1, bptree1_t, BPTREE_OPLIST2(bptree1, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST))

BPTREE_DEF(bptree2, 10, int)
CONCURRENT_DEF(pbtree2, bptree2_t, BPTREE_OPLIST(bptree2))

RBTREE_DEF(rbtree1, int)
CONCURRENT_DEF(prbtree1, rbtree1_t, RBTREE_OPLIST(rbtree1))

DICT_DEF2(string_pool, string_t, string_t)
#define STRING_POOL_OPLIST                                      \
  DICT_OPLIST(string_pool, STRING_OPLIST, STRING_OPLIST)
#define M_OPL_string_pool_t() STRING_POOL_OPLIST

CONCURRENT_DEF(string_pool_ts, string_pool_t, STRING_POOL_OPLIST)
#define STRING_POOL_TS_OPLIST                           \
  CONCURRENT_OPLIST(string_pool_ts, STRING_POOL_OPLIST)
#define M_OPL_string_pool_ts_t() STRING_POOL_TS_OPLIST

/********************************/
parray1_t arr;

static void conso(void *p)
{
  assert (p == NULL);
  for(int i = 0; i < 1000; i++) {
    int j;
    bool b = parray1_pop_blocking(&j, arr, true);
    assert (b);
    assert (j == i);
  }         
}

static void test(void)
{
  m_thread_t idx;
  parray1_init(arr);
  m_thread_create (idx, conso, NULL);
  m_thread_sleep(1000);
  for(int i = 0; i < 1000; i++) {
    parray1_push (arr, i);
    while (!parray1_empty_p(arr));
  }           
  m_thread_join(idx);
  parray1_clear(arr);
}

static void test_basic(void)
{
  pdict1_t dict;
  pdict1_init(dict);
  int z;
  pdict1_set_at(dict, 2, 3);
  pdict1_get_copy(&z, dict, 2);
  assert (z == 3);
  z = 1;
  bool b = pdict1_get_blocking(&z, dict, 2, false);
  assert (b);
  assert (z == 3);
  pdict1_clear(dict);

  M_LET(str, string_t)
  M_LET(pool, string_pool_ts_t) {
    string_pool_ts_set_at (pool, STRING_CTE("A"), STRING_CTE("B"));
    b = string_pool_ts_get_copy (&str, pool, STRING_CTE("A"));
    assert (b);
    assert (string_equal_str_p (str, "B"));
  }
}

int main(void)
{
  test_basic();
  test();
}
