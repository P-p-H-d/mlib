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
#include "m-core.h"
#include "coverage.h"

#include "m-bstring.h"

static void test0(void)
{
    bstring_t b;

    bstring_init(b);
    assert( bstring_empty_p(b) );
    assert( bstring_size(b) == 0);
    assert( bstring_capacity(b) == 0);
    assert( bstring_hash(b) != 0);
    bstring_clear(b);

    bstring_init(b);
    assert( bstring_empty_p(b) );
    bstring_push_back(b, 23);
    assert( !bstring_empty_p(b) );
    assert( bstring_size(b) == 1);
    assert( bstring_capacity(b) != 0);
    assert( bstring_hash(b) != 0);
    bstring_reset(b);
    assert( bstring_empty_p(b) );
    assert( bstring_size(b) == 0);
    assert( bstring_capacity(b) != 0);
    assert( bstring_hash(b) != 0);
    bstring_clear(b);
}

static void test1(void)
{
    bstring_t b;
    uint8_t u;

    bstring_init(b);

    bstring_push_back(b, 23);
    assert( !bstring_empty_p(b) );
    u = bstring_get_byte(b, 0);
    assert(u == 23);
    bstring_set_byte(b, 0, 24);
    u = bstring_get_byte(b, 0);
    assert(u == 24);
    bstring_set_byte(b, 0, 25);
    u = bstring_pop_back(b);
    assert(u == 25);

    for(int i = 0; i < 100; i++)
        bstring_push_back(b, (uint8_t) i);
    for(int i = 0; i < 100; i++) {
        u = bstring_get_byte(b, (size_t) i);
        assert(u == i);
    }
    for(int i = 0; i < 100; i++) {
        u = bstring_pop_front(b);
        assert(u == i);
    }
    assert( bstring_empty_p(b) );
    assert( bstring_size(b) == 0 );

    for(int i = 0; i < 100; i++)
        bstring_push_back(b, (uint8_t) i);
    for(int i = 99; i >= 0 ; i--) {
        u = bstring_pop_back(b);
        assert(u == i);
    }
    assert( bstring_empty_p(b) );
    assert( bstring_size(b) == 0 );

    bstring_resize(b, 400);
    assert( !bstring_empty_p(b) );
    assert( bstring_size(b) == 400 );
    for(int i = 0; i < 400; i++) {
        u = bstring_get_byte(b, (size_t) i);
        assert(u == 0);
        bstring_set_byte(b, (size_t) i, (uint8_t) i);
    }

    bstring_resize(b, 100);
    assert( !bstring_empty_p(b) );
    assert( bstring_size(b) == 100 );
    for(int i = 0; i < 100; i++) {
        u = bstring_get_byte(b, (size_t) i);
        assert(u == i);
    }

    bstring_resize(b, 200);
    assert( !bstring_empty_p(b) );
    assert( bstring_size(b) == 200 );
    for(int i = 0; i < 100; i++) {
        u = bstring_get_byte(b, (size_t) i);
        assert(u == i);
        u = bstring_get_byte(b, (size_t) i+100);
        assert(u == 0);
    }
    bstring_reserve(b, 0);
    assert( bstring_size(b) == 200 );

    bstring_resize(b, 0);
    assert( bstring_size(b) == 0 );
    assert( bstring_capacity(b) != 0 );
    for(int i = 0; i < 1000; i++) {
        bstring_push_back(b, (uint8_t) i);
        u = bstring_pop_front(b);
        assert(u == (uint8_t) i);
    }

    bstring_reserve(b, 0);
    assert( bstring_size(b) == 0 );
    assert( bstring_capacity(b) == 0 );
    bstring_push_back(b, 23);
    assert( !bstring_empty_p(b) );

    bstring_reserve(b, 1000);
    assert( bstring_size(b) == 1);
    assert( bstring_capacity(b) == 1000 );

    bstring_push_back(b, 25);
    bstring_push_back(b, 26);
    const uint8_t *p = bstring_view(b, 0, 3);
    assert( p[0] == 23);
    assert( p[1] == 25);
    assert( p[2] == 26);
    p = bstring_view(b, 1, 2);
    assert( p[0] == 25);
    assert( p[1] == 26);

    uint8_t *w = bstring_acquire_access(b, 0, 3);
    assert( w[0] == 23);
    assert( w[1] == 25);
    assert( w[2] == 26);
    w[1] = 24;
    bstring_release_access(b);
    p = bstring_view(b, 1, 2);
    assert( p[0] == 24);
    assert( p[1] == 26);

    bstring_clear(b);
}

