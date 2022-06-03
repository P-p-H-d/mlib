/*
 * M*LIB - TREE module
 *
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
#ifndef MSTARLIB_TREE_H
#define MSTARLIB_TREE_H

#include "m-core.h"

/* Define a complete tree of 'type'
    https://en.wikipedia.org/wiki/Tree_(data_structure)
   USAGE: 
   TREE_DEF(name, type_t[, type_oplist])
*/
#define M_TREE_DEF(name, ...)                                                 \
    M_TREE_DEF_AS(name, M_C(name, _t), M_C(name, _it_t), __VA_ARGS__)

/* Define a complete tree of 'type'
   as the given name name_t with its associated functions.
   USAGE: 
   TREE_DEF_AS(name, name_t, it_t, type_t[, type_oplist])
*/
#define M_TREE_DEF_AS(name, name_t, it_t, ...)                                \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_TR33_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                   \
             ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
              (name, __VA_ARGS__,                                        name_t, it_t ))) \
  M_END_PROTECTED_CODE

/* Maximum number of child per node.
   Used for assertion. */
#ifndef M_USE_TREE_MAX_CHILD_PER_PARENT
#define M_USE_TREE_MAX_CHILD_PER_PARENT 10000000
#endif


/********************************************************************************/
/********************************** INTERNAL ************************************/
/********************************************************************************/

# define M_TR33_NODE_CONTRACT(node, tree) do {                                \
    M_ASSERT( (tree)->size > 0);                                              \
    M_ASSERT( (node)->parent != M_TR33_NO_NODE);                              \
    M_ASSERT( (node)->parent == M_TR33_ROOT_NODE || (node)->parent >= 0);     \
    M_ASSERT( (node)->parent != M_TR33_ROOT_NODE || (node) == &(tree)->tab[(tree)->root_index] ); \
} while (0)

# define M_TR33_CONTRACT(tree) do {                                           \
    M_ASSERT( (tree)->size >= 0 && (tree)->size <= (tree)->capacity);         \
    M_ASSERT( (tree)->capacity >= 0 );                                        \
    M_ASSERT( (tree)->capacity == 0 || (tree)->tab != NULL);                  \
    M_ASSERT( (tree)->allow_realloc == 1 || (tree)->allow_realloc == 32 );    \
    M_ASSERT( (tree)->free_index >= M_TR33_NO_NODE && (tree)->free_index < (tree)->capacity); \
    M_ASSERT( (tree)->root_index >= M_TR33_NO_NODE && (tree)->root_index < (tree)->capacity); \
    M_ASSERT( (tree)->free_index < 0 || (tree)->tab[(tree)->free_index].parent == M_TR33_NO_NODE); \
    M_ASSERT( (tree)->root_index < 0 || (tree)->tab[(tree)->root_index].parent == M_TR33_ROOT_NODE); \
    M_ASSERT( (tree)->root_index != M_TR33_NO_NODE || (tree)->size == 0);     \
} while (0)

# define M_TR33_IT_CONTRACT(it, valid) do {                                   \
    M_TR33_CONTRACT( (it).tree);                                              \
    M_ASSERT(valid == false || (it).index >= 0);                              \
    if ((it).index >= 0) {                                                    \
        M_ASSERT( (it).index < (it).tree->capacity);                          \
        M_TR33_NODE_CONTRACT(&(it).tree->tab[(it).index], (it).tree);         \
        /* All child of this node have the parent field correctly set */      \
        m_tr33_index_t itj = (it).tree->tab[(it).index].child;                \
        /* They all have their sibling as the left node */                    \
        m_tr33_index_t lftj = M_TR33_NO_NODE;                                 \
        /* We don't have any infinite loop */                                 \
        unsigned cpt = 0;                                                     \
        while (itj >= 0) {                                                    \
            M_ASSERT( (it).tree->tab[itj].parent == (it).index );             \
            M_ASSERT( (it).tree->tab[itj].left == lftj );                     \
            lftj = itj;                                                       \
            itj = (it).tree->tab[itj].right;                                  \
            M_ASSERT( ++cpt < M_USE_TREE_MAX_CHILD_PER_PARENT);               \
        }                                                                     \
    }                                                                         \
} while (0)

/* Deferred evaluation */
#define M_TR33_DEF_P1(arg) M_ID( M_TR33_DEF_P2 arg )

/* Validate the oplist before going further */
#define M_TR33_DEF_P2(name, type, oplist, tree_t, it_t)                       \
  M_IF_OPLIST(oplist)(M_TR33_DEF_P3, M_TR33_DEF_FAILURE)(name, type, oplist, tree_t, it_t)

/* Stop processing with a compilation failure */
#define M_TR33_DEF_FAILURE(name, type, oplist, tree_t, it_t)                  \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(TREE_DEF): the given argument is not a valid oplist: " #oplist)

/* type of an index in the tree. Up to 2^31-1 nodes can be created.*/
typedef int32_t m_tr33_index_t; 

/* Special value for the parent field of a node:
    * M_TR33_ROOT_NODE is only for the root node.
    * M_TR33_NO_NODE is for nodes without parent, i.e. the pool of free nodes.
 */
