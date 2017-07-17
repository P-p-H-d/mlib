/*
 * M*LIB - DICTIONARY Module
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
#ifndef __M_DICT_H
#define __M_DICT_H

#include "m-list.h"
#include "m-array.h"
#include "m-tuple.h"

/* Define a dictionary with the key key_type to the value value_type.
   USAGE: DICT_DEF2(name, key_type, key_oplist, value_type, value_oplist)
*/
#define DICT_DEF2(name, key_type, key_oplist, value_type, value_oplist) \
  TUPLE_DEF2(M_C(dict_pair_, name), (key, key_type, key_oplist), (value, value_type, value_oplist)) \
  M_IF_METHOD(MEMPOOL, key_oplist)(                                     \
  LIST_DEF(M_C(dict_pair_, name), M_C3(dict_pair_,name,_t),             \
           M_OPEXTEND(TUPLE_OPLIST(M_C(dict_pair_, name), key_oplist, value_oplist), MEMPOOL(M_GET_MEMPOOL key_oplist), MEMPOOL_LINKAGE(M_GET_MEMPOOL_LINKAGE key_oplist))) \
  ,                                                                     \
  LIST_DEF(M_C(dict_pair_, name), M_C3(dict_pair_,name,_t), TUPLE_OPLIST(M_C(dict_pair_, name), key_oplist, value_oplist)) \
                                                                        ) \
  ARRAY_DEF(M_C(list_dict_pair_,name), M_C3(list_dict_pair_,name,_t), LIST_OPLIST(M_C(dict_pair_, name), TUPLE_OPLIST(M_C(dict_pair_, name), key_oplist, value_oplist))) \
                                                                        \
  DICTI_DEF2_FUNC(name, key_type, key_oplist, value_type, value_oplist, 0, M_C3(dict_, name, _t), M_C3(dict_it_, name, _t))


/* Define the oplist of a dictionnary.
   USAGE: DICT_OPLIST(name[, oplist of the key type, oplist of the value type]) */
#define DICT_OPLIST(...)                                               \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                          \
  (DICTI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST ),     \
   DICTI_OPLIST(__VA_ARGS__ ))


/* Define a set with the key key_type
   USAGE: DICT_SET_DEF2(name, key_type, key_oplist)
*/
#define DICT_SET_DEF2(name, key_type, key_oplist)                       \
  TUPLE_DEF2(M_C(dict_pair_, name), (key, key_type, key_oplist))        \
  M_IF_METHOD(MEMPOOL, key_oplist)(                                     \
  LIST_DEF(M_C(dict_pair_, name), M_C3(dict_pair_,name,_t),             \
           M_OPEXTEND(TUPLE_OPLIST(M_C(dict_pair_, name), key_oplist), MEMPOOL(M_GET_MEMPOOL key_oplist), MEMPOOL_LINKAGE(M_GET_MEMPOOL_LINKAGE key_oplist))) \
  ,                                                                     \
  LIST_DEF(M_C(dict_pair_, name), M_C3(dict_pair_,name,_t), TUPLE_OPLIST(M_C(dict_pair_, name), key_oplist)) \
                                                                        ) \
  ARRAY_DEF(M_C(list_dict_pair_,name), M_C3(list_dict_pair_,name,_t), LIST_OPLIST(M_C(dict_pair_, name), TUPLE_OPLIST(M_C(dict_pair_, name), key_oplist))) \
                                                                        \
  DICTI_DEF2_FUNC(name, key_type, key_oplist, key_type,                 \
                  (INIT(M_EMPTY_DEFAULT), INIT_SET(M_EMPTY_DEFAULT), SET(M_EMPTY_DEFAULT), CLEAR(M_EMPTY_DEFAULT), \
                   EQUAL(M_EMPTY_DEFAULT), GET_STR(M_EMPTY_DEFAULT), OUT_STR(M_EMPTY_DEFAULT), IN_STR(M_TRUE_DEFAULT)), \
                  1, M_C3(dict_, name, _t), M_C3(dict_it_, name, _t))


/* Define the oplist of a dictionnary.
   USAGE: DICT_SET_OPLIST(name[, oplist of the key type]) */
#define DICT_SET_OPLIST(...)                                           \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                          \
  (DICTI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST ),     \
   DICTI_OPLIST(__VA_ARGS__ , M_RET_ARG2 (__VA_ARGS__, ) ))


/* Open Addressing implementation. Same oplist.
   Need OOR_EQUAL & OOR_SET as methods of the key */
#define DICT_OA_DEF2(name, key_type, key_oplist, value_type, value_oplist) \
  DICTI_OA_DEFI(name, key_type, key_oplist, value_type, value_oplist,   \
                M_GET_OOR_EQUAL key_oplist, M_GET_OOR_SET key_oplist,   \
                0.2, 0.7, M_C3(dict_,name,_t), M_C3(dict_it_,name,_t) )


/********************************** INTERNAL ************************************/

