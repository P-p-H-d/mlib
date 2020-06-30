/*
 * M*LIB - DICTIONARY Module
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
#ifndef MSTARLIB_DICT_H
#define MSTARLIB_DICT_H

#include "m-list.h"
#include "m-array.h"
#include "m-tuple.h"


/* Define a dictionary associating the key key_type to the value value_type and its associated functions.
   USAGE:
     DICT_DEF2(name, key_type, key_oplist, value_type, value_oplist)
   OR
     DICT_DEF2(name, key_type, value_type)
*/
#define DICT_DEF2(name, key_type, ...)                                  \
  DICTI_DEF2_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                ((name, key_type, M_GLOBAL_OPLIST_OR_DEF(key_type)(), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                 (name, key_type, __VA_ARGS__)))


/* Define a dictionary asssociating the key key_type to the value value_type and its associated functions.
   It stores the computed hash value, avoiding the need of recomputing it but increasing memory
   consumption.
   USAGE:
     DICT_STOREHASH_DEF2(name, key_type[, key_oplist], value_type[, value_oplist])
   OR
     DICT_STOREHASH_DEF2(name, key_type[, key_oplist], value_type[, value_oplist])
*/
#define DICT_STOREHASH_DEF2(name, key_type, ...)                        \
  DICTI_SHASH_DEF2_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                       \
                      ((name, key_type, M_GLOBAL_OPLIST_OR_DEF(key_type)(), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                       (name, key_type, __VA_ARGS__)))


/* Define a dictionary associating the key key_type to the value value_type
   with an Open Addressing implementation and its associated functions.
   KEY_OPLIST needs the operators OOR_EQUAL & OOR_SET.
   USAGE:
     DICT_OA_DEF2(name, key_type, key_oplist, value_type, value_oplist)
   OR
     DICT_OA_DEF2(name, key_type, value_type)
*/
#define DICT_OA_DEF2(name, key_type, ...)                               \
  DICTI_OA_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                  ((name, key_type, M_GLOBAL_OPLIST_OR_DEF(key_type)(), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                   (name, key_type, __VA_ARGS__)))


/* Define a set of the key key_type and its associated functions.
   The set is unordered.
   USAGE: DICT_SET_DEF(name, key_type[, key_oplist])
*/
#define DICT_SET_DEF(name, ...)                                         \
  DICTI_SET_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                   ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)()), \
                    (name, __VA_ARGS__)))


/* Define a set of the key key_type 
   with an Open Addressing implementation and its associated functions.
   The set is unordered.
   USAGE: DICT_OASET_DEF(name, key_type[, key_oplist])
*/
#define DICT_OASET_DEF(name, ...)                                         \
  DICTI_OASET_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                     ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)()), \
                      (name, __VA_ARGS__)))


/* Define the oplist of a dictionnary (DICT_DEF2, DICT_STOREHASH_DEF2 or DICT_OA_DEF2).
   USAGE:
     DICT_OPLIST(name, oplist of the key type, oplist of the value type)
   OR
     DICT_OPLIST(name)
*/
#define DICT_OPLIST(...)                                               \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                          \
  (DICTI_OPLIST_P1((__VA_ARGS__, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST )), \
   DICTI_OPLIST_P1((__VA_ARGS__ )))


/* Define the oplist of a dictionnary (DICT_SET_DEF).
   USAGE: DICT_SET_OPLIST(name[, oplist of the key type]) */
#define DICT_SET_OPLIST(...)                                           \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                          \
  (DICTI_SET_OPLIST_P1((__VA_ARGS__, M_DEFAULT_OPLIST)),               \
   DICTI_SET_OPLIST_P1((__VA_ARGS__ )))



/********************************** INTERNAL ************************************/

/* Define a dictionary from the key key_type to the value value_type.
   It is defined as an array of singly linked list (each list
   representing a bucket of items with the same hash value modulo the
   current array size).
*/
/* Deferred evaluation for arg */
#define DICTI_DEF2_P1(arg) DICTI_DEF2_P2 arg

/* Validate the key oplist before going further */
#define DICTI_DEF2_P2(name, key_type, key_oplist, value_type, value_oplist) \
  M_IF_OPLIST(key_oplist)(DICTI_DEF2_P3, DICTI_DEF2_FAILURE)(name, key_type, key_oplist, value_type, value_oplist)

/* Validate the value oplist before going further */
#define DICTI_DEF2_P3(name, key_type, key_oplist, value_type, value_oplist) \
  M_IF_OPLIST(value_oplist)(DICTI_DEF2_P4, DICTI_DEF2_FAILURE)(name, key_type, key_oplist, value_type, value_oplist)

/* Stop processing with a compilation failure */
#define DICTI_DEF2_FAILURE(name, key_type, key_oplist, value_type, value_oplist) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(DICT_DEF2): at least one of the given argument is not a valid oplist: " M_AS_STR(key_oplist) " / " M_AS_STR(value_oplist) )

#define DICTI_DEF2_P4(name, key_type, key_oplist, value_type, value_oplist) \
                                                                        \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, key_type, key_oplist)              \
  M_CHECK_COMPATIBLE_OPLIST(name, 2, value_type, value_oplist)          \
                                                                        \
  TUPLE_DEF2(M_C(name, _pair), (key, key_type, key_oplist), (value, value_type, value_oplist)) \
									\
  M_IF_METHOD(MEMPOOL, key_oplist)					\
  (									\
   LIST_DEF(M_C(name, _list_pair), M_C(name, _pair_t),			\
	    M_OPEXTEND(TUPLE_OPLIST(M_C(name, _pair), key_oplist, value_oplist), \
		       MEMPOOL(M_GET_MEMPOOL key_oplist), MEMPOOL_LINKAGE(M_GET_MEMPOOL_LINKAGE key_oplist))) \
   ,									\
   LIST_DEF(M_C(name, _list_pair), M_C(name, _pair_t),			\
	    TUPLE_OPLIST(M_C(name, _pair), key_oplist, value_oplist))	\
  )                                                                     \
  									\
  ARRAY_DEF(M_C(name, _array_list_pair), M_C(name, _list_pair_t),	\
	    LIST_OPLIST(M_C(name, _list_pair), TUPLE_OPLIST(M_C(name, _pair), key_oplist, value_oplist))) \
									\
  DICTI_FUNC_DEF2_P5(name, key_type, key_oplist, value_type, value_oplist, 0, 0, M_C(name, _t), M_C(name, _it_t))


/* Define a dictionary with the key key_type to the value value_type.
   which stores the computed hash value (avoiding the need of recomputing it).
   It is defined as an array of singly linked list (each list
   representing a bucket).
*/
/* Defered evaluation for arg */
#define DICTI_SHASH_DEF2_P1(arg) DICTI_SHASH_DEF2_P2 arg

/* Validate the key oplist before going further */
#define DICTI_SHASH_DEF2_P2(name, key_type, key_oplist, value_type, value_oplist) \
  M_IF_OPLIST(key_oplist)(DICTI_SHASH_DEF2_P3, DICTI_SHASH_DEF2_FAILURE)(name, key_type, key_oplist, value_type, value_oplist)

/* Validate the value oplist before going further */
#define DICTI_SHASH_DEF2_P3(name, key_type, key_oplist, value_type, value_oplist) \
  M_IF_OPLIST(value_oplist)(DICTI_SHASH_DEF2_P4, DICTI_SHASH_DEF2_FAILURE)(name, key_type, key_oplist, value_type, value_oplist)

/* Stop processing with a compilation failure */
#define DICTI_SHASH_DEF2_FAILURE(name, key_type, key_oplist, value_type, value_oplist) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(DICT_STOREHASH_DEF2): at least one of the given argument is not a valid oplist: " M_AS_STR(key_oplist) " / " M_AS_STR(value_oplist) )

#define DICTI_SHASH_DEF2_P4(name, key_type, key_oplist, value_type, value_oplist) \
									\
  M_CHECK_COMPATIBLE_OPLIST(name, 1, key_type, key_oplist)              \
  M_CHECK_COMPATIBLE_OPLIST(name, 2, value_type, value_oplist)          \
                                                                        \
  TUPLE_DEF2(M_C(name, _pair), (hash, size_t, M_DEFAULT_OPLIST), (key, key_type, key_oplist), (value, value_type, value_oplist)) \
									\
  M_IF_METHOD(MEMPOOL, key_oplist)					\
  (									\
   LIST_DEF(M_C(name, _list_pair), M_C(name, _pair_t),			\
	    M_OPEXTEND(TUPLE_OPLIST(M_C(name, _pair), key_oplist, value_oplist), \
		       MEMPOOL(M_GET_MEMPOOL key_oplist), MEMPOOL_LINKAGE(M_GET_MEMPOOL_LINKAGE key_oplist))) \
   ,									\
   LIST_DEF(M_C(name, _list_pair), M_C(name, _pair_t),			\
             TUPLE_OPLIST(M_C(name, _pair), M_DEFAULT_OPLIST, key_oplist, value_oplist)) \
  )                                                                     \
  									\
  ARRAY_DEF(M_C(name, _array_list_pair), M_C(name, _list_pair_t),	\
	    LIST_OPLIST(M_C(name, _list_pair), TUPLE_OPLIST(M_C(name, _pair), M_DEFAULT_OPLIST, key_oplist, value_oplist))) \
									\
  DICTI_FUNC_DEF2_P5(name, key_type, key_oplist, value_type, value_oplist, 0, 1, M_C(name, _t), M_C(name, _it_t))


/* Define a set with the key key_type
   It is defined as an array of singly linked list (each list
   representing a bucket).
*/
#define DICTI_SET_DEF_P1(arg) DICTI_SET_DEF_P2 arg

/* Validate the key oplist before going further */
#define DICTI_SET_DEF_P2(name, key_type, key_oplist)                    \
  M_IF_OPLIST(key_oplist)(DICTI_SET_DEF_P4, DICTI_SET_DEF_FAILURE)(name, key_type, key_oplist)

/* Stop processing with a compilation failure */
#define DICTI_SET_DEF_FAILURE(name, key_type, key_oplist)\
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(DICT_SET_DEF): the given argument is not a valid oplist: " M_AS_STR(key_oplist) )

