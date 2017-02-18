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

#include "m-list.h"

LIST_DEF(dbl, double)

void f_push_back(list_dbl_t list)
{
  list_dbl_push_back(list, 2.0);
}

double f_iterator(list_dbl_t list)
{
  list_it_dbl_t it;
  double s = 0.0;
  for(list_dbl_it(it, list); !list_dbl_end_p(it); list_dbl_next(it))
    s += *list_dbl_cref(it);
  return s;
}

struct tata {
  int x;
};
struct tutu {
  void (* const init) (void);
  void (* const add) (int *t);
  struct tata *data;
};

int f(int p)
{
  struct tata el_data;
  inline void __attribute__((always_inline)) el_init (void) { el_data.x = 0; };
  inline void __attribute__((always_inline)) el_add (int *t) { el_data.x += *t; };
  const struct tutu el = { .init = el_init, .add = el_add, .data = &el_data };

  el.init();
  el.add (&p);
  return 0;
}

