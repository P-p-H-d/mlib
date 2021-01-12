/*
 * M*LIB - RED BLACK TREE module
 *
 * Copyright (c) 2017-2020, Patrick Pelissier
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
#ifndef MSTARLIB_RBTREE_H
#define MSTARLIB_RBTREE_H

#include "m-core.h"

/* Define a Red/Black binary tree of a given type.
   USAGE: RBTREE_DEF(name, type [, oplist_of_the_type]) */
#define RBTREE_DEF(name, ...)                                                 \
  M_BEGIN_PROTECTED_CODE                                                      \
  RBTREEI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                  \
              ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),     \
                M_T(name, t), M_T(name, node, ct), M_T(name, it, t)),       \
               (name, __VA_ARGS__ ,                                           \
                M_T(name, t), M_T(name, node, ct), M_T(name, it, t))))      \
  M_END_PROTECTED_CODE


/* Define a Red/Black binary tree of a given type
   as the name name_t and the iterator it_t.
   USAGE: RBTREE_DEF_AS(name, name_t, it_t, type [, oplist_of_the_type]) */
#define RBTREE_DEF_AS(name, name_t, it_t, ...)                                \
  M_BEGIN_PROTECTED_CODE                                                      \
  RBTREEI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                  \
              ((name, __VA_ARGS__,                                            \
                M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(),                        \
                name_t, M_T(name, node, ct), it_t),                          \
               (name, __VA_ARGS__,                                            \
                name_t, M_T(name, node, ct), it_t)))                         \
  M_END_PROTECTED_CODE


/* Define the oplist of a rbtree of type.
   USAGE: RBTREE_OPLIST(name [, oplist_of_the_type]) */
#define RBTREE_OPLIST(...)                                                    \
  RBTREEI_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                               \
                 ((__VA_ARGS__, M_DEFAULT_OPLIST),                            \
                  (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

/* Deferred evaluation for the oplist definition,
   so that all arguments are evaluated before further expansion */
#define RBTREEI_OPLIST_P1(arg) RBTREEI_OPLIST_P2 arg

/* Validation of the given oplist */
#define RBTREEI_OPLIST_P2(name, oplist)                                       \
  M_IF_OPLIST(oplist)(RBTREEI_OPLIST_P3, RBTREEI_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define RBTREEI_OPLIST_FAILURE(name, oplist)                                  \
  ((M_LIB_ERROR(ARGUMENT_OF_RBTREE_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* OPLIST definition of a rbtree
   NOTE: IT_REF is not exported so that the contained appears as not modifiable
   by algorithm.*/
#define RBTREEI_OPLIST_P3(name, oplist)                                        \
    (INIT(M_F(name, M_NAMING_INITIALIZE)),                                           \
     INIT_SET(M_F(name, M_NAMING_INIT_WITH)),   \
     INIT_WITH(API_1(M_INIT_VAI)),                                             \
     SET(M_F(name, M_NAMING_SET_AS)),               \
     CLEAR(M_F(name, M_NAMING_FINALIZE)),                                         \
     INIT_MOVE(M_F(name, init_move)),       \
     MOVE(M_F(name, move)),                                                    \
     SWAP(M_F(name, swap)),                                                    \
     TYPE(M_T(name, ct)),      \
     SUBTYPE(M_T(name, subtype_ct)),                                              \
     TEST_EMPTY(M_F(name, M_NAMING_TEST_EMPTY)),  \
     GET_SIZE(M_F(name, M_NAMING_GET_SIZE)),                                       \
     IT_TYPE(M_T(name, it_ct)),            \
     IT_FIRST(M_F(name, M_NAMING_IT_FIRST)),                                   \
     IT_SET(M_F(name, M_NAMING_IT_SET)),                                       \
     IT_LAST(M_F(name, M_NAMING_IT_LAST)), \
     IT_END(M_F(name, M_NAMING_IT_END)),                                       \
     IT_END_P(M_F(name, M_NAMING_IT_TEST_END)),                                \
     IT_LAST_P(M_F(name, M_NAMING_IT_TEST_LAST)),                              \
     IT_EQUAL_P(M_F(name, M_NAMING_IT_TEST_EQUAL)),                            \
     IT_NEXT(M_F(name, next)), \
     IT_CREF(M_F(name, cref)),                                                 \
     CLEAN(M_F(name, M_NAMING_CLEAN)),              \
     PUSH(M_F(name, push)),                                                    \
     GET_MIN(M_F(name, min)),                         \
     GET_MAX(M_F(name, max)),                                                 \
     M_IF_METHOD(GET_STR, oplist)(GET_STR(M_F(name, get_str)), ),             \
     M_IF_METHOD(PARSE_CSTR, oplist)(PARSE_CSTR(M_F(name, parse_cstr)), ),       \
     M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_F(name, out_str)), ),             \
     M_IF_METHOD(IN_STR, oplist)(IN_STR(M_F(name, in_str)), ),                \
     M_IF_METHOD(OUT_SERIAL, oplist)(OUT_SERIAL(M_F(name, out_serial)), ),    \
     M_IF_METHOD(IN_SERIAL, oplist)(IN_SERIAL(M_F(name, in_serial)), ),       \
     M_IF_METHOD(EQUAL, oplist)(EQUAL(M_F(name, M_NAMING_TEST_EQUAL_TO)), ),      \
     M_IF_METHOD(HASH, oplist)(HASH(M_F(name, hash)), ),                      \
     M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist), ),                        \
     M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist), ),            \
     M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist), ))

/* Max depth of the binary tree
   It is at worst twice the depth of a perfectly even tree with maximum elements.
   The maximum number of elements is the max of size_t.
   A perfectly even tree is of depth log2(max(size_t))=CHAR_BIT*sizeof(size_t)
 */
#define RBTREEI_MAX_STACK (2 * CHAR_BIT * sizeof(size_t))

/* Encapsulation of the color of the nodes. */
#define RBTREEI_SET_RED(x)   ((x)->color =  RBTREE_RED)
#define RBTREEI_SET_BLACK(x) ((x)->color =  RBTREE_BLACK)
#define RBTREEI_IS_RED(x)    ((x)->color == RBTREE_RED)
#define RBTREEI_IS_BLACK(x)  ((x)->color == RBTREE_BLACK)
#define RBTREEI_COPY_COLOR(x,y) ((x)->color = (y)->color)
#define RBTREEI_GET_COLOR(x) (true ? (x)->color : (x)->color)
#define RBTREEI_SET_COLOR(x, c) ((x)->color = (c))
#define RBTREEI_GET_CHILD(x, n) ((x)->child[n])
#define RBTREEI_SET_CHILD(x, n, y) ((x)->child[n] = (y))

