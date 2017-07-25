/*
 * M*LIB - RED BLACK TREE module
 *
 * Copyright (c) 2017, Patrick Pelissier
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
#ifndef __M_RBTREE_H
#define __M_RBTREE_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "m-core.h"

/* Define a Red/Black binary tree of a given type.
   USAGE: RBTREE_DEF(name, type [, oplist_of_the_type]) */
#define RBTREE_DEF(name, ...)                                           \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                           \
  (RBTREEI_DEF2(name, __VA_ARGS__, M_DEFAULT_OPLIST,                    \
                M_C3(rbtree_, name, _t), struct M_C3(rbtree_node_, name, _s), M_C3(rbtree_it_, name, _t)), \
   RBTREEI_DEF2(name, __VA_ARGS__ ,                                     \
                M_C3(rbtree_, name, _t), struct M_C3(rbtree_node_, name, _s), M_C3(rbtree_it_, name, _t)))

/* Define the oplist of a rbtree of type.
   USAGE: RBTREE_OPLIST(name [, oplist_of_the_type]) */
#define RBTREE_OPLIST(...)                                            \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                         \
  (RBTREEI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST ),                    \
   RBTREEI_OPLIST(__VA_ARGS__ ))


/********************************** INTERNAL ************************************/

#define RBTREEI_OPLIST(name, oplist)                                    \
  (INIT(M_C3(rbtree_, name, _init)),                                    \
   INIT_SET(M_C3(rbtree_, name, _init_set)),                            \
   SET(M_C3(rbtree_, name, _set)),                                      \
   CLEAR(M_C3(rbtree_, name, _clear)),                                  \
   INIT_MOVE(M_C3(rbtree_, name, _init_move)),                          \
   MOVE(M_C3(rbtree_, name, _move)),                                    \
   SWAP(M_C3(rbtree_, name, _swap)),                                    \
   TYPE(M_C3(rbtree_,name,_t)),                                         \
   SUBTYPE(M_C3(rbtree_type_,name,_t)),                                 \
   IT_TYPE(M_C3(rbtree_it_,name,_t)),                                   \
   IT_FIRST(M_C3(rbtree_,name,_it)),                                    \
   IT_SET(M_C3(rbtree_,name,_it_set)),                                  \
   IT_LAST(M_C3(rbtree_,name,_it_last)),                                \
   IT_END(M_C3(rbtree_,name,_it_end)),                                  \
   IT_END_P(M_C3(rbtree_,name,_end_p)),                                 \
   IT_LAST_P(M_C3(rbtree_,name,_last_p)),                               \
   IT_EQUAL_P(M_C3(rbtree_,name,_it_equal_p)),                          \
   IT_NEXT(M_C3(rbtree_,name,_next)),                                   \
   IT_REF(M_C3(rbtree_,name,_ref)),                                     \
   IT_CREF(M_C3(rbtree_,name,_cref)),                                   \
   CLEAN(M_C3(rbtree_,name,_clean)),                                    \
   PUSH(M_C3(rbtree_,name,_push)),                                      \
   POP(M_C3(rbtree_,name,_pop))                                         \
   GET_MIN(M_C3(rbtree_,name,_min)),                                    \
   GET_MAX(M_C3(rbtree_,name,_max)),                                    \
   M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C3(rbtree_, name, _get_str)),), \
   M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C3(rbtree_, name, _out_str)),), \
   M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C3(rbtree_, name, _in_str)),),  \
   M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C3(rbtree_, name, _equal_p)),),   \
   M_IF_METHOD(HASH, oplist)(HASH(M_C3(rbtree_, name, _hash)),)         \
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

/* Max depth of the binary tree
   It is twice the depth of a perfectly even tree with maximum elements.
   Max number of elements is max of size_t.
   A perfectly even tree is log2(max(size_t))<CHAR_BIT*sizeof(size_t)
 */
#define RBTREEI_MAX_STACK (2*CHAR_BIT*sizeof (size_t))

/* Encapsulation of the color of the nodes. */
#define RBTREEI_SET_RED(x)   ((x)->color =  RBTREE_RED)
#define RBTREEI_SET_BLACK(x) ((x)->color =  RBTREE_BLACK)
#define RBTREEI_IS_RED(x)    ((x)->color == RBTREE_RED)
#define RBTREEI_IS_BLACK(x)  ((x)->color == RBTREE_BLACK)
#define RBTREEI_COPY_COLOR(x,y) ((x)->color = (y)->color)

typedef enum {
  RBTREE_BLACK = 0, RBTREE_RED
} rbtreei_color_e;

// General contact of a tree
#define RBTREEI_CONTRACT(tree)                                          \
  assert ((tree) != NULL);                                              \
  assert ((tree)->node == NULL || RBTREEI_IS_BLACK((tree)->node));      \
  assert ((tree)->size != 0 || (tree)->node == NULL);                   \
  
// Contract of a node (doesn't check for equal depth in black)
#define RBTREEI_CONTRACT_NODE(node)                                     \
  assert((node) != NULL);                                               \
  assert(RBTREEI_IS_BLACK(node) || RBTREEI_IS_RED(node));               \
  assert(RBTREEI_IS_BLACK(node)                                         \
         || (((node)->child[0] == NULL || RBTREEI_IS_BLACK(node->child[0])) \
             && ((node)->child[1] == NULL || RBTREEI_IS_BLACK(node->child[1]))))

