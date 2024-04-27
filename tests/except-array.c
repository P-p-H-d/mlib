/*
 * Test that the array module properly support exceptions
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
#include "m-array.h"
#include "m-string.h"

M_TRY_DEF_ONCE()

ARRAY_DEF(array_obj, test_obj_except__t, TEST_OBJ_EXCEPT_OPLIST)
#define M_OPL_array_obj_t() ARRAY_OPLIST(array_obj, TEST_OBJ_EXCEPT_OPLIST)

static void test1(unsigned n)
{
    FILE *f = m_core_fopen ("a-earray.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(obj, test_obj_except__t)
        M_LET(array, tmp, array_obj_t) {
            for(unsigned i = 0; i < n; i++) {
                test_obj_except__set_ui(obj, i);
                array_obj_push_back(array, obj);
            }
            array_obj_set(tmp, array);
            M_LET( (tmp2, tmp), array_obj_t) {
                array_obj_out_str(f, tmp2);
            }
            array_obj_push_back(tmp, obj);
            array_obj_resize(tmp, 4);
            array_obj_push_new(tmp);
            array_obj_resize(tmp, 20);
            array_obj_set_at(tmp, 10, obj);
            array_obj_push_at(tmp, 10, obj);
            test_obj_except__t *ref = array_obj_safe_get(tmp, 30);
            assert(ref != NULL);
            array_obj_splice(tmp, array);
            M_LET(str, string_t) {
              array_obj_get_str(str, tmp, false);
              bool b = array_obj_parse_str(array, string_get_cstr(str), NULL);
              assert(b);
            }
            array_obj_insert_v(tmp, 12, 3);
            array_obj_emplace_back_ui(tmp, 345);
            array_obj_emplace_back_str(tmp, "345");
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-earray.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(obj, test_obj_except__t)
        M_LET(array, tmp, array_obj_t) {
            bool b = array_obj_in_str(array, f);
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
