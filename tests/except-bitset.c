/*
 * Test that the bitset module properly support exceptions
 * 
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
#include "test-obj-except.h"
#include "m-string.h"
#include "m-bitset.h"

M_TRY_DEF_ONCE()

static void test1(unsigned n)
{
    FILE *f = m_core_fopen ("a-ebitset.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(data, tmp, bitset_t) {
            for(unsigned i = 0; i < 100*n; i++) {
                bitset_push_back(data, (bool) (i % 2));
            }
            bitset_set(tmp, data);
            bitset_resize(tmp, 10000);
            bitset_reserve(tmp, 20000);
            bitset_push_at(tmp, 23, true);
            M_LET( (tmp2, tmp), bitset_t) {
                bitset_out_str(f, tmp2);
            }
            M_LET(str, string_t) {
                bitset_get_str(str, tmp, false);
                bool b = bitset_parse_str(tmp, string_get_cstr(str), NULL);
                assert(b);
            }
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-ebitset.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(data, bitset_t) {
            bool b = bitset_in_str(data, f);
            (void)b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);
}

int main(void)
{
    do_test1(test1);
    exit(0);
}