// Color of a node of a Red/Black tree
typedef enum {
  RBTREE_BLACK = 0, RBTREE_RED
} rbtreei_color_e;

// General contact of a Read/Black tree
#define RBTREEI_CONTRACT(tree) do {                                           \
    M_ASSERT((tree) != NULL);                                                \
    M_ASSERT((tree)->node == NULL || RBTREEI_IS_BLACK((tree)->node));        \
    M_ASSERT((tree)->size != 0 || (tree)->node == NULL);                     \
  } while (0)

// Contract of a node (doesn't check for equal depth in black)
#define RBTREEI_CONTRACT_NODE(node) do {                                       \
    M_ASSERT((node) != NULL);                                                  \
    M_ASSERT(RBTREEI_IS_BLACK(node) || RBTREEI_IS_RED(node));                  \
    M_ASSERT(RBTREEI_IS_BLACK(node)                                            \
       || (((node)->child[0] == NULL || RBTREEI_IS_BLACK(node->child[0]))      \
           && ((node)->child[1] == NULL || RBTREEI_IS_BLACK(node->child[1]))));\
  } while (0)

//TODO: UPDATE method shall be a separate method than push method

/* Deferred evaluation for the rbtree definition,
   so that all arguments are evaluated before further expansion */
#define RBTREEI_DEF_P1(arg) RBTREEI_DEF_P2 arg

/* Validate the oplist before going further */
#define RBTREEI_DEF_P2(name, type, oplist, tree_t, node_t, it_t)              \
  M_IF_OPLIST(oplist)(RBTREEI_DEF_P3, RBTREEI_DEF_FAILURE)(name, type, oplist, tree_t, node_t, it_t)

/* Stop processing with a compilation failure */
#define RBTREEI_DEF_FAILURE(name, type, oplist, tree_t, note_t, it_t)         \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(RBTREE_DEF): the given argument is not a valid oplist: " #oplist)

/* Internal rbtree definition
   - name: prefix to be used
   - type: type of the elements of the rbtree
   - oplist: oplist of the type of the elements of the container
   - tree_t: alias for the type of the container
   - it_t: alias for the iterator of the container
   - node_t: alias for the node of an element of the container
 */
#define RBTREEI_DEF_P3(name, type, oplist, tree_t, node_t, it_t)              \
                                                                              \
  /* Node of Red/Black tree.                                                  \
     Each node has up to two child, a color (Red or black)                    \
     and the data stored in it */                                             \
  typedef struct M_T(name, node, s) {                                         \
    struct M_T(name, node, s) *child[2];                                      \
    type data;                                                                \
    rbtreei_color_e color;                                                    \
  } node_t;                                                                   \
                                                                              \
  /* Define the Red/Black tree */                                             \
  typedef struct M_T(name, s) {                                              \
    size_t size;    /* Number of elements in the tree */                      \
    node_t *node;   /* Root node of the tree */                               \
  } tree_t[1];                                                                \
  typedef struct M_T(name, s) *M_T(name, ptr);                              \
  typedef const struct M_T(name, s) *M_T(name, srcptr);                     \
                                                                              \
  /* Iterator on a tree. The iterator stores the full path to the             \
  current node through all its parents and its depth */                       \
  typedef struct M_T(name, it, s) {                                           \
    node_t *stack[RBTREEI_MAX_STACK];                                         \
    int8_t  which[RBTREEI_MAX_STACK];                                         \
    unsigned int cpt;                                                         \
  } it_t[1];                                                                  \
                                                                              \
  /* Definition of the alias used by the oplists */                           \
  typedef type   M_T(name, subtype, ct);                                      \
  typedef tree_t M_T(name, ct);                                              \
  typedef it_t   M_T(name, it, ct);                                           \
                                                                              \
  /* Link with fast memory allocator if requested */                          \
  M_IF_METHOD(MEMPOOL, oplist)(                                               \
    /* Definition of the memory pool of this kind of node */                  \
    MEMPOOL_DEF(M_I(name, mempool), node_t)                                  \
    /* Definition of the global variable used to reference this pool */       \
    M_GET_MEMPOOL_LINKAGE oplist M_T(name, mempool, t) M_GET_MEMPOOL oplist;  \
    /* Allocator function */                                                  \
    static inline node_t *M_F(name, int, new)(void) {                          \
      return M_F(name, mempool, alloc)(M_GET_MEMPOOL oplist);                 \
    }                                                                         \
    /* Deallocator function */                                                \
    static inline void M_C(name,_int_del)(node_t *ptr) {                      \
      M_F(name, mempool, free)(M_GET_MEMPOOL oplist, ptr);                    \
    }                                                                         \
                                                                              \
    , /* No mempool allocation */                                             \
    /* Callic allocator function (common case) */                             \
    static inline node_t *M_F(name, int, new)(void) {                          \
      return M_CALL_NEW(oplist, node_t);                                      \
    }                                                                         \
    /* Callic deallocator function (common case) */                           \
    static inline void M_F(name, int, del)(node_t *ptr) {                      \
      M_CALL_DEL(oplist, ptr);                                                \
    }                                                                 )       \
                                                                              \
  /* Check if the given oplist is compatible with the given type */           \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  /* Generate all functions */                                                \
  RBTREEI_DEF_P4(name, type, oplist, tree_t, node_t, it_t)


