/*
 * Copyright (c) 2017-2025, Patrick Pelissier
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
#include "test-obj.h"
#include "m-tree.h"
#include "m-string.h"
#include "coverage.h"

#define M_GET_STRING_INT(str, x, append)                                      \
  (append ? m_string_cat_printf : m_string_printf) (str, "%d", x)
#define M_OUT_STR_INT(f, x)                                      \
  fprintf (f, "%d", x)

START_COVERAGE
TREE_DEF(tree, int, M_OPEXTEND(M_DEFAULT_OPLIST, GET_STR(M_GET_STRING_INT),PARSE_STR(m_core_parse_sint M_IPTR),IN_STR(m_core_fscan_sint M_IPTR), OUT_STR(M_OUT_STR_INT)))
END_COVERAGE
TREE_DEF(tree_mpz, testobj_t, TESTOBJ_OPLIST)
TREE_DEF(tree_string, string_t)

#define numberof(x) (int)(sizeof (x) / sizeof((x)[0]))

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
    tree_it_t it0 = root;
    assert( tree_size(t) == 1);
    assert( tree_empty_p(t) == false);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0}]"));

    tree_it_t it2 = tree_insert_child(root, 2);
    assert( tree_size(t) == 2);
    assert( tree_empty_p(t) == false);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{2}]}]"));

    tree_it_t it1 = tree_insert_down(root, 1);
    assert( tree_size(t) == 3);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{1,[{2}]}]}]"));

    tree_it_t it3 = tree_insert_left(it2, 3);
    assert( tree_size(t) == 4);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{1,[{3},{2}]}]}]"));

    tree_it_t it4 = tree_insert_right(it2, 4);
    assert( tree_size(t) == 5);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{1,[{3},{2},{4}]}]}]"));

    root = tree_insert_up(root, -1);
    assert( tree_size(t) == 6);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{2},{4}]}]}]}]"));

    tree_it_t it5 = tree_insert_up(it2, 5);
    assert( tree_size(t) == 7);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4}]}]}]}]"));

    tree_it_t it6 = tree_insert_child(it4, 6);
    assert( tree_size(t) == 8);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{6}]}]}]}]}]"));

    tree_it_t it7 = tree_insert_child(it4, 7);
    assert( tree_size(t) == 9);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{7},{6}]}]}]}]}]"));

    tree_it_t it8 = tree_insert_child(it7, 8);
    assert( tree_size(t) == 10);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{7,[{8}]},{6}]}]}]}]}]"));

    tree_insert_left(it7, 9);
    assert( tree_size(t) == 11);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{9},{7,[{8}]},{6}]}]}]}]}]"));

    tree_insert_right(it7, 10);
    assert( tree_size(t) == 12);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{9},{7,[{8}]},{10},{6}]}]}]}]}]"));

    assert( *tree_cref(it7) == 7 );
    assert( *tree_cref(tree_down(it7)) == 8 );
    assert( *tree_cref(tree_up(it7)) == 4);
    assert( *tree_cref(tree_left(it7)) == 9);
    assert( *tree_cref(tree_right(it7)) == 10);
    assert( *tree_cref(tree_down(root)) == 0);

    assert( tree_end_p(tree_up(root)));
    assert( tree_end_p(tree_left(root)));
    assert( tree_end_p(tree_right(root)));

    assert( *tree_down_ref(it7) == 8);
    assert( *tree_up_ref(it7) == 4);
    assert( *tree_left_ref(it7) == 9);
    assert( *tree_right_ref(it7) == 10);

    assert( *tree_down_ref(root) == 0);
    assert(  tree_up_ref(root) == NULL);
    assert(  tree_left_ref(root) == NULL);
    assert(  tree_right_ref(root) == NULL);

    assert( tree_down_ref(it6) == NULL);

    assert( *tree_ref(root) == -1);
    assert( *tree_ref(it6) == 6);
    assert( *tree_cref(root) == -1);
    assert( *tree_cref(it6) == 6);

    assert( tree_root_p(root) == true);
    assert( tree_root_p(it7) == false);
    assert( tree_root_p(it6) == false);

    assert( tree_node_p(root) == true);
    assert( tree_node_p(it7) == true);
    assert( tree_node_p(it6) == false);

    assert( tree_leaf_p(root) == false);
    assert( tree_leaf_p(it7) == false);
    assert( tree_leaf_p(it6) == true);

    assert(tree_degree(root) == 1);
    assert(tree_degree(it7) == 1);
    assert(tree_degree(it4) == 4);
    assert(tree_degree(it6) == 0);

    assert(tree_depth(root) == 0);
    assert(tree_depth(it7) == 4);
    assert(tree_depth(it6) == 4);

    assert(tree_tree(root) == &t[0]);

    int i = 0;
    tree_it_t it;
    for(it = tree_it(t); !tree_end_p(it); tree_next(&it), i++) {
      static const int tab[] = {-1,0,1,3,5,2,4,9,7,8,10,6};
      assert( i < numberof (tab) );
      assert( *tree_ref(it) == tab[i] );
    }

    for(i = 0, it = tree_it_post(t); !tree_end_p(it); tree_next_post(&it), i++) {
      static const int tab[] = {3, 2, 5, 9, 8, 7, 10, 6, 4, 1, 0, -1};
      assert( i < numberof (tab) );
      assert( *tree_cref(it) == tab[i] );
    }

    for(i = 0, it = tree_it_subpre(it4); !tree_end_p(it); tree_next_subpre(&it, it4), i++) {
      static const int tab[] = {4,9,7,8,10,6};
      assert( i < numberof (tab) );
      assert( *tree_ref(it) == tab[i] );
    }

    for(i = 0, it = tree_it_subpre(it7); !tree_end_p(it); tree_next_subpre(&it, it7), i++) {
      static const int tab[] = {7,8};
      assert( i < numberof (tab) );
      assert( *tree_ref(it) == tab[i] );
    }

    for(i = 0, it = tree_it_subpost(it4); !tree_end_p(it); tree_next_subpost(&it, it4), i++) {
      static const int tab[] = {9,8,7,10,6,4};
      assert( i < numberof (tab) );
      assert( *tree_ref(it) == tab[i] );
    }

    for(i = 0, it = tree_it_subpost(it7); !tree_end_p(it); tree_next_subpost(&it, it7), i++) {
      static const int tab[] = {8, 7};
      assert( i < numberof (tab) );
      assert( *tree_ref(it) == tab[i] );
    }

    size_t capa = tree_capacity(t);
    tree_reserve(t, 0);
    M_ASSERT(tree_capacity(t) == capa);
    tree_reserve(t, 100000);
    M_ASSERT(tree_capacity(t) == 100000);

    // Nothing to test for lock?
    tree_lock(t, true);
    tree_lock(t, false);

    assert( tree_it_equal_p(root, root) == true);
    assert( tree_it_equal_p(root, it7) == false);

    it = tree_lca(it4, it7);
    assert( tree_it_equal_p(it, it4));

    it = tree_lca(it3, it7);
    assert( tree_it_equal_p(it, it1));
    it = tree_lca(it7, it3);
    assert( tree_it_equal_p(it, it1));

    it = it3;
    bool b = tree_it_right(&it);
    assert(b);
    assert(tree_it_equal_p(it, it5));
    b = tree_it_left(&it);
    assert(b);
    assert(tree_it_equal_p(it, it3));
    b = tree_it_up(&it);
    assert(b);
    b = tree_it_up(&it);
    assert(b);
    assert(tree_it_equal_p(it, it0));
    b = tree_it_up(&it);
    assert(b);
    b = tree_it_up(&it);
    assert(!b);
    b = tree_it_down(&it);
    assert(b);
    b = tree_it_down(&it);
    assert(b);
    b = tree_it_down(&it);
    assert(b);
    assert(tree_it_equal_p(it, it3));
    b = tree_it_down(&it);
    assert(!b);

    b = tree_remove(it7);
    assert(b);
    assert( tree_size(t) == 11);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{9},{8},{10},{6}]}]}]}]}]"));

    it7 = tree_it_end(t);
    b = tree_remove(it7);
    assert(!b);
    assert( tree_size(t) == 11);

    b = tree_remove(it8);
    assert(b);
    assert( tree_size(t) == 10);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{9},{10},{6}]}]}]}]}]"));

    tree_prune(it4);
    assert( tree_size(t) == 6);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]}]}]}]}]"));

    tree_swap_at(it3, it5, false);
    assert( tree_size(t) == 6);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{5},{3,[{2}]}]}]}]}]"));
    tree_swap_at(it3, it5, true);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3,[{2}]},{5}]}]}]}]"));

    tree_swap_at(it0, it2, false);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{-1,[{2,[{1,[{3,[{0}]},{5}]}]}]}]"));
    tree_swap_at(root, it0, false);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{2,[{1,[{3,[{-1}]},{5}]}]}]}]"));
    tree_swap_at(root, it3, false);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{2,[{1,[{-1,[{3}]},{5}]}]}]}]"));

    tree_graft_child(it0, root);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{2,[{1,[{5}]}]}]}]"));

    tree_sort_child(it0);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{2,[{1,[{5}]}]}]}]"));

    tree_swap_at(root, it2, true);
    tree_sort_child(it0);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{2,[{1,[{5}]}]}]}]"));

    int data = 10;
    tree_move_up(it2, &data);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{10,[{2,[{1,[{5}]}]}]}]}]"));

    data = 11;
    tree_move_left(it2, &data);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{10,[{11},{2,[{1,[{5}]}]}]}]}]"));

    data = 12;
    tree_move_right(it2, &data);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{10,[{11},{2,[{1,[{5}]}]},{12}]}]}]"));

    data = 13;
    tree_move_child(it2, &data);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{10,[{11},{2,[{13},{1,[{5}]}]},{12}]}]}]"));

    data = 14;
    tree_move_down(it2, &data);
    tree_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{0,[{-1,[{3}]},{10,[{11},{2,[{14,[{13},{1,[{5}]}]}]},{12}]}]}]"));

    tree_it_set(&it, root);
    assert(tree_it_equal_p(it, root));

    size_t hash = tree_hash(t);
    assert(hash != 0);

    string_clear(s);
    tree_clear(t);
}

/* Random insertion in a tree.
   We let the normal assertions of the tree checking for violations of the tree properties. */
