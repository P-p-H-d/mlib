/*
 * Copyright (c) 2017-2018, Patrick Pelissier
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
#include "test-obj.h"
#include "m-string.h"
#include "m-rbtree.h"

static bool uint_in_str(unsigned int *u, FILE *f)
{
  int n = fscanf(f, "%u", u);
  return n == 1;
}

static bool uint_parse_str(unsigned int *u, const char str[], const char **endptr)
{
  char *end;
  *u = strtol(str, &end, 0);
  if (endptr) { *endptr = (const char*) end; }
  return (uintptr_t) end != (uintptr_t) str;
}

static void uint_out_str(FILE *f, unsigned int u)
{
  fprintf(f, "%u", u);
}

static void uint_get_str(string_t str, unsigned int u, bool append)
{
  (append ? string_cat_printf : string_printf) (str, "%u", u);
}

#include "coverage.h"
START_COVERAGE
RBTREE_DEF(rbtree_uint, unsigned int, M_OPEXTEND(M_DEFAULT_OPLIST, IN_STR(uint_in_str M_IPTR), OUT_STR(uint_out_str), GET_STR(uint_get_str), PARSE_STR(uint_parse_str M_IPTR)) )
END_COVERAGE

RBTREE_DEF(rbtree_float, float)
RBTREE_DEF(rbtree_mpz, testobj_t, TESTOBJ_OPLIST)

#define UINT_OPLIST RBTREE_OPLIST(rbtree_uint)
#define FLOAT_OP RBTREE_OPLIST(rbtree_float)

static void test_uint(void)
{
  rbtree_uint_t tree, tree2, tree3;
  rbtree_uint_init(tree);
  assert (rbtree_uint_empty_p(tree));
  rbtree_uint_init_set(tree2, tree);
  assert (rbtree_uint_empty_p(tree2));

  rbtree_uint_push(tree, 10);
  rbtree_uint_push(tree, 5);
  rbtree_uint_push(tree, 17);
  rbtree_uint_push(tree, 3);
  rbtree_uint_push(tree, 11);
  assert(rbtree_uint_size(tree) == 5);
  assert(rbtree_uint_empty_p(tree) == false);
  const unsigned int tab[] = {3, 5, 10, 11, 17};
  int k = 0;
  for M_EACH(item, tree, UINT_OPLIST) {
      assert (tab[k] == *item);
      k ++;
    }
  assert (k == 5);
  rbtree_uint_it_t it, it2;
  for (rbtree_uint_it_last(it, tree) ; !rbtree_uint_end_p(it) ; rbtree_uint_previous(it)) {
    k--;
    rbtree_uint_it_set(it2, it);
    assert(rbtree_uint_it_equal_p(it2, it));
    assert (tab[k] == *rbtree_uint_cref(it2) );
  }
  rbtree_uint_it_end(it, tree);
  assert (rbtree_uint_end_p(it));
  assert(!rbtree_uint_it_equal_p(it2, it));
  assert( k == 0);
  assert (rbtree_uint_hash(tree) != 0);

  rbtree_uint_init(tree3);
  rbtree_uint_push(tree3, 5);
  rbtree_uint_move(tree3, tree);
  rbtree_uint_clear(tree3);

  rbtree_uint_init_move(tree, tree2);
  rbtree_uint_init_move(tree2, tree);
  assert (rbtree_uint_empty_p(tree2));
  
  const unsigned int max = 1001;
  for(unsigned int num = 1; num < max; num++) {
    rbtree_uint_init(tree);
    for(unsigned int i = 0; i < num; i++)
      rbtree_uint_push(tree, i);
    assert(rbtree_uint_size(tree) == num);
    assert(rbtree_uint_empty_p(tree) == false);
    if (num > 10) {
      rbtree_uint_push(tree, 10);
      assert(rbtree_uint_size(tree) == num);
      assert(rbtree_uint_empty_p(tree) == false);
    }
    unsigned int count = 0;
    for M_EACH(item, tree, UINT_OPLIST) {
        assert (count == *item);
        count ++;
      }
    assert (*rbtree_uint_min(tree) == 0);
    assert (*rbtree_uint_max(tree) == num-1);
    assert (*rbtree_uint_cmin(tree) == 0);
    assert (*rbtree_uint_cmax(tree) == num-1);
    assert (count == num);
    rbtree_uint_set(tree2, tree);
    unsigned int *ptr = rbtree_uint_get(tree2, num/2);
    assert (ptr != NULL);
    assert (*ptr == num/2);
    const unsigned int *cptr = rbtree_uint_cget(tree2, num/2);
    assert (cptr != NULL);
    assert (*cptr == num/2);
    rbtree_uint_clear(tree);

    bool b = rbtree_uint_pop_at(NULL, tree2, num+1);
    assert (b == false);
    for(unsigned int i = 0; i < num; i+=3) {
      b = rbtree_uint_pop_at(NULL, tree2, i);
      assert(b);
      ptr = rbtree_uint_get(tree2, i);
      assert (ptr == NULL);
    }
    for(unsigned int i = 1; i < num; i+=3) {
      b = rbtree_uint_pop_at(NULL, tree2, i);
      assert(b);
      ptr = rbtree_uint_get(tree2, i);
      assert (ptr == NULL);
    }    
  }

  rbtree_uint_init (tree);
  assert(rbtree_uint_empty_p(tree));
  rbtree_uint_clean(tree2);
  rbtree_uint_push(tree2, 17421);
  assert(rbtree_uint_size(tree2) == 1);
  rbtree_uint_swap(tree, tree2);
  assert(rbtree_uint_empty_p(tree2));
  assert(rbtree_uint_size(tree) == 1);
  unsigned int *ptr = rbtree_uint_get(tree, 17421);
  assert(ptr != NULL && *ptr == 17421);
  
  rbtree_uint_clear(tree);
  rbtree_uint_clear(tree2);
}

static void test_uint_permut(void)
{
  const unsigned int max = 1001;
  unsigned int tab[1001];
  for(unsigned int num = 1; num < max; num++) {
    rbtree_uint_t tree;
    rbtree_uint_init(tree);
    for(unsigned int i = 0; i < num; i++)
      tab[i] = i;
    for(unsigned int i = 0; i < 2 * num; i++) {
      int j = rand() % num;
      int k = rand() % num;
      M_SWAP(unsigned int, tab[j], tab[k]);
    }
    for(unsigned int i = 0; i < num; i++)
      rbtree_uint_push(tree, tab[i]);
    unsigned int k = 0;
    for M_EACH(item, tree, UINT_OPLIST) {
      assert (k == *item);
      k++;
    }
    rbtree_uint_clear(tree);
  }
}


static void test_float(void)
{
  M_LET(tree, FLOAT_OP) {
    for(float f = -17.42; f < 17.42; f+=0.01)
      rbtree_float_push (tree, f);
    bool p = false;
    float g;
    for M_EACH(item, tree, FLOAT_OP) {
        if (p) {
          assert (g < *item);
        }
        g = *item;
        p = true;
      }
  }
}

static void test_io(void)
{
  M_LET(str, STRING_OPLIST)
  M_LET(tree1, tree2, UINT_OPLIST) {
    // Empty one
    FILE *f = fopen ("a-mrbtree.dat", "wt");
    if (!f) abort();
    rbtree_uint_out_str(f, tree1);
    fclose (f);

    f = fopen ("a-mrbtree.dat", "rt");
    if (!f) abort();
    bool b = rbtree_uint_in_str (tree2, f);
    assert (b == true);
    assert (rbtree_uint_equal_p (tree1, tree2));
    fclose(f);

    rbtree_uint_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[]"));
    const char *sp;
    b = rbtree_uint_parse_str(tree2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(rbtree_uint_equal_p(tree1, tree2));

    // Fill in data
    for(unsigned int i = 0 ; i < 10; i++)
      rbtree_uint_push(tree1, i);

    f = fopen ("a-mrbtree.dat", "wt");
    if (!f) abort();
    rbtree_uint_out_str(f, tree1);
    fclose (f);

    f = fopen ("a-mrbtree.dat", "rt");
    if (!f) abort();
    b = rbtree_uint_in_str (tree2, f);
    assert (b == true);
    assert (rbtree_uint_equal_p (tree1, tree2));
    fclose(f);

    rbtree_uint_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[0,1,2,3,4,5,6,7,8,9]"));
    b = rbtree_uint_parse_str(tree2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(rbtree_uint_equal_p(tree1, tree2));
  }
  
}

int main(void)
{
  test_uint();
  test_uint_permut();
  test_float();
  test_io();
  exit(0);
}
