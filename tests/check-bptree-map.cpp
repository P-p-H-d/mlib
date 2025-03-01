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
#include <map>
#include "m-bptree.h"

BPTREE_DEF2(tree_int, 5, int, int)

// Container OPLIST
#define CONT_OPL   BPTREE_OPLIST2(tree_int, M_OPL_int(), M_OPL_int() )

// C++ Base class of the item in the container
#define BASE_CLASS int

// C++ Container class
#define CONT_CLASS std::map<int,int>

// Compare the M*LIB container a to the C++ container b
#define CMP_CONT(a, b) cmp_cont(a, b)

#define CLASS_IT_TO_INT(it) (it)->second
#define TYPE_IT_TO_INT(it)  (it)->value

void cmp_cont(const tree_int_t a, const std::map<int,int> &b)
{
  tree_int_it_t ita;
  tree_int_it(ita, a);
  auto          itb = b.begin();
  while (itb != b.end()) {
    assert(!tree_int_end_p(ita));
    const int *k0 = tree_int_cref(ita)->key_ptr;
    const int &k1 = itb->first;
    assert(*k0 == k1);
    const int *v0 = tree_int_cref(ita)->value_ptr;
    const int &v1 = itb->second;
    assert(*v0 == v1);
    itb++;
    tree_int_next(ita);
  }
  assert(tree_int_end_p(ita));
}

#define DEFAULT_NUMBER 1000000

#define HAVE_GET_KEY_THROUGH_FIND
#define HAVE_SET_KEY
#define HAVE_KEY_PTR

#define push_back insert
#include "check-generic.hpp"
