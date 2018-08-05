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
#ifndef MSTARLIB_BPTREE_H
#define MSTARLIB_BPTREE_H

#include "m-core.h"

/* Define a B+tree which maps a key to a value, of size N
   with its associated functions.
   USAGE: 
   BPTREE_DEF2(name, N, key_t, key_oplist, value_t, value_oplist)
   OR
   BPTREE_DEF2(name, N, key_t, value_t)
*/
#define BPTREE_DEF2(name, N, key_type, ...)                             \
  BPTREEI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                            \
                 ((name, N, key_type, M_GLOBAL_OPLIST_OR_DEF(key_type), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), 1, M_C(name, _t), M_C(name, _node_t), M_C(name, _pit_t), M_C(name, _it_t) ), \
                  (name, N, key_type, __VA_ARGS__, 1, M_C(name, _t), M_C(name, _node_t), M_C(name, _pit_t), M_C(name, _it_t))))


/* Define a B+tree of a given type, of size N.
   with its associated functions
   USAGE: BPTREE_DEF(name, N, type, [, oplist_of_the_type]) */
#define BPTREE_DEF(name, N, ...)					\
  BPTREEI_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)				\
  ((name, N, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__), 0, M_C(name, _t), M_C(name, _node_t), M_C(name, _pit_t), M_C(name, _it_t) ), \
   (name, N, __VA_ARGS__,                                    __VA_ARGS__,                                       0, M_C(name, _t), M_C(name, _node_t), M_C(name, _pit_t), M_C(name, _it_t) )))


/* Define the oplist of a rbtree of type (from BPTREE_DEF).
   USAGE: BPTREE_OPLIST(name [, oplist_of_the_type]) */
#define BPTREE_OPLIST(...)                                              \
  BPTREEI_KEY_OPLIST(M_IF_NARGS_EQ1(__VA_ARGS__)                        \
                 ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__) ),  \
                  (__VA_ARGS__ )))

/* Define the oplist of a rbtree of type (from BPTREE_DEF2).
   USAGE: BPTREE_OPLIST2(name, key_oplist, value_oplist) */
#define BPTREE_OPLIST2(name, key_oplist, value_oplist)  \
  (INIT(M_C(name, _init)),						\
   INIT_SET(M_C(name, _init_set)),					\
   SET(M_C(name, _set)),						\
   CLEAR(M_C(name, _clear)),						\
   INIT_MOVE(M_C(name, _init_move)),					\
   MOVE(M_C(name, _move)),						\
   SWAP(M_C(name, _swap)),						\
   TYPE(M_C(name,_t)),							\
   SUBTYPE(M_C(name, _type_t)),						\
   IT_TYPE(M_C(name, _it_t)),						\
   IT_FIRST(M_C(name,_it)),						\
   IT_SET(M_C(name,_it_set)),						\
   IT_END(M_C(name,_it_end)),						\
   IT_END_P(M_C(name,_end_p)),						\
   IT_EQUAL_P(M_C(name,_it_equal_p)),					\
   IT_NEXT(M_C(name,_next)),						\
   IT_REF(M_C(name,_ref)),						\
   IT_CREF(M_C(name,_cref)),						\
   CLEAN(M_C(name,_clean)),						\
   GET_MIN(M_C(name,_min)),						\
   GET_MAX(M_C(name,_max)),						\
   M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(GET_STR(M_C(name, _get_str)),), \
   M_IF_METHOD_BOTH(PARSE_STR, key_oplist, value_oplist)(PARSE_STR(M_C(name, _parse_str)),), \
   M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(OUT_STR(M_C(name, _out_str)),), \
   M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(IN_STR(M_C(name, _in_str)),), \
   M_IF_METHOD_BOTH(EQUAL, key_oplist, value_oplist)(EQUAL(M_C(name, _equal_p)),), \
   M_IF_METHOD_BOTH(HASH, key_oplist, value_oplist)(HASH(M_C(name, _hash)),) \
   ,M_IF_METHOD(NEW, key_oplist)(NEW(M_GET_NEW oplist),)                \
   ,M_IF_METHOD(REALLOC, key_oplist)(REALLOC(M_GET_REALLOC oplist),)    \
   ,M_IF_METHOD(DEL, key_oplist)(DEL(M_GET_DEL oplist),)                \
   )
  