#define DICTI_SET_DEF_P4(name, key_type, key_oplist)                    \
									\
  M_CHECK_COMPATIBLE_OPLIST(name, 1, key_type, key_oplist)              \
                                                                        \
  TUPLE_DEF2(M_C(name, _pair), (key, key_type, key_oplist))		\
									\
  M_IF_METHOD(MEMPOOL, key_oplist)					\
  (									\
   LIST_DEF(M_C(name, _list_pair), M_C(name, _pair_t),			\
	    M_OPEXTEND(TUPLE_OPLIST(M_C(name, _pair), key_oplist),	\
		       MEMPOOL(M_GET_MEMPOOL key_oplist), MEMPOOL_LINKAGE(M_GET_MEMPOOL_LINKAGE key_oplist))) \
   ,									\
   LIST_DEF(M_C(name, _list_pair), M_C(name, _pair_t), TUPLE_OPLIST(M_C(name, _pair), key_oplist)) \
  )                                                                     \
									\
  ARRAY_DEF(M_C(name, _array_list_pair), M_C(name, _list_pair_t),	\
	    LIST_OPLIST(M_C(name, _list_pair), TUPLE_OPLIST(M_C(name, _pair), key_oplist))) \
                                                                        \
  DICTI_FUNC_DEF2_P5(name, key_type, key_oplist, key_type,                 \
                  M_EMPTY_OPLIST, 1, 0, M_C(name, _t), M_C(name, _it_t))