#define MAX_NODE_INSERT 5000
static tree_it_t g_tree[MAX_NODE_INSERT];
static tree_it_t g_root;
static unsigned g_num;

static void insert(unsigned cmd, unsigned ref)
{
  assert( g_num < MAX_NODE_INSERT);
  assert( cmd < 5);
  assert( ref <= g_num);
  int val = (int) g_num + 1;
  //printf("Command %d for value %d at %d\n", cmd, val, ref);
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
  tree_t t, t0;
  string_t s;
  tree_init(t);
  tree_init_set(t0, t);
  string_init(s);
  g_root = g_tree[++ g_num] = tree_set_root(t, 1);
  tree_get_str(s, t, false);
  for(unsigned i = 0; i < MAX_NODE_INSERT; i++) {
    unsigned cmd = rand_get() % 5;
    unsigned ref = 1 + (rand_get() % g_num);
    insert(cmd, ref);
    tree_get_str(s, t, false);
    tree_set(t0, t);
    // 2 swap_at operation ==> no op
    unsigned ref1 = 1 + (rand_get() % g_num);
    unsigned ref2 = 1 + (rand_get() % g_num);
    tree_swap_at(g_tree[ref1], g_tree[ref2], false);
    tree_swap_at(g_tree[ref1], g_tree[ref2], false);
    assert(tree_equal_p(t, t0));
    // insert then remove operation ==> no op
    cmd = rand_get() % 5;
    ref = 1 + (rand_get() % g_num);
    if (!tree_it_equal_p(g_tree[ref], g_root)) {
      // We only do it if the ref node is not root (too many corner cases to undo)
      insert(cmd, ref);
      assert(!tree_equal_p(t, t0));
      tree_remove( g_tree[g_num]);
      g_num--;
      assert(tree_equal_p(t, t0));
      tree_swap(t, t0);
      assert(tree_equal_p(t, t0));
    }
  }
  string_clear(s);
  tree_clear(t0);
  tree_init_move(t0, t);
  tree_init_set(t, t0);
  assert(tree_equal_p(t, t0));

  tree_set(t, t0);
  tree_it_t it;
  for(size_t i = 1; i < tree_size(t0); i++) {
    it = tree_it(t);
    tree_next(&it);
    bool b = tree_remove(it);
    assert(b);
    assert(tree_size(t)== tree_size(t0) - i);
  }
  bool b = tree_remove(tree_it(t));
  assert(b);
  assert(tree_size(t)== 0);

  tree_set(t, t0);
  for(size_t i = 1; i < tree_size(t0); i++) {
    it = tree_it(t);
    b = tree_remove(it);
    assert(b);
    assert(tree_size(t)== tree_size(t0) - i);
  }
  b = tree_remove(tree_it(t));
  assert(b);
  assert(tree_size(t)== 0);

  for(size_t i = 0; i < tree_size(t0); i++) {
    tree_set(t, t0);
    size_t j = 0;
    for(it = tree_it(t); j <= i && !tree_end_p(it); tree_next(&it), j++) {
      if (i ==j)
        *tree_ref(it) = 147852369;
    }
    assert(!tree_equal_p(t, t0));
    tree_set(t, t0);
    j = 0;
    for(it = tree_it(t); j <= i && !tree_end_p(it); tree_next(&it), j++) {
      if (i ==j) {
        tree_remove(it);
        break;
      }
    }
    it = tree_it(t);
    while (tree_it_down(&it)) ;
    while (tree_it_right(&it)) ;
    tree_insert_right(it, 185555569);
    assert(!tree_equal_p(t, t0));
    assert(!tree_equal_p(t0, t));
  }

  tree_move(t, t0);
  tree_clear(t);
}