/********************************** INTERNAL ************************************/

// deferred evaluation
#define BPTREEI_KEY_OPLIST(arg) BPTREEI_KEY_OPLIST2 arg

#define BPTREEI_KEY_OPLIST2(name, oplist)                               \
  (INIT(M_C(name, _init)),						\
   INIT_SET(M_C(name, _init_set)),					\
   SET(M_C(name, _set)),						\
   CLEAR(M_C(name, _clear)),						\
   INIT_MOVE(M_C(name, _init_move)),					\
   MOVE(M_C(name, _move)),						\
   SWAP(M_C(name, _swap)),						\
   TYPE(M_C(name,_t)),							\
   SUBTYPE(M_C(name, _type_t)),						\
   IT_TYPE(M_C(name, _it_t)),						\
   IT_FIRST(M_C(name,_it)),						\
   IT_SET(M_C(name,_it_set)),						\
   IT_END(M_C(name,_it_end)),						\
   IT_END_P(M_C(name,_end_p)),						\
   IT_EQUAL_P(M_C(name,_it_equal_p)),					\
   IT_NEXT(M_C(name,_next)),						\
   IT_REF(M_C(name,_ref)),						\
   IT_CREF(M_C(name,_cref)),						\
   CLEAN(M_C(name,_clean)),						\
   PUSH(M_C(name,_push)),						\
   POP(M_C(name,_pop))							\
   GET_MIN(M_C(name,_min)),						\
   GET_MAX(M_C(name,_max)),						\
   M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C(name, _get_str)),),		\
   M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_C(name, _parse_str)),),   \
   M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C(name, _out_str)),),		\
   M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C(name, _in_str)),),		\
   M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C(name, _equal_p)),),		\
   M_IF_METHOD(HASH, oplist)(HASH(M_C(name, _hash)),)			\
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

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

// Deferred evaluation.
#define BPTREEI_DEF_P1(arg) BPTREEI_DEF_P2 arg