#define RBTREEI_DEF_P4(name, type, oplist, tree_t, node_t, it_t)              \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_INITIALIZE)(tree_t tree)                                         \
  {                                                                           \
    M_ASSERT(tree != NULL);                                                  \
    tree->size = 0;                                                           \
    tree->node = NULL;                                                        \
    RBTREEI_CONTRACT(tree);                                                   \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_CLEAN)(tree_t tree)                                        \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                   \
    node_t *stack[RBTREEI_MAX_STACK];                                         \
    unsigned int cpt = 0;                                                     \
    /* If nothing (no node) nothing to clean: return */                       \
    if (tree->node == NULL) return;                                           \
    /* Parse all the tree */                                                  \
    stack[cpt++] = tree->node;                                                \
    while (cpt > 0) {                                                         \
      node_t *n = stack[cpt-1];                                               \
      /* Go down to the bottom left node that exists */                       \
      while (true) {                                                          \
        RBTREEI_CONTRACT_NODE(n);                                             \
        /* If there is a left child, get it */                                \
        if (n->child[0] != NULL) {                                            \
          M_ASSERT(cpt < RBTREEI_MAX_STACK);                                 \
          stack[cpt++] = n->child[0];                                         \
          n = n->child[0];                                                    \
          stack[cpt-2]->child[0] = NULL;                                      \
        /* If there is a right child, get it */                               \
        } else if (n->child[1] != NULL) {                                     \
          M_ASSERT(cpt < RBTREEI_MAX_STACK);                                 \
          stack[cpt++] = n->child[1];                                         \
          n = n->child[1];                                                    \
          stack[cpt-2]->child[1] = NULL;                                      \
        /* No left nor right child, node can be deleted */                    \
        } else {                                                              \
          break;                                                              \
        }                                                                     \
      }                                                                       \
      M_ASSERT(n == stack[cpt - 1]);                                         \
      /* Clear the bottom left node */                                        \
      M_CALL_CLEAR(oplist, n->data);                                          \
      M_F(name, int, del) (n);                                                 \
      M_ASSERT((stack[cpt-1] = NULL) == NULL);                                \
      /* Go up to the parent */                                               \
      cpt--;                                                                  \
    }                                                                         \
    /* Mark the root node as empty */                                         \
    tree->node = NULL;                                                        \
    tree->size = 0;                                                           \
   }                                                                          \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_FINALIZE)(tree_t tree)                                        \
  {                                                                           \
    /* Nothing more than clean the tree as everything is cleared */           \
    M_F(name, M_NAMING_CLEAN)(tree);                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, push)(tree_t tree, type const data)                        \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                   \
    node_t *tab[RBTREEI_MAX_STACK];                                           \
    int8_t  which[RBTREEI_MAX_STACK];                                         \
    unsigned int cpt = 0;                                                     \
    node_t *n = tree->node;                                                   \
    /* If there is no root node, create a new node */                         \
    if (n == NULL) {                                                          \
      n = M_C(name,_int_new)();                                               \
      if (M_UNLIKELY(n == NULL)) {                                           \
        M_MEMORY_FULL(sizeof (node_t));                                       \
        return;                                                               \
      }                                                                       \
      /* Copy the data in the root node */                                    \
      M_CALL_INIT_SET(oplist, n->data, data);                                 \
      /* Mark the root node as black */                                       \
      n->child[0] = n->child[1] = NULL;                                       \
      RBTREEI_SET_BLACK(n);                                                  \
      tree->node = n;                                                         \
      M_ASSERT(tree->size == 0);                                              \
      tree->size = 1;                                                         \
      RBTREEI_CONTRACT(tree);                                                 \
      return;                                                                 \
    }                                                                         \
    /* Search for insertion point in the tree */                              \
    tab[cpt] = n;                                                             \
    while (n != NULL) {                                                       \
      RBTREEI_CONTRACT_NODE(n);                                               \
      int cmp = M_CALL_CMP(oplist, n->data, data);                            \
      if (cmp == 0) {                                                         \
        /* key found ==> stop analysis */                                     \
        break;                                                                \
      } else {                                                                \
        /* go left (if cmp > 0) or right (if cmp < 0) */                      \
        int s = (cmp < 0);                                                    \
        which[cpt++] = (int8_t) s;                                            \
        n = n->child[s];                                                      \
      }                                                                       \
      /* We cannot overflow the max depth of a tree */                        \
      M_ASSERT(cpt < RBTREEI_MAX_STACK);                                     \
      tab[cpt] = n;                                                           \
    }                                                                         \
    /* If found, update the data (default is set) */                          \
    if (n != NULL) {                                                          \
      /* FIXME: Use of SET method instead of UPDATE ? */                      \
      M_IF_METHOD(UPDATE, oplist)(M_CALL_UPDATE(oplist, n->data, data),       \
                                  M_CALL_SET(oplist, n->data, data));      \
      RBTREEI_CONTRACT(tree);                                                \
      return;                                                                 \
    }                                                                         \
    /* Create new node to store the data */                                   \
    n = M_C(name,_int_new)();                                                 \
    if (M_UNLIKELY(n == NULL)) {                                            \
      M_MEMORY_FULL(sizeof (node_t));                                        \
      return;                                                                 \
    }                                                                         \
    /* Copy the data and mark the node as red */                              \
    M_CALL_INIT_SET(oplist, n->data, data);                                   \
    n->child[0] = n->child[1] = NULL;                                         \
    RBTREEI_SET_RED(n);                                                      \
    /* Add it in the iterator */                                              \
    M_ASSERT(tab[cpt] == NULL);                                              \
    tab[cpt] = n;                                                             \
    /* Add it in the tree */                                                  \
    tree->size ++;                                                            \
    M_ASSERT(tab[cpt-1]->child[0+which[cpt-1]] == NULL);                      \
    tab[cpt-1]->child[0+which[cpt-1]] = n;                                    \
    /* Fix the tree to still respect the red/back properties */               \
    while (cpt >= 2                                                           \
           && RBTREEI_IS_RED(tab[cpt-1])                                      \
           && tab[cpt-2]->child[1-which[cpt-2]] != NULL                       \
           && RBTREEI_IS_RED(tab[cpt-2]->child[1-which[cpt-2]])) {            \
      RBTREEI_SET_BLACK(tab[cpt-1]);                                          \
      RBTREEI_SET_BLACK(tab[cpt-2]->child[1-which[cpt-2]]);                   \
      RBTREEI_SET_RED(tab[cpt-2]);                                            \
      cpt-=2;                                                                 \
    }                                                                         \
    /* root is always black */                                                \
    RBTREEI_SET_BLACK(tab[0]);                                                \
    if (cpt <= 1 || RBTREEI_IS_BLACK(tab[cpt-1])) {                           \
      RBTREEI_CONTRACT(tree);                                                \
      return;                                                                 \
    }                                                                         \
    /* Read the grand-parent, the parent and the element */                   \
    node_t *pp = tab[cpt-2];                                                  \
    node_t *p  = tab[cpt-1];                                                  \
    node_t *x  = tab[cpt];                                                    \
    int i      = which[cpt-2];                                                \
    int j      = 1 - i;                                                       \
    M_ASSERT(i == 0 || i == 1);                                              \
    /* We need to do some rotations */                                        \
    if (i == which[cpt-1]) {                                                  \
      /* The child is the left child of its parent */                         \
      /* OR The child is the right child of its parent */                     \
      /* Right rotation: cpt is the new grand-parent.                         \
         x is its left child, the grand-parent is the right one */            \
      pp->child[i] = p->child[j];                                             \
      p->child[i] = x;                                                        \
      p->child[j] = pp;                                                       \
      RBTREEI_SET_BLACK(p);                                                   \
      RBTREEI_SET_RED(pp);                                                    \
    } else {                                                                  \
      M_ASSERT(j == which[cpt-1]);                                           \
      /* The child is the right child of its parent */                        \
      /* OR The child is the left child of its parent */                      \
      /* Left rotation */                                                     \
      pp->child[i] = x->child[j];                                             \
      p->child[j]  = x->child[i];                                             \
      x->child[i]  = p;                                                       \
      x->child[j]  = pp;                                                      \
      RBTREEI_SET_BLACK(x);                                                   \
      RBTREEI_SET_RED(p);                                                     \
      RBTREEI_SET_RED(pp);                                                    \
      p = x;                                                                  \
    }                                                                         \
    /* Insert the new grand parent */                                         \
    if (cpt == 2) {                                                           \
      tree->node = p;                                                         \
    } else {                                                                  \
      M_ASSERT(cpt >= 3);                                                    \
      tab[cpt-3]->child[which[cpt-3]] = p;                                    \
    }                                                                         \
    /* Done */                                                                \
    RBTREEI_CONTRACT(tree);                                                  \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_F(name, M_NAMING_GET_SIZE)(const tree_t tree)                                   \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    return tree->size;                                                        \
  }                                                                           \
                                                                              \
  /* Set the iterator to the first (child=0) or last (child=1) element */     \
  static inline void                                                          \
  M_F(name, int, it)(it_t it, const tree_t tree, int child)                   \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    M_ASSERT(it != NULL);                                                    \
    M_ASSERT(child == 0 || child == 1);                                      \
    unsigned int cpt = 0;                                                     \
    if (tree->node != NULL) {                                                 \
      it->which[cpt] = (int8_t) child;                                        \
      node_t *n = it->stack[cpt++] = tree->node;                              \
      /* Go down the tree and fill in the iterator */                         \
      while (n->child[child] != NULL) {                                       \
        M_ASSERT(cpt < RBTREEI_MAX_STACK);                                   \
        n = n->child[child];                                                  \
        it->which[cpt] = (int8_t) child;                                      \
        it->stack[cpt++] = n;                                                 \
      }                                                                       \
      M_ASSERT(n == it->stack[cpt - 1]);                                     \
    }                                                                         \
    it->cpt = cpt;                                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_FIRST)(it_t it, const tree_t tree)                            \
  {                                                                           \
    M_F(name, int, it)(it, tree, 0);                                          \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_LAST)(it_t it, const tree_t tree)                       \
  {                                                                           \
    M_F(name, int, it)(it, tree, 1);                                           \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_END)(it_t it, const tree_t tree)                        \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    M_ASSERT(it != NULL);                                                    \
    it->cpt = 0;                                                              \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_IT_SET)(it_t it, const it_t ref)                           \
  {                                                                           \
    M_ASSERT(it != NULL && ref != NULL);                                     \
    *it = *ref;                                                               \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, M_NAMING_IT_TEST_END)(const it_t it)                                      \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    return it->cpt == 0;                                                      \
  }                                                                           \
                                                                              \
  /* Go to the next (child = 0)or previous element (child = 1) */             \
  static inline void                                                          \
  M_F(name, int, next)(it_t it, int child)                                    \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    M_ASSERT(child == 0 || child == 1);                                      \
    if (it->cpt == 0) return;                                                 \
    unsigned int cpt = it->cpt - 1;                                           \
    node_t *n = it->stack[cpt];                                               \
    /* Get the other child */                                                 \
    const int right = 1 ^ child;                                              \
    if (n->child[right] != NULL && it->which[cpt] == child) {                 \
      /* Going right */                                                       \
      M_ASSERT(cpt < RBTREEI_MAX_STACK);                                     \
      n = n->child[right];                                                    \
      it->which[cpt++] = (int8_t) right;                                      \
      it->stack[cpt] = n;                                                     \
      it->which[cpt++] = (int8_t) child;                                      \
      /* Going left */                                                        \
      while (n->child[child] != NULL) {                                       \
        M_ASSERT(cpt < RBTREEI_MAX_STACK);                                   \
        n = n->child[child];                                                  \
        it->which[cpt] = (int8_t) child;                                      \
        it->stack[cpt++] = n;                                                 \
      }                                                                       \
      M_ASSERT(n == it->stack[cpt - 1]);                                     \
    } else {                                                                  \
      /* Going up */                                                          \
      while (cpt > 0 && it->which[cpt-1] == right) cpt--;                     \
    }                                                                         \
    it->cpt = cpt;                                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, next)(it_t it)                                                    \
  {                                                                           \
    M_F(name, int, next)(it, 0);                                             \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, previous)(it_t it)                                         \
  {                                                                           \
    M_F(name, int, next)(it, 1);                                              \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F(name, ref)(const it_t it)                                        \
  {                                                                           \
    M_ASSERT(it != NULL && it->cpt > 0);                                      \
    /* NOTE: partially unsafe if the user modify the order of the el */       \
    return &(it->stack[it->cpt-1]->data);                                     \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, cref)(const it_t it)                                       \
  {                                                                           \
    return M_CONST_CAST(type, M_F(name, ref)(it));                     \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, M_NAMING_IT_TEST_EQUAL)(const it_t it1, const it_t it2)     \
  {                                                                           \
    M_ASSERT(it1 != NULL && it2 != NULL);                                     \
    return it1->cpt == it2->cpt                                               \
      && it1->stack[it1->cpt-1] == it2->stack[it2->cpt-1];                    \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, it, from)(it_t it, const tree_t tree, type const data)             \
  {                                                                           \
    RBTREEI_CONTRACT (tree);                                                  \
    M_ASSERT(it != NULL);                                                    \
    unsigned int cpt = 0;                                                     \
    node_t *n = tree->node;                                                   \
    it->stack[cpt] =  n;                                                      \
    while (n != NULL) {                                                       \
      int cmp = M_CALL_CMP(oplist, n->data, data);                            \
      if (cmp == 0)                                                           \
        break;                                                                \
      int child = (cmp < 0);                                                  \
      it->which[cpt++] = (int8_t) child;                                      \
      n = n->child[child];                                                    \
      M_ASSERT(cpt < RBTREEI_MAX_STACK);                                     \
      it->stack[cpt] =  n;                                                    \
    }                                                                         \
    it->cpt = cpt;                                                            \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_P(name, it, until)(it_t it, type const data)                      \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    if (it->cpt == 0) return true;                                            \
    M_ASSERT(it->cpt > 0 && it->cpt < RBTREEI_MAX_STACK);                    \
    node_t *n = it->stack[it->cpt-1];                                         \
    int cmp = M_CALL_CMP(oplist, n->data, data);                              \
    return (cmp >= 0);                                                        \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_P(name, it, while)(it_t it, type const data)                      \
  {                                                                           \
    M_ASSERT(it != NULL);                                                    \
    if (it->cpt == 0) return true;                                            \
    M_ASSERT(it->cpt > 0 && it->cpt < RBTREEI_MAX_STACK);                    \
    node_t *n = it->stack[it->cpt-1];                                         \
    int cmp = M_CALL_CMP(oplist, n->data, data);                              \
    return (cmp <= 0);                                                        \
  }                                                                           \
                                                                              \
  static inline bool M_ATTR_DEPRECATED                                        \
  M_P(name, it, to)(it_t it, type const data)                         \
  {                                                                           \
    return M_P(name, it, until)(it, data);                            \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F(name, min)(const tree_t tree)                                    \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    node_t *n = tree->node;                                                   \
    if (M_UNLIKELY(n == NULL) ) return NULL;                                 \
    while (n->child[0] != NULL) {                                             \
      RBTREEI_CONTRACT_NODE(n);                                              \
      n = n->child[0];                                                        \
    }                                                                         \
    return &n->data;                                                          \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F(name, max)(const tree_t tree)                                    \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    node_t *n = tree->node;                                                   \
    if (M_UNLIKELY(n == NULL) ) return NULL;                                 \
    while (n->child[1] != NULL) {                                             \
      RBTREEI_CONTRACT_NODE(n);                                              \
      n = n->child[1];                                                        \
    }                                                                         \
    return &n->data;                                                          \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, cmin)(const tree_t tree)                                   \
  {                                                                           \
    return M_CONST_CAST(type, M_F(name, min)(tree));                   \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, cmax)(const tree_t tree)                                   \
  {                                                                           \
    return M_CONST_CAST(type, M_F(name, max)(tree));                   \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F(name, M_NAMING_GET)(const tree_t tree, type const data)                   \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    node_t *n = tree->node;                                                   \
    /* Go down the tree */                                                    \
    while (n != NULL) {                                                       \
      RBTREEI_CONTRACT_NODE(n);                                              \
      int cmp = M_CALL_CMP(oplist, n->data, data);                            \
      if (cmp == 0) {                                                         \
        return &n->data;                                                      \
      } else {                                                                \
        /* Go left (if cmp > 0) or right (if cmp < 0) */                      \
        n = n->child[cmp < 0];                                                \
      }                                                                       \
    }                                                                         \
    return NULL;                                                              \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, cget)(const tree_t tree, type const data)                  \
  {                                                                           \
    return M_CONST_CAST(type, M_F(name, M_NAMING_GET)(tree, data));             \
  }                                                                           \
                                                                              \
  /* Create a copy of the given node (recursively) */                         \
  static inline node_t *                                                      \
  M_F(name, int, copyn)(const node_t *o)                                      \
  {                                                                           \
    if (o == NULL) return NULL;                                               \
    node_t *n = M_F(name, int, new)();                                         \
    if (M_UNLIKELY(n == NULL)) {                                            \
      M_MEMORY_FULL(sizeof (node_t));                                        \
      return NULL;                                                            \
    }                                                                         \
    M_CALL_INIT_SET(oplist, n->data, o->data);                                \
    n->child[0] = M_F(name, int, copyn)(o->child[0]);                         \
    n->child[1] = M_F(name, int, copyn)(o->child[1]);                         \
    RBTREEI_COPY_COLOR(n, o);                                                \
    return n;                                                                 \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_INIT_WITH)(tree_t tree, const tree_t ref)                   \
  {                                                                           \
    RBTREEI_CONTRACT(ref);                                                   \
    M_ASSERT(tree != NULL && tree != ref);                                   \
    tree->size = ref->size;                                                   \
    /* Copy the root node recursively */                                      \
    tree->node = M_F(name, int_copyn)(ref->node);                            \
    RBTREEI_CONTRACT(tree);                                                  \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_SET_AS)(tree_t tree, const tree_t ref)                        \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    RBTREEI_CONTRACT(ref);                                                   \
    if (tree == ref) return;                                                  \
    M_F(name, M_NAMING_FINALIZE)(tree);                                             \
    M_F(name, M_NAMING_INIT_WITH)(tree, ref);                                     \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, init_move)(tree_t tree, tree_t ref)                        \
  {                                                                           \
    RBTREEI_CONTRACT(ref);                                                   \
    M_ASSERT(tree != NULL && tree != ref);                                   \
    tree->size = ref->size;                                                   \
    tree->node = ref->node;                                                   \
    ref->node = NULL;                                                         \
    ref->size = 0;                                                            \
    RBTREEI_CONTRACT(tree);                                                  \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, move)(tree_t tree, tree_t ref)                                    \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    RBTREEI_CONTRACT(ref);                                                   \
    M_ASSERT(tree != ref);                                                   \
    M_F(name, M_NAMING_FINALIZE)(tree);                                         \
    M_F(name, init_move)(tree, ref);                                          \
    RBTREEI_CONTRACT (tree);                                                  \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, swap)(tree_t tree1, tree_t tree2)                          \
  {                                                                           \
    RBTREEI_CONTRACT (tree1);                                                 \
    RBTREEI_CONTRACT (tree2);                                                 \
    M_SWAP(size_t, tree1->size, tree2->size);                                 \
    M_SWAP(node_t *, tree1->node, tree2->node);                               \
    RBTREEI_CONTRACT (tree1);                                                 \
    RBTREEI_CONTRACT (tree2);                                                 \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, M_NAMING_TEST_EMPTY)(const tree_t tree)                                \
  {                                                                           \
    RBTREEI_CONTRACT(tree);                                                  \
    return tree->size == 0;                                                   \
  }                                                                           \
                                                                              \
  /* Take care of the case n == NULL too */                                   \
  static inline bool                                                          \
  M_F(name, int, is_black)(const node_t *n)                                   \
  {                                                                           \
    return (n == NULL) ? true : RBTREEI_IS_BLACK(n);                          \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, int, set_black)(node_t *n)                                        \
  {                                                                           \
    if (n != NULL) RBTREEI_SET_BLACK(n);                                      \
  }                                                                           \
                                                                              \
  static inline node_t *                                                      \
  M_F(name, int, rotate)(node_t *pp, node_t *ppp, const bool right)           \
  {                                                                           \
    M_ASSERT(pp != NULL && ppp != NULL);                                     \
    bool left = !right;                                                       \
    node_t *p = pp->child[right];                                             \
    M_ASSERT(p != NULL);                                                     \
    pp->child[right] = p->child[left];                                        \
    p->child[left]  = pp;                                                     \
    /* Fix grandparent with new parent */                                     \
    M_ASSERT(ppp->child[0] == pp || ppp->child[1] == pp);                     \
    ppp->child[(ppp->child[0] != pp)] = p;                                    \
    return p;                                                                 \
  }                                                                           \
                                                                              \
  M_IF_DEBUG(                                                                 \
  /* Compute the depth of a node */                                           \
  static size_t                                                               \
  M_F(name, int, depth)(const node_t *n)                                      \
  {                                                                           \
    if (n == NULL) return 1;                                                  \
    return RBTREEI_IS_BLACK (n)                                               \
      + M_F(name, int, depth)(n->child[0]);                                   \
  }                                                                           \
  )                                                                           \
                                                                              \
  static inline bool                                                          \
  M_F(name, pop_at)(type *data_ptr, tree_t tree, type const key)             \
  {                                                                           \
    RBTREEI_CONTRACT (tree);                                                  \
    node_t *tab[RBTREEI_MAX_STACK];                                           \
    int8_t  which[RBTREEI_MAX_STACK];                                         \
    unsigned int cpt = 0;                                                     \
    node_t *n = tree->node;                                                   \
    which[0] = 0;                                                             \
    tab[cpt++] = (node_t*)(void*) &tree->node; /* FIXME: To clean! */	        \
    /* Search for the deletion point */                                       \
    tab[cpt] = n;                                                             \
    while (n != NULL) {                                                       \
      RBTREEI_CONTRACT_NODE(n);                                              \
      M_ASSERT(M_F(name, int, depth)(n->child[0])                             \
               == M_F(name, int, depth)(n->child[1]));                          \
      int cmp = M_CALL_CMP(oplist, n->data, key);                             \
      if (cmp == 0) {                                                         \
        break;                                                                \
      }                                                                       \
      int i = (cmp < 0);                                                      \
      which[cpt++] = (int8_t) i;                                              \
      n = n->child[i];                                                        \
      M_ASSERT(cpt < RBTREEI_MAX_STACK);                                     \
      tab[cpt] = n;                                                           \
    }                                                                         \
    M_ASSERT(tab[cpt] == n);                                                 \
    /* If not found, fail */                                                  \
    if (n == NULL) {                                                          \
      return false;                                                           \
    }                                                                         \
    unsigned int cpt_n = cpt;                                                 \
    node_t *v = n;     /* the replacement node */                             \
    node_t *u;         /* the deleted node */                                 \
    rbtreei_color_e v_color = RBTREEI_GET_COLOR(v);                           \
    /* Classical removal of a node from a binary tree */                      \
    if (v->child[0] != NULL && v->child[1] != NULL) {                         \
      /* node has 2 child. */                                                 \
      /* Get the element right next to the deleted one */                     \
      v = v->child[1];                                                        \
      which[cpt++] = 1;                                                       \
      tab[cpt] = v;                                                           \
      while (v != NULL) {                                                     \
        /* Always left node */                                                \
        RBTREEI_CONTRACT_NODE (v);                                            \
        M_ASSERT(M_F(name, int, depth)(v->child[0])                           \
               == M_F(name, int, depth)(v->child[1]));                        \
        which[cpt++] = 0;                                                     \
        v = v->child[0];                                                      \
        M_ASSERT(cpt < RBTREEI_MAX_STACK);                                   \
        tab[cpt] = v;                                                         \
      }                                                                       \
      /* Pop the last element to get the last non-null element */             \
      v = tab[--cpt];                                                         \
      M_ASSERT(v != NULL);                                                   \
      u = v->child[1];                                                        \
      /* Replace 'v' by 'u' in the tree */                                    \
      M_ASSERT(tab[cpt-1]->child[which[cpt-1]] == v);                         \
      tab[cpt-1]->child[which[cpt-1]] = u;                                    \
      /* Replace 'n' by 'v' in the tree */                                    \
      M_ASSERT(tab[cpt_n-1] != NULL);                                         \
      M_ASSERT(tab[cpt_n-1]->child[which[cpt_n-1]] == n);                     \
      tab[cpt_n-1]->child[which[cpt_n-1]] = v;                                \
      v->child[0] = n->child[0];                                              \
      v->child[1] = n->child[1];                                              \
      v_color = RBTREEI_GET_COLOR(v);                                         \
      RBTREEI_COPY_COLOR(v, n);                                               \
      tab[cpt_n] = v;                                                         \
      /* For the algorithm, 'u' is now the deleted node */                    \
    } else {                                                                  \
      /* 1 or no child to the node. Replace the element */                    \
      v = n;                                                                  \
      u = v->child[(n->child[0] == NULL)];                                    \
      M_ASSERT(cpt_n >= 1 && tab[cpt_n-1]->child[which[cpt_n-1]] == n);      \
      M_ASSERT(n->child[(n->child[0] != NULL)] == NULL);                     \
      tab[cpt_n-1]->child[which[cpt_n-1]] = u;                                \
      /* in all cases, this node shall be set to black */                     \
    }                                                                         \
                                                                              \
    /* Rebalance from child to root */                                        \
    if (v_color == RBTREE_BLACK                                               \
        && M_F(name, int, is_black)(u)) {                                     \
      /* tab[0] is NULL, tab[1] is root, u is double black */                 \
      node_t *p = u, *s;                                                      \
      while (cpt >= 2) {                                                      \
        p  = tab[--cpt];                                                      \
        bool nbChild = which[cpt];                                            \
        M_ASSERT(p != NULL && u == p->child[nbChild]);                       \
        s = p->child[!nbChild];                                               \
        /* if sibling is red, perform a rotation to move sibling up */        \
        if (!M_F(name, int, is_black)(s)) {                                   \
          p = M_F(name, int, rotate) (p, tab[cpt-1], !nbChild);               \
          RBTREEI_SET_BLACK(p); /* was sibling */                             \
          tab[cpt] = p;                                                       \
          which[cpt++] = nbChild;                                             \
          p = p->child[nbChild];  /* was parent */                            \
          M_ASSERT(p != NULL);                                               \
          RBTREEI_SET_RED(p);                                                 \
          s = p->child[!nbChild];                                             \
          M_ASSERT (M_C(name, _int_is_black)(s));                             \
        }                                                                     \
        M_ASSERT(p != NULL && u == p->child[nbChild]);                       \
        /* if both childreen of s are black */                                \
        /* perform recoloring and recur on parent if black */                 \
        if (s != NULL                                                         \
            && M_C(name, _int_is_black)(s->child[0])                          \
            && M_C(name, _int_is_black)(s->child[1])) {                       \
          M_ASSERT(M_F(name, int, depth)(s->child[0]) == M_C(name, _int_depth)(s->child[1])); \
          RBTREEI_SET_RED(s);                                                 \
          if (RBTREEI_IS_RED(p)) {                                            \
            RBTREEI_SET_BLACK(p);                                             \
            RBTREEI_CONTRACT_NODE(p);                                         \
            M_ASSERT(M_F(name, int, depth)(p->child[0]) == M_C(name, _int_depth)(p->child[1])); \
            break;                                                            \
          }                                                                   \
          u = p;                                                              \
        } else  {                                                             \
          M_ASSERT(s != NULL);                                               \
          /* at least one child of 's' is red */                              \
          /* perform rotation(s) */                                           \
          bool childIsRight = !M_F(name, int, is_black)(s->child[1]);        \
          rbtreei_color_e p_color = RBTREEI_GET_COLOR(p);                    \
          if (childIsRight != nbChild) {                                      \
            /* left-left or right-right case */                               \
            p = M_C(name, _int_rotate) (p, tab[cpt-1], childIsRight);         \
          } else {                                                            \
            s = M_C(name, _int_rotate) (s, p, childIsRight);                  \
            p = M_C(name, _int_rotate) (p, tab[cpt-1], !nbChild);             \
          }                                                                   \
          RBTREEI_SET_COLOR(p, p_color);                                      \
          M_ASSERT(p->child[0] != NULL && p->child[1] != NULL);               \
          RBTREEI_SET_BLACK(p->child[0]);                                     \
          RBTREEI_SET_BLACK(p->child[1]);                                     \
          RBTREEI_CONTRACT_NODE(p);                                           \
          M_ASSERT(M_C(name, _int_depth)(p->child[0]) == M_C(name, _int_depth)(p->child[1])); \
          break;                                                              \
        }                                                                     \
      } /* while */                                                           \
      if (cpt == 1 /* root has been reached? */ ) {                           \
        M_F(name, int, set_black)(p);                                         \
        M_ASSERT(tree->node == p);                                           \
      }                                                                       \
    } else {                                                                  \
      M_C(name, _int_set_black)(u);                                           \
    }                                                                         \
    M_ASSERT(tree->node == NULL || RBTREEI_IS_BLACK(tree->node));            \
    /* delete it */                                                     \
    if (data_ptr != NULL)                                               \
      M_DO_MOVE(oplist, *data_ptr, n->data);                            \
    else                                                                \
      M_CALL_CLEAR(oplist, n->data);                                    \
    M_F(name, int, del)(n);						                                  \
    tree->size--;                                                      \
    RBTREEI_CONTRACT(tree);                                            \
    return true;                                                        \
  }                                                                     \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool M_F(name, M_NAMING_TEST_EQUAL_TO)                     \
    (const tree_t t1, const tree_t t2) {                                \
    RBTREEI_CONTRACT(t1);                                                     \
    RBTREEI_CONTRACT(t2);                                                     \
    if (t1->size != t2->size) return false;                                   \
    it_t it1;                                                                 \
    it_t it2;                                                                 \
    /* NOTE: We can't compare two tree directly as they can be                \
       structurally different but functionality equal (you get this by  \
       constructing the tree in a different way). We have to                  \
       compare the ordered value within the tree. */                          \
    M_F(name, M_NAMING_IT_FIRST)(it1, t1);                                            \
    M_F(name, M_NAMING_IT_FIRST)(it2, t2);                                            \
    while (!M_F(name, M_NAMING_IT_TEST_END)(it1)                        \
           && !M_F(name, M_NAMING_IT_TEST_END)(it2)) {                  \
      type const *ref1 = M_F(name, cref)(it1);                         \
      type const *ref2 = M_F(name, cref)(it2);                         \
      if (M_CALL_EQUAL(oplist, *ref1, *ref2) == false)                        \
        return false;                                                         \
      M_F(name, next)(it1);                                            \
      M_F(name, next)(it2);                                            \
    }                                                                         \
    return M_F(name, M_NAMING_IT_TEST_END)(it1)                         \
        && M_F(name, M_NAMING_IT_TEST_END)(it2);                          \
  }                                                                           \
  , /* NO EQUAL METHOD */ )                                                   \
                                                                              \
  M_IF_METHOD(HASH, oplist)(                                                  \
  static inline size_t M_F(name, hash)(const tree_t t1) {                     \
    RBTREEI_CONTRACT(t1);                                                     \
    M_HASH_DECL(hash);                                                        \
    /* NOTE: We can't compute the hash directly for the same reason           \
       than for EQUAL operator. */                                            \
    it_t it1;                                                                 \
    M_F(name, M_NAMING_IT_FIRST)(it1, t1);                                            \
    while (!M_F(name, M_NAMING_IT_TEST_END)(it1)) {                                   \
      type const *ref1 = M_F(name, cref)(it1);                         \
      M_HASH_UP(hash, M_CALL_HASH(oplist, *ref1));                            \
      M_F(name, next)(it1);                                            \
    }                                                                         \
    return M_HASH_FINAL(hash);                                               \
  }                                                                           \
  , /* NO HASH METHOD */ )                                                    \
                                                                              \
  M_IF_METHOD(GET_STR, oplist)(                                               \
  static inline void M_F(name, get_str)(string_t str,                         \
                                         tree_t const t1, bool append) {      \
    RBTREEI_CONTRACT(t1);                                                     \
    M_ASSERT(str != NULL);                                                    \
    (append ? M_F(string, M_NAMING_CONCATENATE_WITH, cstr):                   \
              M_F(string, M_NAMING_SET_AS, cstr)) (str, "[");                 \
    /* NOTE: The print is really naive, and not really efficient */           \
    bool commaToPrint = false;                                                \
    it_t it1;                                                                 \
    M_F(name, M_NAMING_IT_FIRST)(it1, t1);                                            \
    while (!M_F(name, M_NAMING_IT_TEST_END)(it1)) {                                   \
      if (commaToPrint)                                                       \
        M_F(string, push_back) (str, M_GET_SEPARATOR oplist);                       \
      commaToPrint = true;                                                    \
      type const *ref1 = M_F(name, cref)(it1);                         \
      M_CALL_GET_STR(oplist, str, *ref1, true);                               \
      M_F(name, next)(it1);                                            \
    }                                                                         \
    M_F(string, push_back) (str, ']');                                              \
  }                                                                           \
  , /* NO GET_STR */ )                                                        \
                                                                              \
  M_IF_METHOD(OUT_STR, oplist)(                                               \
  static inline void                                                          \
  M_F(name, out_str)(FILE *file, tree_t const rbtree)                  \
  {                                                                           \
    RBTREEI_CONTRACT(rbtree);                                                 \
    M_ASSERT(file != NULL);                                                  \
    fputc ('[', file);                                                        \
    it_t it;                                                                  \
    bool commaToPrint = false;                                                \
    for (M_F(name, M_NAMING_IT_FIRST)(it, rbtree) ;                                   \
         !M_F(name, M_NAMING_IT_TEST_END)(it);                                        \
         M_F(name, next)(it)){                                         \
      if (commaToPrint)                                                       \
        fputc (M_GET_SEPARATOR oplist, file);                                 \
      commaToPrint = true;                                                    \
      type const *item = M_F(name, cref)(it);                          \
      M_CALL_OUT_STR(oplist, file, *item);                                    \
    }                                                                         \
    fputc (']', file);                                                        \
  }                                                                           \
  , /* no out_str */ )                                                        \
                                                                              \
  M_IF_METHOD(PARSE_CSTR, oplist)(                                             \
  static inline bool                                                          \
  M_F(name, parse_cstr)(tree_t rbtree, const char str[], const char **endp) \
  {                                                                           \
    RBTREEI_CONTRACT(rbtree);                                                 \
    M_ASSERT(str != NULL);                                               \
    M_F(name, M_NAMING_CLEAN)(rbtree);                                           \
    bool success = false;                                                     \
    int c = *str++;                                                           \
    if (M_UNLIKELY(c != '[')) goto exit;                                     \
    c = *str++;                                                               \
    if (M_UNLIKELY(c == ']')) { success = true; goto exit; }                 \
    if (M_UNLIKELY(c == 0)) goto exit;                                       \
    str--;                                                                    \
    type item;                                                                \
    M_CALL_INIT(oplist, item);                                                \
    do {                                                                      \
      bool b = M_CALL_PARSE_CSTR(oplist, item, str, &str);                     \
      do { c = *str++; } while (isspace(c));                                  \
      if (b == false || c == 0) goto exit_clear;                              \
      M_F(name, push)(rbtree, item);                                   \
    } while (c == M_GET_SEPARATOR oplist);                                    \
    success = (c == ']');                                                     \
  exit_clear:                                                                 \
    M_CALL_CLEAR(oplist, item);                                               \
  exit:                                                                       \
    if (endp) *endp = str;                                                    \
    return success;                                                           \
  }                                                                           \
  , /* no parse_cstr */ )                                                      \
                                                                              \
  M_IF_METHOD(IN_STR, oplist)(                                                \
  static inline bool                                                          \
  M_F(name, in_str)(tree_t rbtree, FILE *file)                         \
  {                                                                           \
    RBTREEI_CONTRACT(rbtree);                                                 \
    M_ASSERT(file != NULL);                                              \
    M_F(name, M_NAMING_CLEAN)(rbtree);                                           \
    int c = fgetc(file);                                                      \
    if (M_UNLIKELY (c != '[')) return false;                                  \
    c = fgetc(file);                                                          \
    if (M_UNLIKELY (c == ']')) return true;                                   \
    if (M_UNLIKELY (c == EOF)) return false;                                  \
    ungetc(c, file);                                                          \
    type item;                                                                \
    M_CALL_INIT(oplist, item);                                                \
    do {                                                                      \
      bool b = M_CALL_IN_STR(oplist, item, file);                             \
      do { c = fgetc(file); } while (isspace(c));                             \
      if (b == false || c == EOF) break;                                      \
      M_F(name, push)(rbtree, item);                                   \
    } while (c == M_GET_SEPARATOR oplist);                                    \
    M_CALL_CLEAR(oplist, item);                                               \
    return c == ']';                                                          \
  }                                                                           \
  , /* no in_str */ )                                                         \
                                                                              \
  M_IF_METHOD(OUT_SERIAL, oplist)(                                            \
  static inline m_serial_return_code_t                                        \
  M_F(name, out_serial)(m_serial_write_t f, tree_t const t1)           \
  {                                                                           \
    RBTREEI_CONTRACT(t1);                                                     \
    M_ASSERT(f != NULL && f->m_interface != NULL);                           \
    m_serial_local_t local;                                                   \
    m_serial_return_code_t ret;                                               \
    M_T(name, subtype, ct) const *item;                                       \
    bool first_done = false;                                                  \
    it_t it;                                                                  \
    ret = f->m_interface->write_array_start(local, f, t1->size);              \
    for (M_F(name, M_NAMING_IT_FIRST)(it, t1) ;                                       \
         !M_F(name, M_NAMING_IT_TEST_END)(it);                                        \
         M_F(name, next)(it)){                                         \
      item = M_F(name, cref)(it);                                      \
      if (first_done)                                                         \
        ret |= f->m_interface->write_array_next(local, f);                    \
      ret |= M_CALL_OUT_SERIAL(oplist, f, *item);                             \
      first_done = true;                                                      \
    }                                                                         \
    ret |= f->m_interface->write_array_end(local, f);                         \
    return ret & M_SERIAL_FAIL;                                               \
  }                                                                           \
  , /* no OUT_SERIAL */ )                                                     \
                                                                              \
  M_IF_METHOD(IN_SERIAL, oplist)(                                             \
  static inline m_serial_return_code_t                                        \
  M_F(name, in_serial)(tree_t t1, m_serial_read_t f)                   \
  {                                                                           \
    RBTREEI_CONTRACT(t1);                                                     \
    M_ASSERT(f != NULL && f->m_interface != NULL);                           \
    m_serial_local_t local;                                                   \
    m_serial_return_code_t ret;                                               \
    size_t estimated_size = 0;                                                \
    type key;                                                                 \
    M_F(name, M_NAMING_CLEAN)(t1);                                               \
    ret = f->m_interface->read_array_start(local, f, &estimated_size);        \
    if (M_UNLIKELY(ret != M_SERIAL_OK_CONTINUE)) return ret;                 \
    M_CALL_INIT(oplist, key);                                                 \
    do {                                                                      \
      ret = M_CALL_IN_SERIAL(oplist, key, f);                                 \
      if (ret != M_SERIAL_OK_DONE) { break; }                                 \
      M_F(name, push)(t1, key);                                        \
    } while ((ret = f->m_interface->read_array_next(local, f)) == M_SERIAL_OK_CONTINUE); \
    M_CALL_CLEAR(oplist, key);                                                \
    return ret;                                                               \
  }                                                                           \
  , /* no in_serial */ )                                                      \
                                                                              \
                                                                              \

// TODO: specialized _sort shall do nothing, but shall check the requested order. How ?


#endif
