/*
 * Copyright (c) 2021-2025, Patrick Pelissier
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
#include <list>
#include "m-list.h"

LIST_DUAL_PUSH_DEF(list_int, int)

// Container OPLIST
#define CONT_OPL   LIST_OPLIST(list_int, M_OPL_int())

// C++ Base class of the item in the container
#define BASE_CLASS int

// C++ Container class
#define CONT_CLASS std::list<int>

// Compare the M*LIB container a to the C++ container b
#define CMP_CONT(a, b) cmp_cont(a, b)

void cmp_cont(const list_int_t a, const std::list<int> &b)
{
  list_int_it_t ita;
  list_int_it(ita, a);
  auto          itb = b.rbegin();
  while (itb != b.rend()) {
    assert(!list_int_end_p(ita));
    const int *b0 = list_int_cref(ita);
    const int &b1 = *itb;
    assert(*b0 == b1);
    itb++;
    list_int_next(ita);
  }
  assert(list_int_end_p(ita));
}

#define DEFAULT_NUMBER 100000

#define HAVE_PUSH_FRONT
#define HAVE_REVERSE_IT

#include "check-generic.hpp"
