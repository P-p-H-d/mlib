/*
 * Test that the list module properly support exceptions
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

M_TRY_DEF_ONCE()

static void test1(unsigned n)
{
    FILE *f = m_core_fopen ("a-estring.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(str, tmp, string_t) {
            for(unsigned i = 0; i < 10*n; i++) {
                string_push_back(str, (char) (i + 'A'));
            }
            string_set(tmp, str);
            M_LET( (tmp2, tmp), string_t) {
                string_out_str(f, tmp2);
            }
            M_LET(tmp2, string_t) {
                string_set_str(tmp2, "This is a long string with more characters than needed to have a long string");
            }
            M_LET(tmp2, string_t) {
                string_set_strn(tmp2, "This is a long string with more characters than needed to have a long string", 40);
            }
            M_LET(tmp2, string_t) {
                string_cat_str(tmp, " Let's perform a concatenate operation.");
                string_cat(tmp2, tmp);
            }
            M_LET(tmp2, string_t) {
                string_set_str(tmp2, "This is a long string with more characters than needed to have a long string");
                string_replace_str(tmp2, "string", "long string");
            }
            M_LET(tmp2, string_t) {
                string_set_str(tmp2, "This is a long string with more characters than needed to have a long string");
                string_replace_at(tmp2, 10, 3, "Powers Powers Powers Powers Powers Powers Powers Powers Powers Powers");
            }
            M_LET(tmp2, string_t) {
                string_set_str(tmp2, "string string string string string string string string string string string string string string string string ");
                string_replace_all_str(tmp2, "string", "very long string");
            }
            M_LET(tmp2, string_t) {
                string_set_ui(tmp2, 4000000000UL);
            }
            M_LET(tmp2, string_t) {
                string_set_si(tmp2, -2000000000L);
            }
            M_LET(tmp2, string_t) {
                string_printf(tmp2, "%s = %d", "This is the first variable I want to set", 2000000000L);
                string_cat_printf(tmp2, "%s = %d", "This is the first variable I want to set", 2000000000L);
            }
            string_get_str(str, tmp, false);
            bool b = string_parse_str(tmp, string_get_cstr(str), NULL);
            string_it_t it;
            unsigned i = 0;
            for( string_it(it, str); !string_end_p(it); string_next(it)) {
                string_it_set_ref(it, str, ' ' + i++);
            }
            assert(b);
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-estring.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(str, string_t) {
            bool b = string_in_str(str, f);
            (void)b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-estring.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(str, string_t) {
            bool b = m_string_fgets(str, f, M_STRING_READ_FILE);
            (void)b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-estring.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(str, string_t) {
            bool b = string_fget_word(str, " \t,;.:/!=+", f);
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
