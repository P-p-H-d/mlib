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
#include "m-list.h"
#include "m-string.h"

M_TRY_DEF_ONCE()

LIST_DEF(list_obj, test_obj_except__t, TEST_OBJ_EXCEPT_OPLIST)
#define M_OPL_list_obj_t() LIST_OPLIST(list_obj, TEST_OBJ_EXCEPT_OPLIST)

LIST_DUAL_PUSH_DEF(list2_obj, test_obj_except__t, TEST_OBJ_EXCEPT_OPLIST)
#define M_OPL_list2_obj_t() LIST_OPLIST(list2_obj, TEST_OBJ_EXCEPT_OPLIST)

static void test1(unsigned n)
{
    FILE *f = m_core_fopen ("a-elist.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(obj, test_obj_except__t)
        M_LET(list, tmp, list_obj_t) {
            for(unsigned i = 0; i < n; i++) {
                test_obj_except__set_ui(obj, i);
                list_obj_push_back(list, obj);
            }
            list_obj_set(tmp, list);
            M_LET( (tmp2, tmp), list_obj_t) {
                list_obj_out_str(f, tmp2);
            }
            list_obj_push_new(tmp);
            list_obj_splice(tmp, list);
            M_LET(str, string_t) {
              list_obj_get_str(str, tmp, false);
              bool b = list_obj_parse_str(list, string_get_cstr(str), NULL);
              assert(b);
            }
            list_obj_it_t it; list_obj_it(it, tmp); list_obj_next(it);
            list_obj_insert(tmp, it, obj);
            list_obj_emplace_back_ui(list, 345);
            list_obj_emplace_back_str(list, "345");
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-elist.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(obj, test_obj_except__t)
        M_LET(list, tmp, list_obj_t) {
            bool b = list_obj_in_str(list, f);
            (void)b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);
}

static void test2(unsigned n)
{
    FILE *f = m_core_fopen ("a-elist.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(obj, test_obj_except__t)
        M_LET(list, tmp, list2_obj_t) {
            for(unsigned i = 0; i < n; i++) {
                test_obj_except__set_ui(obj, i);
                list2_obj_push_back(list, obj);
            }
            list2_obj_set(tmp, list);
            M_LET( (tmp2, tmp), list2_obj_t) {
                list2_obj_out_str(f, tmp2);
            }
            list2_obj_push_back_new(tmp);
            list2_obj_splice(tmp, list);
            M_LET(str, string_t) {
              list2_obj_get_str(str, tmp, false);
              bool b = list2_obj_parse_str(list, string_get_cstr(str), NULL);
              assert(b);
            }
            list2_obj_it_t it; list2_obj_it(it, tmp); list2_obj_next(it);
            list2_obj_insert(tmp, it, obj);
            list2_obj_emplace_back_ui(tmp, 345);
            list2_obj_emplace_back_str(tmp, "345");
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-elist.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(obj, test_obj_except__t)
        M_LET(list, tmp, list2_obj_t) {
            bool b = list2_obj_in_str(list, f);
            (void)b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);
}

static void test3(unsigned n)
{
    M_TRY(test1) {
        M_LET(obj, test_obj_except__t)
        M_LET(list, tmp, list2_obj_t) {
            for(unsigned i = 0; i < n; i++) {
                test_obj_except__set_ui(obj, i);
                list2_obj_push_front(list, obj);
            }
            list2_obj_push_front_new(list);
            list2_obj_emplace_front_ui(list, 345);
            list2_obj_emplace_front_str(list, "345");
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
}

int main(void)
{
    do_test1(test1);
    do_test1(test2);
    do_test1(test3);
    exit(0);
}
