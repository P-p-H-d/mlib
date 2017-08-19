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
#include <stdio.h>
#include <gmp.h> // For testing purpose only.

#include "m-string.h"
#include "m-deque.h"

#include "coverage.h"

START_COVERAGE
DEQUE_DEF(deque, int)
END_COVERAGE

static void test_ti1(int n)
{
  deque_t d;

  deque_init(d);

  for(int i =0; i < n ;i++) {
    deque_push_back(d, i);
    deque_push_front(d, i);
  }
  assert (*deque_back(d) == n-1);
  assert (*deque_front(d) == n-1);
  int s = 0;
  for(int i =0; i < 2*n ;i++) {
    int z;
    if ((i % 3) == 0)
      deque_pop_front(&z, d);
    else
      deque_pop_back(&z, d);
    s += z;
  }
  assert (s == n*(n-1) );
  assert (deque_empty_p(d));
  
  deque_clear(d);
}

int main(void)
{
  test_ti1(10);
  test_ti1(100);
  test_ti1(1000);
  test_ti1(10000);
}
