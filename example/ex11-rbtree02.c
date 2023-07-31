#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "m-rbtree.h"
#include "m-string.h"

// Let's define a Red Black Tree over the constant C strings
// Notice that we need to specify the oplist for this case
// so that M*LIB handle the pointers to const char correctly. 
RBTREE_DEF(tree_cstr, const char *, M_CSTR_OPLIST)

// The classic C way
// It is probably the best for beginner in M*LIB to start like this.
static void classic1(void)
{
  // Let's declare a tree on the stack.
  // Even if the tree itself is allocated on the stack,
  // the nodes will be allocated on the heap.
  tree_cstr_t tree;
  // Initialize the tree using the provided constructor
  tree_cstr_init(tree);

  // Let's fill in the tree
  // Note that the object of the tree are pointer
  // to array of char. Therefore the strings themselves
  // are not stored in the tree but only a reference to then
  // As such, the pointed objects shall not moved nor being
  // destroyed, which is the case for const C string.
  tree_cstr_push (tree, "Hello");
  tree_cstr_push (tree, "World");
  tree_cstr_push (tree, "How");
  tree_cstr_push (tree, "Are");
  tree_cstr_push (tree, "You");

  // Let's print the tree.
  // We'll use the OUT_STR method to display the tree
  // without iterating ourself in the tree.
  printf("\nUsing C string, the generated (sorted) tree is ");
  tree_cstr_out_str(stdout, tree);
  printf ("\n");

  // Let's search for the key "How" in the tree.
  // If it is not found a NULL pointer is returned.
  // Otherwise a pointer to the key within the tree is returned.
  // This pointer is valid until a modification operation on the tree is done.
  const char *present = tree_cstr_get(tree, "How") == NULL ? "not " : "";
  printf("The word 'How' is %spresent\n", present);

  present = tree_cstr_get(tree, "Not") == NULL ? "not " : "";
  printf("The word 'Not' is %spresent\n", present);

  // Destroy the tree by calling the destructor.
  tree_cstr_clear(tree);
}


// Let's define another Red Black Tree over the M*LIB strings
// Notice that we don't need to specify the oplist for this case.
// since the oplist of a string_t is registered globally.
RBTREE_DEF(tree_str, string_t)

// The classic C way
// It is probably the best for beginner in M*LIB to start like this.
static void classic2(void)
{
  tree_str_t tree;

  tree_str_init(tree);

  // STRING_CTE enables to construct a temporary const string_t from a const char *
  // usable for function call.
  // Note that this time the full string is stored within the tree,
  // and not only a pointer to it.
  tree_str_push (tree, STRING_CTE("Hello"));
  tree_str_push (tree, STRING_CTE("World"));
  tree_str_push (tree, STRING_CTE("How"));
  tree_str_push (tree, STRING_CTE("Are"));
  // We may also push a const C string into a string_t by calling the emplace function
  tree_str_emplace (tree, "You");

  // Let's print the tree
  printf("\nUsing M*LIB string_t, the generated (sorted) tree is ");
  tree_str_out_str(stdout, tree);
  printf ("\n");

  // Let's search for the string "How"
  // We convert it to a temporary string_t, and let's test
  // if it is found or not by testing if the returned pointer to string_t
  // is null or not.
  const char *present = tree_str_get(tree, STRING_CTE("How")) == NULL ? "not " : "";
  printf("The word 'How' is %spresent\n", present);

  present = tree_str_get(tree, STRING_CTE("Not")) == NULL ? "not " : "";
  printf("The word 'Not' is %spresent\n", present);

  tree_str_clear(tree);
}


// Registering the oplist helps writting the macro
// as we can only refer to the type in the macro.
#define M_OPL_tree_str_t() RBTREE_OPLIST(tree_str, STRING_OPLIST)

static void macro(void)
{
  // Using macro (shorter but may be unfamiliar syntax for novive with M*LIB)
  // This will declare the variable 'tree' as a 'tree_str_t' and initialize with
  // the given initialization list '("Hello"), ("World"), ("How"), ("Are"), ("You")'
  // The strings are given within '()' so that the constructor performs an emplace
  // of the value to push in the tree (to initialize a string_t with a const char * type)
  // instead of a push (which takes a compliant string_t as argument).
  // An alternative is to use STRING_CTE for the arguments.
  M_LET( (tree, ("Hello"), ("World"), ("How"), ("Are"), ("You")), tree_str_t) {
    // In C11, the M_PRINT macro allows the use of a simplified syntax for printing.
    M_PRINT("\nUsing M*LIB string_t + macro, the generated (sorted) tree is ", (tree, tree_str_t), "\n");
    
    const char *present = tree_str_get(tree, STRING_CTE("How")) == NULL ? "not " : "";
    printf("The word 'How' is %spresent\n", present);
    
    present = tree_str_get(tree, STRING_CTE("Not")) == NULL ? "not " : "";
    printf("The word 'Not' is %spresent\n", present);
  }
}
  
int main(void)
{
  classic1();
  classic2();
  macro();
  return 0;
}
