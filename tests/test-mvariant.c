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
#include <gmp.h>

#include "m-string.h"
#include "m-variant.h"

START_COVERAGE
VARIANT_DEF2(pair,
             (key, int, M_DEFAULT_OPLIST),
             (value, float, M_DEFAULT_OPLIST) )
END_COVERAGE

static void test_pair(void)
{
  pair_t p;
  pair_init (p);

  assert (pair_empty_p(p));
  assert (!pair_key_p(p));
  assert (!pair_value_p(p));

  pair_set_key(p, 17);
  assert (!pair_empty_p(p));
  assert (pair_key_p(p));
  assert (!pair_value_p(p));
  int *i = pair_get_key(p);
  assert (*i == 17);
  float *f = pair_get_value(p);
  assert (f == NULL);
  
  pair_set_value(p, 42.0);
  assert (!pair_empty_p(p));
  assert (!pair_key_p(p));
  assert (pair_value_p(p));
  i = pair_get_key(p);
  assert (i == NULL);
  f = pair_get_value(p);
  assert (*f == 42.0);

  pair_set_empty(p);
  assert (pair_empty_p(p));
  assert (!pair_key_p(p));
  assert (!pair_value_p(p));
  i = pair_get_key(p);
  assert (i == NULL);
  f = pair_get_value(p);
  assert (f == NULL);

  pair_t p2;
  pair_set_key(p, 15);
  pair_init_set(p2, p);
  pair_set_value(p, 5.5);
  assert (!pair_empty_p(p2));
  assert (pair_key_p(p2));
  assert (!pair_value_p(p2));
  i = pair_get_key(p2);
  assert (*i == 15);

  assert (pair_value_p(p));
  pair_set(p, p2);
  assert (!pair_value_p(p));
  assert (pair_key_p(p));
  i = pair_get_key(p);
  assert (*i == 15);
  
  pair_clear(p2);
  pair_clear(p);
}

VARIANT_DEF2(triple,
             (s, string_t, STRING_OPLIST),
             (z, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear) )),
             (i, int, M_DEFAULT_OPLIST) )

static void
test_triple(void)
{
  triple_t t;
  triple_init(t);
  string_t s;
  string_init(s);
  mpz_t    z;
  mpz_init(z);

  mpz_set_str(z, "125567890456789345784567812345678", 10);
  string_set_str(s, "FHZKJHFKZUHFKZHFUHZFUHURHYERUYEUIRYEIURYIEYRIU");
  assert( triple_type(t) == triple_EMPTY);

  triple_set_s(t, s);
  assert( triple_type(t) == triple_s);
  triple_set_s(t, s);
  assert( triple_type(t) == triple_s);
  assert( string_equal_p (s, *triple_get_s(t)));
  
  triple_set_z(t, z);
  assert( triple_type(t) == triple_z);
  triple_set_z(t, z);
  assert( triple_type(t) == triple_z);
  assert( mpz_cmp (z, *triple_get_z(t)) == 0);

  mpz_clear(z);
  string_clear(s); 
  triple_clear(t);
}

VARIANT_DEF2(single,
             (s, string_t, STRING_OPLIST))
             
int main(void)
{
  test_pair();
  test_triple();
  return 0;
}

