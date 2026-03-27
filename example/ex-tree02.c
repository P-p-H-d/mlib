#include <stdio.h>
#include <stdbool.h>
#include "m-tuple.h"
#include "m-variant.h"

TUPLE_DEF2(tree_node,
	   (value, int),
	   (left, struct tree_s *, M_PTR_OPLIST),
	   (right, struct tree_s *, M_PTR_OPLIST))
#define M_OPL_tree_node_t() TUPLE_OPLIST(tree_node, M_BASIC_OPLIST, M_PTR_OPLIST, M_PTR_OPLIST)
  
VARIANT_DEF2(tree, 	   
	     (Empty, bool),
	     (Leaf, int),
	     (Node, tree_node_t))
#define M_OPL_tree_t() VARIANT_OPLIST(tree, M_BOOL_OPLIST, M_BASIC_OPLIST, TUPLE_OPLIST(tree_node, M_BASIC_OPLIST, M_PTR_OPLIST, M_PTR_OPLIST))
  
static int tree_sum(tree_t t)
{
  if (tree_Empty_p(t))
    return 0;
  if (tree_Leaf_p(t))
    return *tree_get_Leaf(t);
  struct tree_node_s *n = *tree_get_Node(t);
  return n->value + tree_sum(n->left) + tree_sum(n->right);
}

int main(void) {
  // TODO: Improve way of building the data...
  M_LET(n1, n2, tree_node_t)
    M_LET(tree, node2, leaf3, leaf4, leaf5, tree_t) {
    tree_init_set_Leaf(leaf3, 3);
    tree_init_set_Leaf(leaf4, 4);
    tree_init_set_Leaf(leaf5, 5);
    tree_node_init_emplace(n2, 2, leaf3, leaf4);
    tree_init_set_Node(node2, n2);
    tree_node_init_emplace(n1, 1, node2, leaf5);
    tree_init_set_Node(tree, n1);
    printf("sum = %d\n", tree_sum(tree));
  }
}
