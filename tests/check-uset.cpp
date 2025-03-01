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
#include <unordered_set>
#include "m-dict.h"

DICT_SET_DEF(dict_int, int)

// Container OPLIST
#define CONT_OPL   DICT_SET_OPLIST(dict_int, M_OPL_int())

// C++ Base class of the item in the container
#define BASE_CLASS int

// C++ Container class
#define CONT_CLASS std::unordered_set<int>

// Compare the M*LIB container a to the C++ container b
#define CMP_CONT(a, b) cmp_cont(a, b)

void cmp_cont(const dict_int_t a, const std::unordered_set<int> &b)
{
  size_t n = 0;
  auto          itb = b.begin();
  while (itb != b.end()) {
    const int &b1 = *itb;
    const int *b0 = dict_int_get(a, b1);
    assert(b0 != nullptr);
    assert(*b0 == b1);
    itb++;
    n++;
  }
  assert(dict_int_size(a) == n);
}

#define DEFAULT_NUMBER 1000000

#define HAVE_GET_KEY_THROUGH_FIND
#define DONT_HAVE_SEQUENCE_IT

#define push_back insert
#include "check-generic.hpp"