/* Define the structure of a chained dictionnary */
#define DICTI_FUNC_DEF2_P5(name, key_type, key_oplist, value_type, value_oplist, isSet, isStoreHash, dict_t, dict_it_t) \
                                                                        \
  /* NOTE:                                                              \
     if isSet is true, all methods of value_oplist are NOP methods */   \
                                                                        \
  typedef struct M_C(name, _s) {					\
    size_t used, lower_limit, upper_limit;                              \
    M_C(name, _array_list_pair_t) table;				\
  } dict_t[1];                                                          \
                                                                        \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
                                                                        \
  M_IF(isSet)(                                                          \
              typedef key_type M_C(name, _type_t);                      \
              ,                                                         \
              typedef struct M_C(name, _pair_s) M_C(name, _type_t);     \
                                                                        ) \
  typedef key_type M_C(name, _key_type_t);                              \
  typedef value_type M_C(name, _value_type_t);                          \
                                                                        \
  typedef struct M_C(name, _it_s) {					\
    M_C(name, _array_list_pair_it_t) array_it;				\
    M_C(name, _list_pair_it_t) list_it;					\
  } dict_it_t[1];							\
                                                                        \
  static inline void                                                    \
  M_C(name, _init)(dict_t map)						\
  {                                                                     \
    assert (map != NULL);						\
    map->used = 0;                                                      \
    M_C(name, _array_list_pair_init)(map->table);			\
    M_C(name, _array_list_pair_resize)(map->table, DICTI_INITIAL_SIZE); \
    map->lower_limit = DICTI_LOWER_BOUND(DICTI_INITIAL_SIZE);           \
    map->upper_limit = DICTI_UPPER_BOUND(DICTI_INITIAL_SIZE);           \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init_set)(dict_t map, const dict_t org)			\
  {                                                                     \
    DICTI_CONTRACT(name, org);                                          \
    assert (map != org);                                                \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    M_C(name, _array_list_pair_init_set)(map->table, org->table);	\
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _set)(dict_t map, const dict_t org)				\
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    DICTI_CONTRACT(name, org);                                          \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    M_C(name, _array_list_pair_set)(map->table, org->table);		\
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name,_clear)(dict_t map)						\
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    M_C(name, _array_list_pair_clear)(map->table);			\
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init_move)(dict_t map, dict_t org)				\
  {                                                                     \
    DICTI_CONTRACT(name, org);                                          \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    M_C(name, _array_list_pair_init_move)(map->table, org->table);	\
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _swap)(dict_t d1, dict_t d2)				\
  {                                                                     \
    DICTI_CONTRACT(name, d1);                                           \
    DICTI_CONTRACT(name, d2);                                           \
    M_SWAP (size_t, d1->used, d2->used);                                \
    M_SWAP (size_t, d1->lower_limit, d2->lower_limit);                  \
    M_SWAP (size_t, d1->upper_limit, d2->upper_limit);                  \
    M_C(name, _array_list_pair_swap)(d1->table, d2->table);		\
    DICTI_CONTRACT(name, d1);                                           \
    DICTI_CONTRACT(name, d2);                                           \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _move)(dict_t map, dict_t org)				\
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    DICTI_CONTRACT(name, org);                                          \
    assert (map != org);                                                \
    M_C(name,_clear)(map);						\
    M_C(name,_init_move)(map, org);					\
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name,_clean)(dict_t map)						\
  {                                                                     \
    M_C(name, _array_list_pair_clean)(map->table);			\
    M_C(name, _array_list_pair_resize)(map->table, DICTI_INITIAL_SIZE); \
    map->lower_limit = DICTI_LOWER_BOUND(DICTI_INITIAL_SIZE);           \
    map->upper_limit = DICTI_UPPER_BOUND(DICTI_INITIAL_SIZE);           \
    map->used = 0;                                                      \
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name,_empty_p)(const dict_t map)					\
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    return map->used == 0;                                              \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name,_size)(const dict_t map)					\
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    return map->used;                                                   \
  }                                                                     \
                                                                        \
  static inline value_type *                                            \
  M_C(name, _get)(const dict_t map, key_type const key)			\
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    size_t hash = M_CALL_HASH(key_oplist, key);                         \
    size_t i = hash & (M_C(name, _array_list_pair_size)(map->table) - 1); \
    const M_C(name, _list_pair_t) *list_ptr =				\
      M_C(name, _array_list_pair_cget)(map->table, i);                  \
    M_C(name, _list_pair_it_t) it;					\
    for(M_C(name, _list_pair_it)(it, *list_ptr);			\
        !M_C(name, _list_pair_end_p)(it);				\
        M_C(name, _list_pair_next)(it)) {				\
      M_C(name, _pair_t) *ref = M_C(name, _list_pair_ref)(it);		\
      M_IF(isStoreHash)(if ((*ref)->hash != hash) { continue; }, )      \
      if (M_CALL_EQUAL(key_oplist, (*ref)->key, key))                   \
        return &(*ref)->M_IF(isSet)(key, value);                        \
    }                                                                   \
    return NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline value_type const *                                      \
  M_C(name, _cget)(const dict_t map, key_type const key)                \
  {                                                                     \
    return M_CONST_CAST(value_type, M_C(name,_get)(map,key));           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _int_resize_up)(dict_t map)					\
  {                                                                     \
    /* NOTE: Contract may not be fullfilled here */                     \
    size_t old_size = M_C(name, _array_list_pair_size)(map->table);	\
    size_t new_size = old_size * 2;                                     \
    if (M_UNLIKELY (new_size <= old_size)) {				\
      M_MEMORY_FULL((size_t)-1);					\
    }									\
    /* Resize the table of the dictionnary */				\
    M_C(name, _array_list_pair_resize)(map->table, new_size);		\
    /* Move the items to the new upper part */                          \
    for(size_t i = 0; i < old_size; i++) {                              \
      M_C(name, _list_pair_t) *list =					\
        M_C(name, _array_list_pair_get)(map->table, i);			\
      if (M_C(name, _list_pair_empty_p)(*list))				\
        continue;                                                       \
      /* We need to scan each item and recompute its hash to know       \
         if it remains inplace or shall be moved to the upper part.*/   \
      M_C(name, _list_pair_it_t) it;					\
      M_C(name, _list_pair_it)(it, *list);				\
      while (!M_C(name, _list_pair_end_p)(it)) {			\
        M_C(name, _pair_ptr) pair = *M_C(name, _list_pair_ref)(it);	\
        size_t hash = M_IF(isStoreHash)(pair->hash, M_CALL_HASH(key_oplist, pair->key)); \
        if ((hash & (new_size-1)) >= old_size) {                        \
          assert( (hash & (new_size-1)) == (i + old_size));             \
          M_C(name, _list_pair_t) *new_list =				\
            M_C(name, _array_list_pair_get)(map->table, i + old_size);	\
          M_C(name, _list_pair_splice_back)(*new_list, *list, it);	\
          /* Splice_back has updated the iterator to the next one */    \
        } else {                                                        \
          M_C(name, _list_pair_next)(it);				\
        }                                                               \
      }                                                                 \
    }                                                                   \
    map->upper_limit = DICTI_UPPER_BOUND(new_size);                     \
    map->lower_limit = DICTI_LOWER_BOUND(new_size);                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _int_resize_down)(dict_t map)				\
  {                                                                     \
    /* NOTE: Contract may not be fullfilled here */                     \
    size_t old_size = M_C(name, _array_list_pair_size)(map->table);	\
    assert ((old_size % 2) == 0);                                       \
    size_t new_size = old_size / 2;                                     \
    assert (new_size >= DICTI_INITIAL_SIZE);                            \
    /* Move all items from the upper part to the lower part of the table */ \
    /* NOTE: We don't need to recompute the hash to move them! */	\
    for(size_t i = new_size; i < old_size; i++) {                       \
      M_C(name, _list_pair_t) *list =					\
        M_C(name, _array_list_pair_get)(map->table, i);			\
      if (M_C(name, _list_pair_empty_p)(*list))				\
        continue;                                                       \
      M_C(name, _list_pair_t) *new_list =				\
	M_C(name, _array_list_pair_get)(map->table, i - new_size);	\
      M_C(name, _list_pair_splice)(*new_list, *list);			\
    }                                                                   \
    /* Resize the table of the dictionary */                            \
    M_C(name, _array_list_pair_resize)(map->table, new_size);		\
    map->upper_limit = DICTI_UPPER_BOUND(new_size);                     \
    map->lower_limit = DICTI_LOWER_BOUND(new_size);                     \
  }                                                                     \
  									\
  static inline void                                                    \
  M_IF(isSet)(M_C(name, _push), M_C(name, _set_at))                     \
       (dict_t map, key_type const key                                  \
        M_IF(isSet)(, M_DEFERRED_COMMA value_type const value))         \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
									\
    size_t hash = M_CALL_HASH(key_oplist, key);                         \
    size_t i = hash & (M_C(name, _array_list_pair_size)(map->table) - 1); \
    M_C(name, _list_pair_t) *list_ptr =					\
      M_C(name, _array_list_pair_get)(map->table, i);                   \
    M_C(name, _list_pair_it_t) it;					\
    for(M_C(name, _list_pair_it)(it, *list_ptr);			\
        !M_C(name, _list_pair_end_p)(it);				\
        M_C(name, _list_pair_next)(it)) {				\
      M_C(name, _pair_ptr) ref = *M_C(name, _list_pair_ref)(it);	\
      M_IF(isStoreHash)(if (ref->hash != hash) continue;, )             \
      if (M_CALL_EQUAL(key_oplist,ref->key, key)) {                     \
        M_CALL_SET(value_oplist, ref->value, value);                    \
        return;                                                         \
      }                                                                 \
    }                                                                   \
    M_C(name, _pair_init_set2)(*M_C(name, _list_pair_push_raw)(*list_ptr), \
                               M_IF(isStoreHash)(hash M_DEFERRED_COMMA,) \
			       key                                      \
                               M_IF(isSet)(, M_DEFERRED_COMMA value));  \
    map->used ++;                                                       \
    if (M_UNLIKELY (map->used > map->upper_limit) )                     \
      M_C(name,_int_resize_up)(map);					\
    DICTI_CONTRACT(name, map);                                          \
  }                                                                     \
  									\
  static inline value_type *                                            \
  M_C(name, _get_at)(dict_t map, key_type const key)                    \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
									\
    size_t hash = M_CALL_HASH(key_oplist, key);                         \
    size_t i = hash & (M_C(name, _array_list_pair_size)(map->table) - 1); \
    M_C(name, _list_pair_t) *list_ptr =					\
      M_C(name, _array_list_pair_get)(map->table, i);                   \
    M_C(name, _list_pair_it_t) it;					\
    for(M_C(name, _list_pair_it)(it, *list_ptr);			\
        !M_C(name, _list_pair_end_p)(it);				\
        M_C(name, _list_pair_next)(it)) {				\
      M_C(name, _pair_ptr) ref = *M_C(name, _list_pair_ref)(it);	\
      M_IF(isStoreHash)(if (ref->hash != hash) continue;, )             \
      if (M_CALL_EQUAL(key_oplist, ref->key, key)) {                    \
        return &ref->M_IF(isSet)(key, value);                           \
      }                                                                 \
    }                                                                   \
    M_C(name, _pair_t) *ref = M_C(name, _list_pair_push_new)(*list_ptr);\
    M_IF(isStoreHash)(M_C(name, _pair_set_hash)(*ref, hash);,)          \
    M_C(name, _pair_set_key)(*ref, key);                                \
    map->used ++;                                                       \
    if (M_UNLIKELY (map->used > map->upper_limit) ) {                   \
      M_C(name,_int_resize_up)(map);					\
      /* Even if the array is being resized, the pointer 'ref'          \
         shall still point to the same item in the bucket (it may still \
         be in a different bucket) */                                   \
    }                                                                   \
    DICTI_CONTRACT(name, map);                                          \
    return &(*ref)->M_IF(isSet)(key, value);                            \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name, _erase)(dict_t map, key_type const key)			\
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
                                                                        \
    bool ret = false;                                                   \
    size_t hash = M_CALL_HASH(key_oplist, key);                         \
    size_t i = hash & (M_C(name, _array_list_pair_size)(map->table) - 1); \
    M_C(name, _list_pair_t) *list_ptr =					\
      M_C(name, _array_list_pair_get)(map->table, i);                   \
    M_C(name, _list_pair_it_t) it;					\
    for(M_C(name, _list_pair_it)(it, *list_ptr);			\
        !M_C(name, _list_pair_end_p)(it);				\
        M_C(name, _list_pair_next)(it)) {				\
      M_C(name, _pair_ptr) ref = *M_C(name, _list_pair_ref)(it);	\
      M_IF(isStoreHash)(if (ref->hash != hash) continue;, )             \
      if (M_CALL_EQUAL(key_oplist, ref->key, key)) {                    \
        M_C(name, _list_pair_remove)(*list_ptr, it);			\
        map->used --;                                                   \
        ret = true;                                                     \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    if (M_UNLIKELY (map->used < map->lower_limit) )                     \
      M_C(name, _int_resize_down)(map);					\
    return ret;                                                         \
  }                                                                     \
                                                                        \
  static inline void							\
  M_C(name, _it)(dict_it_t it, const dict_t d)				\
  {									\
    DICTI_CONTRACT(name, d);						\
    M_C(name, _array_list_pair_it)(it->array_it, d->table);		\
    M_C(name, _list_pair_t) *ref =					\
      M_C(name, _array_list_pair_ref)(it->array_it);			\
    M_C(name, _list_pair_it)(it->list_it, *ref);			\
    while (M_C(name, _list_pair_end_p)(it->list_it)) {			\
      M_C(name, _array_list_pair_next)(it->array_it);			\
      if (M_UNLIKELY (M_C(name, _array_list_pair_end_p)(it->array_it)))	\
	break;								\
      ref = M_C(name, _array_list_pair_ref)(it->array_it);		\
      M_C(name, _list_pair_it)(it->list_it, *ref);			\
    }									\
  }									\
  									\
  static inline void							\
  M_C(name, _it_set)(dict_it_t it, const dict_it_t ref)			\
  {									\
    assert (it != NULL && ref != NULL);					\
    M_C(name, _array_list_pair_it_set)(it->array_it,			\
				       ref->array_it);			\
    M_C(name, _list_pair_it_set)(it->list_it, ref->list_it);		\
  }									\
  									\
  static inline void							\
  M_C(name, _it_end)(dict_it_t it, const dict_t d)			\
  {									\
    DICTI_CONTRACT(name, d);						\
    M_C(name, _array_list_pair_it_end)(it->array_it, d->table);		\
  }									\
  									\
  static inline bool							\
  M_C(name, _end_p)(const dict_it_t it)					\
  {									\
    assert (it != NULL);						\
    return M_C(name, _list_pair_end_p)(it->list_it);			\
  }									\
  									\
  static inline void							\
  M_C(name, _next)(dict_it_t it)					\
  {									\
    assert(it != NULL);							\
    M_C(name, _list_pair_next)(it->list_it);				\
    M_C(name, _list_pair_t) *ref;					\
    while (M_C(name, _list_pair_end_p)(it->list_it)) {			\
      M_C(name, _array_list_pair_next)(it->array_it);			\
      if (M_C(name, _array_list_pair_end_p)(it->array_it))		\
	break;								\
      ref = M_C(name, _array_list_pair_ref)(it->array_it);		\
      M_C(name, _list_pair_it)(it->list_it, *ref);			\
    }									\
  }									\
  									\
  static inline bool							\
  M_C(name, _last_p)(const dict_it_t it)				\
  {									\
    assert (it != NULL);						\
    dict_it_t it2;							\
    M_C(name,_it_set)(it2, it);						\
    M_C(name, _next)(it2);						\
    return M_C(name, _end_p)(it2);					\
  }									\
  									\
  static inline bool							\
  M_C(name, _it_equal_p)(const dict_it_t it1, const dict_it_t it2)	\
  {									\
    assert (it1 != NULL && it2 != NULL);				\
    return M_C(name, _list_pair_it_equal_p)(it1->list_it,		\
                                            it2->list_it);		\
  }									\
  									\
  static inline M_C(name, _type_t) *                                    \
  M_C(name, _ref)(const dict_it_t it)				        \
  {									\
    assert(it != NULL);							\
    /* NOTE: partially unsafe if the user modify the 'key'              \
       in a non equivalent way */                                       \
    M_IF(isSet)(                                                        \
                return &(*M_C(name, _list_pair_ref)(it->list_it))->key; \
                ,                                                       \
                return *M_C(name, _list_pair_ref)(it->list_it);         \
                                                                        ) \
  }									\
  									\
  static inline const M_C(name, _type_t) *                              \
  M_C(name, _cref)(const dict_it_t it)                                  \
  {									\
    assert(it != NULL);							\
    M_IF(isSet)(                                                        \
                return &(*M_C(name, _list_pair_cref)(it->list_it))->key; \
                ,                                                       \
                return *M_C(name, _list_pair_cref)(it->list_it);        \
                                                                        ) \
  }									\
                                                                        \
  M_IF_METHOD(EQUAL, value_oplist)(					\
  static inline bool                                                    \
  M_C(name, _equal_p)(const dict_t dict1, const dict_t dict2)           \
  {									\
    assert (dict1 != NULL && dict2 != NULL);				\
    /* NOTE: Key type has mandatory equal operator */			\
    /* First the easy cases */                                          \
    if (M_LIKELY (dict1->used != dict2->used))                          \
      return false;                                                     \
    if (M_UNLIKELY (dict1->used == 0))                                  \
      return true;                                                      \
    /* Otherwise this is the slow path :                                \
       both dictionary may not have arrays with the same size, but      \
       still the dictionnaries shall be equal as they contain the same  \
       items. */                                                        \
    dict_it_t it;                                                       \
    for(M_C(name, _it)(it, dict1) ;                                     \
        !M_C(name, _end_p)(it);                                         \
        M_C(name, _next)(it)) {                                         \
      const M_C(name, _type_t) *item = M_C(name, _cref)(it);            \
      value_type *ptr = M_C(name, _get)(dict2, M_IF(isSet)(*item, item->key)); \
      if (ptr == NULL)                                                  \
        return false;                                                   \
      if (M_CALL_EQUAL(value_oplist, item->value, *ptr) == false)       \
        return false;                                                   \
    }                                                                   \
    return true;                                                        \
  }									\
  , /* no value equal */ )						\
                                                                        \
  DICTI_FUNC_ADDITIONAL_DEF2(name, key_type, key_oplist, value_type, value_oplist, isSet, dict_t, dict_it_t)


/* Define additional functions for dictionnary (Common for all kinds of dictionnary).
   Do not used any fields of the dictionnary but the public API */
#define DICTI_FUNC_ADDITIONAL_DEF2(name, key_type, key_oplist, value_type, value_oplist, isSet, dict_t, dict_it_t) \
  									\
  M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(			\
  static inline void                                                    \
  M_C(name, _get_str)(string_t str, const dict_t dict, const bool append) \
  {                                                                     \
    STRINGI_CONTRACT (str);                                             \
    (append ? string_cat_str : string_set_str) (str, "{");              \
    dict_it_t it;                                                       \
    bool print_comma = false;                                           \
    for (M_C(name, _it)(it, dict) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      if (print_comma)                                                  \
        string_push_back (str, ',');                                    \
      const M_C(name, _type_t) *item = M_C(name, _cref)(it);            \
      M_IF(isSet)(                                                      \
                  M_CALL_GET_STR(key_oplist, str, *item, true);         \
                  ,                                                     \
                  M_CALL_GET_STR(key_oplist, str, item->key, true);     \
                  string_push_back (str, ':');                          \
                  M_CALL_GET_STR(value_oplist, str, item->value, true); \
                  )                                                     \
      print_comma = true;                                               \
    }                                                                   \
    string_push_back (str, '}');                                        \
    STRINGI_CONTRACT(str);                                              \
  }                                                                     \
  , /* no GET_STR */ )							\
                                                                        \
  M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(                  \
  static inline void                                                    \
  M_C(name, _out_str)(FILE *file, const dict_t dict)			\
  {                                                                     \
    assert (file != NULL);                                              \
    fputc ('{', file);                                                  \
    dict_it_t it;                                                       \
    bool print_comma = false;                                           \
    for (M_C(name, _it)(it, dict) ;					\
         !M_C(name, _end_p)(it);					\
         M_C(name, _next)(it)){						\
      if (print_comma)                                                  \
        fputc (',', file);                                              \
      const M_C(name, _type_t) *item = M_C(name, _cref)(it);            \
      M_IF(isSet)(                                                      \
                  M_CALL_OUT_STR(key_oplist, file, *item);              \
                  ,                                                     \
                  M_CALL_OUT_STR(key_oplist, file, item->key);          \
                  fputc (':', file);                                    \
                  M_CALL_OUT_STR(value_oplist, file, item->value);      \
                  )                                                     \
      print_comma = true;                                               \
    }                                                                   \
    fputc ('}', file);                                                  \
  }                                                                     \
  , /* no OUT_STR */ )							\
 									\
  M_IF_METHOD_BOTH(PARSE_STR, key_oplist, value_oplist)(                \
  static inline bool                                                    \
  M_C(name, _parse_str)(dict_t dict, const char str[], const char **endp) \
  {                                                                     \
    assert (str != NULL);                                               \
    M_C(name, _clean)(dict);                                            \
    bool success = false;                                               \
    int c = *str++;                                                     \
    if (M_UNLIKELY (c != '{')) goto exit;                               \
    c = *str++;                                                         \
    if (M_UNLIKELY (c == '}')) { success = true; goto exit;}            \
    if (M_UNLIKELY (c == 0)) goto exit;                                 \
    str--;                                                              \
    key_type key;                                                       \
    M_IF(isSet)( ,value_type value);                                    \
    M_CALL_INIT(key_oplist, key);                                       \
    M_CALL_INIT(value_oplist, value);                                   \
    do {                                                                \
      while (isspace((int) *str)) str++;                                \
      bool b = M_CALL_PARSE_STR(key_oplist, key, str, &str);            \
      M_IF(isSet)(                                                      \
                  if (b == false) { goto exit_clear; }                  \
                  M_C(name, _push)(dict, key);                          \
                  ,                                                     \
                  do { c = *str++; } while (isspace(c));                \
                  if (b == false || c != ':') { goto exit_clear; }      \
                  b = M_CALL_PARSE_STR(value_oplist, value, str, &str); \
                  if (b == false) { goto exit_clear; }                  \
                  M_C(name, _set_at)(dict, key, value);                 \
                )                                                       \
      do { c = *str++; } while (isspace(c));                            \
    } while (c == ',');                                                 \
    success = (c == '}');                                               \
  exit_clear:                                                           \
    M_CALL_CLEAR(key_oplist, key);                                      \
    M_CALL_CLEAR(value_oplist, value);                                  \
  exit:                                                                 \
    if (endp) *endp = str;                                              \
    return success;                                                     \
  }                                                                     \
  , /* no PARSE_STR */ )                                                \
 									\
  M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(                   \
  static inline bool                                                    \
  M_C(name, _in_str)(dict_t dict, FILE *file)				\
  {                                                                     \
    assert (file != NULL);                                              \
    M_C(name, _clean)(dict);						\
    int c = fgetc(file);                                                \
    if (M_UNLIKELY (c != '{')) return false;                            \
    c = fgetc(file);                                                    \
    if (M_UNLIKELY (c == '}')) return true;                             \
    if (M_UNLIKELY (c == EOF)) return true;                             \
    ungetc(c, file);                                                    \
    key_type key;                                                       \
    M_CALL_INIT(key_oplist, key);                                       \
    M_IF(isSet)(,                                                       \
                value_type value;                                       \
                M_CALL_INIT(value_oplist, value);                       \
                )                                                       \
    do {                                                                \
      do { c = fgetc(file); } while (isspace(c));                       \
      ungetc(c, file);                                                  \
      bool b = M_CALL_IN_STR(key_oplist, key, file);                    \
      M_IF(isSet)(                                                      \
                  if (b == false) { break; }                            \
                  M_C(name, _push)(dict, key);                          \
                  ,                                                     \
                  do { c = fgetc(file); } while (isspace(c));           \
                  if (b == false || c == EOF) { break; }                \
                  if (c != ':') { c = 0; break; }                       \
                  b = M_CALL_IN_STR(value_oplist, value, file);         \
                  if (b == false) { c = 0; break; }                     \
                  M_C(name, _set_at)(dict, key, value);                 \
                                                                        ) \
      do { c = fgetc(file); } while (isspace(c));                       \
    } while (c == ',');							\
    M_CALL_CLEAR(key_oplist, key);                                      \
    M_IF(isSet)(,                                                       \
                M_CALL_CLEAR(value_oplist, value);                      \
                )                                                       \
    return c == '}';                                                    \
  }                                                                     \
  , /* no IN_STR */ )							\
                                                                        \
  M_IF_METHOD_BOTH(OUT_SERIAL, key_oplist, value_oplist)(               \
  static inline m_serial_return_code_t                                  \
  M_C(name, _out_serial)(m_serial_write_t f, dict_t const t1)           \
  {                                                                     \
    assert (f != NULL && f->m_interface != NULL);                         \
    m_serial_local_t local;                                             \
    m_serial_return_code_t ret;                                         \
    const M_C(name, _type_t) *item;                                     \
    bool first_done = false;                                            \
    dict_it_t it;                                                       \
    /* Format is different between associative container                \
       & set container */                                               \
    M_IF(isSet)(							\
                ret = f->m_interface->write_array_start(local, f, M_C(name, _size)(t1)); \
                for (M_C(name, _it)(it, t1) ;                           \
                     !M_C(name, _end_p)(it);                            \
                     M_C(name, _next)(it)){                             \
                  item = M_C(name, _cref)(it);                          \
                  if (first_done)                                       \
                    ret |= f->m_interface->write_array_next(local, f);    \
                  ret |= M_CALL_OUT_SERIAL(key_oplist, f, *item);       \
                  first_done = true;                                    \
                }                                                       \
                ret |= f->m_interface->write_array_end(local, f);         \
                ,                                                       \
                ret = f->m_interface->write_map_start(local, f, M_C(name, _size)(t1)); \
                for (M_C(name, _it)(it, t1) ;                           \
                     !M_C(name, _end_p)(it);                            \
                     M_C(name, _next)(it)){                             \
                  item = M_C(name, _cref)(it);                          \
                  if (first_done)                                       \
                    ret |= f->m_interface->write_map_next(local, f);      \
                  ret |= M_CALL_OUT_SERIAL(key_oplist, f, item->key);   \
                  ret |= f->m_interface->write_map_value(local, f);       \
                  ret |= M_CALL_OUT_SERIAL(value_oplist, f, item->value); \
                  first_done = true;                                    \
                }                                                       \
                ret |= f->m_interface->write_map_end(local, f);           \
                                                                        ) \
      return ret & M_SERIAL_FAIL;                                       \
  }                                                                     \
  , /* no OUT_SERIAL */ )                                               \
                                                                        \
  M_IF_METHOD_BOTH(IN_SERIAL, key_oplist, value_oplist)(                \
  static inline m_serial_return_code_t                                  \
  M_C(name, _in_serial)(dict_t t1, m_serial_read_t f)                   \
  {                                                                     \
    assert (f != NULL && f->m_interface != NULL);                         \
    m_serial_local_t local;                                             \
    m_serial_return_code_t ret;                                         \
    size_t estimated_size = 0;                                          \
    key_type key;                                                       \
    M_C(name,_clean)(t1);						\
    M_IF(isSet)(                                                        \
                ret = f->m_interface->read_array_start(local, f, &estimated_size); \
                if (M_UNLIKELY (ret != M_SERIAL_OK_CONTINUE)) return ret; \
                M_CALL_INIT(key_oplist, key);                           \
                do {                                                    \
                  ret = M_CALL_IN_SERIAL(key_oplist, key, f);           \
                  if (ret != M_SERIAL_OK_DONE) { break; }               \
                  M_C(name, _push)(t1, key);                            \
                } while ((ret = f->m_interface->read_array_next(local, f)) == M_SERIAL_OK_CONTINUE); \
                M_CALL_CLEAR(key_oplist, key);                          \
                ,                                                       \
                value_type value;                                       \
		ret = f->m_interface->read_map_start(local, f, &estimated_size); \
                if (M_UNLIKELY (ret != M_SERIAL_OK_CONTINUE)) return ret; \
                M_CALL_INIT(key_oplist, key);                           \
                M_CALL_INIT (value_oplist, value);			\
		do {                                                    \
                  ret = M_CALL_IN_SERIAL(key_oplist, key, f);           \
                  if (ret != M_SERIAL_OK_DONE)     return M_SERIAL_FAIL; \
                  ret = f->m_interface->read_map_value(local, f);         \
                  if (ret != M_SERIAL_OK_CONTINUE) return M_SERIAL_FAIL; \
                  ret = M_CALL_IN_SERIAL(value_oplist, value, f);       \
                  if (ret != M_SERIAL_OK_DONE)     return M_SERIAL_FAIL; \
                  M_C(name, _set_at)(t1, key, value);			\
                } while ((ret = f->m_interface->read_map_next(local, f)) == M_SERIAL_OK_CONTINUE); \
                M_CALL_CLEAR(key_oplist, key);                          \
                M_CALL_CLEAR(value_oplist, value);                      \
                ) /* End of IF isSet */                                 \
      return ret;                                                       \
  }                                                                     \
  , /* no in_serial */ )                                                \
									\
  M_IF(isSet)(                                                          \
  static inline void	                                                \
  M_C(name, _splice)(dict_t d1, dict_t d2)                              \
  {									\
    dict_it_t it;                                                       \
    /* NOTE: Despite using set_at, the accessing of the item in d1	\
       is not as random as other uses of the HASH table as d2		\
       uses the same order than d1 */					\
    for (M_C(name, _it)(it, d2); !M_C(name, _end_p)(it); M_C(name, _next)(it)){	\
      const M_C(name, _type_t) *item = M_C(name, _cref)(it);            \
      M_C(name, _push)(d1, *item);                                      \
    }									\
    M_C(name, _clean)(d2);						\
  }									\
  ,                                                                     \
  M_IF_METHOD(UPDATE, value_oplist)(                                    \
  static inline void	                                                \
  M_C(name, _splice)(dict_t d1, dict_t d2)                              \
  {									\
    dict_it_t it;                                                       \
    /* NOTE: Despite using set_at, the accessing of the item in d1	\
       is not as random as other uses of the HASH table as d2		\
       uses the same order than d1 */					\
    for (M_C(name, _it)(it, d2); !M_C(name, _end_p)(it); M_C(name, _next)(it)){	\
      const struct M_C(name, _pair_s) *item = M_C(name, _cref)(it);	\
      value_type *ptr = M_C(name, _get)(d1, item->key);			\
      if (ptr == NULL) {						\
	M_C(name, _set_at)(d1, item->key, item->value);			\
      } else {								\
	M_CALL_UPDATE(value_oplist, *ptr, item->value);                 \
      }									\
    }									\
    M_C(name, _clean)(d2);						\
  }									\
  , /* NO UPDATE */) )                                                  \
                                                                        \
  /* HASH method for dictionnary itself seems hard to implement:        \
     we have to handle the case where two dictionnaries are structuraly \
     different, but functionnaly identical (seems they have the same    \
     members, but put in a different order).                            \
     We cannot iterator over the dictionary to compute a hash, as the   \
     order of the items in the dictionnary is not specified: they more  \
     or less follow the hash of the keys, but if the low bits of the    \
     hash of the key is equal, they order may be different.             \
     Or if the table of the dictionnary has different values (this may  \
     be avoided). */



/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define DICTI_OPLIST_P1(arg) DICTI_OPLIST_P2 arg

/* Validation of the given oplists */
#define DICTI_OPLIST_P2(name, key_oplist, value_oplist)		\
  M_IF_OPLIST(key_oplist)(DICTI_OPLIST_P3, DICTI_OPLIST_FAILURE)(name, key_oplist, value_oplist)
#define DICTI_OPLIST_P3(name, key_oplist, value_oplist)		\
  M_IF_OPLIST(value_oplist)(DICTI_OPLIST_P4, DICTI_OPLIST_FAILURE)(name, key_oplist, value_oplist)

/* Prepare a clean compilation failure */
#define DICTI_OPLIST_FAILURE(name, key_oplist, value_oplist)	\
  ((M_LIB_ERROR(ARGUMENT_OF_DICT_OPLIST_IS_NOT_AN_OPLIST, name, key_oplist, value_oplist)))

/* Define the oplist of a dictionnary
   NOTE: IT_REF is not exported so that the contained appears as not modifiable
   by algorithm.*/
#define DICTI_OPLIST_P4(name, key_oplist, value_oplist)			\
  (INIT(M_C(name, _init)),						\
   INIT_SET(M_C(name, _init_set)),					\
   INIT_WITH(API_1(M_INIT_KEY_VAI)),                                    \
   SET(M_C(name, _set)),						\
   CLEAR(M_C(name, _clear)),						\
   INIT_MOVE(M_C(name, _init_move)),					\
   MOVE(M_C(name, _move)),						\
   SWAP(M_C(name, _swap)),						\
   CLEAN(M_C(name, _clean)),                                            \
   TYPE(M_C(name, _t)),							\
   SUBTYPE(M_C(name, _type_t)),                                         \
   TEST_EMPTY(M_C(name,_empty_p)),                                      \
   IT_TYPE(M_C(name, _it_t)),						\
   IT_FIRST(M_C(name,_it)),						\
   IT_SET(M_C(name, _it_set)),						\
   IT_END(M_C(name,_it_end)),						\
   IT_END_P(M_C(name,_end_p)),						\
   IT_LAST_P(M_C(name,_last_p)),					\
   IT_NEXT(M_C(name,_next)),						\
   IT_CREF(M_C(name,_cref))						\
   ,KEY_TYPE(M_C(name, _key_type_t))                                    \
   ,VALUE_TYPE(M_C(name, _value_type_t))                                \
   ,SET_KEY(M_C(name, _set_at))                                         \
   ,GET_KEY(M_C(name, _get))                                            \
   ,GET_SET_KEY(M_C(name, _get_at))                                     \
   ,ERASE_KEY(M_C(name, _erase))                                        \
   ,KEY_OPLIST(key_oplist)                                              \
   ,VALUE_OPLIST(value_oplist)                                          \
   ,GET_SIZE(M_C(name, _size))                                          \
   ,M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(GET_STR(M_C(name, _get_str)),) \
   ,M_IF_METHOD_BOTH(PARSE_STR, key_oplist, value_oplist)(PARSE_STR(M_C(name, _parse_str)),) \
   ,M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(OUT_STR(M_C(name, _out_str)),) \
   ,M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(IN_STR(M_C(name, _in_str)),) \
   ,M_IF_METHOD_BOTH(OUT_SERIAL, key_oplist, value_oplist)(OUT_SERIAL(M_C(name, _out_serial)),) \
   ,M_IF_METHOD_BOTH(IN_SERIAL, key_oplist, value_oplist)(IN_SERIAL(M_C(name, _in_serial)),) \
   ,M_IF_METHOD(EQUAL, value_oplist)(EQUAL(M_C(name, _equal_p)),)	\
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW key_oplist),)                \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC key_oplist),)    \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL key_oplist),)                \
   )

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define DICTI_SET_OPLIST_P1(arg) DICTI_SET_OPLIST_P2 arg