#define RBTREEI_DEF2(name, type, oplist, tree_t, node_t, tree_it_t)     \
                                                                        \
  node_t {                                                              \
    node_t *child[2];                                                   \
    type data;                                                          \
    rbtreei_color_e color;                                              \
  };                                                                    \
                                                                        \
  typedef struct M_C3(rbtree_, name, _s) {                              \
    size_t size;                                                        \
    node_t *node;                                                       \
  } tree_t[1];                                                          \
                                                                        \
  typedef struct {                                                      \
    node_t *stack[RBTREEI_MAX_STACK];                                   \
    char    which[RBTREEI_MAX_STACK];                                   \
    unsigned int cpt;                                                   \
  } tree_it_t[1];                                                       \
                                                                        \
  typedef type M_C3(rbtree_type_, name, _t);                            \
                                                                        \
  M_IF_METHOD(MEMPOOL, oplist)(                                         \
    MEMPOOL_DEF(M_C(mempool_rbtree_,name), node_t)                      \
    M_GET_MEMPOOL_LINKAGE oplist M_C3(mempool_rbtree_, name, _t) M_GET_MEMPOOL oplist; \
    static inline node_t *M_C3(rbtreei_,name,_new)(void) {              \
      return M_C3(mempool_rbtree_, name, _alloc)(M_GET_MEMPOOL oplist); \
    }                                                                   \
    static inline void M_C3(rbtreei_,name,_del)(node_t *ptr) {          \
      M_C3(mempool_rbtree_, name, _free)(M_GET_MEMPOOL oplist, ptr);    \
    }                                                                   \
    RBTREEI_DEF3(name, type, oplist, tree_t, node_t, tree_it_t)         \
    , /* No mempool allocation */                                       \
    static inline node_t *M_C3(rbtreei_,name,_new)(void) {              \
      return M_GET_NEW oplist (node_t);                                 \
    }                                                                   \
    static inline void M_C3(rbtreei_,name,_del)(node_t *ptr) {          \
      M_GET_DEL oplist (ptr);                                           \
    }                                                                   \
    RBTREEI_DEF3(name, type, oplist, tree_t, node_t, tree_it_t) )

