#include <stdio.h>
#include "m-tree.h"

TREE_DEF(tree, int)

// Compute sum of the tree using iterator and a pre-order walk
static int tree_sum_iter(tree_t t)
{
  int s = 0;
  for(tree_it_t it = tree_it(t); !tree_end_p(it); tree_next(&it)) {
    s += *tree_cref(it);
  }
  return s;
}

// Compute sum of the tree using recursive call
static int tree_sum_recur(tree_it_t it)
{
  if (tree_end_p(it))
    return 0;
  if (tree_leaf_p(it))
    return *tree_cref(it);
  assert( tree_degree(it) == 2);
  return *tree_cref(it) + tree_sum_recur(tree_down(it)) + tree_sum_recur(tree_right(tree_down(it)));
}

int main(void)
{
  tree_t t;
  tree_init(t);
  tree_it_t r = tree_set_root(t, 1);
  tree_it_t l = tree_insert_child(r, 2);
  tree_insert_child(r, 5);
  tree_insert_child(l, 3);
  tree_insert_child(l, 4);
  printf("sum = %d vs %d\n", tree_sum_iter(t), tree_sum_recur(tree_it(t)));
  tree_clear(t);
  return 0;
}
