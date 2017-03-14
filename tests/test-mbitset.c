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

#include "m-bitset.h"

static void test1(void)
{
  bitset_t set;

  bitset_init(set);
  for(int i = 0; i < 100; i ++) {
    bitset_push_back(set, i%2);
    assert (bitset_back(set) == (i%2));
  }
  for(int i = 0; i < 100; i ++) {
    assert (bitset_get(set, i) == (i%2));
  }
  assert(bitset_size(set) == 100);
  assert(bitset_empty_p(set) == false);

  bitset_t set2;
  bitset_init_set(set2, set);
  
  bitset_clean(set);
  assert(bitset_size(set) == 0);
  assert(bitset_empty_p(set) == true);

  assert(bitset_size(set2) == 100);
  assert(bitset_empty_p(set2) == false);
  bitset_resize(set2, 50);
  assert(bitset_size(set2) == 50);
  assert(bitset_empty_p(set2) == false);
  for(int i = 0; i < 50; i ++) {
    assert (bitset_get(set2, i) == (i%2));
  }

  // Push from 0
  bitset_push_at(set2, 0, 1);
  assert (bitset_get(set2, 0) == 1);
  for(int i = 1; i < 51; i ++) {
    assert (bitset_get(set2, i) == ((i-1)%2));
  }
  // Push from not aligned bit
  bitset_push_at(set2, 9, 1);
  for(int i = 1; i < 9; i ++) {
    assert (bitset_get(set2, i) == ((i-1)%2));
  }
  assert (bitset_get(set2, 9) == 1);
  for(int i = 10; i < 52; i ++) {
    assert (bitset_get(set2, i) == ((i)%2));
  }

  bool b;
  bitset_pop_back(NULL, set2);
  bitset_pop_back(&b, set2);
  assert(b == false);
  assert(bitset_size(set2) == 50);

  for(int i = 0; i < 200; i++)
    bitset_push_back(set, (i%2));
  bitset_pop_at(&b, set, 0);
  assert (b == false);
  assert (bitset_size(set) == 199);
  bitset_pop_at(&b, set, 0);
  assert (b == true);
  assert (bitset_size(set) == 198);
  bitset_pop_at(&b, set, 9);
  assert (b == true);
  assert (bitset_size(set) == 197);
  bitset_pop_at(&b, set, 9);
  assert (b == false);
  assert (bitset_size(set) == 196);

  bitset_clean(set);
  for(int i = 0; i < 200; i++)
    bitset_push_back(set, (i*17547854547UL)&1) ;
  bitset_it_t it;
  int n = 0;
  for(bitset_it(it, set); ! bitset_end_p(it); bitset_next(it)) {
    bool v = (n*17547854547UL)&1;
    bool c = *bitset_cref(it);
    assert (v == c);
    n++;
  }
#if 0
  n = 0;
  for M_CEACH (item, set, BITSET_OPLIST) {
    bool v = (n*17547854547UL)&1;
    bool c = *item;
    assert (v == c);
    n++;
    }
#endif

  assert(bitset_size(set2) == 50);
  assert(bitset_size(set) == 200);
  bitset_swap (set, set2);
  assert(bitset_size(set) == 50);
  assert(bitset_size(set2) == 200);
  
  bitset_clear(set);
  bitset_clear(set2);
}

static void test_str(void)
{
  bitset_t set1, set2;

  bitset_init(set1);
  bitset_init(set2);

  for(int i = 0; i < 200; i++)
    bitset_push_back(set1, (i*17547854547UL >> 4)&1) ;
  
  FILE *f = fopen ("a.dat", "wt");
  if (!f) abort();
  bitset_out_str(f, set1);
  fclose (f);

  f = fopen ("a.dat", "rt");
  if (!f) abort();
  bool b = bitset_in_str (set2, f);
  assert (b == true);
  assert (bitset_equal_p (set1, set2));
  fclose(f);

  string_t s;
  string_init (s);
  bitset_get_str(s, set1, false);
  assert (string_equal_str_p (s, "[01010110101101011010100101001010010101101011010110101001010010100101011010110101101010010100101001010110101101011010100101001010010101101011010110101001010010100101011010110101101010010100101001010110]"));

  string_clear(s);
  bitset_clear(set1);
  bitset_clear(set2);
}

static void test_logic(void)
{
  M_LET(s1, s2, BITSET_OPLIST) {
    bitset_set_str(s1, "[1010]");
    bitset_set_str(s2, "[1100]");
    bitset_and (s1, s2);
    bitset_set_str(s2, "[1000]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010]");
    bitset_set_str(s2, "[1100]");
    bitset_or (s1, s2);
    bitset_set_str(s2, "[1110]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010]");
    bitset_set_str(s2, "[1100]");
    bitset_xor (s1, s2);
    bitset_set_str(s2, "[0110]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010]");
    bitset_not (s1);
    bitset_set_str(s2, "[0101]");
    assert(bitset_equal_p(s1, s2));

    // Bigger set
    bitset_set_str(s1, "[10101010101010101010]");
    bitset_set_str(s2, "[110000000000000000000]");
    bitset_and (s1, s2);
    bitset_set_str(s2, "[10000000000000000000]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010000000000000000001]");
    bitset_set_str(s2, "[110000000000000000000]");
    bitset_or (s1, s2);
    bitset_set_str(s2, "[111000000000000000000]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010000000000000000001111111111110]");
    bitset_set_str(s2, "[1100000000000000000111111111111111]");
    bitset_xor (s1, s2);
    bitset_set_str(s2, "[0110000000000000000110000000000001]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010000000000000000000000000000000000]");
    bitset_not (s1);
    bitset_set_str(s2, "[0101111111111111111111111111111111111]");
    assert(bitset_equal_p(s1, s2));

  }
}

int main(void)
{
  test1();
  test_str();
  test_logic();
  exit(0);
}