#define M_TR33_ROOT_NODE (-2)
#define M_TR33_NO_NODE (-1)

/* Internal definition:
   - name: prefix to be used
   - type: type of the elements of the tree
   - oplist: oplist of the type of the elements of the tree
   - tree_t: alias for the type of the tree
   - it_t: alias for the iterator of the tree

   A free node is identified as parent as M_TR33_NO_NODE, in which case child is the next item in the list of free node.
   A root node is identified as parent as M_TR33_ROOT_NODE
*/
#define M_TR33_DEF_P3(name, type, oplist, tree_t, it_t)                       \
                                                                              \
    /* Define a node of the tree.                                             \
       Each node of a tree is present in the array of the tree and as such    \
       we don't store the reference of other nodes using pointers but using   \
       integers. It is shorter and avoids allocating too much data.           \
       As such, a node may move on inserting another one, and                 \
       an iterator is not a pointer on the node but an index in this array.   \
       + 'parent' is the parent node of the current node,                     \
          or M_TR33_ROOT_NODE if it is a root.                                \
       + 'child' is the first child node of the current node (the left one)   \
          or M_TR33_NO_NODE if there is none.                                 \
       + 'left' is the left sibling or M_TR33_NO_NODE if there is none        \
       + 'right' is the right sibling or M_TR33_NO_NODE if there is none      \
       + 'data' if the data field of the node containing the given type       \
    */                                                                        \
    typedef struct M_C(name, _node_s) {                                       \
        m_tr33_index_t       parent;                                          \
        m_tr33_index_t       child;                                           \
        m_tr33_index_t       left;                                            \
        m_tr33_index_t       right;                                           \
        type                 data;                                            \
    } M_C(name, _node_ct);                                                    \
                                                                              \
    /* Define a tree:                                                         \
       + size is the number of nodes in the tree,                             \
       + capacity is the allocated size of the array 'tab'                    \
       + root_index is the index of the "first" root in the tree.             \
       + free_index is the list of free nodes in the array 'tab'.             \
       + allow_realloc is a bool encoded as true=1 and false=32               \
       + tab is a pointer to the allocated nodes.                             \
    */                                                                        \
    typedef struct M_C(name, _s) {                                            \
        m_tr33_index_t       size;                                            \
        m_tr33_index_t       capacity;                                        \
        m_tr33_index_t       root_index;                                      \
        m_tr33_index_t       free_index;                                      \
        unsigned             allow_realloc;                                   \
        M_C(name, _node_ct) *tab;                                             \
    } tree_t[1];                                                              \
                                                                              \
    /* Define an iterator that references a node.                             \
    A node is an internal type, whereas the iterator is not.                  \
    A node can be moved, whereas the iterator will always remain valid        \
    until the node is destroyed.                                              \
    It is composed of:                                                        \
      + tree is a pointer to the tree structure                               \
      + index is an index in the array tab, identifying the node.             \
    NOTE: we don't define then using [1] as we want to pass then by value     \
    so that we can return iterator on the inserted elements.                  \
    */                                                                        \
    typedef struct M_C(name, _it_s) {                                         \
        struct M_C(name, _s) *tree;                                           \
        m_tr33_index_t        index;                                          \
    } it_t;                                                                   \
                                                                              \
    static inline void                                                        \
    M_C(name, _init)(tree_t tree) {                                           \
        tree->size = 0;                                                       \
        tree->capacity = 0;                                                   \
        tree->root_index = M_TR33_NO_NODE;                                    \
        tree->free_index = M_TR33_NO_NODE;                                    \
        tree->allow_realloc = true;                                           \
        tree->tab = NULL;                                                     \
        M_TR33_CONTRACT(tree);                                                \
    }                                                                         \
                                                                              \
    static inline void                                                        \
    M_C(name, _reset)(tree_t tree) {                                          \
        M_TR33_CONTRACT(tree);                                                \
        if (tree->size > 0) {                                                 \
            /* We don't scan recursively the node tree, but sequentially */   \
            m_tr33_index_t free_index = tree->free_index;                     \
            for(m_tr33_index_t i = 0 ; i < tree->capacity ; i ++) {           \
                /* If the node is not a free node */                          \
                if (tree->tab[i].parent != M_TR33_NO_NODE) {                  \
                    /* free it */                                             \
                    M_CALL_CLEAR(oplist, tree->tab[i].data);                  \
                    tree->tab[i].parent = M_TR33_NO_NODE;                     \
                    tree->tab[i].child  = free_index;                         \
                    tree->tab[i].left   = M_TR33_NO_NODE;                     \
                    tree->tab[i].right  = M_TR33_NO_NODE;                     \
                    free_index = i;                                           \
                }                                                             \
            }                                                                 \
            /* Update the free index */                                       \
            tree->free_index = free_index;                                    \
            tree->size = 0;                                                   \
            tree->root_index = M_TR33_NO_NODE;                                \
        }                                                                     \
        M_TR33_CONTRACT(tree);                                                \
    }                                                                         \
                                                                              \
    static inline void                                                        \
    M_C(name, _clear)(tree_t tree) {                                          \
        M_C(name, _reset)(tree);                                              \
        M_CALL_FREE(oplist, tree->tab);                                       \
        /* This is so reusing the object implies an assertion failure */      \
        tree->size = 1;                                                       \
        tree->tab = NULL;                                                     \
    }                                                                         \
                                                                              \
    static inline size_t                                                      \
    M_C(name, _size)(const tree_t tree) {                                     \
        M_TR33_CONTRACT(tree);                                                \
        return M_ASSIGN_CAST(size_t, tree->size);                             \
    }                                                                         \
                                                                              \
    static inline void                                                        \
    M_C(name, _reserve)(tree_t tree, size_t alloc) {                          \
        M_TR33_CONTRACT(tree);                                                \
        /* Nothing to do if the request is lower than the current capacity. */ \
        if (alloc <= tree->capacity) {                                        \
            return;                                                           \
        }                                                                     \
        /* Realloc the array */                                               \
        if (M_UNLIKELY (alloc >= INT32_MAX)) {                                \
            M_MEMORY_FULL(sizeof (struct M_C(name, _node_s)) * alloc);        \
            return;                                                           \
        }                                                                     \
        struct M_C(name, _node_s) *ptr =                                      \
            M_CALL_REALLOC(oplist, struct M_C(name, _node_s), tree->tab, alloc); \
        if (M_UNLIKELY (ptr == NULL) ) {                                      \
            M_MEMORY_FULL(sizeof (struct M_C(name, _node_s)) * alloc);        \
            return;                                                           \
        }                                                                     \
        /* Construct the list of free node in the extra allocated pool */     \
        for(size_t i = tree->size ; i < alloc; i++) {                         \
            ptr[i].parent = M_TR33_NO_NODE;                                   \
            ptr[i].left   = M_TR33_NO_NODE;                                   \
            ptr[i].right  = M_TR33_NO_NODE;                                   \
            ptr[i].child  = i + 1;                                            \
        }                                                                     \
        /* The last node has no child in the free node list */                \
        ptr[alloc-1].child = M_TR33_NO_NODE;                                  \
        /* Save the free list state in the tree */                            \
        tree->tab = ptr;                                                      \
        tree->capacity = alloc;                                               \
        M_TR33_CONTRACT(tree);                                                \
    }                                                                         \
                                                                              \
    static inline void                                                        \
    M_C(name, _lock)(tree_t tree, bool lock) {                                \
        M_TR33_CONTRACT(tree);                                                \
        tree->allow_realloc = lock ? 32 : 1;                                  \
        M_TR33_CONTRACT(tree);                                                \
    }                                                                         \
                                                                              \
    static inline m_tr33_index_t                                              \
    M_C3(m_tr33_, name, _alloc_node)(tree_t tree) {                           \
        m_tr33_index_t ret = tree->free_index;                                \
        if (M_UNLIKELY(ret < 0)) {                                            \
            /* No more enough space: realloc the array */                     \
            size_t alloc = M_CALL_INC_ALLOC(oplist, tree->size);              \
            /* Take into account if realloc is allowed */                     \
            alloc <<= tree->allow_realloc;                                    \
            if (M_UNLIKELY (alloc >= INT32_MAX)) {                            \
                M_MEMORY_FULL(sizeof (struct M_C(name, _node_s)) * alloc);    \
                return M_TR33_NO_NODE;                                        \
            }                                                                 \
            struct M_C(name, _node_s) *ptr =                                  \
                M_CALL_REALLOC(oplist, struct M_C(name, _node_s), tree->tab, alloc); \
            if (M_UNLIKELY (ptr == NULL) ) {                                  \
                M_MEMORY_FULL(sizeof (struct M_C(name, _node_s)) * alloc);    \
                return M_TR33_NO_NODE;                                        \
            }                                                                 \
            /* Construct the list of free node in the extra allocated pool */ \
            for(size_t i = tree->size ; i < alloc; i++) {                     \
                ptr[i].parent = M_TR33_NO_NODE;                               \
                ptr[i].left   = M_TR33_NO_NODE;                               \
                ptr[i].right  = M_TR33_NO_NODE;                               \
                ptr[i].child  = i + 1;                                        \
            }                                                                 \
            /* The last node has no child in the free node list */            \
            ptr[alloc-1].child = M_TR33_NO_NODE;                              \
            /* Save the free list state in the tree */                        \
            tree->tab = ptr;                                                  \
            tree->capacity = alloc;                                           \
            ret = tree->size;                                                 \
        }                                                                     \
        /* Pop an element in the list of free nodes */                        \
        tree->free_index = tree->tab[ret].child;                              \
        tree->size ++;                                                        \
        return ret;                                                           \
    }                                                                         \
                                                                              \
    static inline void                                                        \
    M_C3(m_tr33_, name, _free_node)(tree_t tree, m_tr33_index_t i) {          \
        tree->tab[i].parent = M_TR33_NO_NODE;                                 \
        tree->tab[i].left   = M_TR33_NO_NODE;                                 \
        tree->tab[i].right  = M_TR33_NO_NODE;                                 \
        tree->tab[i].child  = tree->free_index;                               \
        tree->size --;                                                        \
        tree->free_index = i;                                                 \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _set_root)(tree_t tree, type const data) {                      \
        M_TR33_CONTRACT(tree);                                                \
        M_C(name, _reset)(tree);                                              \
        m_tr33_index_t i = M_C3(m_tr33_, name, _alloc_node)(tree);            \
        tree->tab[i].parent = M_TR33_ROOT_NODE;                               \
        tree->tab[i].left   = M_TR33_NO_NODE;                                 \
        tree->tab[i].right  = M_TR33_NO_NODE;                                 \
        tree->tab[i].child  = M_TR33_NO_NODE;                                 \
        tree->root_index = i;                                                 \
        M_CALL_INIT_SET(oplist, tree->tab[i].data, data);                     \
        it_t it;                                                              \
        it.tree = tree;                                                       \
        it.index = tree->root_index;                                          \
        M_TR33_CONTRACT(tree);                                                \
        return it;                                                            \
    }                                                                         \
                                                                              \
    /* The iterator references the first root node */                         \
    static inline it_t                                                        \
    M_C(name, _it)(tree_t tree) {                                             \
        M_TR33_CONTRACT(tree);                                                \
        it_t it;                                                              \
        it.tree = tree;                                                       \
        it.index = tree->root_index;                                          \
        M_TR33_IT_CONTRACT(it, false);                                        \
        return it;                                                            \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _it_end)(tree_t tree) {                                         \
        M_TR33_CONTRACT(tree);                                                \
        it_t it;                                                              \
        it.tree = tree;                                                       \
        it.index = M_TR33_NO_NODE;                                            \
        M_TR33_IT_CONTRACT(it, false);                                        \
        return it;                                                            \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _end_p)(it_t it) {                                              \
        M_TR33_IT_CONTRACT(it, false);                                        \
        return it.index < 0;                                                  \
    }                                                                         \
                                                                              \
    static inline type *                                                      \
    M_C(name, _ref)(it_t it) {                                                \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return &it.tree->tab[it.index].data;                                  \
    }                                                                         \
                                                                              \
    static inline type const *                                                \
    M_C(name, _cref)(it_t it) {                                               \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return M_CONST_CAST(type, &it.tree->tab[it.index].data);              \
    }                                                                         \
                                                                              \
    static inline type *                                                      \
    M_C(name, _up_ref)(it_t it) {                                             \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = it.tree->tab[it.index].parent;                     \
        return i < 0 ? NULL : &it.tree->tab[i].data;                          \
    }                                                                         \
                                                                              \
    static inline type *                                                      \
    M_C(name, _down_ref)(it_t it) {                                           \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = it.tree->tab[it.index].child;                      \
        return i < 0 ? NULL : &it.tree->tab[i].data;                          \
    }                                                                         \
                                                                              \
    static inline type *                                                      \
    M_C(name, _left_ref)(it_t it) {                                           \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = it.tree->tab[it.index].left;                       \
        return i < 0 ? NULL : &it.tree->tab[i].data;                          \
    }                                                                         \
                                                                              \
    static inline type *                                                      \
    M_C(name, _right_ref)(it_t it) {                                          \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = it.tree->tab[it.index].right;                      \
        return i < 0 ? NULL : &it.tree->tab[i].data;                          \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _insert_up)(it_t it, type const data) {                         \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = M_C3(m_tr33_, name, _alloc_node)(it.tree);         \
        m_tr33_index_t parent = it.tree->tab[it.index].parent;                \
        it.tree->tab[i].parent = parent;                                      \
        it.tree->tab[i].left  = it.tree->tab[it.index].left;                  \
        it.tree->tab[i].right = it.tree->tab[it.index].right;                 \
        it.tree->tab[i].child = it.index;                                     \
        it.tree->tab[it.index].parent = i;                                    \
        it.tree->tab[it.index].left = M_TR33_NO_NODE;                         \
        it.tree->tab[it.index].right = M_TR33_NO_NODE;                        \
        if (M_UNLIKELY(it.tree->root_index == it.index)) {                    \
            /* We have added a parent to the root node. Update root index */  \
            it.tree->root_index = i;                                          \
        } else { if (it.tree->tab[ parent ].child == it.index) {              \
            /* Update the parent to point to the new child */                 \
            it.tree->tab[ parent ].child = i;                                 \
        } }                                                                   \
        /* Return updated iterator on the inserted node */                    \
        it.index = i;                                                         \
        M_CALL_INIT_SET(oplist, it.tree->tab[i].data, data);                  \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it;                                                            \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _insert_down)(it_t it, type const data) {                       \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = M_C3(m_tr33_, name, _alloc_node)(it.tree);         \
        m_tr33_index_t child = it.tree->tab[it.index].child;                  \
        it.tree->tab[i].parent = it.index;                                    \
        it.tree->tab[i].left  = M_TR33_NO_NODE;                               \
        it.tree->tab[i].right = M_TR33_NO_NODE;                               \
        it.tree->tab[i].child = child;                                        \
        it.tree->tab[it.index].child = i;                                     \
        /* Update the parent of all the childs if at least one exists */      \
        while (child != M_TR33_NO_NODE) {                                     \
            it.tree->tab[child].parent = i;                                   \
            child = it.tree->tab[child].right;                                \
        }                                                                     \
        /* Return updated iterator on the inserted node */                    \
        it.index = i;                                                         \
        M_CALL_INIT_SET(oplist, it.tree->tab[i].data, data);                  \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it;                                                            \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _insert_child)(it_t it, type const data) {                      \
        M_TR33_IT_CONTRACT(it, true);                                         \
        /* Insert a node as a child of another, making the current childreen  \
            of the nodes their siblings */                                    \
        m_tr33_index_t i = M_C3(m_tr33_, name, _alloc_node)(it.tree);         \
        m_tr33_index_t child = it.tree->tab[it.index].child;                  \
        it.tree->tab[i].parent = it.index;                                    \
        it.tree->tab[i].left  = M_TR33_NO_NODE;                               \
        it.tree->tab[i].right = child;                                        \
        it.tree->tab[i].child = M_TR33_NO_NODE;                               \
        /* Update the parent */                                               \
        it.tree->tab[it.index].child = i;                                     \
        /* Update the sibling */                                              \
        if (child != M_TR33_NO_NODE) {                                        \
            it.tree->tab[child].left = i;                                     \
        }                                                                     \
        /* Return updated iterator on the inserted node */                    \
        it.index = i;                                                         \
        M_CALL_INIT_SET(oplist, it.tree->tab[i].data, data);                  \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it;                                                            \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _insert_left)(it_t it, type const data) {                       \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = M_C3(m_tr33_, name, _alloc_node)(it.tree);         \
        m_tr33_index_t left = it.tree->tab[it.index].left;                    \
        m_tr33_index_t parent = it.tree->tab[it.index].parent;                \
        it.tree->tab[i].parent = parent;                                      \
        it.tree->tab[i].left  = left;                                         \
        it.tree->tab[i].right = it.index;                                     \
        it.tree->tab[i].child = M_TR33_NO_NODE;                               \
        it.tree->tab[it.index].left = i;                                      \
        /* If there is a left node, update its right */                       \
        if (left > 0) { it.tree->tab[left].right = i; }                       \
        /* Might insert left of root: update root in this case */             \
        if (M_UNLIKELY(parent == M_TR33_ROOT_NODE)) {                         \
            it.tree->root_index = i;                                          \
        } else { if (it.tree->tab[parent].child == it.index) {                \
            /* Update the first child of the parent */                        \
            it.tree->tab[parent].child = i;                                   \
        } }                                                                   \
        /* Return updated iterator on the inserted node */                    \
        it.index = i;                                                         \
        M_CALL_INIT_SET(oplist, it.tree->tab[i].data, data);                  \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it;                                                            \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _insert_right)(it_t it, type const data) {                      \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t i = M_C3(m_tr33_, name, _alloc_node)(it.tree);         \
        m_tr33_index_t right = it.tree->tab[it.index].right;                  \
        it.tree->tab[i].parent = it.tree->tab[it.index].parent;               \
        it.tree->tab[i].left   = it.index;                                    \
        it.tree->tab[i].right  = right;                                       \
        it.tree->tab[i].child  = M_TR33_NO_NODE;                              \
        if (right > 0) { it.tree->tab[right].left = i; }                      \
        it.tree->tab[it.index].right = i;                                     \
        /* Return updated iterator on the inserted node */                    \
        it.index = i;                                                         \
        M_CALL_INIT_SET(oplist, it.tree->tab[i].data, data);                  \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it;                                                            \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _it_up)(it_t *it) {                                             \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        m_tr33_index_t i = it->tree->tab[it->index].parent;                   \
        bool ret = i >= 0;                                                    \
        if (M_LIKELY(ret)) {                                                  \
            it->index = i;                                                    \
        }                                                                     \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        return ret;                                                           \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _it_down)(it_t *it) {                                           \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        m_tr33_index_t i = it->tree->tab[it->index].child;                    \
        bool ret = i >= 0;                                                    \
        if (M_LIKELY(ret)) {                                                  \
            it->index = i;                                                    \
        }                                                                     \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        return ret;                                                           \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _it_left)(it_t *it) {                                           \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        m_tr33_index_t i = it->tree->tab[it->index].left;                     \
        bool ret = i >= 0;                                                    \
        if (M_LIKELY(ret)) {                                                  \
            it->index = i;                                                    \
        }                                                                     \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        return ret;                                                           \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _it_right)(it_t *it) {                                          \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        m_tr33_index_t i = it->tree->tab[it->index].right;                    \
        bool ret = i >= 0;                                                    \
        if (M_LIKELY(ret)) {                                                  \
            it->index = i;                                                    \
        }                                                                     \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        return ret;                                                           \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _root_p)(it_t it) {                                             \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it.tree->tab[it.index].parent == M_TR33_ROOT_NODE;             \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _node_p)(it_t it) {                                             \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it.tree->tab[it.index].child != M_TR33_NO_NODE;                \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _leaf_p)(it_t it) {                                             \
        M_TR33_IT_CONTRACT(it, true);                                         \
        return it.tree->tab[it.index].child == M_TR33_NO_NODE;                \
    }                                                                         \
                                                                              \
    /* Compute the degree of a node in linear time */                         \
    static inline int32_t                                                     \
    M_C(name, _degree)(it_t it) {                                             \
        M_TR33_IT_CONTRACT(it, true);                                         \
        int32_t ret = 0;                                                      \
        m_tr33_index_t i = it.tree->tab[it.index].child;                      \
        while (i >= 0) {                                                      \
            ret ++;                                                           \
            i = it.tree->tab[i].right;                                        \
        }                                                                     \
        return ret;                                                           \
    }                                                                         \
                                                                              \
    /* Compute the depth of a node in linear time */                          \
    static inline int32_t                                                     \
    M_C(name, _depth)(it_t it) {                                              \
        M_TR33_IT_CONTRACT(it, true);                                         \
        int32_t ret = 0;                                                      \
        m_tr33_index_t i = it.tree->tab[it.index].parent;                     \
        while (i >= 0) {                                                      \
            ret ++;                                                           \
            i = it.tree->tab[i].parent;                                       \
        }                                                                     \
        return ret;                                                           \
    }                                                                         \
                                                                              \
    static inline struct M_C(name, _s) *                                      \
    M_C(name, _tree)(it_t it) {                                               \
        M_TR33_IT_CONTRACT(it, false);                                        \
        return it.tree;                                                       \
    }                                                                         \
                                                                              \
    static inline type *                                                      \
    M_C(name, _unlink)(it_t it) {                                             \
        M_TR33_IT_CONTRACT(it, true);                                         \
        m_tr33_index_t parent, child, left, right, child_r;                   \
        parent = it.tree->tab[it.index].parent;                               \
        child  = it.tree->tab[it.index].child;                                \
        left   = it.tree->tab[it.index].left;                                 \
        right  = it.tree->tab[it.index].right;                                \
        /* Test if No child for this node */                                  \
        if (child == M_TR33_NO_NODE) {                                        \
            /* Remove node from sibling */                                    \
            if (left != M_TR33_NO_NODE) {                                     \
                it.tree->tab[left].right = right;                             \
            }                                                                 \
            if (right != M_TR33_NO_NODE) {                                    \
                it.tree->tab[right].left = left;                              \
            }                                                                 \
            /* Remove node from parent if it is the first child */            \
            if (parent >= 0 && it.tree->tab[parent].child == it.index) {      \
                M_ASSERT(left == M_TR33_NO_NODE);                             \
                it.tree->tab[parent].child = right;                           \
            }                                                                 \
        } else {                                                              \
            /* Merge the child with the current siblings */                   \
            /* Compute the range of childs & update their parent */           \
            child_r = child;                                                  \
            it.tree->tab[child_r].parent = parent;                            \
            while (it.tree->tab[child_r].right != M_TR33_NO_NODE) {           \
                child_r = it.tree->tab[child].right;                          \
                it.tree->tab[child_r].parent = parent;                        \
            }                                                                 \
            /* Remove node from sibling */                                    \
            if (left != M_TR33_NO_NODE) {                                     \
                it.tree->tab[left].right = child;                             \
                it.tree->tab[child].left = left;                              \
            }                                                                 \
            if (right != M_TR33_NO_NODE) {                                    \
                it.tree->tab[right].left = child_r;                           \
                it.tree->tab[child_r].right = right;                          \
            }                                                                 \
            /* Remove node from parent if it is the first child */            \
            if (parent >= 0 && it.tree->tab[parent].child == it.index) {      \
                M_ASSERT(left == M_TR33_NO_NODE);                             \
                it.tree->tab[parent].child = child;                           \
            }                                                                 \
        }                                                                     \
        /* Free the node to the allocator */                                  \
        M_C3(m_tr33_, name, _free_node)(it.tree, it.index);                   \
        return &it.tree->tab[it.index].data;                                  \
    }                                                                         \
                                                                              \
    static inline bool                                                        \
    M_C(name, _remove)(it_t it) {                                             \
        M_TR33_IT_CONTRACT(it, false);                                        \
        if (M_UNLIKELY(it.index < 0)) { return false; }                       \
        type *ptr = M_C(name, _unlink)(it);                                   \
        M_CALL_CLEAR(oplist, *ptr);                                           \
        return true;                                                          \
    }                                                                         \
                                                                              \
    /* Scan all nodes, first the parent then the children (uses with _it) */  \
    static inline void                                                        \
    M_C(name, _next)(it_t *it) {                                              \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        /* First go down, if impossible go right */                           \
        if (M_C(name, _it_down)(it) || M_C(name, _it_right)(it)) {            \
            return;                                                           \
        }                                                                     \
        /* If impossible to go right, move up and then right until impossible */ \
        while (M_C(name, _it_up)(it)) {                                       \
            if (M_C(name, _it_right)(it)) {                                   \
                return;                                                       \
            }                                                                 \
        }                                                                     \
        /* Reach end of tree */                                               \
        it->index = M_TR33_NO_NODE;                                           \
        M_TR33_IT_CONTRACT(*it, false);                                       \
    }                                                                         \
                                                                              \
    /* Scan all nodes, first the children then the parent */                  \
    static inline it_t                                                        \
    M_C(name, _it_post)(tree_t tree) {                                        \
        M_TR33_CONTRACT(tree);                                                \
        it_t it;                                                              \
        it.tree = tree;                                                       \
        it.index = tree->root_index;                                          \
        /* Evaluate child first, so go down to the lowest child */            \
        while (M_C(name, _it_down)(&it)) {}                                   \
        M_TR33_IT_CONTRACT(it, false);                                        \
        return it;                                                            \
    }                                                                         \
                                                                              \
    /* Scan all nodes, first the children then the parent (uses with _it_post) */ \
    static inline void                                                        \
    M_C(name, _next_post)(it_t *it) {                                         \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        /* First go right */                                                  \
        if (M_C(name, _it_right)(it)) {                                       \
            /* then go down */                                                \
            while (M_C(name, _it_down)(it)) {}                                \
            return;                                                           \
        }                                                                     \
        /* If impossible to go right, move up */                              \
        if (M_C(name, _it_up)(it)) {                                          \
            return;                                                           \
        }                                                                     \
        /* Reach end of tree */                                               \
        it->index = M_TR33_NO_NODE;                                           \
        M_TR33_IT_CONTRACT(*it, false);                                       \
    }                                                                         \
                                                                              \
    static bool                                                               \
    M_C(name, _it_equal_p)(it_t it1, it_t it2) {                              \
        M_TR33_IT_CONTRACT(it1, false);                                       \
        M_TR33_IT_CONTRACT(it2, false);                                       \
        return it1.tree == it2.tree && it1.index == it2.index;                \
    }                                                                         \
                                                                              \
    /* Scan the nodes of it_ref, first the parent then the children */        \
    static inline void                                                        \
    M_C(name, _next_section)(it_t *it, it_t it_ref) {                         \
        M_TR33_IT_CONTRACT(*it, true);                                        \
        M_ASSERT(it->tree == it_ref.tree);                                    \
        /* First go down, if impossible go right */                           \
        if (M_C(name, _it_down)(it)) { return; }                              \
        if (M_C(name, _it_right)(it)) { return;}                              \
        /* If impossible to go right, move up and then right for the all section */ \
        while (M_C(name, _it_up)(it) && it->index != it_ref.index) {          \
            if (M_C(name, _it_right)(it)) {                                   \
                return;                                                       \
            }                                                                 \
        }                                                                     \
        /* Reach end of section */                                            \
        it->index = M_TR33_NO_NODE;                                           \
        M_TR33_IT_CONTRACT(*it, false);                                       \
    }                                                                         \
                                                                              \
    static inline void                                                        \
    M_C(name, _prune)(it_t it) {                                              \
        M_TR33_IT_CONTRACT(it, true);                                         \
        /* remove the node, including its childs */                           \
        /* Fast removal of all the childs as we don't need to perfom a clean unlink */ \
        it_t child = it;                                                      \
        M_C(name, _next_section)(&child, it);                                 \
        while (!M_C(name, _end_p)(child)) {                                   \
            M_C3(m_tr33_, name, _free_node)(child.tree, child.index);         \
            M_CALL_CLEAR(oplist, child.tree->tab[child.index].data);          \
            M_C(name, _next_section)(&child, it);                             \
        }                                                                     \
        /* Unlink the removed node */                                         \
        it.tree->tab[it.index].child = M_TR33_NO_NODE;                        \
        bool b = M_C(name, _remove)(it);                                      \
        M_ASSERT(b);                                                          \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _lca)(it_t it1, it_t it2) {                                     \
        M_TR33_IT_CONTRACT(it1, true);                                        \
        M_TR33_IT_CONTRACT(it2, true);                                        \
        /* Compute the Lowest Common Ancestor in linear time */               \
        int32_t d1 = M_C(name, _depth)(it1);                                  \
        int32_t d2 = M_C(name, _depth)(it2);                                  \
        bool b = true;                                                        \
        if (d1 > d2) {                                                        \
            M_SWAP(int32_t, d1, d2);                                          \
            M_SWAP(it_t, it1, it2);                                           \
        }                                                                     \
        /* it2 is deeper than it1 */                                          \
        while (d1 < d2) {                                                     \
            b = M_C(name, _it_up)(&it2);                                      \
            d2--;                                                             \
            assert(b);                                                        \
        }                                                                     \
        /* Move up both iterators until we found the common node */           \
        while (b && it1.index != it2.index) {                                 \
            b = M_C(name, _it_up)(&it1);                                      \
            b = M_C(name, _it_up)(&it2);                                      \
        }                                                                     \
        /* If we went back to the root node, and cannot find a common node */ \
        if (b == false) {                                                     \
            it1.index = M_TR33_NO_NODE;                                       \
        }                                                                     \
        return it1;                                                           \
    }                                                                         \
                                                                              \
    static inline it_t                                                        \
    M_C(name, _swap_at)(it_t it1, it_t it2) {                                 \
        M_ASSERT(it1.tree == it2.tree);                                       \
        M_TR33_IT_CONTRACT(it1, true);                                        \
        M_TR33_IT_CONTRACT(it2, true);                                        \
        m_tr33_index_t tmp1, tmp2;                                            \
        /* Swap left references */                                            \
        tmp1 = it1.tree->tab[it1.index].left;                                 \
        tmp2 = it2.tree->tab[it2.index].left;                                 \
        it1.tree->tab[it1.index].left = tmp2;                                 \
        it2.tree->tab[it2.index].left = tmp1;                                 \
        if (tmp1 >= 0) { it1.tree->tab[tmp1].right = it2.index; }             \
        if (tmp2 >= 0) { it2.tree->tab[tmp2].right = it1.index; }             \
        /* Swap right references */                                           \
        tmp1 = it1.tree->tab[it1.index].right;                                \
        tmp2 = it2.tree->tab[it2.index].right;                                \
        it1.tree->tab[it1.index].right = tmp2;                                \
        it2.tree->tab[it2.index].right = tmp1;                                \
        if (tmp1 >= 0) { it1.tree->tab[tmp1].left = it2.index; }              \
        if (tmp2 >= 0) { it2.tree->tab[tmp2].left = it1.index; }              \
        /* Swap down references */                                            \
        tmp1 = it1.tree->tab[it1.index].child;                                \
        tmp2 = it2.tree->tab[it2.index].child;                                \
        it1.tree->tab[it1.index].child = tmp2;                                \
        it2.tree->tab[it2.index].child = tmp1;                                \
        while (tmp1 >= 0) {                                                   \
            it1.tree->tab[tmp1].parent = it2.index;                           \
            tmp1 = it1.tree->tab[tmp1].right;                                 \
        }                                                                     \
        while (tmp2 >= 0) {                                                   \
            it2.tree->tab[tmp2].parent = it1.index;                           \
            tmp2 = it2.tree->tab[tmp2].right;                                 \
        }                                                                     \
        /* Swap up references */                                              \
        tmp1 = it1.tree->tab[it1.index].parent;                               \
        tmp2 = it2.tree->tab[it2.index].parent;                               \
        it1.tree->tab[it1.index].parent = tmp2;                               \
        it2.tree->tab[it2.index].parent = tmp1;                               \
        if (it1.tree->tab[tmp1].child == it1.index) {                         \
            it1.tree->tab[tmp1].child = it2.index;                            \
        }                                                                     \
        if (it2.tree->tab[tmp2].child == it2.index) {                         \
            it2.tree->tab[tmp2].child = it1.index;                            \
        }                                                                     \
    }                                                                         \
                                                                              \
    static void                                                               \
    M_C(name, _graft_child)(it_t it1, const it_t it2) {                       \
        M_ASSERT(it1.tree == it2.tree);                                       \
        M_TR33_IT_CONTRACT(it1, true);                                        \
        M_TR33_IT_CONTRACT(it2, true);                                        \
        /* Move the node it2 and its child down the node *it1 */              \
        /* Both belongs to the same tree */                                   \
        m_tr33_index_t i = it2.index;                                         \
        /* Unlink it2 except its child */                                     \
        m_tr33_index_t parent = it1.tree->tab[i].parent;                      \
        if (parent >= 0 && it1.tree->tab[parent].child == i) {                \
            it1.tree->tab[parent].child = it1.tree->tab[i].right;             \
        }                                                                     \
        m_tr33_index_t left = it1.tree->tab[i].left;                          \
        if (left >= 0) { it1.tree->tab[left].right = it1.tree->tab[i].right;} \
        m_tr33_index_t right = it1.tree->tab[i].right;                        \
        if (right >= 0) { it1.tree->tab[right].left = it1.tree->tab[i].left;} \
        /* Add the new node */                                                \
        m_tr33_index_t child = it1.tree->tab[it1.index].child;                \
        it1.tree->tab[i].parent = it1.index;                                  \
        it1.tree->tab[i].left  = M_TR33_NO_NODE;                              \
        it1.tree->tab[i].right = child;                                       \
        it1.tree->tab[i].child = M_TR33_NO_NODE;                              \
        /* Update the parent */                                               \
        it1.tree->tab[it1.index].child = i;                                   \
        /* Update the sibling */                                              \
        if (child != M_TR33_NO_NODE) {                                        \
            it1.tree->tab[child].left = i;                                    \
        }                                                                     \
        M_TR33_IT_CONTRACT(it1, true);                                        \
        M_TR33_IT_CONTRACT(it2, true);                                        \
    }
    
// TODO: 
// * insertion function with move semantics.
// * Sort the nodes of a child in function of a userfunction

#endif
