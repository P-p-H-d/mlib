/*
 * Test that the bptree module properly support exceptions
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
#include "m-bptree.h"

M_TRY_DEF_ONCE()

BPTREE_DEF2(bptree_obj, 3, test_obj_except__t, TEST_OBJ_EXCEPT_OPLIST, test_obj_except__t, TEST_OBJ_EXCEPT_OPLIST)
#define M_OPL_bptree_obj_t() BPTREE_OPLIST2(bptree_obj, TEST_OBJ_EXCEPT_OPLIST, TEST_OBJ_EXCEPT_OPLIST)

static void test1(unsigned n)
{
    FILE *f = m_core_fopen ("a-ebptree.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(key, value, test_obj_except__t)
        M_LET(tree, tmp, bptree_obj_t) {
            printf("X1\n");
            for(unsigned i = 0; i < 10U * n; i++) {
                test_obj_except__set_ui(key, 3U * i + 1U);
                test_obj_except__set_ui(value, 11U * i + 7U);
                printf("X2.1 i=%u\n", i);
                bptree_obj_set_at(tree, key, value);
            }
            printf("X2\n");
            
            bptree_obj_set(tmp, tree);

            printf("X3\n");
            M_LET( (tmp2, tmp), bptree_obj_t) {
                bptree_obj_out_str(f, tmp2);
            }

            printf("X4\n");
            bptree_obj_emplace_key_ui_val_ui(tmp, 3U * (5U * n) + 1U, 4000U + n);

            printf("X9\n");
            test_obj_except__set_ui(key, 100U * n + 1U);
            test_obj_except__t *ref = bptree_obj_safe_get(tmp, key);
            assert(ref != NULL);
            test_obj_except__set_ui(*ref, 5000U + n);

            test_obj_except__t *min_ref = bptree_obj_min(tmp);
            test_obj_except__t *max_ref = bptree_obj_max(tmp);
            assert(min_ref != NULL);
            assert(max_ref != NULL);

            printf("X10\n");
            M_LET(str, string_t) {
                bptree_obj_get_str(str, tmp, false);
                bool b = bptree_obj_parse_str(tree, string_get_cstr(str), NULL);
                assert(b);
            }

            printf("X11\n");
            bptree_obj_set(tree, tmp);

            M_LET( (tmp2, tree), bptree_obj_t) {
                bptree_obj_set(tmp, tmp2);
            }

            printf("X12\n");
            bptree_obj_it_t it;
            test_obj_except__set_ui(key, 3U * (7U * n) + 1U);
            bptree_obj_it_from(it, tmp, key);
            if (!bptree_obj_end_p(it)) {
                const bptree_obj_itref_t *item = bptree_obj_cref(it);
                assert(item->key_ptr != NULL);
                assert(item->value_ptr != NULL);
                assert(test_obj_except__cmp(*item->key_ptr, key) >= 0);
            }
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-ebptree.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(tree, bptree_obj_t) {
            bool b = bptree_obj_in_str(tree, f);
            (void)b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    printf("X13\n");
    printf("CPT=%d\n", test_obj_except__init_counter);
}

int main(void)
{
    do_test_exception(test1);
    exit(0);
}