/* Validation of the given oplist */
#define DICTI_SET_OPLIST_P2(name, oplist)					\
  M_IF_OPLIST(oplist)(DICTI_SET_OPLIST_P3, DICTI_SET_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define DICTI_SET_OPLIST_FAILURE(name, oplist)				\
  ((M_LIB_ERROR(ARGUMENT_OF_DICT_SET_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* Define the oplist of a set */
#define DICTI_SET_OPLIST_P3(name, oplist)                               \
  (INIT(M_C(name, _init)),						\
   INIT_SET(M_C(name, _init_set)),					\
   INIT_WITH(API_1(M_INIT_VAI)),                                        \
   SET(M_C(name, _set)),						\
   CLEAR(M_C(name, _clear)),						\
   INIT_MOVE(M_C(name, _init_move)),					\
   MOVE(M_C(name, _move)),						\
   SWAP(M_C(name, _swap)),						\
   CLEAN(M_C(name, _clean)),                                            \
   TYPE(M_C(name, _t)),							\
   SUBTYPE(M_C(name, _type_t)),                                         \
   TEST_EMPTY(M_C(name,_empty_p)),                                      \
   PUSH(M_C(name,_push)),						\
   KEY_TYPE(M_C(name, _key_type_t)),                                    \
   VALUE_TYPE(M_C(name, _key_type_t)),                                  \
   GET_KEY(M_C(name, _get)),                                            \
   GET_SET_KEY(M_C(name, _get_at)),                                     \
   ERASE_KEY(M_C(name, _erase)),                                        \
   KEY_OPLIST(oplist),                                                  \
   VALUE_OPLIST(oplist),                                                \
   GET_SIZE(M_C(name, _size)),                                          \
   IT_TYPE(M_C(name, _it_t)),						\
   IT_FIRST(M_C(name,_it)),						\
   IT_SET(M_C(name, _it_set)),						\
   IT_END(M_C(name,_it_end)),						\
   IT_END_P(M_C(name,_end_p)),						\
   IT_LAST_P(M_C(name,_last_p)),					\
   IT_NEXT(M_C(name,_next)),						\
   IT_REF(M_C(name,_ref)),						\
   IT_CREF(M_C(name,_cref))						\
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C(name, _get_str)),)         \
   ,M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_C(name, _parse_str)),)   \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C(name, _out_str)),)         \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C(name, _in_str)),)            \
   ,M_IF_METHOD(OUT_SERIAL, oplist)(OUT_SERIAL(M_C(name, _out_serial)),) \
   ,M_IF_METHOD(IN_SERIAL, oplist)(IN_SERIAL(M_C(name, _in_serial)),)   \
   ,EQUAL(M_C(name, _equal_p)),                                         \
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                    \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)        \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                    \
   )

