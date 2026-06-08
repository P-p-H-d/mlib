/*
 * Test that the shared-ptr module properly support exceptions
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
#include "m-shared-ptr.h"

M_TRY_DEF_ONCE()

SHARED_PTR_DEF(shared_estring, string_t, STRING_OPLIST)
#define M_OPL_shared_estring_ptr_t() SHARED_DATA_OPLIST(shared_estring, STRING_OPLIST)

static void test1(unsigned n)
{
    FILE *f = m_core_fopen("a-eshared-ptr.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(p, q, shared_estring_ptr_t)
        M_LET((r, q), (s, q), shared_estring_ptr_t) {
            assert( p != NULL && q != NULL && r != NULL && s != NULL);
            shared_estring_remake(q, "Start");
            shared_estring_remake(s, "Restart");
            shared_estring_copy(p, q);
            M_LET((init, ("Init value")), string_t) {
                shared_estring_t *tmp = shared_estring_new_from(init);
                shared_estring_release(tmp);
            }
            shared_estring_swap(p, r);
            shared_estring_reset(p);

            for (unsigned i = 0; i < 8U * n; i++) {
                shared_estring_push(p, (m_string_unicode_t) ('A' + (i % 26U)));
            }

            shared_estring_set(&r, p);

            M_LET(str, string_t) {
                shared_estring_get_str(str, r, false);
                bool b = shared_estring_parse_str(p, string_get_cstr(str), NULL);
                assert(b);
            }

            shared_estring_out_str(f, p);
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen("a-eshared-ptr.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(p, shared_estring_ptr_t) {
            bool b = shared_estring_in_str(p, f);
            (void) b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);
}

#include "m-array.h"

ARRAY_DEF(int_array, int)
#define M_OPL_int_array_t() ARRAY_OPLIST(int_array, M_DEFAULT_OPLIST)
SHARED_PTR_DEF(shared_int_array, int_array_t, M_OPL_int_array_t())
// Note: shared_int_array_ptr_t doesn't really exist as a type.
// We use it as a key for M_LET so that it still generates the proper type (shared_int_array_t*)
// with a proper release handling.
#define M_OPL_shared_int_array_ptr_t() SHARED_PTR_OPLIST(shared_int_array, M_OPL_int_array_t())

static void test2(unsigned n)
{
    M_TRY(test1) {
        M_LET( p, q, r, s, shared_int_array_ptr_t) {
            // p, q, r, s are initialized to NULL since we use SHARED_PTR_OPLIST 
            // instead of SHARED_DATA_OPLIST (to have a pointer semantic), so we can reinitialize them.
            p = shared_int_array_new();
            q = shared_int_array_new();
            r = shared_int_array_clone(q);
            s = shared_int_array_acquire(q);

            for (unsigned i = 0; i < 8U * n; i++) {
                shared_int_array_push(p, (int) i);
            }

            shared_int_array_copy(q, p);
            shared_int_array_set(&r, p);
            shared_int_array_set_at(s, 4U * n, 42);

            int x = -1;
            shared_int_array_safe_get(&x, s, 8U * n + 3U);
            assert(x == 0 || x == 42);
            shared_int_array_safe_get(&x, s, 0);

            shared_int_array_pop(&x, p);

            shared_int_array_swap(p, r);
            shared_int_array_reset(q);
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
}

ARRAY_DEF(string_array, string_t)
#define M_OPL_string_array_t() ARRAY_OPLIST(string_array, M_STRING_OPLIST)
SHARED_PTR_DEF(shared_string_array, string_array_t, M_OPL_string_array_t())
#define M_OPL_shared_string_array_ptr_t() SHARED_PTR_OPLIST(shared_string_array, M_OPL_string_array_t())

static void test3(unsigned n)
{
    FILE *f = m_core_fopen("a-eshared-ptr-string-array.dat", "wt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET( p, q, r, s, shared_string_array_ptr_t) {
            // p, q, r, s are initialized to NULL since we use SHARED_PTR_OPLIST 
            // instead of SHARED_DATA_OPLIST (to have a pointer semantic), so we can reinitialize them.
            p = shared_string_array_new();
            q = shared_string_array_new();
            r = shared_string_array_clone(q);
            s = shared_string_array_acquire(q);

            for (unsigned i = 0; i < n; i++) {
                M_LET((str, ("value")), string_t) {
                    string_cat_printf(str, "-%u", i);
                    shared_string_array_push(p, str);
                }
            }

            string_t moved;
            string_init_set_str(moved, "moved");
            shared_string_array_push_move(p, &moved);
            // Note: moved is cleared by push_move, so no need to clear it here.

            shared_string_array_emplace(p, "emplaced");
            bool b = shared_string_array_try_emplace(p, "try-emplaced");
            assert(b);

            shared_string_array_copy(q, p);
            shared_string_array_set(&r, p);

            M_LET(str, string_t) {
                shared_string_array_safe_get(&str, s, 2U * n + 3U);
                shared_string_array_get_str(str, r, false);
                b = shared_string_array_parse_str(p, string_get_cstr(str), NULL);
                assert(b);
            }

            shared_string_array_swap(p, r);
            shared_string_array_out_str(f, p);
            shared_string_array_reset(q);
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);

    f = m_core_fopen("a-eshared-ptr-string-array.dat", "rt");
    assert(f != NULL);
    M_TRY(test1) {
        M_LET(p, shared_string_array_ptr_t) {
            p = shared_string_array_new();
            bool b = shared_string_array_in_str(p, f);
            (void) b;
        }
    } M_CATCH(test1, 0) {
        // Nothing to do
    }
    fclose(f);
}

int main(void)
{
    do_test_exception(test1);
    do_test_exception(test2);
    do_test_exception(test3);
    exit(0);
}
