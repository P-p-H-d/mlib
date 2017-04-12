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

#include <stdbool.h>
#include <assert.h>
#include <gmp.h>

#include "m-core.h"

static void test_cat(void)
{
#define AandB true
#define AandBandC true
  assert (M_C(A,andB));
  assert (M_C3(A,and,B));
  assert (M_C4(A,and,B,andC));
}

static void test_power2(void)
{
  assert (M_POWEROF2_P(1));
  assert (M_POWEROF2_P(2));
  assert (!M_POWEROF2_P(3));
  assert (M_POWEROF2_P(4));
  assert (!M_POWEROF2_P(5));
  assert (!M_POWEROF2_P(6));
  assert (!M_POWEROF2_P(7));
  assert (M_POWEROF2_P(8));
}

static void test_incdec(void)
{
  assert (M_INC(0) == 1);
  assert (M_INC(1) == 2);
  assert (M_INC(2) == 3);
  assert (M_INC(3) == 4);
  assert (M_INC(4) == 5);
  assert (M_INC(5) == 6);
  assert (M_INC(6) == 7);
  assert (M_INC(7) == 8);
  assert (M_INC(8) == 9);
  assert (M_INC(9) == 10);
  assert (M_INC(10) == 11);
  assert (M_INC(11) == 12);
  assert (M_INC(12) == 13);
  assert (M_INC(13) == 14);
  assert (M_INC(14) == 15);
  assert (M_INC(15) == 16);
  assert (M_INC(16) == 17);
  assert (M_INC(17) == 18);
  assert (M_INC(18) == 19);
  assert (M_INC(19) == 20);
  assert (M_INC(20) == 21);
  assert (M_INC(21) == 22);
  assert (M_INC(22) == 23);
  assert (M_INC(23) == 24);
  assert (M_INC(24) == 25);
  assert (M_INC(25) == 26);
  assert (M_INC(26) == 27);
  assert (M_INC(27) == 28);
  assert (M_INC(28) == 29);

  assert ((0) == M_DEC(1));
  assert ((1) == M_DEC(2));
  assert ((2) == M_DEC(3));
  assert ((3) == M_DEC(4));
  assert ((4) == M_DEC(5));
  assert ((5) == M_DEC(6));
  assert ((6) == M_DEC(7));
  assert ((7) == M_DEC(8));
  assert ((8) == M_DEC(9));
  assert ((9) == M_DEC(10));
  assert ((10) == M_DEC(11));
  assert ((11) == M_DEC(12));
  assert ((12) == M_DEC(13));
  assert ((13) == M_DEC(14));
  assert ((14) == M_DEC(15));
  assert ((15) == M_DEC(16));
  assert ((16) == M_DEC(17));
  assert ((17) == M_DEC(18));
  assert ((18) == M_DEC(19));
  assert ((19) == M_DEC(20));
  assert ((20) == M_DEC(21));
  assert ((21) == M_DEC(22));
  assert ((22) == M_DEC(23));
  assert ((23) == M_DEC(24));
  assert ((24) == M_DEC(25));
  assert ((25) == M_DEC(26));
  assert ((26) == M_DEC(27));
  assert ((27) == M_DEC(28));
  assert ((28) == M_DEC(29));

  assert( M_ADD(2,5) == M_SUB(10,3));
  assert( M_ADD(5,15) == M_SUB(25,5));
}

static void test_return(void)
{
  assert (M_RET_ARG1(1, 0));
  assert (M_RET_ARG2(0, 1, 0));
  assert (M_RET_ARG3(0, 0, 1, 0));
  assert (M_RET_ARG27(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0));
}

static void test_invert(void)
{
  assert (M_RET_ARG1 M_INVERT(0,1));
  assert (M_RET_ARG1 M_INVERT(0,0,1));
  assert (M_RET_ARG1 M_INVERT(0,0,0,1));
  assert (M_RET_ARG1 M_INVERT(0,0,0,0,1));
}

static void test_min(void)
{
  assert (M_MIN(1,2) == 1);
  assert (M_MIN(1,-2) == -2);
  assert (M_MAX(1,2) == 2);
  assert (M_MAX(1,-2) == 1);
}

static void test_empty(void)
{
  assert (M_EMPTY_P() == true);
  assert (M_EMPTY_P(1) == false);
  assert (M_EMPTY_P(,) == false);
  assert (M_EMPTY_P(()) == false);
  assert (M_EMPTY_P("123") == false);
}

#define f(n) (n)*(n) +

static void test_map(void)
{
  assert (M_MAP (f, 1) 0 == 1);
  assert (M_MAP (f, 1, 2) 0 == 5);
  assert (M_MAP (f, 1, 2, 3) 0 == 14);
}

static void test_let(void)
{
  M_LET(z, M_CLASSIC_OPLIST(mpz)) {
    mpz_add(z,z,z);
  }
}

static void test_va(void)
{
  assert (M_RET_ARG1 (M_VA(a),zz) == 1);
  assert (M_RET_ARG2 (M_VA(17),zz) == 17);
  assert (M_RET_ARG1 (M_VA(a,b,c),zz) == 3);
  assert (M_RET_ARG2 (M_VA(17,42,59),zz) == 17);
}

int main(void)
{
  test_cat();
  test_power2();
  test_incdec();
  test_return();
  test_invert();
  test_min();
  test_empty();
  test_map();
  test_let();
  test_va();
  exit(0);
}