/* Define Lower Bound for hash table (TODO: Common macro for both implementation) */
#ifndef DICTI_LOWER_BOUND
#define DICTI_LOWER_BOUND(s) ((s) <= DICTI_INITIAL_SIZE ? 0 : (s) * 1 / 4)
#endif

/* Define Lower Bound for hash table (TODO: Common macro for both implementation) */
#ifndef DICTI_UPPER_BOUND
#define DICTI_UPPER_BOUND(s) ((s) * 2 / 3)
#endif

/* Define initial size of the hash table */
#ifndef DICTI_INITIAL_SIZE
#define DICTI_INITIAL_SIZE   16
#endif

#define DICTI_CONTRACT(name, map) do {                                  \
    assert(map != NULL);                                                \
    assert(map->used <= map->upper_limit);                              \
    assert(map->upper_limit >= DICTI_UPPER_BOUND(DICTI_INITIAL_SIZE));  \
    assert(map->used >= map->lower_limit);                              \
    assert(M_POWEROF2_P(M_C(name, _array_list_pair_size)(map->table))); \
  } while (0)


/****************************************************************************************/
/* Open Addressing implementation */
/****************************************************************************************/

typedef enum {
  DICTI_OA_EMPTY = 0, DICTI_OA_DELETED = 1
} dicti_oa_element_t;

/* Performing Quadratic probing
   Replace it by '1' to perform linear probing */
#ifndef DICTI_OA_PROBING
#define DICTI_OA_PROBING(s) ((s)++)
#endif

/* Lower Bound of the hash table (TODO: Common macro for both dictionnary) */
#ifndef DICTI_OA_LOWER_BOUND
#define DICTI_OA_LOWER_BOUND 0.2
#endif
/* Upper Bound of the hash table (TODO: Common macro for both dictionnary)  */
#ifndef DICTI_OA_UPPER_BOUND
#define DICTI_OA_UPPER_BOUND 0.7
#endif

#define DICTI_OA_CONTRACT(dict) do {					\
    assert ( (dict) != NULL);						\
    assert( (dict)->lower_limit <= (dict)->count && (dict)->count <= (dict)->upper_limit ); \
    assert( (dict)->data != NULL);					\
    assert( M_POWEROF2_P((dict)->mask+1));				\
    assert( (dict)->mask+1 >= DICTI_INITIAL_SIZE);			\
    assert( (dict)->count <= (dict)->mask+1);				\
  } while (0)

#define DICTI_OA_DEF_P1(args) DICTI_OA_DEF_P2 args

/* Validate the key oplist before going further */
#define DICTI_OA_DEF_P2(name, key_type, key_oplist, value_type, value_oplist) \
  M_IF_OPLIST(key_oplist)(DICTI_OA_DEF_P3, DICTI_OA_DEF_FAILURE)(name, key_type, key_oplist, value_type, value_oplist)

/* Validate the value oplist before going further */
#define DICTI_OA_DEF_P3(name, key_type, key_oplist, value_type, value_oplist) \
  M_IF_OPLIST(value_oplist)(DICTI_OA_DEF_P4, DICTI_OA_DEF_FAILURE)(name, key_type, key_oplist, value_type, value_oplist)

/* Stop processing with a compilation failure */
#define DICTI_OA_DEF_FAILURE(name, key_type, key_oplist, value_type, value_oplist) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(DICT_OA_DEF2): at least one of the given argument is not a valid oplist: " M_AS_STR(key_oplist) " / " M_AS_STR(value_oplist) )

#define DICTI_OA_DEF_P4(name, key_type, key_oplist, value_type, value_oplist) \
  DICTI_OA_DEF_P5(name, key_type, key_oplist, value_type, value_oplist, 0, \
                  DICTI_OA_LOWER_BOUND, DICTI_OA_UPPER_BOUND, M_C(name,_t), M_C(name, _it_t) )

#define DICTI_OASET_DEF_P1(args) DICTI_OASET_DEF_P2 args

/* Validate the value oplist before going further */
#define DICTI_OASET_DEF_P2(name, key_type, key_oplist) \
  M_IF_OPLIST(key_oplist)(DICTI_OASET_DEF_P4, DICTI_OASET_DEF_FAILURE)(name, key_type, key_oplist)