static void test2(void)
{
    bstring_t b1, b2;

    bstring_init(b1);
    for(int i = 0; i < 200; i++)
        bstring_push_back(b1, (uint8_t) i);
    bstring_init_set(b2, b1);
    assert (bstring_equal_p(b1, b2));
    assert (bstring_cmp(b1, b2) == 0);

    bstring_push_back(b1, 200);;
    assert (!bstring_equal_p(b1, b2));
    assert (bstring_cmp(b1, b2) > 0);

    bstring_push_back(b2, 201);;
    assert (!bstring_equal_p(b1, b2));
    assert (bstring_cmp(b1, b2) < 0);

    bstring_set_byte(b2, 200, 200);
    bstring_push_back(b2, 202);;
    assert (!bstring_equal_p(b1, b2));
    assert (bstring_cmp(b1, b2) < 0);

    bstring_swap(b1, b2);
    assert (bstring_cmp(b1, b2) > 0);
    bstring_swap(b1, b2);

    bstring_set(b1, b2);
    assert (bstring_equal_p(b1, b2));
    assert (bstring_cmp(b1, b2) == 0);

    bstring_splice(b2, b1);
    assert(bstring_size(b2) == 202*2);
    assert(bstring_size(b1) == 0);

    bstring_clear(b1);
    bstring_init_move(b1, b2);
    assert(bstring_size(b1) == 202*2);
    bstring_init(b2);
    bstring_move(b2,b1);
    assert(bstring_size(b2) == 202*2);
    bstring_clear(b2);
}

static void test3(void)
{
    bstring_t b;
    uint8_t tab1[] = { 1, 2, 3};
    uint8_t tab2[] = { 4, 5, 6};
    uint8_t tab3[] = { 1, 2, 3, 4, 5, 6};
    uint8_t tab4[] = { 1, 2, 3, 4, 5, 6, 7};
    uint8_t tmp[3];

    bstring_init(b);

    bstring_push_back_bytes(b, sizeof tab1, tab1);
    assert( bstring_size(b) == sizeof tab1);
    assert( bstring_equal_bytes_p(b, sizeof tab1, tab1) );
    assert( bstring_cmp_bytes(b, sizeof tab1, tab1) == 0);

    bstring_push_back_bytes(b, sizeof tab2, tab2);
    assert( bstring_size(b) == sizeof tab1 + sizeof tab2);
    assert( bstring_equal_bytes_p(b, sizeof tab3, tab3) );
    assert( bstring_equal_bytes_p(b, 1, tab3) == false);
    assert( bstring_cmp_bytes(b, sizeof tab3, tab3) == 0);
    assert( bstring_cmp_bytes(b, 1, tab3) > 0);
    assert( bstring_cmp_bytes(b, sizeof tab4, tab4) < 0);
    assert( bstring_cmp_bytes(b, sizeof tab2, tab2) < 0);

    bstring_pop_back_bytes(sizeof tmp, tmp, b);
    assert(tmp[0] == 4 && tmp[1] == 5 && tmp[2] == 6);
    assert( bstring_size(b) == 3);
    bstring_pop_back_bytes(sizeof tmp, tmp, b);
    assert(tmp[0] == 1 && tmp[1] == 2 && tmp[2] == 3);
    assert( bstring_size(b) == 0);

    bstring_push_back_bytes(b, sizeof tab3, tab3);
    bstring_pop_front_bytes(sizeof tmp, tmp, b);
    assert(tmp[0] == 1 && tmp[1] == 2 && tmp[2] == 3);
    assert( bstring_size(b) == 3);
    bstring_pop_front_bytes(sizeof tmp, tmp, b);
    assert(tmp[0] == 4 && tmp[1] == 5 && tmp[2] == 6);
    assert( bstring_size(b) == 0);

    bstring_clear(b);
}

static void test4(void)
{
    bstring_t b;
    uint8_t tab1[] = { 1, 2, 6};
    uint8_t tab2[] = { 3, 4, 5};
    uint8_t tab3[] = { 1, 2, 3, 4, 5, 6};
    uint8_t tmp[3];

    bstring_init(b);

    bstring_push_back_bytes(b, sizeof tab1, tab1);
    bstring_push_bytes_at(b, 2, sizeof tab2, tab2);
    assert(bstring_size(b) == 6);
    assert( bstring_equal_bytes_p(b, sizeof tab3, tab3));

    bstring_pop_bytes_at(3, tmp, b, 1);
    assert(tmp[0] == 2 && tmp[1] == 3 && tmp[2] == 4);
    assert(bstring_size(b) == 3);

    bstring_clear(b);
}

static void test_io(void)
{
    uint8_t tab1[] = { 1, 2, 3};
    bstring_t b;

    bstring_init(b);

    FILE *f = fopen("a-mbstring.dat", "wb");
    if (!f) abort();
    size_t n = bstring_fwrite(f, b);
    assert(n == 0);
    fclose (f);

    bstring_push_back_bytes(b, sizeof tab1, tab1);

    f = fopen("a-mbstring.dat", "rb");
    if (!f) abort();
    bool success = bstring_fread(b, f, 0);
    assert(success);
    assert(bstring_size(b) == 0);
    fclose (f);

    bstring_push_back_bytes(b, sizeof tab1, tab1);

    f = fopen("a-mbstring.dat", "wb");
    if (!f) abort();
    n = bstring_fwrite(f, b);
    assert(n == sizeof tab1);
    fclose (f);

    f = fopen("a-mbstring.dat", "rb");
    if (!f) abort();
    success = bstring_fread(b, f, sizeof tab1);
    assert(success);
    assert(bstring_size(b) == sizeof tab1);
    assert(m_bstring_equal_bytes_p(b, sizeof tab1, tab1));
    fclose (f);

    bstring_clear(b);
}

int main(void)
{
    test0();
    test1();
    test2();
    test3();
    test4();
    test_io();
    exit(0);
}
