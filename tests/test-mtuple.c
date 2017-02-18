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
#include "m-tuple.h"

TUPLE_DEF2(pair,
           (key, string_t, (INIT(string_init), INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear))),
           (value, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear) )))

// No cmp defined
TUPLE_DEF2(triple,
           (key, string_t, (INIT(string_init), INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear), CMP(string_cmp))),
           (value, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear) )),
           (data, const char *, ()))

// cmp shall be defined
TUPLE_DEF2(pair3,
           (key, string_t, (INIT(string_init), INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear), CMP(string_cmp), HASH(string_hash), EQUAL(string_equal_p))),
           (value, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), CMP(mpz_cmp) )))

TUPLE_DEF2(pair_str,
           (vala, string_t, STRING_OPLIST),
           (valb, string_t, STRING_OPLIST))

static void check_io(void)
{
  M_LET(s, STRING_OPLIST)
    M_LET(pair, TUPLE_OPLIST(pair_str, STRING_OPLIST, STRING_OPLIST))
    M_LET(pair2, TUPLE_OPLIST(pair_str, STRING_OPLIST, STRING_OPLIST)) {
    string_set_str(pair->vala, "Hello");
    string_set_str(pair->valb, "World");
    pair_str_get_str(s, pair, false);
    assert(string_cmp_str(s, "(\"Hello\",\"World\")") == 0);

    FILE *f = fopen("a.dat", "wt");
    if (!f) abort();
    pair_str_out_str(f, pair);
    fclose(f);
    
    f = fopen("a.dat", "rt");
    if (!f) abort();
    bool b = pair_str_in_str(pair2, f);
    assert (b);
    fclose(f);
    assert (pair_str_equal_p(pair, pair2) == true);
  }
}

int main(void)
{
  pair_t p1, p2;

  pair_init (p1);
  string_set_str(p1->key, "HELLO");
  mpz_set_str(p1->value, "1742", 10);
  pair_init_set (p2, p1);
  assert(mpz_cmp_ui(p2->value, 1742) == 0);
  assert(string_equal_str_p(p2->key, "HELLO"));
  string_set_str(p2->key, "HELLO WORLD");
  mpz_set_str(p2->value, "174217", 10);
  pair_set(p1, p2);
  assert(mpz_cmp_ui(p1->value, 174217) == 0);
  assert(string_equal_str_p(p1->key, "HELLO WORLD"));
  pair_clear(p1);
  pair_clear(p2);

  string_t s;
  mpz_t z;
  pair3_t p3, p4;
  string_init_set_str(s, "HELLO");
  mpz_init_set_ui (z, 1442);
  pair3_init_set2(p3, s, z);
  pair3_init_set2(p4, s, z);
  int i = pair3_cmp (p3, p4);
  assert (i == 0);
  mpz_set_ui (z, 1443);
  pair3_set_value(p4, z);
  i = pair3_cmp (p3, p4);
  assert (i < 0);
  mpz_set_ui (z, 1442);
  string_set_str(s, "HELLN");
  pair3_set_key(p4, s);
  i = pair3_cmp (p3, p4);
  assert (i > 0);
  pair3_clear(p3);
  pair3_clear(p4);
  string_clear(s);
  mpz_clear(z);

  check_io();
  return 0;
}
