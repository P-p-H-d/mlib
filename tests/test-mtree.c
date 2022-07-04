/*
 * Copyright (c) 2017-2022, Patrick Pelissier
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
#include <stdio.h>

#include "m-tree.h"

#include "test-obj.h"
#include "m-string.h"
#include "coverage.h"

#define M_GET_STRING_INT(str, x, append)                                      \
  (append ? m_string_cat_printf : m_string_printf) (str, "%d", x)

START_COVERAGE
TREE_DEF(tree, int, M_OPEXTEND(M_DEFAULT_OPLIST, GET_STR(M_GET_STRING_INT)))
END_COVERAGE
TREE_DEF(tree_mpz, testobj_t, TESTOBJ_OPLIST)

static void test_basic(void)
{
    tree_t   t;
    string_t s;
    tree_init(t);
    string_init(s);

    assert( tree_size(t) == 0);
    assert( tree_empty_p(t) == true);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[]"));

    tree_it_t root = tree_set_root(t, 0);
    assert( tree_size(t) == 1);
    assert( tree_empty_p(t) == false);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0}]"));

    tree_it_t it = tree_insert_child(root, 2);
    assert( tree_size(t) == 2);
    assert( tree_empty_p(t) == false);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{2}]}]"));

    tree_insert_down(root, 1);
    assert( tree_size(t) == 3);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{1,[{2}]}]}]"));

    tree_insert_left(it, 3);
    assert( tree_size(t) == 4);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{1,[{3},{2}]}]}]"));

    tree_it_t it4 = tree_insert_right(it, 4);
    assert( tree_size(t) == 5);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{1,[{3},{2},{4}]}]}]"));

    root = tree_insert_up(root, -1);
    assert( tree_size(t) == 6);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{2},{4}]}]}]}]"));

    tree_insert_up(it, 5);
    assert( tree_size(t) == 7);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4}]}]}]}]"));

    tree_insert_child(it4, 6);
    assert( tree_size(t) == 8);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{6}]}]}]}]}]"));

    tree_insert_child(it4, 7);
    assert( tree_size(t) == 9);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{7},{6}]}]}]}]}]"));

    string_clear(s);
    tree_clear(t);
}

/* Random insertion in a tree.
   We let the normal assertions of the tree to check for errors. */
#define MAX_NODE_INSERT 10000
static tree_it_t g_tree[MAX_NODE_INSERT];
static tree_it_t g_root;
static unsigned g_num;

static void insert(unsigned cmd, unsigned ref)
{
  assert( g_num < MAX_NODE_INSERT);
  assert( cmd < 5);
  assert( ref <= g_num);
  int val = (int) g_num + 1;
  switch (cmd) {
    case 0:
      g_tree[++g_num] = tree_insert_up( g_tree[ref], val);
      if (tree_it_equal_p(g_tree[ref], g_root)) {
        g_root = g_tree[g_num];
      }
      break;
    case 1:
      g_tree[++g_num] = tree_insert_down( g_tree[ref], val);
      break;
    case 2:
      g_tree[++g_num] = tree_insert_child( g_tree[ref], val);
      break;
    case 3:
      if (tree_it_equal_p(g_tree[ref], g_root)) {
        return;
      }
      g_tree[++g_num] = tree_insert_left( g_tree[ref], val);
      break;
    default:
      if (tree_it_equal_p(g_tree[ref], g_root)) {
        return;
      }
      g_tree[++g_num] = tree_insert_right( g_tree[ref], val);
      break;
  }
}


static inline unsigned int rand_get(void)
{
  static unsigned int randValue = 0;
  randValue = randValue * 31421U + 6927U;
  return randValue;
}

static void test_gen(void)
{
  tree_t t;
  string_t s;
  tree_init(t);
  string_init(s);
  g_root = g_tree[++ g_num] = tree_set_root(t, 1);
  tree_get_str(s, t, false);
  for(unsigned i = 0; i < MAX_NODE_INSERT; i++) {
    unsigned cmd = rand_get() % 5;
    unsigned ref = 1 + (rand_get() % g_num);
    insert(cmd, ref);
    tree_get_str(s, t, false);
  }
  string_clear(s);
  tree_clear(t);
}

int main(void)
{
    test_basic();
    test_gen();
    return 0;
}
