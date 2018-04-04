/*
 * M*LIB - B+TREE module
 *
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
#ifndef __M_BPTREE_H
#define __M_BPTREE_H

#include "m-core.h"

/* Define a B+tree which maps a key to a value, of size N.
   USAGE: BPTREE_DEF2(name, N, key_t, key_oplist, value_t, value_oplist) */
#define BPTREE_DEF2(name, N, key_t, key_oplist, value_t, value_oplist)  \
  BPTREEI_DEF2(name, N, key_t, key_oplist, value_t, value_oplist, 1,    \
               M_C(name, _t), M_C(name, _node_t), M_C(name, _pit_t))

/* Define a B+tree of a given type, of size N.
   USAGE: BPTREE_DEF(name, N, type, [, oplist_of_the_type]) */
#define BPTREE_DEF(name, N, type, oplist)                               \
  BPTREEI_DEF2(name, N, type, oplist, type, oplist, 0,                  \
               M_C(name, _t), M_C(name, _node_t), M_C(name, _pit_t))

//TODO: oplist

/********************************** INTERNAL ************************************/

#ifdef NDEBUG
# define BPTREEI_NODE_CONTRACT(N, key_oplist, node, root) do { } while (0)
#else
# define BPTREEI_NODE_CONTRACT(N, key_oplist, node, root) do {          \
    assert ((node) != NULL);                                            \
    assert ((root) != NULL);                                            \
    int  num2     = (node)->num;                                        \
    bool is_leaf2 = num2 <= 0;                                          \
    num2 = num2 < 0 ? -num2 : num2;                                     \
    if ((node) == (root)) {                                             \
      assert( 0 <= num2 && num2 <= N);                                  \
      if (num2 == 0) assert (is_leaf2);                                 \
    } else {                                                            \
      int c2 = N / 2;                                                   \
      assert (c2 > 0);                                                  \
      assert (c2 <= num2 && num2 <= N);                                 \
    }                                                                   \
    for(int i2 = 1; i2 < num2 ; i2++) {                                 \
      assert (M_GET_CMP key_oplist((node)->key[i2-1], (node)->key[i2]) < 0); \
    }                                                                   \
    if ((node)->next != NULL)                                           \
      assert (M_GET_CMP key_oplist((node)->key[num2-1], (node)->next->key[0]) < 0); \
  } while (0)
#endif

#define BPTREEI_CONTRACT(N, key_oplist, b) do {                         \
    assert (N >= 3);  /* TBC: 2 instead ? */                            \
    BPTREEI_NODE_CONTRACT(N, key_oplist, (b)->root, (b)->root);         \
    assert ((b)->root->next == NULL);                                   \
    if ((b)->root->num <= 0) assert (-(b)->root->num == (int) (b)->size); \
  } while (0)

/* Max depth of any B+tree */
#define BPTREEI_MAX_STACK ((int)(CHAR_BIT*sizeof (size_t)))

