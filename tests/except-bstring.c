/*
 * Test that the bstring module properly support exceptions
 * 
 * Copyright (c) 2017-2026, Patrick Pelissier
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
#include "m-bstring.h"

M_TRY_DEF_ONCE()

static void test1(unsigned n)
{
    FILE *f = m_core_fopen ("a-ebstring.dat", "wb");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(bstr, tmp, bstring_t) {
            for(unsigned i = 0; i < 10*n; i++) {
                m_bstring_push_back(bstr, (uint8_t)(i & 0xFF));
            }
            m_bstring_set(tmp, bstr);
            M_LET( (tmp2, tmp), bstring_t) {
                m_bstring_fwrite(f, tmp2);
            }
            M_LET(tmp2, bstring_t) {
                const uint8_t data[] = "This is a long byte string with more bytes than needed to have a long byte string";
                m_bstring_push_back_bytes(tmp2, sizeof data, data);
            }
            M_LET(tmp2, bstring_t) {
                const uint8_t data[] = "This is a long byte string with more bytes than needed to have a long byte string";
                m_bstring_push_back_bytes(tmp2, 40, data);
            }
            M_LET(tmp2, bstring_t) {
                const uint8_t data[] = "Extra bytes to concatenate to the byte string";
                m_bstring_push_back_bytes(tmp, sizeof data, data);
                m_bstring_set(tmp2, tmp);
            }
            M_LET(tmp2, bstring_t) {
                const uint8_t data[] = "This is a long byte string with more bytes than needed to have a long byte string";
                m_bstring_push_back_bytes(tmp2, sizeof data, data);
                m_bstring_push_bytes_at(tmp2, 10, 3, (const uint8_t *)"XYZ");
            }
            M_LET(tmp2, bstring_t) {
                m_bstring_resize(tmp2, 100);
            }
            M_LET(tmp2, bstring_t) {
                m_bstring_reserve(tmp2, 200);
            }
            M_LET(tmp2, bstring_t) {
                m_bstring_splice(tmp2, bstr);
            }
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-ebstring.dat", "rb");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(bstr, bstring_t) {
            bool b = m_bstring_fread(bstr, f, 10*n);
            (void)b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);
}

int main(void)
{
    do_test_exception(test1);
    exit(0);
}