/* Stop processing with a compilation failure */
#define DICTI_OASET_DEF_FAILURE(name, key_type, key_oplist) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(DICT_OASET_DEF): the given argument is not a valid oplist: " M_AS_STR(key_oplist) )

#define DICTI_OASET_DEF_P4(name, key_type, key_oplist)                  \
  DICTI_OA_DEF_P5(name, key_type, key_oplist, key_type, M_EMPTY_OPLIST, 1, \
                  DICTI_OA_LOWER_BOUND, DICTI_OA_UPPER_BOUND, M_C(name,_t), M_C(name, _it_t) )

#define DICTI_OA_DEF_P5(name, key_type, key_oplist, value_type, value_oplist, isSet, coeff_down, coeff_up, dict_t, dict_it_t) \
  									\
  /* NOTE:                                                              \
     if isSet is true, all methods of value_oplist are NOP methods */   \
                                                                        \
  typedef struct M_C(name, _pair_s) {					\
    key_type   key;                                                     \
    M_IF(isSet)( , value_type value;)                                   \
  } M_C(name, _pair_t);							\
  									\
  M_IF(isSet)(                                                          \
  typedef key_type M_C(name, _type_t);                                  \
  ,                                                                     \
  typedef struct M_C(name, _pair_s) M_C(name, _type_t);              )  \
  typedef key_type M_C(name, _key_type_t);                              \
  typedef value_type M_C(name, _value_type_t);                          \
                                                                        \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, key_type, key_oplist)              \
  M_CHECK_COMPATIBLE_OPLIST(name, 2, value_type, value_oplist)          \
                                                                        \
  /* NOTE: We don't want a real oplist for this sub type */             \
  ARRAY_DEF(M_C(name, _array_pair), M_C(name, _pair_t),			\
            (INIT(M_NOTHING_DEFAULT), SET(M_MEMCPY_DEFAULT),            \
             INIT_SET(M_MEMCPY_DEFAULT), CLEAR(M_NOTHING_DEFAULT)))     \
                                                                        \
  typedef struct M_C(name,_s) {						\
    size_t mask, count, count_delete;                                   \
    size_t upper_limit, lower_limit;                                    \
    M_C(name, _pair_t) *data;						\
  } dict_t[1];                                                          \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
  									\
  typedef struct M_C(name, _it_s) {					\
    const struct M_C(name,_s) *dict;                                    \
    size_t index;                                                       \
  } dict_it_t[1];                                                       \
                                                                        \
  static inline void                                                    \
  M_C(name,_int_limit)(dict_t dict, size_t size)			\
  {                                                                     \
    dict->upper_limit = (size_t) (size * coeff_up) - 1;                 \
    dict->lower_limit = (size <= DICTI_INITIAL_SIZE) ? 0 : (size_t) (size * coeff_down) ; \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init)(dict_t dict)						\
  {                                                                     \
    assert(0 <= (coeff_down) && (coeff_down)*2 < (coeff_up) && (coeff_up) < 1); \
    dict->mask = DICTI_INITIAL_SIZE-1;                                  \
    dict->count = 0;                                                    \
    dict->count_delete = 0;                                             \
    M_C(name,_int_limit)(dict, DICTI_INITIAL_SIZE);			\
    dict->data = M_CALL_REALLOC(key_oplist, M_C(name, _pair_t), NULL, DICTI_INITIAL_SIZE); \
    if (dict->data == NULL) {                                           \
      M_MEMORY_FULL(sizeof (M_C(name, _pair_t)) * DICTI_INITIAL_SIZE);  \
      return ;                                                          \
    }                                                                   \
    for(size_t i = 0; i < DICTI_INITIAL_SIZE; i++) {                    \
      M_CALL_OOR_SET(key_oplist, dict->data[i].key, DICTI_OA_EMPTY);    \
    }                                                                   \
    DICTI_OA_CONTRACT(dict);                                            \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _clear)(dict_t dict)					\
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    for(size_t i = 0; i <= dict->mask; i++) {                           \
      if (!M_CALL_OOR_EQUAL(key_oplist, dict->data[i].key, DICTI_OA_EMPTY) \
          && !M_CALL_OOR_EQUAL(key_oplist, dict->data[i].key, DICTI_OA_DELETED)) { \
        M_CALL_CLEAR(key_oplist, dict->data[i].key);                    \
        M_CALL_CLEAR(value_oplist, dict->data[i].value);                \
      }                                                                 \
    }                                                                   \
    M_CALL_FREE(key_oplist, dict->data);                                \
    /* Not really needed, but safer */                                  \
    dict->mask = 0;                                                     \
    dict->data = NULL;                                                  \
  }                                                                     \
  									\
  static inline value_type *                                            \
  M_C(name, _get)(const dict_t dict, key_type const key)		\
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    /* NOTE: Key can not be the representation of empty or deleted */	\
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_EMPTY));      \
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_DELETED));    \
    									\
    M_C(name, _pair_t) *const data = dict->data;			\
    const size_t mask = dict->mask;                                     \
    size_t p = M_CALL_HASH(key_oplist, key) & mask;                     \
                                                                        \
    /* Random access, and probably cache miss */                        \
    if (M_LIKELY (M_CALL_EQUAL(key_oplist, data[p].key, key)) )         \
      return &data[p].M_IF(isSet)(key, value);                          \
    else if (M_LIKELY (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY)) ) \
      return NULL;                                                      \
    									\
    /* Unlikely case */                                                 \
    size_t s = 1;                                                       \
    do {                                                                \
      p = (p + DICTI_OA_PROBING(s)) & mask;                             \
      if (M_CALL_EQUAL(key_oplist, data[p].key, key))                   \
        return &data[p].M_IF(isSet)(key, value);                        \
      assert (s <= dict->mask);                                         \
    } while (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ); \
    									\
    return NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline value_type const *                                      \
  M_C(name, _cget)(const dict_t map, key_type const key)                \
  {                                                                     \
    return M_CONST_CAST(value_type, M_C(name,_get)(map,key));           \
  }                                                                     \
                                                                        \
  M_IF_DEBUG(                                                           \
  static inline bool                                                    \
  M_C(name,_int_control_after_resize)(const dict_t h)			\
  {                                                                     \
    /* This function checks if the reshashing of the dict is ok */	\
    M_C(name, _pair_t) *data = h->data;					\
    size_t empty = 0;                                                   \
    size_t del = 0;                                                     \
    for(size_t i = 0 ; i <= h->mask ; i++) {                            \
      empty += M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_EMPTY); \
      del   += M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_DELETED); \
    }                                                                   \
    assert(del == 0);                                                   \
    assert(empty + h->count == h->mask + 1);                            \
    return true;                                                        \
  }                                                                     \
  )                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name,_int_resize_up)(dict_t h, size_t newSize, bool updateLimit)	\
  {                                                                     \
    size_t oldSize = h->mask+1;                                         \
    assert (newSize >= oldSize);                                        \
    assert (M_POWEROF2_P(newSize));                                     \
    M_C(name, _pair_t) *data = h->data;					\
    /* resize can be called just to delete the items */			\
    if (newSize > oldSize) {                                            \
      data = M_CALL_REALLOC(key_oplist, M_C(name, _pair_t), data, newSize); \
      if (M_UNLIKELY (data == NULL) ) {                                 \
        M_MEMORY_FULL(sizeof (M_C(name, _pair_t)) * newSize);		\
        return ;                                                        \
      }                                                                 \
                                                                        \
      /* First mark the extended space as empty */                      \
      for(size_t i = oldSize ; i < newSize; i++)                        \
        M_CALL_OOR_SET(key_oplist, data[i].key, DICTI_OA_EMPTY);        \
    }                                                                   \
                                                                        \
    /* Then let's rehash all the entries in their **exact** position.   \
       If we can't, let's put them in the 'tmp' array.                  \
       It has been measured that the size of this 'tmp' array is	\
       around 6% of the size of updated dictionnary.			\
       NOTE: This should be much cache friendly than typical hash code  */ \
    M_C(name, _array_pair_t) tmp;					\
    M_C(name, _array_pair_init)(tmp);					\
    const size_t mask = (newSize -1);                                   \
                                                                        \
    for(size_t i = 0 ; i < oldSize; i++) {                              \
      if (!M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_DELETED) && !M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_EMPTY)) { \
        size_t p = M_CALL_HASH(key_oplist, data[i].key) & mask;         \
        if (p != i) {                                                   \
          if (M_LIKELY (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) || M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_DELETED))) { \
            M_DO_INIT_MOVE(key_oplist, data[p].key, data[i].key);       \
            M_DO_INIT_MOVE(value_oplist, data[p].value, data[i].value); \
          } else {                                                      \
            M_C(name, _pair_t) *ptr = M_C(name, _array_pair_push_raw) (tmp); \
            M_DO_INIT_MOVE(key_oplist, ptr->key, data[i].key);          \
            M_DO_INIT_MOVE(value_oplist, ptr->value, data[i].value);    \
          }                                                             \
          M_CALL_OOR_SET(key_oplist, data[i].key, DICTI_OA_EMPTY);      \
        }                                                               \
      } else {                                                          \
        M_CALL_OOR_SET(key_oplist, data[i].key, DICTI_OA_EMPTY);        \
      }                                                                 \
    }                                                                   \
                                                                        \
    /* Let's put back the entries in the tmp array in their right place */ \
    /* NOTE: There should be very few entries in this array             \
       which contains what we weren't be able to fit in the first pass */ \
    while (M_C(name, _array_pair_size)(tmp) > 0) {                      \
      M_C(name, _pair_t) const *item = M_C(name, _array_pair_back)(tmp); \
      size_t p = M_CALL_HASH(key_oplist, item->key) & mask;             \
      /* NOTE: since the first pass, the bucket might be free now */	\
      if (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY)) { \
        size_t s = 1;                                                   \
        do {                                                            \
          p = (p + DICTI_OA_PROBING(s)) & mask;                         \
          assert (s <= h->mask);                                        \
        } while (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ); \
      }                                                                 \
      M_C(name, _array_pair_pop_move)(&data[p], tmp);                   \
    }                                                                   \
                                                                        \
    M_C(name, _array_pair_clear) (tmp);					\
    h->mask = newSize-1;                                                \
    h->count_delete = h->count;                                         \
    if (updateLimit == true) {						\
      M_C(name,_int_limit)(h, newSize);					\
    }									\
    h->data = data;                                                     \
    M_IF_DEBUG (assert (M_C(name,_int_control_after_resize)(h));)	\
    DICTI_OA_CONTRACT(h);                                               \
  }                                                                     \
  									\
  static inline void                                                    \
  M_IF(isSet)(M_C(name, _push), M_C(name,_set_at))                      \
       (dict_t dict, key_type const key                                 \
        M_IF(isSet)(, M_DEFERRED_COMMA value_type const value) )        \
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    /* NOTE: key can not be the representation of empty or deleted */	\
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_EMPTY));        \
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_DELETED));      \
    									\
    M_C(name, _pair_t) *const data = dict->data;			\
    const size_t mask = dict->mask;                                     \
    size_t p = M_CALL_HASH(key_oplist, key) & mask;                     \
    									\
    /* NOTE: Likely cache miss */                                       \
    if (M_UNLIKELY (M_CALL_EQUAL(key_oplist, data[p].key, key)) ) {     \
      M_CALL_SET(value_oplist, data[p].value, value);                   \
      return;                                                           \
    }                                                                   \
    if (M_UNLIKELY (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ) ) { \
      size_t delPos = SIZE_MAX;                                         \
      if (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_DELETED)) delPos = p; \
      size_t s = 1U;                                                    \
      do {                                                              \
        p = (p + DICTI_OA_PROBING(s)) & mask;                           \
        if (M_CALL_EQUAL(key_oplist, data[p].key, key)) {               \
          M_CALL_SET(value_oplist, data[p].value, value);               \
          return;                                                       \
        }                                                               \
        assert (s <= dict->mask);                                       \
        if (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_DELETED) && delPos == (size_t)-1) delPos = p; \
      } while (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ); \
      if (delPos != SIZE_MAX) {	    				        \
	p = delPos;							\
	dict->count_delete --;						\
      }									\
    }                                                                   \
                                                                        \
    M_CALL_INIT_SET(key_oplist, data[p].key, key);                      \
    M_CALL_INIT_SET(value_oplist, data[p].value, value);                \
    dict->count++;                                                      \
    dict->count_delete ++;                                              \
                                                                        \
    if (M_UNLIKELY (dict->count_delete >= dict->upper_limit)) {         \
      size_t newSize = dict->mask+1;                                    \
      if (dict->count > (dict->mask / 2)) {				\
	newSize += newSize;						\
	if (M_UNLIKELY (newSize <= dict->mask+1)) {			\
	  M_MEMORY_FULL((size_t)-1);					\
	}								\
      }									\
      M_C(name,_int_resize_up)(dict, newSize, true);			\
    }                                                                   \
    DICTI_OA_CONTRACT(dict);                                            \
  }                                                                     \
  									\
  static inline value_type *                                            \
  M_C(name,_get_at)(dict_t dict, key_type const key)            	\
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    /* NOTE: key can not be the representation of empty or deleted */	\
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_EMPTY));        \
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_DELETED));      \
    									\
    M_C(name, _pair_t) *const data = dict->data;			\
    const size_t mask = dict->mask;                                     \
    size_t p = M_CALL_HASH(key_oplist, key) & mask;                     \
    									\
    if (M_CALL_EQUAL(key_oplist, data[p].key, key))  {                  \
      return &data[p].M_IF(isSet)(key, value);                          \
    }                                                                   \
    if (M_UNLIKELY (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ) ) { \
      size_t delPos = SIZE_MAX;                                         \
      if (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_DELETED)) delPos = p; \
      size_t s = 1U;                                                    \
      do {                                                              \
        p = (p + DICTI_OA_PROBING(s)) & mask;                           \
        if (M_CALL_EQUAL(key_oplist, data[p].key, key)) {               \
          return &data[p].M_IF(isSet)(key, value);                      \
        }                                                               \
        assert (s <= dict->mask);                                       \
        if (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_DELETED) && delPos == (size_t)-1) delPos = p; \
      } while (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ); \
      if (delPos != SIZE_MAX) {					        \
	p = delPos;							\
	dict->count_delete --;						\
      }									\
    }                                                                   \
                                                                        \
    M_CALL_INIT_SET(key_oplist, data[p].key, key);                      \
    M_CALL_INIT(value_oplist, data[p].value);                           \
    dict->count++;                                                      \
    dict->count_delete ++;                                              \
                                                                        \
    if (M_UNLIKELY (dict->count_delete >= dict->upper_limit)) {         \
      size_t newSize = dict->mask+1;                                    \
      if (dict->count > (dict->mask / 2)) {				\
	newSize += newSize;						\
	if (M_UNLIKELY (newSize <= dict->mask+1)) {			\
	  M_MEMORY_FULL((size_t)-1);					\
	}								\
      }									\
      M_C(name,_int_resize_up)(dict, newSize, true);			\
      /* data is now invalid */						\
      return M_C(name, _get)(dict, key);				\
    }                                                                   \
    DICTI_OA_CONTRACT(dict);                                            \
    return &data[p].M_IF(isSet)(key, value);                            \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name,_int_resize_down)(dict_t h, size_t newSize)			\
  {                                                                     \
    size_t oldSize = h->mask+1;                                         \
    assert (newSize <= oldSize && M_POWEROF2_P(newSize));               \
    if (M_UNLIKELY (newSize < DICTI_INITIAL_SIZE))                      \
      newSize = DICTI_INITIAL_SIZE;                                     \
    const size_t mask = newSize -1;                                     \
    M_C(name, _pair_t) *data = h->data;					\
    M_C(name, _array_pair_t) tmp;					\
    M_C(name, _array_pair_init)(tmp);					\
    									\
    /* Pass 1: scan lower entries, and move them if needed */           \
    for(size_t i = 0; i < newSize; i++) {                               \
      if (M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_EMPTY))    \
        continue;                                                       \
      if (M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_DELETED)) { \
        M_CALL_OOR_SET(key_oplist, data[i].key, DICTI_OA_EMPTY);        \
        continue;                                                       \
      }                                                                 \
      size_t p = M_CALL_HASH(key_oplist, data[i].key) & mask;           \
      if (p != i) {                                                     \
        if (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) || M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_DELETED)) { \
          M_DO_INIT_MOVE(key_oplist, data[p].key, data[i].key);         \
          M_DO_INIT_MOVE(value_oplist, data[p].value, data[i].value);   \
        } else {                                                        \
          M_C(name, _pair_t) *ptr = M_C(name, _array_pair_push_raw) (tmp); \
          M_DO_INIT_MOVE(key_oplist, ptr->key, data[i].key);            \
          M_DO_INIT_MOVE(value_oplist, ptr->value, data[i].value);      \
        }                                                               \
        M_CALL_OOR_SET(key_oplist, data[i].key, DICTI_OA_EMPTY);        \
      }                                                                 \
    }                                                                   \
    /* Pass 2: scan upper entries and move them back */                 \
    for(size_t i = newSize; i < oldSize; i++) {                         \
      if (!M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_DELETED)  \
          && !M_CALL_OOR_EQUAL(key_oplist, data[i].key, DICTI_OA_EMPTY)) { \
        size_t p = M_CALL_HASH(key_oplist, data[i].key) & mask;         \
        assert (p < i);                                                 \
        if (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY)) { \
          size_t s = 1;                                                 \
          do {                                                          \
            p = (p + DICTI_OA_PROBING(s)) & mask;                       \
            assert (s <= h->mask);                                      \
          } while (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ); \
        }                                                               \
        M_DO_INIT_MOVE(key_oplist, data[p].key, data[i].key);           \
        M_DO_INIT_MOVE(value_oplist, data[p].value, data[i].value);     \
      }                                                                 \
    }                                                                   \
    /* Pass 3: scan moved entries and move them back */                 \
    while (M_C(name, _array_pair_size)(tmp) > 0) {                      \
      M_C(name, _pair_t) const *item = M_C(name, _array_pair_back)(tmp); \
      size_t p = M_CALL_HASH(key_oplist, item->key) & mask;             \
      if (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY)) { \
        size_t s = 1;                                                   \
        do {                                                            \
          p = (p + DICTI_OA_PROBING(s)) & mask;                         \
          assert (s <= h->mask);                                        \
        } while (!M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ); \
      }                                                                 \
      M_C(name, _array_pair_pop_move)(&data[p], tmp);                   \
    }                                                                   \
    									\
    M_C(name, _array_pair_clear) (tmp);					\
    h->count_delete = h->count;                                         \
    if (newSize != oldSize) {                                           \
      h->mask = newSize-1;                                              \
      M_C(name,_int_limit)(h, newSize);					\
      h->data = M_CALL_REALLOC(key_oplist, M_C(name, _pair_t), data, newSize); \
      assert (h->data != NULL);                                         \
    }                                                                   \
    M_IF_DEBUG (assert (M_C(name,_int_control_after_resize)(h));)	\
    assert (h->lower_limit < h->count && h->count < h->upper_limit);    \
    DICTI_OA_CONTRACT(h);                                               \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name,_erase)(dict_t dict, const key_type key)			\
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    /* NOTE: key can't be the representation of empty or deleted */	\
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_EMPTY));        \
    assert (!M_CALL_OOR_EQUAL(key_oplist, key, DICTI_OA_DELETED));      \
                                                                        \
    M_C(name, _pair_t) *const data = dict->data;			\
    const size_t mask = dict->mask;                                     \
    size_t p = M_CALL_HASH(key_oplist, key) & mask;                     \
                                                                        \
    /* Random access, and probably cache miss */                        \
    if (M_UNLIKELY (!M_CALL_EQUAL(key_oplist, data[p].key, key)) ) {    \
      if (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY))    \
        return false;                                                   \
      size_t s = 1;                                                     \
      do {                                                              \
        p = (p + DICTI_OA_PROBING(s)) & mask;                           \
        if (M_CALL_OOR_EQUAL(key_oplist, data[p].key, DICTI_OA_EMPTY) ) \
          return false;                                                 \
        assert (s <= dict->mask);                                       \
      } while (!M_CALL_EQUAL(key_oplist, data[p].key, key));            \
    }                                                                   \
    M_CALL_CLEAR(key_oplist, data[p].key);                              \
    M_CALL_CLEAR(value_oplist, data[p].value);                          \
    M_CALL_OOR_SET(key_oplist, data[p].key, DICTI_OA_DELETED);          \
    assert (dict->count >= 1);                                          \
    dict->count--;                                                      \
    if (M_UNLIKELY (dict->count < dict->lower_limit)) {                 \
      M_C(name,_int_resize_down)(dict, (dict->mask+1) >> 1);		\
    }                                                                   \
    DICTI_OA_CONTRACT(dict);                                            \
    return true;                                                        \
  }                                                                     \
                                                                        \
  static inline bool                                                    \
  M_C(name,_empty_p)(const  dict_t dict)                                \
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    return dict->count == 0;                                            \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  M_C(name,_size)(const  dict_t dict)					\
  {                                                                     \
    DICTI_OA_CONTRACT(dict);                                            \
    return dict->count;                                                 \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _init_set)(dict_t map, const dict_t org)			\
  {                                                                     \
    DICTI_OA_CONTRACT(org);                                             \
    assert (map != org);                                                \
    map->mask         = org->mask;                                      \
    map->count        = org->count;                                     \
    map->count_delete = org->count_delete;                              \
    map->upper_limit  = org->upper_limit;                               \
    map->lower_limit  = org->lower_limit;                               \
    map->data = M_CALL_REALLOC(key_oplist, M_C(name, _pair_t), NULL, map->mask+1); \
    if (map->data == NULL) {                                            \
      M_MEMORY_FULL(sizeof (M_C(name, _pair_t)) * (map->mask+1));	\
      return ;                                                          \
    }                                                                   \
    for(size_t i = 0; i <= org->mask; i++) {                            \
      if (M_CALL_OOR_EQUAL(key_oplist, org->data[i].key, DICTI_OA_EMPTY)) { \
        M_CALL_OOR_SET(key_oplist, map->data[i].key, DICTI_OA_EMPTY);   \
      } else if (M_CALL_OOR_EQUAL(key_oplist, org->data[i].key, DICTI_OA_DELETED)) { \
        M_CALL_OOR_SET(key_oplist, map->data[i].key, DICTI_OA_DELETED); \
      } else {                                                          \
        M_CALL_INIT_SET(key_oplist, map->data[i].key, org->data[i].key); \
        M_CALL_INIT_SET(value_oplist, map->data[i].value, org->data[i].value); \
      }                                                                 \
    }                                                                   \
    DICTI_OA_CONTRACT(map);                                             \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _set)(dict_t map, const dict_t org)				\
  {                                                                     \
    DICTI_OA_CONTRACT(map);                                             \
    DICTI_OA_CONTRACT(org);                                             \
    if (M_LIKELY (map != org)) {                                        \
      M_C(name, _clear)(map);						\
      M_C(name, _init_set)(map, org);					\
    }                                                                   \
    DICTI_OA_CONTRACT(map);                                             \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _init_move)(dict_t map, dict_t org)				\
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
  M_C(name, _move)(dict_t map, dict_t org)				\
  {                                                                     \
    DICTI_OA_CONTRACT(map);                                             \
    DICTI_OA_CONTRACT(org);                                             \
    if (M_LIKELY (map != org)) {                                        \
      M_C(name, _clear)(map);						\
      M_C(name, _init_move)(map, org);					\
    }                                                                   \
    DICTI_OA_CONTRACT(map);                                             \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _swap)(dict_t d1, dict_t d2)				\
  {                                                                     \
    DICTI_OA_CONTRACT(d1);                                              \
    DICTI_OA_CONTRACT(d2);                                              \
    M_SWAP (size_t, d1->mask,         d2->mask);                        \
    M_SWAP (size_t, d1->count,        d2->count);                       \
    M_SWAP (size_t, d1->count_delete, d2->count_delete);                \
    M_SWAP (size_t, d1->upper_limit,  d2->upper_limit);                 \
    M_SWAP (size_t, d1->lower_limit,  d2->lower_limit);                 \
    M_SWAP (M_C(name, _pair_t) *, d1->data, d2->data);			\
    DICTI_OA_CONTRACT(d1);                                              \
    DICTI_OA_CONTRACT(d2);                                              \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _clean)(dict_t d)						\
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    for(size_t i = 0; i <= d->mask; i++) {                              \
      if (!M_CALL_OOR_EQUAL(key_oplist, d->data[i].key, DICTI_OA_EMPTY) \
          && !M_CALL_OOR_EQUAL(key_oplist, d->data[i].key, DICTI_OA_DELETED)) { \
        M_CALL_CLEAR(key_oplist, d->data[i].key);                       \
        M_CALL_CLEAR(value_oplist, d->data[i].value);                   \
      }                                                                 \
    }                                                                   \
    d->count = 0;                                                       \
    d->count_delete = 0;                                                \
    d->mask = DICTI_INITIAL_SIZE-1;                                     \
    M_C(name,_int_limit)(d, DICTI_INITIAL_SIZE);			\
    d->data = M_CALL_REALLOC(key_oplist, M_C(name, _pair_t),		\
                             d->data, DICTI_INITIAL_SIZE);              \
    assert(d->data != NULL);                                            \
    for(size_t i = 0; i <= d->mask; i++) {                              \
      M_CALL_OOR_SET(key_oplist, d->data[i].key, DICTI_OA_EMPTY);       \
    }									\
    DICTI_OA_CONTRACT(d);                                               \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it)(dict_it_t it, const dict_t d)				\
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    assert (it != NULL);                                                \
    it->dict = d;                                                       \
    size_t i = 0;                                                       \
    while ((M_CALL_OOR_EQUAL(key_oplist, d->data[i].key, DICTI_OA_EMPTY) \
            || M_CALL_OOR_EQUAL(key_oplist, d->data[i].key, DICTI_OA_DELETED)) \
           && i <= d->mask) {                                           \
      i++;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_set)(dict_it_t it, const dict_it_t ref)			\
  {                                                                     \
    assert (it != NULL);                                                \
    assert (ref != NULL);                                               \
    it->dict = ref->dict;                                               \
    it->index = ref->index;                                             \
    DICTI_OA_CONTRACT (it->dict);                                       \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_last)(dict_it_t it, const dict_t d)                     \
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    assert (it != NULL);                                                \
    it->dict = d;                                                       \
    size_t i = d->mask;                                                 \
    while ((M_CALL_OOR_EQUAL(key_oplist, d->data[i].key, DICTI_OA_EMPTY) \
            || M_CALL_OOR_EQUAL(key_oplist, d->data[i].key, DICTI_OA_DELETED)) \
           && i <= d->mask) {                                           \
      i--;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _it_end)(dict_it_t it, const dict_t d)                      \
  {                                                                     \
    DICTI_OA_CONTRACT(d);                                               \
    assert (it != NULL);                                                \
    it->dict = d;                                                       \
    it->index = d->mask+1;                                              \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _end_p)(const dict_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it->dict);                                       \
    return it->index > it->dict->mask;                                  \
  }                                                                     \
  									\
  static inline void                                                    \
  M_C(name, _next)(dict_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it->dict);                                       \
    size_t i = it->index + 1;                                           \
    while (i <= it->dict->mask &&                                       \
           (M_CALL_OOR_EQUAL(key_oplist, it->dict->data[i].key, DICTI_OA_EMPTY) \
            || M_CALL_OOR_EQUAL(key_oplist, it->dict->data[i].key, DICTI_OA_DELETED))) { \
      i++;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  M_C(name, _previous)(dict_it_t it)					\
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it->dict);                                       \
    /* if index was 0, the operation will overflow, and stops the loop */ \
    size_t i = it->index - 1;                                           \
    while (i <= it->dict->mask &&                                       \
           (M_CALL_OOR_EQUAL(key_oplist, it->dict->data[i].key, DICTI_OA_EMPTY) \
            || M_CALL_OOR_EQUAL(key_oplist, it->dict->data[i].key, DICTI_OA_DELETED))) { \
      i--;                                                              \
    }                                                                   \
    it->index = i;                                                      \
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _last_p)(const dict_it_t it)				\
  {                                                                     \
    assert (it != NULL);                                                \
    dict_it_t it2;                                                      \
    M_C(name,_it_set)(it2, it);						\
    M_C(name, _next)(it2);						\
    return M_C(name, _end_p)(it2);					\
  }                                                                     \
  									\
  static inline bool                                                    \
  M_C(name, _it_equal_p)(const dict_it_t it1,const dict_it_t it2)	\
  {                                                                     \
    assert (it1 != NULL && it2 != NULL);                                \
    DICTI_OA_CONTRACT (it1->dict);                                      \
    DICTI_OA_CONTRACT (it2->dict);                                      \
    return it1->dict == it2->dict && it1->index == it2->index;          \
  }                                                                     \
  									\
  static inline M_C(name, _type_t) *                                    \
  M_C(name, _ref)(const dict_it_t it)                                   \
  {                                                                     \
    assert (it != NULL);                                                \
    DICTI_OA_CONTRACT (it -> dict);                                     \
    const size_t i = it->index;                                         \
    assert (i <= it->dict->mask);                                       \
    assert (!M_CALL_OOR_EQUAL(key_oplist, it->dict->data[i].key, DICTI_OA_EMPTY)); \
    assert (!M_CALL_OOR_EQUAL(key_oplist, it->dict->data[i].key, DICTI_OA_DELETED)); \
    return &it->dict->data[i] M_IF(isSet)(.key, );                      \
  }                                                                     \
  									\
  static inline const  M_C(name, _type_t) *                             \
  M_C(name, _cref)(const dict_it_t it)					\
  {                                                                     \
    return M_CONST_CAST(M_C(name, _type_t), M_C(name, _ref)(it));       \
  }                                                                     \
									\
  static inline void							\
  M_C(name,_reserve)(dict_t dict, size_t capacity)			\
  {									\
    DICTI_OA_CONTRACT(dict);						\
    size_t size;							\
    /* Get the size which will allow to fit this capacity */		\
    size = m_core_roundpow2 ((uint64_t) (capacity * (1.0 / coeff_up)));		\
    /* Test for overflow of the computation */                          \
    if (M_UNLIKELY (size < capacity)) {					\
      M_MEMORY_FULL((size_t)-1);					\
    }									\
    assert (M_POWEROF2_P(size));					\
    if (size > dict->mask+1) {						\
      dict->upper_limit = (size_t) (size * coeff_up) - 1;		\
      M_C(name,_int_resize_up)(dict, size, false);			\
    }									\
    DICTI_OA_CONTRACT(dict);						\
  }									\
  									\
  M_IF_METHOD(EQUAL, value_oplist)(					\
  static inline bool                                                    \
  M_C(name, _equal_p)(const dict_t dict1, const dict_t dict2)           \
  {									\
    DICTI_OA_CONTRACT(dict1);						\
    DICTI_OA_CONTRACT(dict2);                                           \
    /* NOTE: Key type has mandatory equal operator */			\
    /* Easy case */                                                     \
    if (M_LIKELY (dict1->count != dict2->count))                        \
      return false;                                                     \
    if (M_UNLIKELY (dict1->count == 0))                                 \
      return true;                                                      \
    /* Otherwise this is the slow path */                               \
    dict_it_t it;                                                       \
    for(M_C(name, _it)(it, dict1) ;                                     \
        !M_C(name, _end_p)(it);                                         \
        M_C(name, _next)(it)) {                                         \
      const M_C(name, _type_t) *item = M_C(name, _cref)(it);            \
      value_type *ptr = M_C(name, _get)(dict2, M_IF(isSet)(*item, item->key)); \
      if (ptr == NULL)                                                  \
        return false;                                                   \
      if (M_CALL_EQUAL(value_oplist, item->value, *ptr) == false)       \
        return false;                                                   \
    }                                                                   \
    return true;                                                        \
  }									\
  , /* no value equal */ )						\
                                                                        \
  DICTI_FUNC_ADDITIONAL_DEF2(name, key_type, key_oplist, value_type, value_oplist, isSet, dict_t, dict_it_t)


#endif