#define BPTREEI_DEF2(name, N, key_t, key_oplist, value_t, value_oplist, isMap, tree_t, node_t, pit_t) \
                                                                        \
  typedef key_t M_C(name, _type_t);                                     \
                                                                        \
  /* Node of a B+TREE */                                                \
  typedef struct M_C(name, _node_s) {                                   \
    int    num;           /* Abs=Number of keys. Sign <0 is leaf */     \
    key_t  key[N+1];      /* We can temporary push  one more key */     \
    struct M_C(name, _node_s) *next;  /* next node */                   \
    union  M_C(name, _kind_s) {       /* either value or pointer to other nodes */ \
      M_IF(isMap)(value_t        value[N+1];,)                          \
      struct M_C(name, _node_s) *node[N+2];                             \
    } kind;                                                             \
  } *node_t;                                                            \
                                                                        \
  /* A B+TREE is just a pointer to the root node */                     \
  typedef struct M_C(name, _s) {                                        \
    node_t root;                                                        \
    size_t size;                                                        \
  } tree_t[1];                                                          \
                                                                        \
  /* Parent Iterator */                                                 \
  typedef struct M_C(name, _parent_it_s) {                              \
    int num;                                                            \
    node_t parent[BPTREEI_MAX_STACK];                                   \
  } pit_t[1];                                                           \
                                                                        \
  /* Can be optimized later to alloc for leaf or for node */            \
  static inline node_t M_C(name, _new_node)(void)                       \
  {                                                                     \
    node_t n = M_GET_NEW key_oplist (struct M_C(name, _node_s));        \
    if (M_UNLIKELY (n == NULL)) {                                       \
      M_MEMORY_FULL(sizeof (node_t));                                   \
      return NULL;                                                      \
    }                                                                   \
    n->next = NULL;                                                     \
    n->num = 0;                                                         \
    return n;                                                           \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _init)(tree_t b)                         \
  {                                                                     \
    b->root = M_C(name, _new_node)();                                   \
    b->size = 0;                                                        \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
  }                                                                     \
                                                                        \
  static inline bool M_C(name, _is_leaf)(node_t n)                      \
  {                                                                     \
    return n->num <= 0;                                                 \
  }                                                                     \
                                                                        \
  static inline int M_C(name, _get_num)(node_t n)                       \
  {                                                                     \
    int num = n->num;                                                   \
    num = num < 0 ? -num : num;                                         \
    assert (num <= N);                                                  \
    return num;                                                         \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _clean)(tree_t b)                        \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    node_t next, n = b->root;                                           \
    pit_t pit;                                                          \
    int np = 0;                                                         \
    /* Scan down the nodes */                                           \
    while (!M_C(name, _is_leaf)(n)) {                                   \
      pit->parent[np++] = n;                                            \
      assert (np <= BPTREEI_MAX_STACK);                                 \
      n = n->kind.node[0];                                              \
    }                                                                   \
    pit->parent[np++] = n;                                              \
    assert (np <= BPTREEI_MAX_STACK);                                   \
    /* Clean & free non root */                                         \
    for(int i = 0; i < np; i++) {                                       \
      n = pit->parent[i];                                               \
      while (n != NULL) {                                               \
        /* Clear key (& value for leaf) */                              \
        int num = M_C(name, _get_num)(n);                               \
        M_IF(isMap)(bool is_leaf = M_C(name, _is_leaf)(n);,)            \
        for(int j = 0; j < num; j++) {                                  \
          M_GET_CLEAR key_oplist(n->key[i]);                            \
          M_IF(isMap)(if (is_leaf) {                                    \
              M_GET_CLEAR value_oplist (n->kind.value[i]);              \
            },)                                                         \
        }                                                               \
        /* Next node of the same height */                              \
        next = n->next;                                                 \
        if (i != 0) {                                                   \
          M_GET_DEL key_oplist (n);                                     \
        }                                                               \
        n = next;                                                       \
      }                                                                 \
    }                                                                   \
    /* Clean root */                                                    \
    b->root->num = 0;                                                   \
    b->size = 0;                                                        \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _clear)(tree_t b)                        \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    M_C(name, _clean)(b);                                               \
    M_GET_DEL key_oplist (b->root);                                     \
    b->root = NULL;                                                     \
  }                                                                     \
                                                                        \
  static inline bool M_C(name, _empty_p)(const tree_t b)                \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    /* root shall be an empty leaf */                                   \
    return b->size == 0;                                                \
  }                                                                     \
                                                                        \
  static inline size_t M_C(name, _size)(const tree_t b)                 \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    return b->size;                                                     \
  }                                                                     \
                                                                        \
  static inline node_t M_C(name, _search_leaf)(pit_t pit, const tree_t b, key_t const key) \
  {                                                                     \
    node_t n = b->root;                                                 \
    int np = 0;                                                         \
    BPTREEI_NODE_CONTRACT(N, key_oplist, n, b->root);                   \
    while (!M_C(name, _is_leaf)(n)) {                                   \
      assert (np <= BPTREEI_MAX_STACK);                                 \
      int i, hi = n->num;                                               \
      assert (hi > 0);                                                  \
      /* Linear search is usually faster than binary search for         \
         B+TREE (due to cache effect). If a binary tree is faster for   \
         the choosen type and size , it simply means that the           \
         size of B+TREE is too big (TBC) and shall be reduced. */       \
      for(i = 0; i < hi; i++) {                                         \
        if (M_GET_CMP key_oplist (key, n->key[i]) < 0)                  \
          break;                                                        \
      }                                                                 \
      pit->parent[np++] = n;                                            \
      n = n->kind.node[i];                                              \
      assert (n != NULL);                                               \
      BPTREEI_NODE_CONTRACT(N, key_oplist, n, b->root);                 \
    }                                                                   \
    pit->num = np;                                                      \
    return n;                                                           \
  }                                                                     \
                                                                        \
  static inline value_t *M_C(name, _get)(tree_t b, key_t const key)     \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    pit_t pit;                                                          \
    node_t n = M_C(name, _search_leaf)(pit, b, key);                    \
    int cmp = 0;                                                        \
    BPTREEI_NODE_CONTRACT(N, key_oplist, n, b->root);                   \
    for(int i = 0; cmp >= 0 && i < -n->num; i++) {                      \
      cmp = M_GET_CMP key_oplist (key, n->key[i]);                      \
      if (cmp == 0)                                                     \
        return M_IF(isMap)(&n->kind.value[i],&n->key[i]);               \
    }                                                                   \
    return NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline int M_C(name, _search_and_insert_leaf)(node_t n, key_t const key M_IF(isMap)( M_DEFERRED_COMMA value_t const value,) ) \
  {                                                                     \
    assert (M_C(name, _is_leaf)(n));                                    \
    int i, num = M_C(name, _get_num)(n);                                \
    assert (num <= N);                                                  \
    for(i = 0; i < num; i++) {                                          \
      int cmp = M_GET_CMP key_oplist (key, n->key[i]);                  \
      if (M_UNLIKELY (cmp == 0)) {                                      \
        M_IF(isMap)(M_GET_SET value_oplist (n->kind.value[i], value);,) \
          return -1;                                                    \
      } else if (cmp < 0) {                                             \
        /* Move tables to make space for insertion */                   \
        memmove(&n->key[i+1], &n->key[i], sizeof(key_t)*(num-i));       \
        M_IF(isMap)(memmove(&n->kind.value[i+1], &n->kind.value[i], sizeof(value_t)*(num-i));,) \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    M_GET_INIT_SET key_oplist (n->key[i], key);                         \
    M_IF(isMap)(M_GET_INIT_SET value_oplist (n->kind.value[i], value);,) \
    n->num  += -1; /* Increase num as num<0 for leaf */                 \
    return i;                                                           \
  }                                                                     \
                                                                        \
  static inline int M_C(name, _search_and_insert_node)(node_t n, key_t key) \
  {                                                                     \
    assert (!M_C(name, _is_leaf)(n));                                   \
    int i, num = M_C(name, _get_num)(n);                                \
    assert (num <= N);                                                  \
    for(i = 0; i < num; i++) {                                          \
      int cmp = M_GET_CMP key_oplist (key, n->key[i]);                  \
      assert (cmp != 0);                                                \
      if (cmp < 0) {                                                    \
        /* Move tables to make space for insertion */                   \
        memmove(&n->key[i+1], &n->key[i], sizeof(key_t)*(num-i));       \
        memmove(&n->kind.node[i+1], &n->kind.node[i], sizeof(node_t)*(num-i+1)); \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    M_GET_INIT_SET key_oplist (n->key[i], key);                         \
    n->num  += 1;                                                       \
    return i;                                                           \
  }                                                                     \
                                                                        \
  static inline void M_IF(isMap)(M_C(name, _set_at),M_C(name,_push))(tree_t b, key_t const key M_IF(isMap)(M_DEFERRED_COMMA value_t const value,)) \
  {                                                                     \
    pit_t pit;                                                          \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    node_t leaf = M_C(name, _search_leaf)(pit, b, key);                 \
    /* Insert key into the leaf.*/                                      \
    /* NOTE: Even if there is N elements, we can still add one more.*/  \
    int i = M_C(name, _search_and_insert_leaf)(leaf, key M_IF (isMap)(M_DEFERRED_COMMA value,)); \
    b->size += (i >= 0);                                                \
    /* Most likely case: leaf can accept key!*/                         \
    int num = -leaf->num;                                               \
    assert (num > 0);                                                   \
    if (M_LIKELY (num <= N)) {                                          \
      BPTREEI_CONTRACT(N, key_oplist, b);                               \
      return;                                                           \
    }                                                                   \
    assert (i >= 0);                                                    \
    assert (num == N+1);                                                \
    /* leaf is full: need to slip it in two */                          \
    int nnum = (N + 1) / 2;                                             \
    num = N + 1 - nnum;                                                 \
    node_t nleaf = M_C(name, _new_node)();                              \
    /* Move half objects to the new node */                             \
    memmove(&nleaf->key[0], &leaf->key[num], sizeof(key_t)*nnum);       \
    M_IF(isMap)(memmove(&nleaf->kind.value[0], &leaf->kind.value[num], sizeof(value_t)*nnum);,) \
    leaf->num = -num;                                                   \
    nleaf->num = -nnum;                                                 \
    nleaf->next = leaf->next;                                           \
    leaf->next = nleaf;                                                 \
    BPTREEI_NODE_CONTRACT(N, key_oplist, leaf, b->root);                \
    BPTREEI_NODE_CONTRACT(N, key_oplist, nleaf, b->root);               \
    /* Update parent to inject *key_ptr which split between (leaf, nleaf) */ \
    key_t *key_ptr = &nleaf->key[0];                                    \
    while (true) {                                                      \
      if (pit->num == 0) {                                              \
        /* We reach root. Need to increase the height of the tree.*/    \
        node_t parent = M_C(name, _new_node)();                         \
        parent->num = 1;                                                \
        M_GET_INIT_SET key_oplist (parent->key[0], *key_ptr);           \
        parent->kind.node[0] = leaf;                                    \
        parent->kind.node[1] = nleaf;                                   \
        b->root = parent;                                               \
        BPTREEI_CONTRACT(N, key_oplist, b);                             \
        return;                                                         \
      }                                                                 \
      /* Non root node. Get parent */                                   \
      node_t parent = pit->parent[--pit->num];                          \
      /* Insert into parent (It is big enough to receive temporary one more) */ \
      i = M_C(name, _search_and_insert_node)(parent, *key_ptr);         \
      parent->kind.node[i] = leaf;                                      \
      parent->kind.node[i+1] = nleaf;                                   \
      if (M_LIKELY (parent->num <= N)) {                                \
        BPTREEI_CONTRACT(N, key_oplist, b);                             \
        return; /* No need to split parent.*/                           \
      }                                                                 \
      assert (parent->num == N+1);                                      \
      /* Need to split parent in {np} {1} {nnp} */                      \
      int nnp = N / 2;                                                  \
      int np = N - nnp;                                                 \
      assert (nnp > 0 && np > 0 && nnp+np+1 == N+1);                    \
      node_t nparent = M_C(name, _new_node)();                          \
      /* Move half items to new node (Like a classic B-TREEE)           \
         and the median key to the grand-parent*/                       \
      memmove(&nparent->key[0], &parent->key[np+1], sizeof(key_t)*nnp); \
      memmove(&nparent->kind.node[0], &parent->kind.node[np+1], sizeof(node_t)*(nnp+1)); \
      parent->num = np;                                                 \
      nparent->num = nnp;                                               \
      nparent->next = parent->next;                                     \
      parent->next = nparent;                                           \
      BPTREEI_NODE_CONTRACT(N, key_oplist, parent, b->root);            \
      BPTREEI_NODE_CONTRACT(N, key_oplist, nparent, b->root);           \
      /* Prepare for the next step */                                   \
      key_ptr = &parent->key[np];                                       \
      leaf = parent;                                                    \
      nleaf = nparent;                                                  \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline int M_C(name, _search_and_remove_leaf)(node_t n, key_t const key) \
  {                                                                     \
    assert(M_C(name, _is_leaf)(n));                                     \
    int num = M_C(name, _get_num)(n);                                   \
    for(int i = 0; i < num; i++) {                                      \
      int cmp = M_GET_CMP key_oplist(key, n->key[i]);                   \
      if (cmp == 0) {                                                   \
        M_GET_CLEAR key_oplist (n->key[i]);                             \
        M_IF(isMap)(M_GET_CLEAR value_oplist (n->kind.value[i]);,)      \
        memmove(&n->key[i], &n->key[i+1], sizeof(key_t)*(num-1-i));     \
        M_IF(isMap)(memmove(&n->kind.value[i], &n->kind.value[i+1], sizeof(value_t)*(num-1-i));,) \
        n->num -= -1; /* decrease number as num is < 0 */               \
        return i;                                                       \
      }                                                                 \
    }                                                                   \
    return -1; /* Not found */                                          \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _left_shift)(node_t parent, int k)       \
  {                                                                     \
    assert (parent != NULL && !M_C(name, _is_leaf)(parent));            \
    assert (0 <= k && k < M_C(name, _get_num)(parent));                 \
    node_t left = parent->kind.node[k];                                 \
    node_t right = parent->kind.node[k+1];                              \
    assert (left != NULL && right != NULL);                             \
    int num_left = M_C(name, _get_num)(left);                           \
    int num_right = M_C(name, _get_num)(right);                         \
    assert (num_left > N/2);                                            \
    assert (num_right < N/2);                                           \
    /* Move one item from the left node to the right node */            \
    memmove(&right->key[1], &right->key[0], sizeof(key_t)*num_right);   \
    if (M_C(name, _is_leaf)(left)) {                                    \
      M_IF(isMap)(memmove (&right->kind.value[1], &right->kind.value[0], sizeof(value_t)*num_right);,) \
      memmove (&right->key[0], &left->key[num_left-1], sizeof (key_t)); \
      M_IF(isMap)(memmove (&right->kind.value[0], &left->kind.value[num_left-1], sizeof (value_t));,) \
      right->num = -num_right - 1;                                      \
      left->num = -num_left + 1;                                        \
      M_GET_SET key_oplist (parent->key[k], right->key[0]);             \
    } else {                                                            \
      memmove(&right->kind.node[1], &right->kind.node[0], sizeof(node_t)*(num_right+1)); \
      /* parent[k] is move to right[0] (clear). parent[k] is therefore clear */ \
      memmove(&right->key[0], &parent->key[k], sizeof(key_t));          \
      right->kind.node[0] = left->kind.node[num_left];                  \
      right->num = num_right + 1;                                       \
      left->num = num_left - 1;                                         \
      /* left[n-1] is move to parent[k] (clear). left[n-1] is therefore clear */ \
      memmove(&parent->key[k], &left->key[num_left-1], sizeof (key_t)); \
    }                                                                   \
    assert (right->num != 0);                                           \
    assert (left->num != 0);                                            \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _right_shift)(node_t parent, int k)      \
  {                                                                     \
    assert (parent != NULL && !M_C(name, _is_leaf)(parent));            \
    assert (0 <= k && k < M_C(name, _get_num)(parent));                 \
    node_t left = parent->kind.node[k];                                 \
    node_t right = parent->kind.node[k+1];                              \
    assert (left != NULL && right != NULL);                             \
    int num_left = M_C(name, _get_num)(left);                           \
    int num_right = M_C(name, _get_num)(right);                         \
    assert (num_left < N/2);                                            \
    assert (num_right > N/2);                                           \
    /* Move one item from right to left. */                             \
    if (M_C(name, _is_leaf)(right)) {                                   \
      memmove (&left->key[num_left], &right->key[0], sizeof(key_t));    \
      memmove (&right->key[0], &right->key[1], sizeof(key_t)*(num_right-1)); \
      M_IF(isMap)(memmove (&left->kind.value[num_left], &right->kind.value[0], sizeof (value_t));,) \
      M_IF(isMap)(memmove (&right->kind.value[0], &right->kind.value[1], sizeof(value_t)*(num_right-1));,) \
      right->num = -num_right + 1;                                      \
      left->num = -num_left - 1;                                        \
      M_GET_SET key_oplist (parent->key[k], right->key[0]);             \
    } else {                                                            \
      memmove (&left->key[num_left], &parent->key[k], sizeof (key_t));  \
      memmove (&parent->key[k], &right->key[0], sizeof (key_t));        \
      memmove (&right->key[0], &right->key[1], sizeof(key_t)*(num_right-1)); \
      left->kind.node[num_left+1] = right->kind.node[0];                \
      memmove (&right->kind.node[0], &right->kind.node[1], sizeof(node_t)*num_right); \
      right->num = num_right - 1;                                       \
      left->num = num_left + 1;                                         \
    }                                                                   \
    assert (right->num != 0);                                           \
    assert (left->num != 0);                                            \
  }                                                                     \
                                                                        \
  static inline void M_C(name, _merge_node)(node_t parent, int k, bool leaf) \
  {                                                                     \
    assert (parent != NULL && !M_C(name, _is_leaf)(parent));            \
    assert (0 <= k && k < M_C(name, _get_num(parent)));                 \
    node_t left = parent->kind.node[k];                                 \
    node_t right = parent->kind.node[k+1];                              \
    assert (left != NULL && right != NULL);                             \
    int num_parent = M_C(name, _get_num)(parent);                       \
    int num_left   = M_C(name, _get_num)(left);                         \
    int num_right  = M_C(name, _get_num)(right);                        \
                                                                        \
    if (leaf) {                                                         \
      assert (num_left + num_right <= N);                               \
      memmove(&left->key[num_left], &right->key[0], sizeof(key_t)*num_right); \
      M_IF(isMap)(memmove(&left->kind.value[num_left], &right->kind.value[0], sizeof(value_t)*num_right);,) \
      left->num = -num_left - num_right;                                \
    } else {                                                            \
      assert (num_left + num_right + 1 <= N);                           \
      memmove(&left->key[num_left+1], &right->key[0], sizeof(key_t)*num_right); \
      memmove(&left->kind.node[num_left+1], &right->kind.node[0], sizeof(node_t)*(num_right+1)); \
      M_GET_INIT_SET key_oplist (left->key[num_left], parent->key[k]);  \
      left->num = num_left + 1 + num_right;                             \
    }                                                                   \
    left->next = right->next;                                           \
    M_GET_DEL key_oplist (right);                                       \
    /* remove k'th key from the parent */                               \
    M_GET_CLEAR key_oplist (parent->key[k]);                            \
    memmove(&parent->key[k], &parent->key[k+1], sizeof(key_t)*(num_parent - k - 1)); \
    memmove(&parent->kind.node[k+1], &parent->kind.node[k+2], sizeof(node_t)*(num_parent - k -1)); \
    parent->num --;                                                     \
  }                                                                     \
                                                                        \
  /* We can also cache the index when we descend the tree.              \
     TODO: Bench if it worth it.*/                                      \
  static inline int M_C(name, _search_node)(node_t parent, node_t child) \
  {                                                                     \
    assert (!M_C(name, _is_leaf)(parent));                              \
    int num = M_C(name, _get_num)(parent);                              \
    for(int i = 0; i <= num; i++) {                                     \
      if (parent->kind.node[i] == child)                                \
        return i;                                                       \
    }                                                                   \
    assert(false);                                                      \
  }                                                                     \
                                                                        \
  static inline bool M_C(name, _remove)(tree_t b, key_t const key)      \
  {                                                                     \
    pit_t pit;                                                          \
    node_t leaf = M_C(name, _search_leaf)(pit, b, key);                 \
    int k = M_C(name, _search_and_remove_leaf)(leaf, key);              \
    /* If not found, or number of keys greater than N>2 or root */      \
    if (k < 0) return false;                                            \
    b->size --;                                                         \
    if (M_LIKELY (M_C(name, _get_num)(leaf) >= N/2) || pit->num == 0) return true; \
    /* Leaf is too small. Needs rebalancing */                          \
    assert (M_C(name, _get_num)(leaf) == N/2-1);                        \
    bool pass1 = true;                                                  \
    while (true) {                                                      \
      assert (pit->num > 0);                                            \
      /* Search for node 'leaf' in parent */                            \
      node_t parent = pit->parent[--pit->num];                          \
      assert (parent != NULL);                                          \
      k = M_C(name, _search_node)(parent, leaf);                        \
      /* Look for the neighboor of the removed key. */                  \
      /* if we can still one key to keep our node balanced */           \
      if (k > 0 && M_C(name, _get_num)(parent->kind.node[k-1]) > N/2) { \
        M_C(name, _left_shift)(parent, k-1);                            \
        return true;                                                    \
      } else if (k < M_C(name, _get_num)(parent)                        \
                 && M_C(name, _get_num)(parent->kind.node[k+1]) > N/2) { \
        M_C(name, _right_shift)(parent, k);                             \
        return true;                                                    \
      }                                                                 \
      /* Merge both nodes, removing 'k' from parent */                  \
      if (k == M_C(name, _get_num)(parent))                             \
        k--;                                                            \
      assert(k >= 0 && k < M_C(name, _get_num)(parent));                \
      /* Merge 'k' & 'k+1' & remove 'k' from parent */                  \
      M_C(name, _merge_node)(parent, k, pass1);                         \
      /* Check if we need to backport */                                \
      if (M_C(name, _get_num)(parent) >= N/2) return true;              \
      if (pit->num == 0) {                                              \
        /* We reach the root */                                         \
        if (M_C(name, _get_num)(parent) == 0) {                         \
          /* Update root (deleted) */                                   \
          b->root = parent->kind.node[0];                               \
          M_GET_DEL key_oplist (parent);                                \
        }                                                               \
        return true;                                                    \
      }                                                                 \
      /* Next iteration */                                              \
      leaf = parent;                                                    \
      pass1 = false;                                                    \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline bool M_C(name, _pop)(value_t *ptr, tree_t b, key_t const key) \
  {                                                                     \
    if (ptr != NULL) {                                                  \
      value_t *ref = M_C(name, _get)(b, key);                           \
      if (ref == NULL) {                                                \
        return false;                                                   \
      }                                                                 \
      M_GET_SET value_oplist (*ptr, *ref);                              \
    }                                                                   \
    return M_C(name, _remove)(b, key);                                  \
  }                                                                     \

#endif
