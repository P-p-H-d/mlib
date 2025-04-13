/*
 * Copyright (c) 2017-2025, Patrick Pelissier
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

#include "test-obj.h"
#include "m-string.h"
#include "m-array.h"
#include "m-list.h"
#include "m-generic.h"
#include "coverage.h"

// Generic is not supported if not C11
// TCC has some issues in the preprocessing.
// See https://lists.nongnu.org/archive/html/tinycc-devel/2023-12/msg00004.html
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__TINYC__)

ARRAY_DEF(array_int, int)
#define M_OPL_array_int_t() ARRAY_OPLIST(array_int, M_BASIC_OPLIST)

LIST_DEF(list_int, int)
#define M_OPL_list_int_t() LIST_OPLIST(list_int, M_BASIC_OPLIST)

static void h(string_t x)
{
	string_set_str(x, "TEST STRING");
}

// Global variable to test genericity on a global variable
const string_t gx;

#define FLT1 (GENTYPE(float), TYPE(float), INIT(M_INIT_BASIC), INIT_SET(M_SET_BASIC), SET(M_SET_BASIC),               \
   CLEAR(M_NOTHING_DEFAULT) )
#define INT1 (GENTYPE(int), TYPE(int), INIT(M_INIT_BASIC), INIT_SET(M_SET_BASIC), SET(M_SET_BASIC),               \
   CLEAR(M_NOTHING_DEFAULT) )

//NOTE: string_t is registered by the CORE component of the MLIB organization

#define M_GENERIC_ORG_2() (USER)
#define M_GENERIC_ORG_USER_COMP_1() (CORE)
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_6() FLT1
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_1() INT1

#define M_GENERIC_ORG_3() (ARRAY)
#define M_GENERIC_ORG_ARRAY_COMP_5() (AINT)
#define M_GENERIC_ORG_ARRAY_COMP_AINT_OPLIST_6() M_OPL_array_int_t()
#define M_GENERIC_ORG_ARRAY_COMP_10() (LINT)
#define M_GENERIC_ORG_ARRAY_COMP_LINT_OPLIST_30() M_OPL_list_int_t()

static bool test_empty(const string_t p)
{
  return empty_p(p);
}

static void test_string(string_t p)
{
  string_t s , d;

  init(s);
  h(s);
  init_set(d, s);
  h(d);
  init_set(p, s);
  bool b = test_empty(p);
  assert(!b);
  push(p, 'c');
  out_str(stdout, p);
  clear(s);
  clear(d);

  float f;
  init(f);
  clear(f);

  clear(p);
}

static void test_array(void)
{
	M_LET(a, array_int_t) {
    bool b = empty_p(a);
    assert(b);
		push(a, 14);
    b = empty_p(a);
    assert(!b);
		push(a, 15);
		out_str(stdout, a);
		for each(it, a) {
			printf("=%d, ", *it);
		}
    int z;
    pop(&z, a);
    assert(z == 15);
 	}
	// Just change the type, same code!
	M_LET(a, list_int_t) {
		push(a, 14);
		push(a, 15);
		out_str(stdout, a);
		for each(it, a) {
			printf("=%d, ", *it);
		}
 	}
}

static void test_print(void)
{
    int x = 0;
    M_PRINT("X= ", x, "\n");
    M_LET( (s, "Hello"), string_t) {
        M_PRINT("s=", s, "\n");
    }
    M_LET( (a, 3,4,5), array_int_t) {
        M_PRINT("a=", a, "\n");
    }
}

int main(void)
{
	FILE *f = freopen("atmp-generic.dat", "w", stdout);
	if (!f) abort();
	string_t p;
	test_string(p);
	test_array();
	test_print();
	exit(0);
}

#else
int main(void)
{
  exit(0);
}
#endif
