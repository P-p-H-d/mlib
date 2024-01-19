/*
 * Copyright (c) 2017-2024, Patrick Pelissier
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
#include <stdint.h>
#include "m-dict.h"

DICT_DEF2(map_foo, int, float)
DICT_DEF2(map_bar, long, float)
DICT_DEF2(map_buz, char, float)

extern void foo(map_foo_t our_map );
void foo(map_foo_t our_map )
{
  for (int i = 0; i < 10; ++i)
    map_foo_set_at(our_map, i, 0.5f * (float) i );
  map_foo_set_at( our_map, 32, 0.34f  );
  map_foo_set_at( our_map, 233, 0.324f  );
  printf( "%f\n", *map_foo_get( our_map, 5 ) );
}

extern void bar(map_bar_t our_map);
void bar(map_bar_t our_map)
{
  for (long i = 0; i < 10; ++i)
    map_bar_set_at( our_map, i, 0.5f * (float) i );
  map_bar_set_at( our_map, 32, 0.34f  );
  map_bar_set_at( our_map, 233, 0.324f );
  printf( "%f\n", *map_bar_get( our_map, 5 ) );
}

extern void buz( map_buz_t our_map);
void buz( map_buz_t our_map)
{
  for (char i = 0; i < 10; ++i)
    map_buz_set_at( our_map, i, 0.5f * (float) i );
  map_buz_set_at( our_map, 32, 0.34f  );
  map_buz_set_at( our_map, (char) 233, 0.324f  );
}
