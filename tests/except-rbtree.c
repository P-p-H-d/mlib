/*
 * Test that the rbtree module properly support exceptions
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
#include "m-rbtree.h"

M_TRY_DEF_ONCE()

RBTREE_DEF(rbtree_obj, test_obj_except__t, TEST_OBJ_EXCEPT_OPLIST)
#define M_OPL_rbtree_obj_t() RBTREE_OPLIST(rbtree_obj, TEST_OBJ_EXCEPT_OPLIST)

static void test1(unsigned n)
{
    FILE *f = m_core_fopen ("a-erbtree.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(val, test_obj_except__t)
        M_LET(tree, tmp, rbtree_obj_t) {
            for (unsigned i = 0; i < 10U * n; i++) {
                test_obj_except__set_ui(val, 3U * i + 1U);
                rbtree_obj_push(tree, val);
            }

            rbtree_obj_t tmp3;
            rbtree_obj_init_set(tmp3, tree);
            rbtree_obj_clear(tmp3);

            rbtree_obj_set(tmp, tree);

            M_LET( (tmp2, tmp), rbtree_obj_t) {
                rbtree_obj_out_str(f, tmp2);
            }

            rbtree_obj_emplace_ui(tmp, 5000U + n);

            test_obj_except__t *min_ref = rbtree_obj_min(tmp);
            test_obj_except__t *max_ref = rbtree_obj_max(tmp);
            assert(min_ref != NULL);
            assert(max_ref != NULL);

            M_LET(str, string_t) {
                rbtree_obj_get_str(str, tmp, false);
                bool b = rbtree_obj_parse_str(tree, string_get_cstr(str), NULL);
                assert(b);
            }

            rbtree_obj_it_t it;
            test_obj_except__set_ui(val, 3U * (7U * n) + 1U);
            rbtree_obj_it_from(it, tmp, val);
            if (!rbtree_obj_end_p(it)) {
                const test_obj_except__t *item = rbtree_obj_cref(it);
                assert(item != NULL);
            }
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen ("a-erbtree.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(tree, rbtree_obj_t) {
            bool b = rbtree_obj_in_str(tree, f);
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