#define BPTREEI_DEF_P2(name, N, key_t, key_oplist, value_t, value_oplist, isMap, tree_t, node_t, pit_t, it_t) \
                                                                        \
  M_IF(isMap)(                                                          \
              typedef struct M_C(name, _pair_s) {                       \
                key_t *key_ptr;                                         \
                value_t *value_ptr;                                     \
              } M_C(name, _type_t);                                     \
              ,                                                         \
              typedef key_t M_C(name, _type_t);                         \
                                                                        ) \
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
  /* Iterator */                                                        \
  typedef struct M_C(name, _it_s) {                                     \
    M_IF(isMap)(struct M_C(name, _pair_s) pair;,)                       \
    node_t node;                                                        \
    int    idx;                                                         \
  } it_t[1];                                                            \
                                                                        \
  /* Can be optimized later to alloc for leaf or for node */            \
  static inline node_t M_C(name, _new_node)(void)                       \
  {                                                                     \
    node_t n = M_GET_NEW key_oplist (struct M_C(name, _node_s));        \
    if (M_UNLIKELY (n == NULL)) {                                       \
      M_MEMORY_FULL(sizeof (node_t));                                   \
      assert (0);                                                       \
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
    /* We consider the empty node as a leaf */				\
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
          M_GET_CLEAR key_oplist(n->key[j]);                            \
          M_IF(isMap)(if (is_leaf) {                                    \
              M_GET_CLEAR value_oplist (n->kind.value[j]);              \
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
    /* Once the tree is clean, only the root remains */                 \
    M_GET_DEL key_oplist (b->root);                                     \
    b->root = NULL;                                                     \
  }                                                                     \
                                                                        \
  static inline node_t M_C(name, _copy_node)(const node_t o, const node_t root) \
  {									\
    node_t n = M_C(name, _new_node)();					\
    n->num = o->num;							\
    n->next = NULL;							\
    int num = M_C(name, _get_num)(o);					\
    for(int i = 0; i < num; i++) {					\
      M_GET_INIT_SET key_oplist(n->key[i], o->key[i]);			\
    }									\
    if (M_C(name, _is_leaf)(o)) {					\
      M_IF(isMap)(							\
	for(int i = 0; i < num; i++) {			                \
	  M_GET_INIT_SET value_oplist (n->kind.value[i], o->kind.value[i]); \
	}								\
	,)								\
    } else {								\
      for(int i = 0; i <= num; i++) {					\
	n->kind.node[i] = M_C(name, _copy_node)(o->kind.node[i], root);	\
      }									\
      for(int i = 0; i < num; i++) {					\
	node_t current = n->kind.node[i];                               \
        node_t next = n->kind.node[i+1];                                \
        current->next = next;                                           \
        while (!M_C(name, _is_leaf)(current)) {                         \
          assert(!M_C(name, _is_leaf)(next));                           \
          current = current->kind.node[current->num];                   \
          next    = next->kind.node[0];                                 \
          current->next = next;                                         \
        }                                                               \
      }									\
    }									\
    BPTREEI_NODE_CONTRACT(N, key_oplist, n, (o==root) ? n : root);      \
    return n;								\
  }									\
									\
  static inline void M_C(name, _init_set)(tree_t b, const tree_t o)	\
  {									\
    BPTREEI_CONTRACT(N, key_oplist, o);                                 \
    assert (b != NULL);							\
    b->root = M_C(name, _copy_node)(o->root, o->root);                  \
    b->size = o->size;							\
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
  }									\
									\
  static inline void M_C(name, _set)(tree_t b, const tree_t o)		\
  {									\
    M_C(name, _clear)(b);						\
    M_C(name, _init_set)(b, o);						\
  }									\
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
  static inline const value_t *M_C(name, _cget)(tree_t b, key_t const key) \
  {                                                                     \
    return M_CONST_CAST(value_t, M_C(name, _get)(b, key));		\
  }                                                                     \
                                                                        \
  static inline int M_C(name, _search_and_insert_leaf)(node_t n, key_t const key M_IF(isMap)( M_DEFERRED_COMMA value_t const value,) ) \
  {                                                                     \
    assert (M_C(name, _is_leaf)(n));                                    \
    int i, num = M_C(name, _get_num)(n);                                \
    assert (num <= N);                                                  \
    for(i = 0; i < num; i++) {                                          \
      int cmp = M_GET_CMP key_oplist (key, n->key[i]);                  \
      if (cmp <= 0) {                                                   \
        if (M_UNLIKELY (cmp == 0)) {                                    \
          M_IF(isMap)(M_GET_SET value_oplist (n->kind.value[i], value);,) \
            return -1;                                                  \
        }                                                               \
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
    if (i < 0) {                                                        \
      BPTREEI_CONTRACT(N, key_oplist, b);                               \
      return;                                                           \
    }                                                                   \
    b->size ++;                                                         \
    /* Most likely case: leaf can accept key!*/                         \
    int num = -leaf->num;                                               \
    assert (num > 0);                                                   \
    if (M_LIKELY (num <= N)) {                                          \
      BPTREEI_CONTRACT(N, key_oplist, b);                               \
      return;                                                           \
    }                                                                   \
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
     TODO: Bench if this is worth the effort.*/                         \
  static inline int M_C(name, _search_node)(node_t parent, node_t child) \
  {                                                                     \
    assert (!M_C(name, _is_leaf)(parent));                              \
    int num = M_C(name, _get_num)(parent);                              \
    for(int i = 0; i <= num; i++) {                                     \
      if (parent->kind.node[i] == child)                                \
        return i;                                                       \
    }                                                                   \
    assert(false);                                                      \
    return -1; /* unreachable */					\
  }                                                                     \
                                                                        \
  static inline bool M_C(name, _remove)(tree_t b, key_t const key)      \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
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
  static inline bool M_C(name, _pop_at)(value_t *ptr, tree_t b, key_t const key) \
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
                                                                        \
  static inline void                                                    \
  M_C(name, _it)(it_t it, const tree_t b)                               \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    assert (it != NULL);                                                \
    node_t n = b->root;                                                 \
    /* Scan down the nodes */                                           \
    while (!M_C(name, _is_leaf)(n)) {                                   \
      n = n->kind.node[0];                                              \
    }                                                                   \
    it->node = n;                                                       \
    it->idx  = 0;                                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it_end)(it_t it, const tree_t b)                           \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    assert (it != NULL);                                                \
    node_t n = b->root;                                                 \
    /* Scan down the nodes */                                           \
    while (!M_C(name, _is_leaf)(n)) {                                   \
      n = n->kind.node[n->num];                                         \
    }                                                                   \
    it->node = n;                                                       \
    it->idx  = -n->num;							\
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _it_set)(it_t itd, const it_t its)                          \
  {                                                                     \
    assert (itd != NULL && its != NULL);                                \
    itd->node = its->node;                                              \
    itd->idx  = its->idx;                                               \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _end_p)(it_t it)                                            \
  {                                                                     \
    assert (it != NULL && it->node != NULL);                            \
    assert (M_C(name, _is_leaf)(it->node));                             \
    return it->node->next ==NULL && it->idx >= -it->node->num;          \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _it_equal_p)(const it_t it1, const it_t it2)                \
  {                                                                     \
    return it1->node == it2->node && it1->idx == it2->idx;              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _next)(it_t it)                                             \
  {                                                                     \
    assert (it != NULL && it->node != NULL);                            \
    it->idx ++;                                                         \
    if (it->idx >= -it->node->num && it->node->next != NULL) {          \
      it->node = it->node->next;                                        \
      it->idx = 0;                                                      \
    }                                                                   \
  }                                                                     \
                                                                        \
  static inline M_C(name, _type_t) *                                    \
  M_C(name, _ref)(it_t it)                                              \
  {                                                                     \
    assert (it != NULL && it->node != NULL);                            \
    assert (it->idx <= -it->node->num);                                 \
    M_IF(isMap)(                                                        \
                it->pair.key_ptr = &it->node->key[it->idx];             \
                it->pair.value_ptr = &it->node->kind.value[it->idx];    \
                return &it->pair                                        \
                ,                                                       \
                return &it->node->key[it->idx]                          \
                                                                        ); \
  }                                                                     \
                                                                        \
  static inline const M_C(name, _type_t) *                              \
  M_C(name, _cref)(it_t it)                                             \
  {                                                                     \
    return M_CONST_CAST(M_C(name, _type_t), M_C(name, _ref)(it));       \
  }                                                                     \
									\
                                                                        \
  static inline void                                                    \
  M_C(name, _it_from)(it_t it, const tree_t b, key_t const key)		\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    assert (it != NULL);                                                \
    pit_t pit;								\
    node_t n = M_C(name, _search_leaf)(pit, b, key);			\
    it->node = n;                                                       \
    int i;								\
    BPTREEI_NODE_CONTRACT(N, key_oplist, n, b->root);                   \
    for(i = 0; i < -n->num; i++) {					\
      if (M_GET_CMP key_oplist (key, n->key[i]) <= 0)			\
	break;								\
    }                                                                   \
    if (i == -n->num && n->next != NULL) {				\
      it->node = n->next;						\
      i = 0;								\
    }									\
    it->idx  = i;                                                       \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _it_to_p)(it_t it, key_t const key)				\
  {                                                                     \
    assert (it != NULL);                                                \
    node_t n = it->node;						\
    if (it->idx >= -n->num) return true;				\
    int cmp = M_GET_CMP key_oplist (n->key[it->idx], key);		\
    return (cmp >= 0);                                                  \
  }                                                                     \
                                                                        \
  static inline value_t *						\
  M_C(name, _min)(const tree_t b)					\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    assert (b->size > 0);						\
    node_t n = b->root;                                                 \
    /* Scan down the nodes */                                           \
    while (!M_C(name, _is_leaf)(n)) {                                   \
      n = n->kind.node[0];                                              \
    }                                                                   \
    return &n->M_IF(isMap)(kind.value, key)[0];				\
  }                                                                     \
  									\
  static inline value_t *						\
  M_C(name, _max)(const tree_t b)					\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);                                 \
    node_t n = b->root;                                                 \
    /* Scan down the nodes */                                           \
    while (!M_C(name, _is_leaf)(n)) {                                   \
      n = n->kind.node[n->num];						\
    }                                                                   \
    return &n->M_IF(isMap)(kind.value, key)[-n->num-1];			\
  }                                                                     \
  									\
  static inline const value_t *						\
  M_C(name, _cmin)(const tree_t tree)					\
  {                                                                     \
    return M_CONST_CAST(value_t, M_C(name, _min)(tree));		\
  }                                                                     \
  									\
  static inline const value_t *						\
  M_C(name, _cmax)(const tree_t tree)					\
  {                                                                     \
    return M_CONST_CAST(value_t, M_C(name, _max)(tree));		\
  }                                                                     \
    									\
  static inline void                                                    \
  M_C(name, _init_move)(tree_t b, tree_t ref)				\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, ref);				\
    assert (b != NULL && b != ref);					\
    b->size = ref->size;						\
    b->root = ref->root;						\
    ref->root = NULL;                                                   \
    BPTREEI_CONTRACT(N, key_oplist, b);					\
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _move)(tree_t b, tree_t ref)				\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, b);					\
    BPTREEI_CONTRACT(N, key_oplist, ref);				\
    assert (b != ref);							\
    M_C(name,_clear)(b);						\
    M_C(name,_init_move)(b, ref);					\
    BPTREEI_CONTRACT(N, key_oplist, b);					\
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _swap)(tree_t tree1, tree_t tree2)				\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, tree1);				\
    BPTREEI_CONTRACT(N, key_oplist, tree2);				\
    M_SWAP(size_t, tree1->size, tree2->size);                           \
    M_SWAP(node_t, tree1->root, tree2->root);				\
    BPTREEI_CONTRACT(N, key_oplist, tree1);				\
    BPTREEI_CONTRACT(N, key_oplist, tree2);				\
  }                                                                     \
									\
  BPTREEI_FUNC_ADDITIONAL_DEF2(name, N, key_t, key_oplist, value_t, value_oplist, isMap, tree_t, node_t, pit_t, it_t)


