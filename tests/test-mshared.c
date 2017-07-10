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
#include <gmp.h>
#include <assert.h>

#include "m-shared.h"

#include "coverage.h"
START_COVERAGE
SHARED_PTR_DEF(int, int)
SHARED_PTR_DEF(mpz, mpz_t, M_CLASSIC_OPLIST(mpz))
END_COVERAGE

static int f(const shared_int_t p)
{
  return *shared_int_cref(p);
}

int main(void)
{
  shared_int_t p1, p2;

  int *p = M_ASSIGN_CAST(int*, malloc(sizeof(int)));
  assert (p != NULL);
  
  shared_int_init2(p1, p);
  assert(!shared_int_NULL_p(p1));
  assert(shared_int_ref(p1) != NULL);

  *shared_int_ref(p1) = 12;

  shared_int_init_set(p2, p1);
  assert(f(p2) == 12);

  shared_int_clean(p1);
  assert(shared_int_NULL_p(p1));
  assert(!shared_int_NULL_p(p2));
  assert(f(p2) == 12);
  
  shared_int_set(p1, p2);

  shared_int_clean(p2);
  assert(!shared_int_NULL_p(p1));  
  assert(shared_int_NULL_p(p2));
  assert(f(p1) == 12);
  shared_int_set(p2, p1);
  assert(!shared_int_NULL_p(p1));  
  assert(!shared_int_NULL_p(p2));
  assert(f(p2) == 12);

  shared_int_clean(p2);
  shared_int_clean(p1);
  
  assert(shared_int_NULL_p(p1));
  assert(shared_int_NULL_p(p2));

  shared_int_clear(p2);
  shared_int_clear(p1);
  
  shared_int_init2(p1, NULL);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);
                  
  shared_int_init(p1);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);

  shared_int_init_new(p1);
  assert(!shared_int_NULL_p(p1));
  shared_int_clear(p1);
  
  exit(0);
}
