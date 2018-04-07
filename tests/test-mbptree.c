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
#include <stdlib.h>
#include "m-bptree.h"
#include "mympz.h"
#include "coverage.h"

START_COVERAGE
BPTREE_DEF2(btree, 3, int, M_DEFAULT_OPLIST, int, M_DEFAULT_OPLIST)
END_COVERAGE
BPTREE_DEF2(btree_my, 3, my_mpz_t, MYMPZ2_OPLIST, my_mpz_t, MYMPZ2_OPLIST)
  
BPTREE_DEF(btree_intset, 13, int)
BPTREE_DEF(btree_myset, 15, my_mpz_t, MYMPZ2_OPLIST)

static void test1(void)
{
  btree_t b;
  btree_init(b);
  assert (btree_empty_p(b));
  btree_set_at(b, 12, 34);
  assert (!btree_empty_p(b));
  assert (btree_get(b, 12) != NULL);
  assert (*btree_get(b, 12) == 34);
  btree_set_at(b, 11, 35);
  assert (!btree_empty_p(b));
  assert (btree_get(b, 11) != NULL);
  assert (*btree_get(b, 11) == 35);
  btree_set_at(b, 1, 36);
  assert (!btree_empty_p(b));
  assert (btree_get(b, 1) != NULL);
  assert (*btree_get(b, 1) == 36);
  btree_set_at(b, 25, 37);
  assert (!btree_empty_p(b));
  assert (btree_get(b, 25) != NULL);
  assert (*btree_get(b, 25) == 37);
  btree_set_at(b, -25, 38);
  assert (btree_get(b, -25) != NULL);
  assert (*btree_get(b, -25) == 38);
  btree_set_at(b, 5, 39);
  assert (*btree_get(b, -25) == 38);
  btree_set_at(b, 5, 40);
  assert (*btree_get(b, 5) == 40);
  btree_set_at(b, -5, 41);
  assert (*btree_get(b, -5) == 41);
  for(int i = -1000; i < 1000; i+=3) {
    btree_set_at(b, i, 2*i);
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  for(int i = -1000+1; i < 1000; i+=3) {
    btree_set_at(b, i, 2*i);
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  for(int i = -1000+2; i < 1000; i+=3) {
    btree_set_at(b, i, 2*i);
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  for(int i = -1000; i < 1000; i++) {
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  btree_remove(b, 0);
  assert (btree_get(b, 0) == NULL);
  btree_remove(b, 1);
  assert (btree_get(b, 1) == NULL);
  btree_remove(b, 2);
  assert (btree_get(b, 2) == NULL);
  btree_remove(b, 0);
  assert (btree_get(b, 0) == NULL);
  btree_remove(b, -1);
  assert (btree_get(b, -1) == NULL);
  btree_remove(b, -2);
  assert (btree_get(b, -2) == NULL);
  btree_remove(b, 3);
  assert (btree_get(b, 3) == NULL);
  btree_remove(b, 4);
  assert (btree_get(b, 4) == NULL);
  btree_remove(b, -3);
  assert (btree_get(b, -3) == NULL);
  btree_remove(b, 5);
  assert (btree_get(b, 5) == NULL);
  btree_remove(b, -5);
  assert (btree_get(b, -5) == NULL);
  btree_remove(b, -4);
  assert (btree_get(b, -4) == NULL);
  btree_remove(b, 6);
  assert (btree_get(b, 6) == NULL);
  btree_remove(b, 7);
  assert (btree_get(b, 7) == NULL);
  for(int i = -1000; i < 0; i++) {
    btree_remove(b, i);
    assert (btree_get(b, i) == NULL);
  }
  for(int i = 0; i < 1000; i++) {
    btree_remove(b, i);
    assert (btree_get(b, i) == NULL);
  }
  btree_clear(b);
}

int main(void)
{
  test1();

  exit(0);
}
