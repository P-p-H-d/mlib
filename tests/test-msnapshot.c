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
#include <gmp.h> // For testing purpose only.
#include "m-snapshot.h"

SNAPSHOT_DEF(uint, unsigned int)
SNAPSHOT_DEF(mpz, mpz_t,
             (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)))

static void test_uint(void)
{
  unsigned int *p_w;
  const unsigned int *p_r;
  snapshot_uint_t t;
  snapshot_uint_init(t);

  p_r = snapshot_uint_look(t);
  assert (p_r == snapshot_uint_get_consummed(t));
  assert(*p_r == 0);
  assert(snapshot_uint_look(t) == p_r);
  
  p_w = snapshot_uint_take(t);
  assert (p_w != p_r);
  *p_w = 1;
  assert (*p_r == 0);
  assert (snapshot_uint_look(t) != p_r);
  assert (*snapshot_uint_look(t) == 0);
  p_r = snapshot_uint_look(t);
  
  p_w = snapshot_uint_take(t);
  *p_w = 2;
  assert (*snapshot_uint_look(t) == 1);
  
  p_w = snapshot_uint_take(t);
  assert (p_w == snapshot_uint_get_produced(t));
  *p_w = 3;
  p_w = snapshot_uint_take(t);
  *p_w = 4;
  assert (*snapshot_uint_look(t) == 3);

  snapshot_uint_t t2;
  snapshot_uint_init_set (t2, t);
  snapshot_uint_clear(t);
  assert (*snapshot_uint_look(t) == 3);
  snapshot_uint_init(t);
  snapshot_uint_set (t, t2);
  assert (*snapshot_uint_look(t) == 3);
}

int main(void)
{
  test_uint();
  exit(0);
}