static void test_io(void)
{
  bool b;
  tree_t t1, t2;
  tree_init(t1);
  tree_init_set(t2, t1);
  assert(tree_empty_p(t2));

  // Test empty
  FILE *f = m_core_fopen ("a-mtree.dat", "wt");
  if (!f) abort();
  tree_out_str(f, t1);
  fclose (f);

  f = m_core_fopen ("a-mtree.dat", "rt");
  if (!f) abort();
  b = tree_in_str (t2, f);
  assert (b == true);
  assert (tree_equal_p (t1, t2));
  assert(tree_empty_p(t2));
  fclose(f);

  M_LET(s, string_t) {
    tree_get_str(s, t1, false);
    assert(string_equal_str_p(s, "[]"));
    const char *sp;
    b = tree_parse_str(t2, string_get_cstr(s), &sp);
    assert(b);
    assert(*sp == 0);
    assert(tree_equal_p(t1, t2));
    assert(tree_empty_p(t2));
  }

  // Not empty
  tree_it_t root = tree_set_root(t1, 0);
  tree_it_t it = tree_insert_child(root, 2);
  tree_insert_down(root, 1);
  tree_insert_left(it, 3);
  tree_it_t it4 = tree_insert_right(it, 4);
  tree_insert_up(root, -1);
  tree_insert_up(it, 5);
  tree_insert_child(it4, 6);
  tree_it_t it7 = tree_insert_child(it4, 7);
  tree_insert_child(it7, 8);
  tree_insert_left(it7, 9);
  tree_insert_right(it7, 10);

  tree_set(t2, t1);
  assert (tree_equal_p (t1, t2));
  it = tree_it(t2);
  tree_insert_child(it, 60);
  assert (!tree_equal_p (t1, t2));
  tree_reset(t2);

  M_LET(s, string_t) {
    tree_get_str(s, t1, false);
    assert(string_equal_str_p(s, "[{-1,[{0,[{1,[{3},{5,[{2}]},{4,[{9},{7,[{8}]},{10},{6}]}]}]}]}]"));
    const char *sp;
    b = tree_parse_str(t2, string_get_cstr(s), &sp);
    assert(b);
    assert(*sp == 0);
    assert(tree_equal_p(t1, t2));

    string_set_str(s, "[{1,[{0,[{1,[{3},{5,[{2}]},{4,[{9},{7,[{8}]},{0},{6}]}]}]}]}]");
    for(size_t i = 0; i < string_size(s); i++) {
      string_set_str(s, "[{1,[{0,[{1,[{3},{5,[{2}]},{4,[{9},{7,[{8}]},{0},{6}]}]}]}]}]");
      string_replace_at(s, i, 1, "");
      b = tree_parse_str(t2, string_get_cstr(s), &sp);
      assert(!b);
      f = m_core_fopen ("a-mtree.dat", "wt");
      if (!f) abort();
      fprintf(f, "%s", string_get_cstr(s));
      fclose (f);
      f = m_core_fopen ("a-mtree.dat", "rt");
      if (!f) abort();
      b = tree_in_str (t2, f);
      assert(!b);
      fclose(f);
    }
  }

  f = m_core_fopen ("a-mtree.dat", "wt");
  if (!f) abort();
  tree_out_str(f, t1);
  fclose (f);
  f = m_core_fopen ("a-mtree.dat", "rt");
  if (!f) abort();
  b = tree_in_str (t2, f);
  assert (b == true);
  assert (tree_equal_p (t1, t2));
  fclose(f);

  tree_clear(t1);
  tree_clear(t2);
}

static void test_emplace(void)
{
  tree_string_t t;
  tree_string_init(t);
  tree_string_it_t root = tree_string_emplace_root(t, "Hello");
  root = tree_string_emplace_up(root, "Up");
  tree_string_it_t it = tree_string_emplace_down(root, "Up2");
  tree_string_emplace_left(it, "Up0");
  tree_string_emplace_right(it, "Up3");
  tree_string_emplace_child(it, "World");
  M_LET(s, string_t) {
    tree_string_get_str(s, t, false);
    assert(string_equal_str_p(s, "[{\"Up\",[{\"Up0\"},{\"Up2\",[{\"World\"},{\"Hello\"}]},{\"Up3\"}]}]"));
  }
  tree_string_clear(t);
}

int main(void)
{
  test_basic();
  test_gen();
  test_io();
  test_emplace();
  testobj_final_check();
  return 0;
}