/* Define a chained dictionnary */
#define DICTI_DEF2_FUNC(name, key_type, key_oplist, value_type, value_oplist, isSet, dict_t, dict_it_t) \
                                                                        \
  typedef struct M_C3(dict_, name, _s) {                                \
    size_t used, lower_limit, upper_limit;                              \
    M_C3(array_list_dict_pair_,name,_t) table;                          \
  } dict_t[1];                                                          \
                                                                        \
  typedef struct M_C3(dict_it_, name, _s) {                             \
    M_C3(array_it_list_dict_pair_, name, _t) array_it;                  \
    M_C3(list_it_dict_pair_, name, _t) list_it;                         \
  } dict_it_t[1];                                                       \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _init)(dict_t map)                                  \
  {                                                                     \
    map->used = 0;                                                      \
    M_C3(array_list_dict_pair_,name,_init)(map->table);                 \
    M_C3(array_list_dict_pair_,name,_resize)(map->table, DICTI_INITIAL_SIZE); \
    map->lower_limit = DICTI_LOWER_BOUND(DICTI_INITIAL_SIZE);           \
    map->upper_limit = DICTI_UPPER_BOUND(DICTI_INITIAL_SIZE);           \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _init_set)(dict_t map, const dict_t org)            \
  {                                                                     \
    DICTI_CONTRACT(name, org);                                          \
    assert (map != org);                                                \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    M_C3(array_list_dict_pair_,name,_init_set)(map->table, org->table); \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _set)(dict_t map, const dict_t org)                 \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    DICTI_CONTRACT(name, org);                                          \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    M_C3(array_list_dict_pair_,name,_set)(map->table, org->table);      \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_,name,_clear)(dict_t map)                                   \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    M_C3(array_list_dict_pair_,name,_clear)(map->table);                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _init_move)(dict_t map, dict_t org)                 \
  {                                                                     \
    DICTI_CONTRACT(name, org);                                          \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    M_C3(array_list_dict_pair_,name,_init_move)(map->table, org->table); \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _swap)(dict_t d1, dict_t d2)                        \
  {                                                                     \
    DICTI_CONTRACT(name, d1);                                           \
    DICTI_CONTRACT(name, d2);                                           \
    M_SWAP (size_t, d1->used, d2->used);                                \
    M_SWAP (size_t, d1->lower_limit, d2->lower_limit);                  \
    M_SWAP (size_t, d1->upper_limit, d2->upper_limit);                  \
    M_C3(array_list_dict_pair_,name,_swap)(d1->table, d2->table);       \
    DICTI_CONTRACT(name, d1);                                           \
    DICTI_CONTRACT(name, d2);                                           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _move)(dict_t map, dict_t org)                      \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    DICTI_CONTRACT(name, org);                                          \
    assert (map != org);                                                \
    M_C3(dict_,name,_clear)(map);                                       \
    M_C3(dict_,name,_init_move)(map, org);                              \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_,name,_clean)(dict_t map)                                   \
  {                                                                     \
    M_C3(array_list_dict_pair_,name,_clean)(map->table);                \
    M_C3(array_list_dict_pair_,name,_resize)(map->table, DICTI_INITIAL_SIZE); \
    map->lower_limit = DICTI_LOWER_BOUND(DICTI_INITIAL_SIZE);           \
    map->upper_limit = DICTI_UPPER_BOUND(DICTI_INITIAL_SIZE);           \
    map->used = 0;                                                      \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C3(dict_,name,_size)(const dict_t map)                              \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    return map->used;                                                   \
  }                                                                     \
                                                                        \
  static inline value_type *                                            \
  M_C3(dict_,name,_get)(const dict_t map, const key_type key)           \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    size_t hash = M_GET_HASH key_oplist (key);                          \
    hash = hash & (M_C3(array_list_dict_pair_,name,_size)(map->table) - 1); \
    const M_C3(list_dict_pair_,name,_t) *list_ptr =                     \
      M_C3(array_list_dict_pair_,name,_cget)(map->table, hash);         \
    M_C3(list_it_dict_pair_,name,_t) it;                                \
    for(M_C3(list_dict_pair_,name,_it)(it, *list_ptr);                  \
        !M_C3(list_dict_pair_,name,_end_p)(it);                         \
        M_C3(list_dict_pair_,name,_next)(it)) {                         \
      M_C3(dict_pair_,name,_t) *ref = M_C3(list_dict_pair_,name,_ref)(it); \
      if (M_GET_EQUAL key_oplist ((*ref)->key, key))                    \
        return &(*ref)->M_IF(isSet)(key, value);                        \
    }                                                                   \
    return NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dicti_,name,_resize_up)(dict_t map)                              \
  {                                                                     \
    /* NOTE: Contract may not be fullfilled here */                     \
    size_t old_size = M_C3(array_list_dict_pair_,name,_size)(map->table); \
    size_t new_size = old_size * 2;                                     \
    /* Resize the dictionnary */                                        \
    M_C3(array_list_dict_pair_,name,_resize)(map->table, new_size);     \
    /* Move the items to the new upper part */                          \
    for(size_t i = 0; i < old_size; i++) {                              \
      M_C3(list_dict_pair_,name,_t) *list =                             \
        M_C3(array_list_dict_pair_,name,_get)(map->table, i);           \
      if (M_C3(list_dict_pair_,name,_empty_p)(*list))                   \
        continue;                                                       \
      M_C3(list_it_dict_pair_,name,_t) it;                              \
      M_C3(list_dict_pair_,name,_it)(it, *list);                        \
      while (!M_C3(list_dict_pair_,name,_end_p)(it)) {                  \
        M_C3(dict_pair_,name,_ptr) pair = *M_C3(list_dict_pair_,name,_ref)(it); \
        size_t hash = M_GET_HASH key_oplist (pair->key);                \
        if ((hash & (new_size-1)) >= old_size) {                        \
          assert( (hash & (new_size-1)) == (i + old_size));             \
          M_C3(list_dict_pair_,name,_t) *new_list =                     \
            M_C3(array_list_dict_pair_,name,_get)(map->table, i + old_size); \
          M_C3(list_dict_pair_,name,_splice_back)(*new_list, *list, it); \
        } else {                                                        \
          M_C3(list_dict_pair_,name,_next)(it);                         \
        }                                                               \
      }                                                                 \
    }                                                                   \
    map->upper_limit = DICTI_UPPER_BOUND(new_size);                     \
    map->lower_limit = DICTI_LOWER_BOUND(new_size);                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dicti_,name,_resize_down)(dict_t map)                            \
  {                                                                     \
    /* NOTE: Contract may not be fullfilled here */                     \
    size_t old_size = M_C3(array_list_dict_pair_,name,_size)(map->table); \
    assert ((old_size % 2) == 0);                                       \
    size_t new_size = old_size / 2;                                     \
    assert (new_size >= DICTI_INITIAL_SIZE);                            \
    /* Move all items to the lower part of the dictionnary */           \
    for(size_t i = new_size; i < old_size; i++) {                       \
      M_C3(list_dict_pair_,name,_t) *list =                             \
        M_C3(array_list_dict_pair_,name,_get)(map->table, i);           \
      if (M_C3(list_dict_pair_,name,_empty_p)(*list))                   \
        continue;                                                       \
      M_C3(list_it_dict_pair_,name,_t) it;                              \
      M_C3(list_dict_pair_,name,_it)(it, *list);                        \
      while (!M_C3(list_dict_pair_,name,_end_p)(it)) {                  \
        M_IF_DEBUG(M_C3(dict_pair_,name,_ptr) pair = *M_C3(list_dict_pair_,name,_ref)(it);) \
        M_IF_DEBUG(size_t hash = M_GET_HASH key_oplist (pair->key);)    \
        M_IF_DEBUG(assert ((hash & (old_size -1)) == i);)               \
        M_IF_DEBUG(assert ((hash & (new_size -1)) == i - new_size);)    \
        M_C3(list_dict_pair_,name,_t) *new_list =                       \
          M_C3(array_list_dict_pair_,name,_get)(map->table, i - new_size); \
        M_C3(list_dict_pair_,name,_splice_back)(*new_list, *list, it);  \
      }                                                                 \
    }                                                                   \
    /* Resize the dictionary */                                         \
    M_C3(array_list_dict_pair_,name,_resize)(map->table, new_size);     \
    map->upper_limit = DICTI_UPPER_BOUND(new_size);                     \
    map->lower_limit = DICTI_LOWER_BOUND(new_size);                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_,name,_set_at)(dict_t map, key_type const key               \
                           M_IF(isSet)(, M_DEFERRED_COMMA value_type const value)) \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
                                                                        \
    size_t hash = M_GET_HASH key_oplist(key);                           \
    hash = hash & (M_C3(array_list_dict_pair_,name,_size)(map->table) - 1); \
    M_C3(list_dict_pair_,name,_t) *list_ptr =                           \
      M_C3(array_list_dict_pair_,name,_get)(map->table, hash);          \
    M_C3(list_it_dict_pair_,name,_t) it;                                \
    for(M_C3(list_dict_pair_,name,_it)(it, *list_ptr);                  \
        !M_C3(list_dict_pair_,name,_end_p)(it);                         \
        M_C3(list_dict_pair_,name,_next)(it)) {                         \
      M_C3(dict_pair_,name,_ptr) ref = *M_C3(list_dict_pair_,name,_ref)(it); \
      if (M_GET_EQUAL key_oplist(ref->key, key)) {                      \
        M_GET_SET value_oplist(ref->value, value);                      \
        return;                                                         \
      }                                                                 \
    }                                                                   \
    M_C3(dict_pair_,name,_init_set2)(*M_C3(list_dict_pair_,name,_push_raw)(*list_ptr), \
                                     key M_IF(isSet)(, M_DEFERRED_COMMA value)); \
    map->used ++;                                                       \
    if (M_UNLIKELY (map->used > map->upper_limit) )                     \
      M_C3(dicti_,name,_resize_up)(map);                                \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(dict_,name,_remove)(dict_t map, key_type const key)              \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
                                                                        \
    bool ret = false;                                                   \
    size_t hash = M_GET_HASH key_oplist(key);                           \
    hash = hash & (M_C3(array_list_dict_pair_,name,_size)(map->table) - 1); \
    M_C3(list_dict_pair_,name,_t) *list_ptr =                           \
      M_C3(array_list_dict_pair_,name,_get)(map->table, hash);          \
    M_C3(list_it_dict_pair_,name,_t) it;                                \
    for(M_C3(list_dict_pair_,name,_it)(it, *list_ptr);                  \
        !M_C3(list_dict_pair_,name,_end_p)(it);                         \
        M_C3(list_dict_pair_,name,_next)(it)) {                         \
      M_C3(dict_pair_,name,_ptr) ref = *M_C3(list_dict_pair_,name,_ref)(it); \
      if (M_GET_EQUAL key_oplist(ref->key, key)) {                      \
        M_C3(list_dict_pair_,name,_remove)(*list_ptr, it);              \
        map->used --;                                                   \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    if (M_UNLIKELY (map->used < map->lower_limit) )                     \
      M_C3(dicti_, name, _resize_down)(map);                            \
    return ret;                                                         \
  }                                                                     \
                                                                        \
 static inline void                                                     \
 M_C3(dict_, name, _it)(dict_it_t it, const dict_t d)                   \
 {                                                                      \
   DICTI_CONTRACT(name, d);                                             \
   M_C3(array_list_dict_pair_, name, _it)(it->array_it, d->table);      \
   M_C3(list_dict_pair_,name,_t) *ref =                                 \
     M_C3(array_list_dict_pair_, name, _ref)(it->array_it);             \
   M_C3(list_dict_pair_, name, _it)(it->list_it, *ref);                 \
   while (M_C3(list_dict_pair_, name, _end_p)(it->list_it)) {           \
     M_C3(array_list_dict_pair_, name, _next)(it->array_it);            \
     if (M_C3(array_list_dict_pair_, name, _end_p)(it->array_it))       \
       break;                                                           \
     ref = M_C3(array_list_dict_pair_, name, _ref)(it->array_it);       \
     M_C3(list_dict_pair_, name, _it)(it->list_it, *ref);               \
   }                                                                    \
 }                                                                      \
                                                                        \
 static inline void                                                     \
 M_C3(dict_, name, _it_set)(dict_it_t it, const dict_it_t ref)          \
 {                                                                      \
   assert (it != NULL && ref != NULL);                                  \
   M_C3(array_list_dict_pair_, name, _it_set)(it->array_it,             \
                                              ref->array_it);           \
   M_C3(list_dict_pair_, name, _it_set)(it->list_it, ref->list_it);     \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C3(dict_, name, _end_p)(const dict_it_t it)                          \
 {                                                                      \
   assert (it != NULL);                                                 \
   return M_C3(list_dict_pair_, name, _end_p)(it->list_it);             \
 }                                                                      \
                                                                        \
 static inline void                                                     \
 M_C3(dict_, name, _next)(dict_it_t it)                                 \
 {                                                                      \
   assert(it != NULL);                                                  \
   M_C3(list_dict_pair_, name, _next)(it->list_it);                     \
   M_C3(list_dict_pair_,name,_t) *ref;                                  \
   while (M_C3(list_dict_pair_, name, _end_p)(it->list_it)) {           \
     M_C3(array_list_dict_pair_, name, _next)(it->array_it);            \
     if (M_C3(array_list_dict_pair_, name, _end_p)(it->array_it))       \
       break;                                                           \
     ref = M_C3(array_list_dict_pair_, name, _ref)(it->array_it);       \
     M_C3(list_dict_pair_, name, _it)(it->list_it, *ref);               \
   }                                                                    \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C3(dict_, name, _last_p)(const dict_it_t it)                         \
 {                                                                      \
   assert (it != NULL);                                                 \
   dict_it_t it2;                                                       \
   M_C3(dict_, name,_it_set)(it2, it);                                  \
   M_C3(dict_, name, _next)(it2);                                       \
   return M_C3(dict_, name, _end_p)(it2);                               \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 M_C3(dict_, name, _it_equal_p)(const dict_it_t it1, const dict_it_t it2) \
 {                                                                      \
   assert (it1 != NULL && it2 != NULL);                                 \
   return M_C3(list_dict_pair_, name, _it_equal_p)(it1->list_it,        \
                                                   it2->list_it);       \
 }                                                                      \
                                                                        \
 static inline M_C3(dict_pair_,name,_t) *                               \
 M_C3(dict_, name, _ref)(const dict_it_t it)                            \
 {                                                                      \
   assert(it != NULL);                                                  \
   /* NOTE: partially unsafe if the user modify the 'key'. */           \
   return M_C3(list_dict_pair_, name, _ref)(it->list_it);               \
 }                                                                      \
                                                                        \
 static inline const M_C3(dict_pair_,name,_t) *                         \
 M_C3(dict_, name, _cref)(const dict_it_t it)                           \
 {                                                                      \
   assert(it != NULL);                                                  \
   return M_C3(list_dict_pair_, name, _cref)(it->list_it);              \
 }                                                                      \
                                                                        \
 M_IF_METHOD(EQUAL, value_oplist)(                                      \
 static inline bool                                                     \
 M_C3(dict_, name, _equal_p)(dict_t dict1, dict_t dict2)                \
 {                                                                      \
   assert (dict1 != NULL && dict2 != NULL);                             \
   /* NOTE: Key type has mandatory equal operator */                    \
   return dict1->used == dict2-> used &&                                \
     M_C3(array_list_dict_pair_, name, _equal_p)(dict1->table,          \
                                                 dict2->table);         \
 }                                                                      \
 , /* no value equal */ )                                               \
                                                                        \
 DICTI_DEF2_FUNC_ADDITIONAL(name, key_type, key_oplist, value_type, value_oplist, isSet, dict_t, dict_it_t)


/* Define additional functions for dictionnary.
   Do not used any fields of the dictionnary */
#define DICTI_DEF2_FUNC_ADDITIONAL(name, key_type, key_oplist, value_type, value_oplist, isSet, dict_t, dict_it_t) \
                                                                        \
 M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(                   \
 static inline void                                                     \
 M_C3(dict_, name, _get_str)(string_t str, dict_t dict,                 \
                             const bool append)                         \
  {                                                                     \
    STRING_CONTRACT (str);                                              \
    DICTI_CONTRACT(name, dict);                                         \
    (append ? string_cat_str : string_set_str) (str, "{");              \
    dict_it_t it;                                                       \
    for (M_C3(dict_, name, _it)(it, dict) ;                             \
         !M_C3(dict_, name, _end_p)(it);                                \
         M_C3(dict_, name, _next)(it)){                                 \
      const M_C3(dict_pair_,name,_t) *item =                            \
        M_C3(dict_, name, _cref)(it);                                   \
      M_GET_GET_STR key_oplist (str, (*item)->key, true);               \
      string_push_back (str, ':');                                      \
      M_GET_GET_STR value_oplist (str, (*item)->value, true);           \
      if (!M_C3(dict_, name, _last_p)(it))                              \
        string_push_back (str, ',');                                    \
    }                                                                   \
    string_push_back (str, '}');                                        \
    STRING_CONTRACT(str);                                               \
  }                                                                     \
 , /* no GET_STR */ )                                                   \
                                                                        \
 M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(                   \
 static inline void                                                     \
 M_C3(dict_, name, _out_str)(FILE *file, const dict_t dict)             \
  {                                                                     \
    assert (file != NULL);                                              \
    DICTI_CONTRACT(name, dict);                                         \
    fputc ('{', file);                                                  \
    dict_it_t it;                                                       \
    for (M_C3(dict_, name, _it)(it, dict) ;                             \
         !M_C3(dict_, name, _end_p)(it);                                \
         M_C3(dict_, name, _next)(it)){                                 \
      const M_C3(dict_pair_,name,_t) *item =                            \
        M_C3(dict_, name, _cref)(it);                                   \
      M_GET_OUT_STR key_oplist (file, (*item)->key);                    \
      fputc (':', file);                                                \
      M_GET_OUT_STR value_oplist (file, (*item)->value);                \
      if (!M_C3(dict_, name, _last_p)(it))                              \
        fputc (',', file);                                              \
    }                                                                   \
    fputc ('}', file);                                                  \
  }                                                                     \
 , /* no OUT_STR */ )                                                   \
                                                                        \
 M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(                    \
 static inline bool                                                     \
 M_C3(dict_, name, _in_str)(dict_t dict, FILE *file)                    \
  {                                                                     \
    assert (file != NULL);                                              \
    DICTI_CONTRACT(name, dict);                                         \
    M_C3(dict_, name, _clean)(dict);                                    \
    char c = fgetc(file);                                               \
    if (c != '{') return false;                                         \
    c = fgetc(file);                                                    \
    if (c == '}') return true;                                          \
    ungetc(c, file);                                                    \
    key_type key;                                                       \
    M_IF(isSet)( ,value_type value);                                    \
    M_GET_INIT key_oplist (key);                                        \
    M_GET_INIT value_oplist (value);                                    \
    do {                                                                \
      bool b = M_GET_IN_STR key_oplist (key, file);                     \
      c = fgetc(file);                                                  \
      if (!b) { break; }                                                \
      if (c != ':') { c = 0; break; }                                   \
      b = M_GET_IN_STR value_oplist (value, file);                      \
      if (!b) { c = 0; break; }                                         \
      M_C3(dict_, name, _set_at)(dict, key                              \
                                 M_IF(isSet)( , M_DEFERRED_COMMA value)); \
      c = fgetc(file);                                                  \
    } while (c == ',' && !feof(file) && !ferror(file));                 \
    M_GET_CLEAR key_oplist (key);                                       \
    M_GET_CLEAR value_oplist (value);                                   \
    return c == '}';                                                    \
  }                                                                     \
 , /* no IN_STR */ )                                                    \
                                                                        \

/* Define the oplist of a dictionnary */
#define DICTI_OPLIST(name, key_oplist, value_oplist)                    \
  (INIT(M_C3(dict_, name, _init)),                                      \
   INIT_SET(M_C3(dict_, name, _init_set)),                              \
   SET(M_C3(dict_, name, _set)),                                        \
   CLEAR(M_C3(dict_, name, _clear)),                                    \
   INIT_MOVE(M_C3(dict_, name, _init_move)),                            \
   MOVE(M_C3(dict_, name, _move)),                                      \
   SWAP(M_C3(dict_, name, _swap)),                                      \
   TYPE(M_C3(dict_, name, _t)),                                         \
   SUBTYPE(M_C3(dict_pair_,name,_t)),                                   \
   IT_TYPE(M_C3(dict_it_,name,_t)),                                     \
   IT_FIRST(M_C3(dict_,name,_it)),                                      \
   IT_SET(M_C3(dict_, name, _it_set)),                                  \
   IT_END_P(M_C3(dict_,name,_end_p)),                                   \
   IT_LAST_P(M_C3(dict_,name,_last_p)),                                 \
   IT_NEXT(M_C3(dict_,name,_next)),                                     \
   IT_REF(M_C3(dict_,name,_ref)),                                       \
   IT_CREF(M_C3(dict_,name,_cref))                                      \
   ,OPLIST(PAIR_OPLIST(key_oplist, value_oplist))                       \
   ,M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(GET_STR(M_C3(dict_, name, _get_str)),) \
   ,M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(OUT_STR(M_C3(dict_, name, _out_str)),) \
   ,M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(IN_STR(M_C3(dict_, name, _in_str)),) \
   ,M_IF_METHOD(EQUAL, value_oplist)(EQUAL(M_C3(dict_, name, _equal_p)),) \
   ,KEY_OPLIST(key_oplist)                                              \
   ,VALUE_OPLIST(value_oplist)                                          \
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

#define DICTI_LOWER_BOUND(s) ((s) <= DICTI_INITIAL_SIZE ? 0 : (s) * 1 / 4)
#define DICTI_UPPER_BOUND(s) ((s) * 2 / 3)
#define DICTI_INITIAL_SIZE   16

#define DICTI_CONTRACT(name, map) do {                                  \
    assert(map->used <= map->upper_limit);                              \
    assert(M_POWEROF2_P(M_C3(array_list_dict_pair_,name,_size)(map->table))); \
  } while (0)


/****************************************************************************************/
/* Open Addressing implementation */
/****************************************************************************************/

typedef enum {
  DICTI_OA_EMPTY = 0, DICTI_OA_DELETED = 1
} dicti_oa_element_t;

/* Performing Quadratic probing
   Replace it by '1' to perform linear probing */
#define DICTI_OA_PROBING(s) ((s)++)

#define DICTI_OA_CONTRACT(dict)                                         \
  assert ( (dict) != NULL);                                             \
  assert( (dict)->lower_limit <= (dict)->count && (dict)->count <= (dict)->upper_limit ); \
  assert( (dict)->data != NULL);                                        \
  assert( M_POWEROF2_P((dict)->mask+1));                                \
  assert( (dict)->mask+1 >= DICTI_INITIAL_SIZE);                        \
  assert( (dict)->count <= (dict)->mask+1);                             \

#define DICTI_OA_DEFI(name, key_type, key_oplist, value_type, value_oplist, oor_equal_p, oor_set, coeff_down, coeff_up, dict_t, dict_it_t) \
                                                                        \
  typedef struct M_C3(dict_pair_,name,_s) {                             \
    key_type   key;                                                     \
    value_type value;                                                   \
  } M_C3(dict_pair_,name,_t);                                           \
                                                                        \
  /* NOTE: We don't want a real oplist for this type */                 \
  ARRAY_DEF(M_C(dicti_,name), M_C3(dict_pair_,name,_t),                 \
            (INIT(M_NOTHING_DEFAULT), SET(M_MEMCPY_DEFAULT),            \
             INIT_SET(M_MEMCPY_DEFAULT), CLEAR(M_NOTHING_DEFAULT)))     \
                                                                        \
  typedef struct M_C3(dict_,name,_s) {                                  \
    size_t mask, count, count_delete;                                   \
    size_t upper_limit, lower_limit;                                    \
    M_C3(dict_pair_,name,_t) *data;                                     \
  } dict_t[1];                                                          \
                                                                        \
  typedef struct M_C3(dict_it_,name,_s) {                               \
    struct M_C3(dict_,name,_s) *dict;                                   \
    size_t index;                                                       \
  } dict_it_t[1];                                                       \
                                                                        \
  static inline void                                                    \
  M_C3(dicti_,name,_limit)(dict_t dict, size_t size)                    \
  {                                                                     \
    dict->upper_limit = (size_t) (size * coeff_up) - 1;                 \
    dict->lower_limit = (size <= DICTI_INITIAL_SIZE) ? 0 : (size_t) (size * coeff_down) ; \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_,name,_init)(dict_t dict)                                   \
  {                                                                     \
    assert(0 <= (coeff_down) && (coeff_down)*2 < (coeff_up) && (coeff_up) < 1); \
    dict->mask = DICTI_INITIAL_SIZE-1;                                  \
    dict->count = 0;                                                    \
    dict->count_delete = 0;                                             \
    M_C3(dicti_,name,_limit)(dict, DICTI_INITIAL_SIZE);                 \
    dict->data = M_GET_REALLOC key_oplist (M_C3(dict_pair_,name,_t), NULL, DICTI_INITIAL_SIZE); \
    if (dict->data == NULL) {                                           \
      M_MEMORY_FULL(sizeof (M_C3(dict_pair_,name,_t)) * DICTI_INITIAL_SIZE); \
      return ;                                                          \
    }                                                                   \
    for(size_t i = 0; i < DICTI_INITIAL_SIZE; i++) {                    \
      oor_set(dict->data[i].key, DICTI_OA_EMPTY);                       \
    }                                                                   \
    DICTI_OA_CONTRACT(dict);                                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_,name,_clear)(dict_t dict)                                  \
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    for(size_t i = 0; i <= dict->mask; i++) {                           \
      if (!oor_equal_p(dict->data[i].key, DICTI_OA_EMPTY)               \
          && !oor_equal_p(dict->data[i].key, DICTI_OA_DELETED)) {       \
        M_GET_CLEAR key_oplist (dict->data[i].key);                     \
        M_GET_CLEAR value_oplist (dict->data[i].value);                 \
      }                                                                 \
    }                                                                   \
    M_GET_FREE key_oplist (dict->data);                                 \
    /* Not really needed, but safer */                                  \
    dict->mask = 0;                                                     \
    dict->data = NULL;                                                  \
  }                                                                     \
                                                                        \
  static inline value_type *                                            \
  M_C3(dict_,name,_get)(const dict_t dict, key_type const key)          \
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    assert (!oor_equal_p(key, DICTI_OA_EMPTY));                         \
    assert (!oor_equal_p(key, DICTI_OA_DELETED));                       \
                                                                        \
    M_C3(dict_pair_,name,_t) *const data = dict->data;                  \
    const size_t mask = dict->mask;                                     \
    size_t p = M_GET_HASH key_oplist (key) & mask;                      \
                                                                        \
    /* Random access, and probably cache miss */                        \
    if (M_LIKELY (M_GET_EQUAL key_oplist (data[p].key, key)) )          \
      return &data[p].value;                                            \
    else if (M_LIKELY (oor_equal_p (data[p].key, DICTI_OA_EMPTY)) )     \
      return NULL;                                                      \
                                                                        \
    /* Unlikely case */                                                 \
    size_t s = 1;                                                       \
    do {                                                                \
      p = (p + DICTI_OA_PROBING(s)) & mask;                             \
      if (M_GET_EQUAL key_oplist (data[p].key, key))                    \
        return &data[p].value;                                          \
      assert (s <= dict->mask);                                         \
    } while (!oor_equal_p(data[p].key, DICTI_OA_EMPTY) );               \
                                                                        \
    return NULL;                                                        \
  }                                                                     \
                                                                        \
  M_IF_DEBUG(                                                           \
  static inline bool                                                    \
  M_C3(dicti_,name,_control_after_resize)(const dict_t h)               \
  {                                                                     \
    M_C3(dict_pair_,name,_t) *data = h->data;                           \
    size_t empty = 0;                                                   \
    size_t del = 0;                                                     \
    for(size_t i = 0 ; i <= h->mask ; i++) {                            \
      empty += oor_equal_p(data[i].key, DICTI_OA_EMPTY);                \
      del   += oor_equal_p(data[i].key, DICTI_OA_DELETED);              \
    }                                                                   \
    assert(del == 0);                                                   \
    assert(empty + h->count == h->mask + 1);                            \
    return true;                                                        \
  }                                                                     \
  )                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dicti_,name,_resize_up)(dict_t h, size_t newSize, bool updateLimit) \
  {                                                                     \
    size_t oldSize = h->mask+1;                                         \
    assert (newSize >= oldSize);                                        \
    assert (M_POWEROF2_P(newSize));                                     \
    M_C3(dict_pair_,name,_t) *data = h->data;                           \
    if (newSize > oldSize) {                                            \
      data = M_GET_REALLOC key_oplist (M_C3(dict_pair_,name,_t), data, newSize); \
      if (M_UNLIKELY (data == NULL) ) {                                 \
        M_MEMORY_FULL(sizeof (M_C3(dict_pair_,name,_t)) * newSize);     \
        return ;                                                        \
      }                                                                 \
                                                                        \
      /* First mark the extended space as empty */                      \
      for(size_t i = oldSize ; i < newSize; i++)                        \
        oor_set(data[i].key, DICTI_OA_EMPTY);                           \
    }                                                                   \
                                                                        \
    /* Then let's rehash all the entries in their **exact** position.   \
       If we can't, let's put them in the 'tmp' array.                  \
       It has been measured that the size of this 'tmp' array is	\
       around 6% of the size of updated dictionnary.			\
       NOTE: This should be much cache friendly than typical hash code  */ \
    M_C3(array_dicti_,name,_t) tmp;                                     \
    M_C3(array_dicti_,name,_init)(tmp);                                 \
    const size_t mask = (newSize -1);                                   \
                                                                        \
    for(size_t i = 0 ; i < oldSize; i++) {                              \
      if (!oor_equal_p(data[i].key, DICTI_OA_DELETED) && !oor_equal_p(data[i].key, DICTI_OA_EMPTY)) { \
        size_t p = M_GET_HASH key_oplist (data[i].key) & mask;          \
        if (p != i) {                                                   \
          if (oor_equal_p(data[p].key, DICTI_OA_EMPTY) || oor_equal_p(data[p].key, DICTI_OA_DELETED)) { \
            M_DO_INIT_MOVE(key_oplist, data[p].key, data[i].key);       \
            M_DO_INIT_MOVE(value_oplist, data[p].value, data[i].value); \
          } else {                                                      \
            M_C3(dict_pair_,name,_t) *ptr = M_C3(array_dicti_,name,_push_raw) (tmp); \
            M_DO_INIT_MOVE(key_oplist, ptr->key, data[i].key);          \
            M_DO_INIT_MOVE(value_oplist, ptr->value, data[i].value);    \
          }                                                             \
          oor_set(data[i].key, DICTI_OA_EMPTY);                         \
        }                                                               \
      } else {                                                          \
        oor_set(data[i].key, DICTI_OA_EMPTY);                           \
      }                                                                 \
    }                                                                   \
                                                                        \
    /* Let's put back the entries in the tmp array in their right place */ \
    /* NOTE: There should be very few entries in this array             \
       which contains what we weren't be able to fit in the first pass */ \
    for(size_t i = 0; i < M_C3(array_dicti_,name,_size)(tmp); i++) {    \
      M_C3(dict_pair_,name,_t) *item = M_C3(array_dicti_,name,_get)(tmp, i); \
      size_t p = M_GET_HASH key_oplist (item->key) & mask;              \
      if (!oor_equal_p(data[p].key, DICTI_OA_EMPTY)) {                  \
        size_t s = 1;                                                   \
        do {                                                            \
          p = (p + DICTI_OA_PROBING(s)) & mask;                         \
          assert (s <= h->mask);                                        \
        } while (!oor_equal_p(data[p].key, DICTI_OA_EMPTY) );           \
      }                                                                 \
      /* FIXME: How can I use INIT_MOVE without garbaging the array? ==> POP_INIT_MOVE ! */ \
      M_GET_INIT_SET key_oplist (data[p].key, item->key);               \
      M_GET_INIT_SET value_oplist (data[p].value, item->value);         \
    }                                                                   \
                                                                        \
    M_C3(array_dicti_,name,_clear) (tmp);                               \
    h->mask = newSize-1;                                                \
    h->count_delete = h->count;                                         \
    if (updateLimit == true) {						\
      M_C3(dicti_,name,_limit)(h, newSize);				\
    }									\
    h->data = data;                                                     \
    M_IF_DEBUG (assert (M_C3(dicti_,name,_control_after_resize)(h));)   \
    DICTI_OA_CONTRACT(h);                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_,name,_set_at)(dict_t dict, key_type const key,             \
                           value_type const value)                      \
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    assert (!oor_equal_p(key, DICTI_OA_EMPTY));                         \
    assert (!oor_equal_p(key, DICTI_OA_DELETED));                       \
                                                                        \
    M_C3(dict_pair_,name,_t) *const data = dict->data;                  \
    const size_t mask = dict->mask;                                     \
    size_t p = M_GET_HASH key_oplist (key) & mask;                      \
                                                                        \
    /* NOTE: Likely cache miss */                                       \
    if (M_UNLIKELY (M_GET_EQUAL key_oplist (data[p].key, key)) ) {      \
      M_GET_SET value_oplist (data[p].value, value);                    \
      return;                                                           \
    }                                                                   \
    if (M_UNLIKELY (!oor_equal_p(data[p].key, DICTI_OA_EMPTY) ) ) {     \
      size_t delPos = -1;                                               \
      if (oor_equal_p(data[p].key, DICTI_OA_DELETED)) delPos = p;       \
      size_t s = 1;                                                     \
      do {                                                              \
        p = (p + DICTI_OA_PROBING(s)) & mask;                           \
        if (M_GET_EQUAL key_oplist (data[p].key, key)) {                \
          M_GET_SET value_oplist (data[p].value, value);                \
          return;                                                       \
        }                                                               \
        assert (s <= dict->mask);                                       \
        if (oor_equal_p(data[p].key, DICTI_OA_DELETED) && delPos == (size_t)-1) delPos = p; \
      } while (!oor_equal_p(data[p].key, DICTI_OA_EMPTY) );             \
      if (delPos != (size_t) -1) p = delPos;                            \
      if (oor_equal_p(data[p].key, DICTI_OA_DELETED)) dict->count_delete-=1; \
    }                                                                   \
                                                                        \
    M_GET_INIT_SET key_oplist (data[p].key, key);                       \
    M_GET_INIT_SET value_oplist (data[p].value, value);                 \
    dict->count++;                                                      \
    dict->count_delete ++;                                              \
                                                                        \
    if (M_UNLIKELY (dict->count_delete >= dict->upper_limit)) {         \
      size_t newSize = dict->mask+1;                                    \
      if (dict->count > (dict->mask / 2)) newSize += newSize;           \
      M_C3(dicti_,name,_resize_up)(dict, newSize, true);		\
    }                                                                   \
    DICTI_OA_CONTRACT(dict);                                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dicti_,name,_resize_down)(dict_t h, size_t newSize)              \
  {                                                                     \
    size_t oldSize = h->mask+1;                                         \
    assert (newSize <= oldSize && M_POWEROF2_P(newSize));               \
    if (M_UNLIKELY (newSize < DICTI_INITIAL_SIZE))                      \
      newSize = DICTI_INITIAL_SIZE;                                     \
    const size_t mask = newSize -1;                                     \
    M_C3(dict_pair_,name,_t) *data = h->data;                           \
    M_C3(array_dicti_,name,_t) tmp;                                     \
    M_C3(array_dicti_,name,_init)(tmp);                                 \
                                                                        \
    /* Pass 1: scan lower entries, and move them if needed */           \
    for(size_t i = 0; i < newSize; i++) {                               \
      if (oor_equal_p(data[i].key, DICTI_OA_EMPTY))                     \
        continue;                                                       \
      if (oor_equal_p(data[i].key, DICTI_OA_DELETED)) {                 \
        oor_set(data[i].key, DICTI_OA_EMPTY);                           \
        continue;                                                       \
      }                                                                 \
      size_t p = M_GET_HASH key_oplist (data[i].key) & mask;            \
      if (p != i) {                                                     \
        if (oor_equal_p(data[p].key, DICTI_OA_EMPTY) || oor_equal_p(data[p].key, DICTI_OA_DELETED)) { \
          M_DO_INIT_MOVE(key_oplist, data[p].key, data[i].key);         \
          M_DO_INIT_MOVE(value_oplist, data[p].value, data[i].value);   \
        } else {                                                        \
          M_C3(dict_pair_,name,_t) *ptr = M_C3(array_dicti_,name,_push_raw) (tmp); \
          M_DO_INIT_MOVE(key_oplist, ptr->key, data[i].key);            \
          M_DO_INIT_MOVE(value_oplist, ptr->value, data[i].value);      \
        }                                                               \
        oor_set(data[i].key, DICTI_OA_EMPTY);                           \
      }                                                                 \
    }                                                                   \
    /* Pass 2: scan upper entries and move them back */                 \
    for(size_t i = newSize; i < oldSize; i++) {                         \
      if (!oor_equal_p(data[i].key, DICTI_OA_DELETED)                   \
          && !oor_equal_p(data[i].key, DICTI_OA_EMPTY)) {               \
        size_t p = M_GET_HASH key_oplist (data[i].key) & mask;          \
        assert (p < i);                                                 \
        if (!oor_equal_p(data[p].key, DICTI_OA_EMPTY)) {                \
          size_t s = 1;                                                 \
          do {                                                          \
            p = (p + DICTI_OA_PROBING(s)) & mask;                       \
            assert (s <= h->mask);                                      \
          } while (!oor_equal_p(data[p].key, DICTI_OA_EMPTY) );         \
        }                                                               \
        M_DO_INIT_MOVE(key_oplist, data[p].key, data[i].key);           \
        M_DO_INIT_MOVE(value_oplist, data[p].value, data[i].value);     \
      }                                                                 \
    }                                                                   \
    /* Pass 3: scan moved entries and move them back */                 \
    for(size_t i = 0; i < M_C3(array_dicti_,name,_size)(tmp); i++) {    \
      M_C3(dict_pair_,name,_t) *item = M_C3(array_dicti_,name,_get)(tmp, i); \
      size_t p = M_GET_HASH key_oplist (item->key) & mask;              \
      if (!oor_equal_p(data[p].key, DICTI_OA_EMPTY)) {                  \
        size_t s = 1;                                                   \
        do {                                                            \
          p = (p + DICTI_OA_PROBING(s)) & mask;                         \
          assert (s <= h->mask);                                        \
        } while (!oor_equal_p(data[p].key, DICTI_OA_EMPTY) );           \
      }                                                                 \
      M_GET_INIT_SET key_oplist (data[p].key, item->key);               \
      M_GET_INIT_SET value_oplist (data[p].value, item->value);         \
    }                                                                   \
                                                                        \
    M_C3(array_dicti_,name,_clear) (tmp);                               \
    h->count_delete = h->count;                                         \
    if (newSize != oldSize) {                                           \
      h->mask = newSize-1;                                              \
      M_C3(dicti_,name,_limit)(h, newSize);                             \
      h->data = M_GET_REALLOC key_oplist (M_C3(dict_pair_,name,_t), data, newSize); \
      assert (h->data != NULL);                                         \
    }                                                                   \
    M_IF_DEBUG (assert (M_C3(dicti_,name,_control_after_resize)(h));)   \
    assert (h->lower_limit < h->count && h->count < h->upper_limit);    \
    DICTI_OA_CONTRACT(h);                                               \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(dict_,name,_remove)(dict_t dict, const key_type key)             \
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    assert (!oor_equal_p(key, DICTI_OA_EMPTY));                         \
    assert (!oor_equal_p(key, DICTI_OA_DELETED));                       \
                                                                        \
    M_C3(dict_pair_,name,_t) *const data = dict->data;                  \
    const size_t mask = dict->mask;                                     \
    size_t p = M_GET_HASH key_oplist (key) & mask;                      \
                                                                        \
    /* Random access, and probably cache miss */                        \
    if (M_UNLIKELY (!M_GET_EQUAL key_oplist (data[p].key, key)) ) {     \
      if (oor_equal_p (data[p].key, DICTI_OA_EMPTY))                    \
        return false;                                                   \
      size_t s = 1;                                                     \
      do {                                                              \
        p = (p + DICTI_OA_PROBING(s)) & mask;                           \
        if (oor_equal_p(data[p].key, DICTI_OA_EMPTY) )                  \
          return false;                                                 \
        assert (s <= dict->mask);                                       \
      } while (!M_GET_EQUAL key_oplist (data[p].key, key));             \
    }                                                                   \
    M_GET_CLEAR key_oplist (data[p].key);                               \
    M_GET_CLEAR value_oplist (data[p].value);                           \
    oor_set (data[p].key, DICTI_OA_DELETED);                            \
    assert (dict->count >= 1);                                          \
    dict->count--;                                                      \
    if (M_UNLIKELY (dict->count < dict->lower_limit)) {                 \
      M_C3(dicti_,name,_resize_down)(dict, (dict->mask+1) >> 1);        \
    }                                                                   \
    DICTI_OA_CONTRACT(dict);                                            \
    return true;                                                        \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C3(dict_,name,_size)(const  dict_t dict)                            \
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    return dict->count;                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _init_set)(dict_t map, const dict_t org)            \
  {                                                                     \
    DICTI_OA_CONTRACT(org);                                             \
    assert (map != org);                                                \
    map->mask         = org->mask;                                      \
    map->count        = org->count;                                     \
    map->count_delete = org->count_delete;                              \
    map->upper_limit  = org->upper_limit;                               \
    map->lower_limit  = org->lower_limit;                               \
    map->data = M_GET_REALLOC key_oplist (M_C3(dict_pair_,name,_t), NULL, map->mask+1); \
    if (map->data == NULL) {                                            \
      M_MEMORY_FULL(sizeof (M_C3(dict_pair_,name,_t)) * (map->mask+1)); \
      return ;                                                          \
    }                                                                   \
    for(size_t i = 0; i <= org->mask; i++) {                            \
      if (oor_equal_p(org->data[i].key, DICTI_OA_EMPTY)) {              \
        oor_set(map->data[i].key, DICTI_OA_EMPTY);                      \
      } else if (oor_equal_p(org->data[i].key, DICTI_OA_DELETED)) {     \
        oor_set(map->data[i].key, DICTI_OA_DELETED);                    \
      } else {                                                          \
        M_GET_INIT_SET key_oplist (map->data[i].key, org->data[i].key); \
        M_GET_INIT_SET value_oplist (map->data[i].value, org->data[i].value); \
      }                                                                 \
    }                                                                   \
    DICTI_OA_CONTRACT(map);                                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _set)(dict_t map, const dict_t org)                 \
  {                                                                     \
    DICTI_OA_CONTRACT(map);                                             \
    DICTI_OA_CONTRACT(org);                                             \
    if (M_LIKELY (map != org)) {                                        \
      M_C3(dict_, name, _clear)(map);                                   \
      M_C3(dict_, name, _init_set)(map, org);                           \
    }                                                                   \
    DICTI_OA_CONTRACT(map);                                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _init_move)(dict_t map, dict_t org)                 \
  {                                                                     \
    DICTI_OA_CONTRACT(org);                                             \
    assert (map != org);                                                \
    map->mask         = org->mask;                                      \
    map->count        = org->count;                                     \
    map->count_delete = org->count_delete;                              \
    map->upper_limit  = org->upper_limit;                               \
    map->lower_limit  = org->lower_limit;                               \
    map->data         = org->data;                                      \
    /* Mark org as cleared (safety) */                                  \
    org->mask         = 0;                                              \
    org->data         = NULL;                                           \
    DICTI_OA_CONTRACT(map);                                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _move)(dict_t map, dict_t org)                      \
  {                                                                     \
    DICTI_OA_CONTRACT(map);                                             \
    DICTI_OA_CONTRACT(org);                                             \
    if (M_LIKELY (map != org)) {                                        \
      M_C3(dict_, name, _clear)(map);                                   \
      M_C3(dict_, name, _init_move)(map, org);                          \
    }                                                                   \
    DICTI_OA_CONTRACT(map);                                             \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _swap)(dict_t d1, dict_t d2)                        \
  {                                                                     \
    DICTI_OA_CONTRACT(d1);                                              \
    DICTI_OA_CONTRACT(d2);                                              \
    M_SWAP (size_t, d1->mask,         d2->mask);                        \
    M_SWAP (size_t, d1->count,        d2->count);                       \
    M_SWAP (size_t, d1->count_delete, d2->count_delete);                \
    M_SWAP (size_t, d1->upper_limit,  d2->upper_limit);                 \
    M_SWAP (size_t, d1->lower_limit,  d2->lower_limit);                 \
    M_SWAP (M_C3(dict_pair_,name,_t) *, d1->data, d2->data);            \
    DICTI_OA_CONTRACT(d1);                                              \
    DICTI_OA_CONTRACT(d2);                                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _clean)(dict_t d)                                   \
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    for(size_t i = 0; i <= d->mask; i++) {                              \
      if (!oor_equal_p(d->data[i].key, DICTI_OA_EMPTY)                  \
          && !oor_equal_p(d->data[i].key, DICTI_OA_DELETED)) {          \
        M_GET_CLEAR key_oplist (d->data[i].key);                        \
        M_GET_CLEAR value_oplist (d->data[i].value);                    \
        oor_set(d->data[i].key, DICTI_OA_EMPTY);                        \
      }                                                                 \
    }                                                                   \
    d->count = 0;                                                       \
    d->count_delete = 0;                                                \
    d->mask = DICTI_INITIAL_SIZE-1;                                     \
    M_C3(dicti_,name,_limit)(d, DICTI_INITIAL_SIZE);                    \
    d->data = M_GET_REALLOC key_oplist (M_C3(dict_pair_,name,_t),       \
                                        d->data, DICTI_INITIAL_SIZE);   \
    DICTI_OA_CONTRACT(d);                                               \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _it)(dict_it_t it, dict_t d)                        \
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    assert (it != NULL);                                                \
    it->dict = d;                                                       \
    size_t i = 0;                                                       \
    while ((oor_equal_p (d->data[i].key, DICTI_OA_EMPTY)                \
            || oor_equal_p (d->data[i].key, DICTI_OA_DELETED))          \
           && i <= d->mask) {                                           \
      i++;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _it_set)(dict_it_t it, const dict_it_t ref)         \
  {                                                                     \
    assert (it != NULL);                                                \
    assert (ref != NULL);                                               \
    it->dict = ref->dict;                                               \
    it->index = ref->index;                                             \
    DICTI_OA_CONTRACT (it->dict);                                       \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _it_last)(dict_it_t it,  dict_t d)                  \
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    assert (it != NULL);                                                \
    it->dict = d;                                                       \
    size_t i = d->mask;                                                 \
    while ((oor_equal_p (d->data[i].key, DICTI_OA_EMPTY)                \
            || oor_equal_p (d->data[i].key, DICTI_OA_DELETED))          \
           && i <= d->mask) {                                           \
      i--;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _it_end)(dict_it_t it,  dict_t d)                   \
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    assert (it != NULL);                                                \
    it->dict = d;                                                       \
    it->index = d->mask+1;                                              \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(dict_, name, _end_p)(const dict_it_t it)                         \
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it->dict);                                       \
    return it->index > it->dict->mask;                                  \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _next)(dict_it_t it)                                \
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it->dict);                                       \
    size_t i = it->index + 1;                                           \
    while ((oor_equal_p (it->dict->data[i].key, DICTI_OA_EMPTY)         \
            || oor_equal_p (it->dict->data[i].key, DICTI_OA_DELETED))   \
           && i <= it->dict->mask) {                                    \
      i++;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C3(dict_, name, _previous)(dict_it_t it)                            \
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it->dict);                                       \
    /* if index was 0, the operation will overflow, and stops the loop */ \
    size_t i = it->index - 1;                                           \
    while ((oor_equal_p (it->dict->data[i].key, DICTI_OA_EMPTY)         \
            || oor_equal_p (it->dict->data[i].key, DICTI_OA_DELETED))   \
           && i <= it->dict->mask) {                                    \
      i--;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(dict_, name, _last_p)(const dict_it_t it)                        \
  {                                                                     \
    assert (it != NULL);                                                \
    dict_it_t it2;                                                      \
    M_C3(dict_, name,_it_set)(it2, it);                                 \
    M_C3(dict_, name, _next)(it2);                                      \
    return M_C3(dict_, name, _end_p)(it2);                              \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C3(dict_, name, _it_equal_p)(const dict_it_t it1,const dict_it_t it2) \
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    DICTI_OA_CONTRACT (it1->dict);                                      \
    DICTI_OA_CONTRACT (it2->dict);                                      \
    return it1->dict == it2->dict && it1->index == it2->index;          \
  }                                                                     \
                                                                        \
  /* FIXME: Wrong level of indirection compared to std dict? */         \
  static inline M_C3(dict_pair_,name,_t) *                              \
  M_C3(dict_, name, _ref)(const dict_it_t it)                           \
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it -> dict);                                     \
    const size_t i = it->index;                                         \
    assert (i <= it->dict->mask);                                       \
    assert (!oor_equal_p (it->dict->data[i].key, DICTI_OA_EMPTY));      \
    assert (!oor_equal_p (it->dict->data[i].key, DICTI_OA_DELETED));    \
    return &it->dict->data[i];                                          \
  }                                                                     \
                                                                        \
  static inline void							\
  M_C3(dict_,name,_reserve)(dict_t dict, size_t capacity)		\
  {									\
    DICTI_OA_CONTRACT(dict);						\
    size_t size;							\
    /* Get the size which will allow to fit this capacity */		\
    size = m_core_roundpow2 (capacity * (1.0 / coeff_up));		\
    assert (M_POWEROF2_P(size));					\
    if (size > dict->mask+1) {						\
      dict->upper_limit = (size_t) (size * coeff_up) - 1;		\
      M_C3(dicti_,name,_resize_up)(dict, size, false);			\
    }									\
    DICTI_OA_CONTRACT(dict);						\
  }									\
									\
 DICTI_DEF2_FUNC_ADDITIONAL(name, key_type, key_oplist, value_type, value_oplist, 0, dict_t, dict_it_t)


#endif