/* Define additional functions. Do not used any fields but the already defined methods */
#define BPTREEI_FUNC_ADDITIONAL_DEF2(name, N, key_t, key_oplist, value_t, value_oplist, isMap, tree_t, node_t, pit_t, it_t) \
									\
  M_IF_METHOD_BOTH(EQUAL, key_oplist, value_oplist)(			\
  static inline bool M_C(name,_equal_p)(const tree_t t1, const tree_t t2) { \
    BPTREEI_CONTRACT(N, key_oplist, t1);				\
    BPTREEI_CONTRACT(N, key_oplist, t2);				\
    if (t1->size != t2->size) return false;                             \
    if (t1->size == 0) return true;					\
    /* Slow comparaison */						\
    it_t it1;								\
    it_t it2;								\
    /* NOTE: We can't compare two trees directly as they can be		\
       structuraly different but functionnaly equal (you get this by    \
       constructing the tree in a different way). We have to            \
       compare the ordered value within the tree. */                    \
    M_C(name, _it)(it1, t1);						\
    M_C(name, _it)(it2, t2);						\
    while (!M_C(name, _end_p)(it1)					\
           && !M_C(name, _end_p)(it2)) {				\
      const M_C(name, _type_t) *ref1 = M_C(name, _cref)(it1);		\
      const M_C(name, _type_t) *ref2 = M_C(name, _cref)(it2);		\
      M_IF(isMap)(							\
      if (!M_GET_EQUAL key_oplist (*ref1->key_ptr, *ref2->key_ptr))     \
        return false;                                                   \
      if (!M_GET_EQUAL value_oplist (*ref1->value_ptr, *ref2->value_ptr)) \
        return false;                                                   \
      ,									\
      if (!M_GET_EQUAL key_oplist (*ref1, *ref2))                       \
        return false;                                                   \
									) \
      M_C(name, _next)(it1);						\
      M_C(name, _next)(it2);						\
    }                                                                   \
    return M_C(name, _end_p)(it1)					\
      && M_C(name, _end_p)(it2);					\
  }                                                                     \
  , /* NO EQUAL METHOD */ )                                             \
									\
  M_IF_METHOD_BOTH(HASH, key_oplist, value_oplist)(                     \
  static inline size_t M_C(name,_hash)(const tree_t t1) {               \
    BPTREEI_CONTRACT(N, key_oplist, t1);				\
    M_HASH_DECL(hash);                                                  \
    /* NOTE: We can't compute the hash directly for the same reason     \
       than for EQUAL operator. */                                      \
    it_t it1;								\
    M_C(name, _it)(it1, t1);						\
    while (!M_C(name, _end_p)(it1)) {					\
      const M_C(name, _type_t) *ref1 = M_C(name, _cref)(it1);		\
      M_IF(isMap)(							\
		  M_HASH_UP(hash, M_GET_HASH key_oplist (*ref1->key_ptr)); \
		  M_HASH_UP(hash, M_GET_HASH value_oplist (*ref1->value_ptr)); \
		  ,							\
		  M_HASH_UP(hash, M_GET_HASH key_oplist (*ref1));	\
									) \
      M_C(name, _next)(it1);						\
    }                                                                   \
    return M_HASH_FINAL (hash);						\
  }                                                                     \
  , /* NO HASH METHOD */ )                                              \
									\
  M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(                  \
  static inline void M_C(name, _get_str)(string_t str,                  \
					 const tree_t t1, bool append) { \
    BPTREEI_CONTRACT(N, key_oplist, t1);				\
    assert(str != NULL);                                                \
    (append ? string_cat_str : string_set_str) (str, "[");              \
    /* NOTE: The print is really naive, and not really efficient */     \
    bool commaToPrint = false;                                          \
    it_t it;								\
    for (M_C(name, _it)(it, t1) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)) {					\
      if (commaToPrint)                                                 \
        string_push_back (str, M_GET_SEPARATOR key_oplist);		\
      commaToPrint = true;                                              \
      const M_C(name, _type_t) *ref1 = M_C(name, _cref)(it);		\
      M_IF(isMap)(							\
		  M_GET_GET_STR key_oplist(str, *ref1->key_ptr, true);	\
		  string_cat_str(str, ":");				\
		  M_GET_GET_STR value_oplist(str, *ref1->value_ptr, true) \
		  ,							\
		  M_GET_GET_STR key_oplist(str, *ref1, true);		\
									); \
    }									\
    string_push_back (str, ']');                                        \
  }                                                                     \
  , /* NO GET_STR */ )                                                  \
									\
  M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(                  \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *file, tree_t t1)				\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, t1);				\
    assert (file != NULL);                                              \
    fputc ('[', file);							\
    bool commaToPrint = false;                                          \
    it_t it;								\
    for (M_C(name, _it)(it, t1) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      if (commaToPrint)                                                 \
        fputc (M_GET_SEPARATOR key_oplist, file);			\
      commaToPrint = true;                                              \
      const M_C(name, _type_t) *ref1 = M_C(name, _cref)(it);		\
      M_IF(isMap)(							\
		  M_GET_OUT_STR key_oplist(file, *ref1->key_ptr);	\
		  fputc (':', file);					\
		  M_GET_OUT_STR value_oplist(file, *ref1->value_ptr)	\
		  ,							\
		  M_GET_OUT_STR key_oplist(file, *ref1);		\
									); \
    }                                                                   \
    fputc (']', file);							\
  }                                                                     \
  , /* no out_str */ )                                                  \
                                                                        \
  M_IF_METHOD_BOTH(PARSE_STR, key_oplist, value_oplist)(                \
  static inline bool                                                    \
  M_C(name, _parse_str)(tree_t t1, const char str[], const char **endp) \
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, t1);				\
    assert (str != NULL);                                               \
    M_C(name,_clean)(t1);						\
    bool success = false;                                               \
    int c = *str++;                                                     \
    if (M_UNLIKELY (c != '[')) goto exit;                               \
    c = *str++;                                                         \
    if (M_UNLIKELY (c == ']')) { success = true; goto exit;}            \
    if (M_UNLIKELY (c == 0)) goto exit;                                 \
    str--;                                                              \
    key_t key;								\
    M_GET_INIT key_oplist (key);					\
    M_IF(isMap)(value_t value;						\
		M_GET_INIT value_oplist (value);			\
		,)							\
    do {                                                                \
      bool b = M_GET_PARSE_STR key_oplist (key, str, &str);             \
      do { c = *str++; } while (isspace(c));                            \
      if (b == false) goto exit;                                        \
      M_IF(isMap)(if (c != ':') goto exit;                              \
                  b = M_GET_PARSE_STR value_oplist(value, str, &str);   \
		  do { c = *str++; } while (isspace(c));                \
		  if (b == false || c == 0) goto exit;			\
		  M_C(name, _set_at)(t1, key, value)			\
		  ,							\
		  M_C(name, _push)(t1, key)				\
		  );							\
    } while (c == M_GET_SEPARATOR key_oplist);				\
    M_GET_CLEAR key_oplist (key);					\
    M_IF(isMap)(M_GET_CLEAR value_oplist (value);			\
		,)							\
    success = (c == ']');                                               \
  exit:                                                                 \
    if (endp) *endp = str;                                              \
    return success;                                                     \
  }                                                                     \
  , /* no parse_str */ )                                                \
                                                                        \
  M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(                   \
  static inline bool                                                    \
  M_C(name, _in_str)(tree_t t1, FILE *file)				\
  {                                                                     \
    BPTREEI_CONTRACT(N, key_oplist, t1);				\
    assert (file != NULL);                                              \
    M_C(name,_clean)(t1);						\
    int c = fgetc(file);						\
    if (M_UNLIKELY (c != '[')) return false;                            \
    c = fgetc(file);                                                    \
    if (M_UNLIKELY (c == ']')) return true;                             \
    if (M_UNLIKELY (c == EOF)) return false;                            \
    ungetc(c, file);                                                    \
    key_t key;								\
    M_GET_INIT key_oplist (key);					\
    M_IF(isMap)(value_t value;						\
		M_GET_INIT value_oplist (value);			\
		,)							\
    do {                                                                \
      bool b = M_GET_IN_STR key_oplist (key, file);			\
      do { c = fgetc(file); } while (isspace(c));                       \
      if (b == false) break;                                            \
      M_IF(isMap)(if (c!=':') break;                                    \
                  b = M_GET_IN_STR value_oplist(value, file);           \
		  do { c = fgetc(file); } while (isspace(c));           \
		  if (b == false || c == EOF) break;			\
		  M_C(name, _set_at)(t1, key, value)			\
		  ,							\
		  M_C(name, _push)(t1, key)				\
		  );							\
    } while (c == M_GET_SEPARATOR key_oplist);				\
    M_GET_CLEAR key_oplist (key);					\
    M_IF(isMap)(M_GET_CLEAR value_oplist (value);			\
		,)							\
    return c == ']';                                                    \
  }                                                                     \
  , /* no in_str */ )                                                   \


#endif