#define RBTREEI_DEF3(name, type, oplist, tree_t, node_t, tree_it_t)     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _init)(tree_t tree)                               \
  {                                                                     \
    assert (tree != NULL);                                              \
    tree->size = 0;                                                     \
    tree->node = NULL;                                                  \
    RBTREEI_CONTRACT(tree);                                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _clean)(tree_t tree)                              \
  {                                                                     \
    RBTREEI_CONTRACT(tree);                                             \
    node_t *stack[RBTREEI_MAX_STACK];                                   \
    unsigned int cpt = 0;                                               \
    if (tree->node == NULL) return;                                     \
    stack[cpt++] = tree->node;                                          \
    while (cpt > 0) {                                                   \
      node_t *n = stack[cpt-1];                                         \
      while (true) {                                                    \
        RBTREEI_CONTRACT_NODE(n);                                       \
        if (n->child[0] != NULL) {                                      \
          assert (cpt < RBTREEI_MAX_STACK);                             \
          stack[cpt++] = n->child[0];                                   \
          n = n->child[0];                                              \
          stack[cpt-2]->child[0] = NULL;                                \
        } else if (n->child[1] != NULL) {                               \
          assert (cpt < RBTREEI_MAX_STACK);                             \
          stack[cpt++] = n->child[1];                                   \
          n = n->child[1];                                              \
          stack[cpt-2]->child[1] = NULL;                                \
        } else                                                          \
          break;                                                        \
      }                                                                 \
      assert (n == stack[cpt - 1]);                                     \
      M_GET_CLEAR oplist (n->data);                                     \
      M_C3(rbtreei_,name,_del) (n);                                     \
      assert((stack[cpt-1] = NULL) == NULL);                            \
      cpt--;                                                            \
    }                                                                   \
    tree->node = NULL;                                                  \
    tree->size = 0;                                                     \
   }                                                                    \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _clear)(tree_t tree)                              \
  {                                                                     \
    M_C3(rbtree_, name, _clean)(tree);                                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _push)(tree_t tree, type const data)              \
  {                                                                     \
    RBTREEI_CONTRACT(tree);                                             \
    node_t *tab[RBTREEI_MAX_STACK];                                     \
    char which[RBTREEI_MAX_STACK];                                      \
    unsigned int cpt = 0;                                               \
    node_t *n = tree->node;                                             \
    /* If nothing, create new node */                                   \
    if (n == NULL) {                                                    \
      n = M_C3(rbtreei_,name,_new)();                                   \
      if (M_UNLIKELY (n == NULL)) {                                     \
        M_MEMORY_FULL(sizeof (node_t));                                 \
        return;                                                         \
      }                                                                 \
      M_GET_INIT_SET oplist (n->data, data);                            \
      n->child[0] = n->child[1] = NULL;                                 \
      RBTREEI_SET_BLACK (n);                                            \
      tree->node = n;                                                   \
      assert(tree->size == 0);                                          \
      tree->size = 1;                                                   \
      RBTREEI_CONTRACT(tree);                                           \
      return;                                                           \
    }                                                                   \
    /* Search for insertion point */                                    \
    tab[cpt] = n;                                                       \
    while (n != NULL) {                                                 \
      RBTREEI_CONTRACT_NODE(n);                                         \
      int cmp = M_GET_CMP oplist (n->data, data);                       \
      if (cmp == 0) {                                                   \
        break;                                                          \
      } else if (cmp > 0) {                                             \
        which[cpt++] = 0;                                               \
        n = n->child[0];                                                \
      } else {                                                          \
        which[cpt++] = 1;                                               \
        n = n->child[1];                                                \
      }                                                                 \
      assert (cpt < RBTREEI_MAX_STACK);                                 \
      tab[cpt] = n;                                                     \
    }                                                                   \
    /* If found, update the data (default is set) */                    \
    if (n != NULL) {                                                    \
      M_IF_METHOD (UPDATE, oplist)(M_GET_UPDATE oplist, M_GET_SET oplist) \
        (n->data, data);                                                \
      RBTREEI_CONTRACT (tree);                                          \
      return;                                                           \
    }                                                                   \
    /* Create new */                                                    \
    n = M_C3(rbtreei_,name,_new)();                                     \
    if (M_UNLIKELY (n == NULL) ) {                                      \
      M_MEMORY_FULL (sizeof (node_t));                                  \
      return;                                                           \
    }                                                                   \
    M_GET_INIT_SET oplist (n->data, data);                              \
    n->child[0] = n->child[1] = NULL;                                   \
    RBTREEI_SET_RED (n);                                                \
    assert (tab[cpt] == NULL);                                          \
    tab[cpt] = n;                                                       \
    tree->size ++;                                                      \
    /* Add it in the tree */                                            \
    assert(tab[cpt-1]->child[0+which[cpt-1]] == NULL);                  \
    tab[cpt-1]->child[0+which[cpt-1]] = n;                              \
    /* Fix the tree */                                                  \
    while (cpt >= 2                                                     \
           && RBTREEI_IS_RED(tab[cpt-1])                                \
           && tab[cpt-2]->child[1-which[cpt-2]] != NULL                 \
           && RBTREEI_IS_RED(tab[cpt-2]->child[1-which[cpt-2]])) {      \
      RBTREEI_SET_BLACK(tab[cpt-1]);                                    \
      RBTREEI_SET_BLACK(tab[cpt-2]->child[1-which[cpt-2]]);             \
      RBTREEI_SET_RED(tab[cpt-2]);                                      \
      cpt-=2;                                                           \
    }                                                                   \
    /* root is always black */                                          \
    RBTREEI_SET_BLACK(tab[0]);                                          \
    if (cpt <= 1 || RBTREEI_IS_BLACK(tab[cpt-1])) {                     \
      RBTREEI_CONTRACT (tree);                                          \
      return;                                                           \
    }                                                                   \
    /* Read the grand-father, the father and the element */             \
    node_t *pp = tab[cpt-2];                                            \
    node_t *p  = tab[cpt-1];                                            \
    node_t *x  = tab[cpt];                                              \
    /* We need to do some rotations */                                  \
    if (which[cpt-2] == 0) {                                            \
      /* The father is the left child of the grand-father */            \
      if (which[cpt-1] == 0) {                                          \
        /* The child is the left child of its father */                 \
        /* Right rotation: cpt is the new grand-father.                 \
           x is its left child, the grand-father is the right one */    \
        pp->child[0] = p->child[1];                                     \
        p->child[0] = x;                                                \
        p->child[1] = pp;                                               \
        RBTREEI_SET_BLACK(p);                                           \
        RBTREEI_SET_RED(pp);                                            \
      } else {                                                          \
        /* The child is the right child of its father */                \
        /* Left rotation */                                             \
        pp->child[0] = x->child[1];                                     \
        p->child[1]  = x->child[0];                                     \
        x->child[0]  = p;                                               \
        x->child[1]  = pp;                                              \
        RBTREEI_SET_BLACK(x);                                           \
        RBTREEI_SET_RED(p);                                             \
        RBTREEI_SET_RED(pp);                                            \
        p = x;                                                          \
      }                                                                 \
    } else {                                                            \
      /* The father is the right child of the grand-father */           \
      if (which[cpt-1] == 0) {                                          \
        /* The child is the left child of its father */                 \
        pp->child[1] = x->child[0];                                     \
        p->child[0]  = x->child[1];                                     \
        x->child[1]  = p;                                               \
        x->child[0]  = pp;                                              \
        RBTREEI_SET_BLACK(x);                                           \
        RBTREEI_SET_RED(p);                                             \
        RBTREEI_SET_RED(pp);                                            \
        p = x;                                                          \
      } else {                                                          \
        /* The child is the right child of its father */                \
        pp->child[1] = p->child[0];                                     \
        p->child[1] = x;                                                \
        p->child[0] = pp;                                               \
        RBTREEI_SET_BLACK(p);                                           \
        RBTREEI_SET_RED(pp);                                            \
      }                                                                 \
    }                                                                   \
    /* Insert the new grand father */                                   \
    if (cpt == 2) {                                                     \
      tree->node = p;                                                   \
    } else {                                                            \
      assert (cpt >= 3);                                                \
      tab[cpt-3]->child[(int) which[cpt-3]] = p;                        \
    }                                                                   \
    /* Done */                                                          \
    RBTREEI_CONTRACT (tree);                                            \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C3(rbtree_, name, _size)(const tree_t tree)                         \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    return tree->size;                                                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtreei_, name, _it)(tree_it_t it, const tree_t tree, int child) \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    assert (it != NULL);                                                \
    assert (child == 0 || child == 1);                                  \
    unsigned int cpt = 0;                                               \
    if (tree->node != NULL) {                                           \
      it->which[cpt] = child;                                           \
      node_t *n = it->stack[cpt++] = tree->node;                        \
      while (n->child[child] != NULL) {                                 \
        assert (cpt < RBTREEI_MAX_STACK);                               \
        n = n->child[child];                                            \
        it->which[cpt] = child;                                         \
        it->stack[cpt++] = n;                                           \
      }                                                                 \
      assert (n == it->stack[cpt - 1]);                                 \
    }                                                                   \
    it->cpt = cpt;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _it)(tree_it_t it, const tree_t tree)             \
  {                                                                     \
    M_C3(rbtreei_, name, _it)(it, tree, 0);                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _it_last)(tree_it_t it, const tree_t tree)        \
  {                                                                     \
    M_C3(rbtreei_, name, _it)(it, tree, 1);                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _it_end)(tree_it_t it, const tree_t tree)         \
  {                                                                     \
    assert (it != NULL && tree != NULL);                                \
    it->cpt = 0;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _it_set)(tree_it_t it, const tree_it_t ref)       \
  {                                                                     \
    assert (it != NULL && ref != NULL);                                 \
    *it = *ref;                                                         \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(rbtree_, name, _end_p)(const tree_it_t it)                       \
  {                                                                     \
    assert (it != NULL);                                                \
    return it->cpt == 0;                                                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtreei_, name, _next)(tree_it_t it, int child)                  \
  {                                                                     \
    assert (it != NULL);                                                \
    assert (child == 0 || child == 1);                                  \
    if (it->cpt == 0) return;                                           \
    unsigned int cpt = it->cpt - 1;                                     \
    node_t *n = it->stack[cpt];                                         \
    const int right = 1 ^ child;                                        \
    if (n->child[right] != NULL && it->which[cpt] == child) {           \
      /* Going right */                                                 \
      assert (cpt < RBTREEI_MAX_STACK);                                 \
      n = n->child[right];                                              \
      it->which[cpt++] = right;                                         \
      it->stack[cpt] = n;                                               \
      it->which[cpt++] = child;                                         \
      /* Going left */                                                  \
      while (n->child[child] != NULL) {                                 \
        assert (cpt < RBTREEI_MAX_STACK);                               \
        n = n->child[child];                                            \
        it->which[cpt] = child;                                         \
        it->stack[cpt++] = n;                                           \
      }                                                                 \
      assert (n == it->stack[cpt - 1]);                                 \
    } else {                                                            \
      /* Going up */                                                    \
      while (cpt > 0 && it->which[cpt-1] == right) cpt--;               \
    }                                                                   \
    it->cpt = cpt;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _next)(tree_it_t it)                              \
  {                                                                     \
    M_C3(rbtreei_, name, _next)(it, 0);                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _previous)(tree_it_t it)                          \
  {                                                                     \
    M_C3(rbtreei_, name, _next)(it, 1);                                 \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(rbtree_, name, _ref)(const tree_it_t it)                         \
  {                                                                     \
    assert(it != NULL && it->cpt > 0);                                  \
    /* NOTE: partially unsafe if the user modify the order of the el */ \
    return &(it->stack[it->cpt-1]->data);                               \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(rbtree_, name, _cref)(const tree_it_t it)                        \
  {                                                                     \
    return M_CONST_CAST(type, M_C3(rbtree_, name, _ref)(it));           \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(rbtree_, name, _it_equal_p)(const tree_it_t it1,                 \
                                   const tree_it_t it2)                 \
  {                                                                     \
    return it1->cpt == it2->cpt                                         \
      && it1->stack[it1->cpt-1] == it2->stack[it2->cpt-1];              \
  }                                                                     \
                                                                        \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _it_from)(tree_it_t it,                           \
                                const tree_t tree, type const data)     \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    assert (it != NULL);                                                \
    unsigned int cpt = 0;                                               \
    node_t *n = tree->node;                                             \
    it->stack[cpt] =  n;                                                \
    while (n != NULL) {                                                 \
      int cmp = M_GET_CMP oplist (n->data, data);                       \
      if (cmp == 0)                                                     \
        break;                                                          \
      int child = (cmp < 0);                                            \
      it->which[cpt++] = child;                                         \
      n = n->child[child];                                              \
      assert (cpt < RBTREEI_MAX_STACK);                                 \
      it->stack[cpt] =  n;                                              \
    }                                                                   \
    it->cpt = cpt;                                                      \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
    M_C3(rbtree_, name, _it_to_p)(tree_it_t it, type const data)        \
  {                                                                     \
    assert (it != NULL);                                                \
    if (it->cpt == 0) return true;                                      \
    assert (it->cpt > 0 && it->cpt < RBTREEI_MAX_STACK);                \
    node_t *n = it->stack[it->cpt-1];                                   \
    int cmp = M_GET_CMP oplist (n->data, data);                         \
    return (cmp >= 0);                                                  \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(rbtree_, name, _min)(const tree_t tree)                          \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    node_t *n = tree->node;                                             \
    if (M_UNLIKELY (n == NULL) ) return NULL;                           \
    while (n->child[0] != NULL) {                                       \
      RBTREEI_CONTRACT_NODE (n);                                        \
      n = n->child[0];                                                  \
    }                                                                   \
    return &n->data;                                                    \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(rbtree_, name, _max)(const tree_t tree)                          \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    node_t *n = tree->node;                                             \
    if (M_UNLIKELY (n == NULL) ) return NULL;                           \
    while (n->child[1] != NULL) {                                       \
      RBTREEI_CONTRACT_NODE (n);                                        \
      n = n->child[1];                                                  \
    }                                                                   \
    return &n->data;                                                    \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(rbtree_, name, _cmin)(const tree_t tree)                         \
  {                                                                     \
    return M_CONST_CAST(type, M_C3(rbtree_, name, _min)(tree));         \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(rbtree_, name, _cmax)(const tree_t tree)                         \
  {                                                                     \
    return M_CONST_CAST(type, M_C3(rbtree_, name, _max)(tree));         \
  }                                                                     \
                                                                        \
  static inline type *                                                  \
  M_C3(rbtree_, name, _get)(const tree_t tree, type const data)         \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    node_t *n = tree->node;                                             \
    while (n != NULL) {                                                 \
      RBTREEI_CONTRACT_NODE (n);                                        \
      int cmp = M_GET_CMP oplist (n->data, data);                       \
      if (cmp == 0) {                                                   \
        return &n->data;                                                \
      } else if (cmp > 0) {                                             \
        n = n->child[0];                                                \
      } else {                                                          \
        n = n->child[1];                                                \
      }                                                                 \
    }                                                                   \
    return NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline const type *                                            \
  M_C3(rbtree_, name, _cget)(const tree_t tree, type const data)        \
  {                                                                     \
    return M_CONST_CAST(type, M_C3(rbtree_, name, _get)(tree, data));   \
  }                                                                     \
                                                                        \
  static inline node_t *                                                \
  M_C3(rbtreei_,name,_copyn)(const node_t *o)                           \
  {                                                                     \
    if (o == NULL) return NULL;                                         \
    node_t *n = M_C3(rbtreei_,name,_new)();                             \
    if (M_UNLIKELY (n == NULL) ) {                                      \
      M_MEMORY_FULL (sizeof (node_t));                                  \
      return NULL;                                                      \
    }                                                                   \
    M_GET_INIT_SET oplist (n->data, o->data);                           \
    n->child[0] = M_C3(rbtreei_,name,_copyn)(o->child[0]);              \
    n->child[1] = M_C3(rbtreei_,name,_copyn)(o->child[1]);              \
    RBTREEI_COPY_COLOR (n, o);                                          \
    return n;                                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _init_set)(tree_t tree, const tree_t ref)         \
  {                                                                     \
    RBTREEI_CONTRACT (ref);                                             \
    assert (tree != NULL && tree != ref);                               \
    tree->size = ref->size;                                             \
    tree->node = M_C3(rbtreei_,name,_copyn)(ref->node);                 \
    RBTREEI_CONTRACT (tree);                                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _set)(tree_t tree, const tree_t ref)              \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    RBTREEI_CONTRACT (ref);                                             \
    if (tree == ref) return;                                            \
    M_C3(rbtree_,name,_clear)(tree);                                    \
    M_C3(rbtree_,name,_init_set)(tree, ref);                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _init_move)(tree_t tree, tree_t ref)              \
  {                                                                     \
    RBTREEI_CONTRACT (ref);                                             \
    assert (tree != NULL && tree != ref);                               \
    tree->size = ref->size;                                             \
    tree->node = ref->node;                                             \
    ref->node = NULL;                                                   \
    ref->size = 0;                                                      \
    RBTREEI_CONTRACT (tree);                                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _move)(tree_t tree, tree_t ref)                   \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    RBTREEI_CONTRACT (ref);                                             \
    assert (tree != ref);                                               \
    M_C3(rbtree_,name,_clear)(tree);                                    \
    M_C3(rbtree_,name,_init_move)(tree, ref);                           \
    RBTREEI_CONTRACT (tree);                                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtree_, name, _swap)(tree_t tree1, tree_t tree2)                \
  {                                                                     \
    RBTREEI_CONTRACT (tree1);                                           \
    RBTREEI_CONTRACT (tree2);                                           \
    size_t size = tree1->size;                                          \
    tree1->size = tree2->size;                                          \
    tree2->size = size;                                                 \
    node_t *node = tree1->node;                                         \
    tree1->node = tree2->node;                                          \
    tree2->node = node;                                                 \
    RBTREEI_CONTRACT (tree1);                                           \
    RBTREEI_CONTRACT (tree2);                                           \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(rbtree_, name, _empty_p)(const tree_t tree)                      \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    return tree->size == 0;                                             \
  }                                                                     \
                                                                        \
  static inline rbtreei_color_e                                         \
  M_C3(rbtreei_, name, _get_color)(const node_t *n)                     \
  {                                                                     \
    return (n == NULL) ? RBTREE_BLACK : n->color;                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(rbtreei_, name, _set_color)(node_t *n, rbtreei_color_e c)        \
  {                                                                     \
    if (n != NULL) n->color = c;                                        \
  }                                                                     \
                                                                        \
  static inline node_t *                                                \
  M_C3(rbtreei_, name, _rotate)(node_t *pp, node_t *ppp, const bool right) \
  {                                                                     \
    assert (pp != NULL && ppp != NULL);                                 \
    bool left = !right;                                                 \
    node_t *p = pp->child[right];                                       \
    assert (p != NULL);                                                 \
    pp->child[right] = p->child[left];                                  \
    p->child[left]  = pp;                                               \
    /* Fix grandparent with new parent */                               \
    assert(ppp->child[0] == pp || ppp->child[1] == pp);                 \
    ppp->child[(ppp->child[0] != pp)] = p;                              \
    return p;                                                           \
  }                                                                     \
                                                                        \
  M_IF_DEBUG(                                                           \
  static size_t                                                         \
  M_C3(rbtreei_,name,_depth)(const node_t *n)                           \
  {                                                                     \
    if (n == NULL) return 1;                                            \
    return RBTREEI_IS_BLACK (n)                                         \
      + M_C3(rbtreei_,name,_depth)(n->child[0]);                        \
  }                                                                     \
  )                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(rbtree_, name, _pop)(type *data_ptr, tree_t tree, type const key) \
  {                                                                     \
    RBTREEI_CONTRACT (tree);                                            \
    node_t *tab[RBTREEI_MAX_STACK];                                     \
    unsigned char which[RBTREEI_MAX_STACK];                             \
    unsigned int cpt = 0;                                               \
    node_t *n = tree->node;                                             \
    which[0] = 0;                                                       \
    tab[cpt++] = (node_t*) &tree->node; /* FIXME: To clean! */          \
    /* Search for the deletion point */                                 \
    tab[cpt] = n;                                                       \
    while (n != NULL) {                                                 \
      RBTREEI_CONTRACT_NODE (n);                                        \
      assert(M_C3(rbtreei_,name,_depth)(n->child[0])                    \
             == M_C3(rbtreei_,name,_depth)(n->child[1]));               \
      int cmp = M_GET_CMP oplist (n->data, key);                        \
      if (cmp == 0) {                                                   \
        break;                                                          \
      }                                                                 \
      int i = (cmp < 0);                                                \
      which[cpt++] = i;                                                 \
      n = n->child[i];                                                  \
      assert (cpt < RBTREEI_MAX_STACK);                                 \
      tab[cpt] = n;                                                     \
    }                                                                   \
    assert (tab[cpt] == n);                                             \
    /* If not found, fail */                                            \
    if (n == NULL) {                                                    \
      return false;                                                     \
    }                                                                   \
    unsigned int cpt_n = cpt;                                           \
    node_t *v = n;     /* the replacement node */                       \
    node_t *u;         /* the deleted node */                           \
    rbtreei_color_e v_color = v->color;                                 \
    /* Classical removal of a node from a binary tree */                \
    if (v->child[0] != NULL && v->child[1] != NULL) {                   \
      /* node has 2 child. */                                           \
      /* Get the element right next to the deleted one */               \
      v = v->child[1];                                                  \
      which[cpt++] = 1;                                                 \
      tab[cpt] = v;                                                     \
      while (v != NULL) {                                               \
        /* Always left node */                                          \
        RBTREEI_CONTRACT_NODE (v);                                      \
        assert(M_C3(rbtreei_,name,_depth)(v->child[0])                  \
               == M_C3(rbtreei_,name,_depth)(v->child[1]));             \
        which[cpt++] = 0;                                               \
        v = v->child[0];                                                \
        assert (cpt < RBTREEI_MAX_STACK);                               \
        tab[cpt] = v;                                                   \
      }                                                                 \
      /* Pop the last element to get the last non-null element */       \
      v = tab[--cpt];                                                   \
      assert (v != NULL);                                               \
      u = v->child[1];                                                  \
      /* Replace 'v' by 'u' in the tree */                              \
      assert(tab[cpt-1]->child[which[cpt-1]] == v);                     \
      tab[cpt-1]->child[which[cpt-1]] = u;                              \
      /* Replace 'n' by 'v' in the tree */                              \
      assert(tab[cpt_n-1] != NULL);                                     \
      assert(tab[cpt_n-1]->child[which[cpt_n-1]] == n);                 \
      tab[cpt_n-1]->child[which[cpt_n-1]] = v;                          \
      v->child[0] = n->child[0];                                        \
      v->child[1] = n->child[1];                                        \
      v_color = v->color;                                               \
      v->color = n->color;                                              \
      tab[cpt_n] = v;                                                   \
      /* For the algorithm, 'u' is now the deleted node */              \
    } else {                                                            \
      /* 1 or no child to the node. Replace the element */              \
      v = n;                                                            \
      u = v->child[(n->child[0] == NULL)];                              \
      assert (cpt_n >= 1 && tab[cpt_n-1]->child[which[cpt_n-1]] == n);  \
      assert (n->child[(n->child[0] != NULL)] == NULL);                 \
      tab[cpt_n-1]->child[which[cpt_n-1]] = u;                          \
      /* in all cases, this node shall be set to black */               \
    }                                                                   \
                                                                        \
    /* Rebalance from child to root */                                  \
    if (v_color == RBTREE_BLACK                                         \
        && M_C3(rbtreei_,name,_get_color)(u) == RBTREE_BLACK) {         \
      /* tab[0] is NULL, tab[1] is root, u is double black */           \
      node_t *p = u, *s;                                                \
      while (cpt >= 2) {                                                \
        p  = tab[--cpt];                                                \
        bool nbChild = which[cpt];                                      \
        assert (p != NULL && u == p->child[nbChild]);                   \
        s = p->child[!nbChild];                                         \
        /* if sibling is red, perform a rotation to move sibling up */  \
        if (M_C3(rbtreei_,name,_get_color)(s) == RBTREE_RED) {          \
          p = M_C3(rbtreei_,name,_rotate) (p, tab[cpt-1], !nbChild);    \
          RBTREEI_SET_BLACK(p); /* was sibling */                       \
          tab[cpt] = p;                                                 \
          which[cpt++] = nbChild;                                       \
          p = p->child[nbChild];  /* was parent */                      \
          assert (p != NULL);                                           \
          RBTREEI_SET_RED(p);                                           \
          s = p->child[!nbChild];                                       \
          assert (M_C3(rbtreei_,name,_get_color)(s) == RBTREE_BLACK);   \
        }                                                               \
        assert (p != NULL && u == p->child[nbChild]);                   \
        /* if both childreen of s are black */                          \
        /* perform recoloring and recur on parent if black */           \
        if (s != NULL                                                   \
            && M_C3(rbtreei_,name,_get_color)(s->child[0])== RBTREE_BLACK \
            && M_C3(rbtreei_,name,_get_color)(s->child[1])== RBTREE_BLACK) { \
          assert(M_C3(rbtreei_,name,_depth)(s->child[0]) == M_C3(rbtreei_,name,_depth)(s->child[1])); \
          RBTREEI_SET_RED(s);                                           \
          if (RBTREEI_IS_RED(p)) {                                      \
            RBTREEI_SET_BLACK(p);                                       \
            RBTREEI_CONTRACT_NODE(p);                                   \
            assert(M_C3(rbtreei_,name,_depth)(p->child[0]) == M_C3(rbtreei_,name,_depth)(p->child[1])); \
            break;                                                      \
          }                                                             \
          u = p;                                                        \
        } else  {                                                       \
          assert (s != NULL);                                           \
          /* at least one child of 's' is red */                        \
          /* perform rotation(s) */                                     \
          bool childIsRight =  M_C3(rbtreei_,name,_get_color)(s->child[1])== RBTREE_RED; \
          rbtreei_color_e p_color = p->color;                           \
          if (childIsRight != nbChild) {                                \
            /* left-left or right-right case */                         \
            p = M_C3(rbtreei_,name,_rotate) (p, tab[cpt-1], childIsRight); \
          } else {                                                      \
            s = M_C3(rbtreei_,name,_rotate) (s, p, childIsRight);       \
            p = M_C3(rbtreei_,name,_rotate) (p, tab[cpt-1], !nbChild);  \
          }                                                             \
          p->color = p_color;                                           \
          assert(p->child[0] != NULL && p->child[1] != NULL);           \
          RBTREEI_SET_BLACK(p->child[0]);                               \
          RBTREEI_SET_BLACK(p->child[1]);                               \
          RBTREEI_CONTRACT_NODE(p);                                     \
          assert(M_C3(rbtreei_,name,_depth)(p->child[0]) == M_C3(rbtreei_,name,_depth)(p->child[1])); \
          break;                                                        \
        }                                                               \
      } /* while */                                                     \
      if (cpt == 1 /* root has been reached? */ ) {                     \
        M_C3(rbtreei_,name,_set_color)(p, RBTREE_BLACK);                \
        assert (tree->node == p);                                       \
      }                                                                 \
    } else {                                                            \
      M_C3(rbtreei_,name,_set_color)(u, RBTREE_BLACK);                  \
    }                                                                   \
    assert (tree->node == NULL || RBTREEI_IS_BLACK(tree->node));        \
    /* delete it */                                                     \
    if (data_ptr != NULL)                                               \
      M_GET_SET oplist (*data_ptr, n->data);                            \
    M_GET_CLEAR oplist (n->data);                                       \
    M_C3(rbtreei_,name,_del) (n);                                       \
    tree->size --;                                                      \
    RBTREEI_CONTRACT (tree);                                            \
    return true;                                                        \
  }                                                                     \
                                                                        \
  M_IF_METHOD(EQUAL, oplist)(                                           \
  static inline bool M_C3(rbtree_,name,_equal_p)(const tree_t t1, const tree_t t2) { \
    RBTREEI_CONTRACT(t1);                                               \
    RBTREEI_CONTRACT(t2);                                               \
    if (t1->size != t2->size) return false;                             \
    tree_it_t it1;                                                      \
    tree_it_t it2;                                                      \
    /* NOTE: We can't compare two tree directly as they can be          \
       structuraly different but functionnaly equal (you get this by    \
       constructing the tree in a different way). We have to            \
       compare the ordered value within the tree. */                    \
    M_C3(rbtree_, name, _it)(it1, t1);                                  \
    M_C3(rbtree_, name, _it)(it2, t2);                                  \
    while (!M_C3(rbtree_, name, _end_p)(it1)                            \
           && !M_C3(rbtree_, name, _end_p)(it2)) {                      \
      const type *ref1 = M_C3(rbtree_, name, _cref)(it1);               \
      const type *ref2 = M_C3(rbtree_, name, _cref)(it2);               \
      if (M_GET_EQUAL oplist (*ref1, *ref2) == false)                   \
        return false;                                                   \
      M_C3(rbtree_, name, _next)(it1);                                  \
      M_C3(rbtree_, name, _next)(it2);                                  \
    }                                                                   \
    return M_C3(rbtree_, name, _end_p)(it1)                             \
      && M_C3(rbtree_, name, _end_p)(it2);                              \
  }                                                                     \
  , /* NO EQUAL METHOD */ )                                             \
                                                                        \
  M_IF_METHOD(HASH, oplist)(                                            \
  static inline size_t M_C3(rbtree_,name,_hash)(const tree_t t1) {      \
    RBTREEI_CONTRACT(t1);                                               \
    M_HASH_DECL(hash);                                                  \
    /* NOTE: We can't compute the hash directly for the same reason     \
       than for EQUAL operator. */                                      \
    tree_it_t it1;                                                      \
    M_C3(rbtree_, name, _it)(it1, t1);                                  \
    while (!M_C3(rbtree_, name, _end_p)(it1)) {                         \
      const type *ref1 = M_C3(rbtree_, name, _cref)(it1);               \
      M_HASH_UP(hash, M_GET_HASH oplist (*ref1));                       \
    }                                                                   \
    return hash;                                                        \
  }                                                                     \
  , /* NO HASH METHOD */ )                                              \
                                                                        \
  M_IF_METHOD(GET_STR, oplist)(                                         \
  static inline void M_C3(rbtree_,name,_get_str)(string_t str,          \
                                       const tree_t t1, bool append) {  \
    RBTREEI_CONTRACT(t1);                                               \
    assert(str != NULL);                                                \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    /* NOTE: The print is really naive, and not really efficient */     \
    bool commaToPrint = false;                                          \
    tree_it_t it1;                                                      \
    M_C3(rbtree_, name, _it)(it1, t1);                                  \
    while (!M_C3(rbtree_, name, _end_p)(it1)) {                         \
      if (commaToPrint)                                                 \
        string_push_back (str, M_GET_SEPARATOR oplist);                 \
      commaToPrint = true;                                              \
      const type *ref1 = M_C3(rbtree_, name, _cref)(it1);               \
      M_GET_GET_STR oplist(str, *ref1, true);                           \
      M_C3(rbtree_, name, _next)(it1);                                  \
    }                                                                   \
    string_push_back (str, ']');                                        \
  }                                                                     \
  , /* NO GET_STR */ )                                                  \
                                                                        \
  M_IF_METHOD(OUT_STR, oplist)(                                         \
  static inline void                                                    \
  M_C3(rbtree_, name, _out_str)(FILE *file, tree_t rbtree)              \
  {                                                                     \
    RBTREEI_CONTRACT(rbtree);                                           \
    assert (file != NULL);                                              \
    fprintf (file, "[");                                                \
    tree_it_t it;                                                       \
    bool commaToPrint = false;                                          \
    for (M_C3(rbtree_, name, _it)(it, rbtree) ;                         \
         !M_C3(rbtree_, name, _end_p)(it);                              \
         M_C3(rbtree_, name, _next)(it)){                               \
      if (commaToPrint)                                                 \
        fputc (M_GET_SEPARATOR oplist, file);                           \
      commaToPrint = true;                                              \
      const type *item = M_C3(rbtree_, name, _cref)(it);                \
      M_GET_OUT_STR oplist (file, *item);                               \
    }                                                                   \
    fprintf (file, "]");                                                \
  }                                                                     \
  , /* no out_str */ )                                                  \
                                                                        \
  M_IF_METHOD(IN_STR, oplist)(                                          \
  static inline bool                                                    \
  M_C3(rbtree_, name, _in_str)(tree_t rbtree, FILE *file)               \
  {                                                                     \
    RBTREEI_CONTRACT(rbtree);                                           \
    assert (file != NULL);                                              \
    M_C3(rbtree_, name,_clean)(rbtree);                                 \
    char c = fgetc(file);                                               \
    if (c != '[') return false;                                         \
    c = fgetc(file);                                                    \
    if (c == ']') return true;                                          \
    ungetc(c, file);                                                    \
    type item;                                                          \
    M_GET_INIT oplist (item);                                           \
    do {                                                                \
      bool b = M_GET_IN_STR oplist (item, file);                        \
      c = fgetc(file);                                                  \
      if (!b) break;                                                    \
      M_C3(rbtree_, name, _push)(rbtree, item);                         \
    } while (c == M_GET_SEPARATOR oplist && !feof(file) && !ferror(file)); \
    M_GET_CLEAR oplist (item);                                          \
    return c == ']';                                                    \
  }                                                                     \
  , /* no in_str */ )                                                   \
                                                                        \

// TODO: specialized _sort shall do nothing, but shall check the requested order. How ?


#endif
