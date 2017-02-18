/*
 * MLIB - DICTIONARY Module
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
#include "m-string.h"
#include "m-tuple.h"

/* Define a dictionnary with the key key_type to the value value_type. */
#define DICT_DEF2(name, key_type, key_oplist, value_type, value_oplist) \
  TUPLE_DEF2(CAT(dict_pair_, name), (key, key_type, key_oplist), (value, value_type, value_oplist)) \
  LIST_DEF(CAT(dict_pair_, name), CAT3(dict_pair_,name,_t), TUPLE_OPLIST(CAT(dict_pair_, name), key_oplist, value_oplist)) \
  ARRAY_DEF(CAT(list_dict_pair_,name), CAT3(list_dict_pair_,name,_t), LIST_OPLIST(CAT(dict_pair_, name))) \
                                                                        \
  typedef struct CAT3(dict_, name, _s) {                                \
    size_t used, lower_limit, upper_limit;                              \
    CAT3(array_list_dict_pair_,name,_t) table;                          \
  } CAT3(dict_, name, _t)[1];                                           \
                                                                        \
  typedef struct CAT3(dict_it_, name, _s) {                             \
    CAT3(array_it_list_dict_pair_, name, _t) array_it;                  \
    CAT3(list_it_dict_pair_, name, _t) list_it;                         \
  } CAT3(dict_it_, name, _t)[1];                                        \
                                                                        \
  static inline void                                                    \
  CAT3(dict_, name, _init)(CAT3(dict_,name,_t) map)                     \
  {                                                                     \
    map->used = 0;                                                      \
    CAT3(array_list_dict_pair_,name,_init)(map->table);                 \
    CAT3(array_list_dict_pair_,name,_resize)(map->table, DICTI_INITIAL_SIZE); \
    map->lower_limit = DICTI_LOWER_BOUND(DICTI_INITIAL_SIZE);           \
    map->upper_limit = DICTI_UPPER_BOUND(DICTI_INITIAL_SIZE);           \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_, name, _init_set)(CAT3(dict_,name,_t) map,                 \
                               const CAT3(dict_,name,_t) org)           \
  {                                                                     \
    DICTI_CONTRACT(name, org);                                          \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    CAT3(array_list_dict_pair_,name,_init_set)(map->table, org->table); \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_, name, _set)(CAT3(dict_,name,_t) map,                      \
                          const CAT3(dict_,name,_t) org)                \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    DICTI_CONTRACT(name, org);                                          \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    CAT3(array_list_dict_pair_,name,_set)(map->table, org->table);      \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_,name,_clear)(CAT3(dict_,name,_t) map)                      \
  {                                                                     \
    CAT3(array_list_dict_pair_,name,_clear)(map->table);                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_, name, _init_move)(CAT3(dict_,name,_t) map,                \
                                CAT3(dict_,name,_t) org)                \
  {                                                                     \
    DICTI_CONTRACT(name, org);                                          \
    map->used = org->used;                                              \
    map->lower_limit = org->lower_limit;                                \
    map->upper_limit = org->upper_limit;                                \
    CAT3(array_list_dict_pair_,name,_init_move)(map->table, org->table); \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_, name, _move)(CAT3(dict_,name,_t) map,                     \
                           CAT3(dict_,name,_t) org)                     \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    DICTI_CONTRACT(name, org);                                          \
    assert (map != org);                                                \
    CAT3(dict_,name,_clear)(map);                                       \
    CAT3(dict_,name,_init_move)(map, org);                              \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_,name,_clean)(CAT3(dict_,name,_t) map)                      \
  {                                                                     \
    CAT3(array_list_dict_pair_,name,_clean)(map->table);                \
    CAT3(array_list_dict_pair_,name,_resize)(map->table, DICTI_INITIAL_SIZE); \
    map->lower_limit = DICTI_LOWER_BOUND(DICTI_INITIAL_SIZE);           \
    map->upper_limit = DICTI_UPPER_BOUND(DICTI_INITIAL_SIZE);           \
    map->used = 0;                                                      \
  }                                                                     \
                                                                        \
  static inline size_t                                                  \
  CAT3(dict_,name,_size)(const CAT3(dict_,name,_t) map)                 \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    return map->used;                                                   \
  }                                                                     \
                                                                        \
  static inline value_type *                                            \
  CAT3(dict_,name,_get)(const CAT3(dict_,name,_t) map, const key_type key) \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
    size_t hash = M_GET_HASH key_oplist (key);                            \
    hash = hash & (CAT3(array_list_dict_pair_,name,_size)(map->table) - 1); \
    const CAT3(list_dict_pair_,name,_t) *list_ptr =                     \
      CAT3(array_list_dict_pair_,name,_cget)(map->table, hash);         \
    CAT3(list_it_dict_pair_,name,_t) it;                                \
    for(CAT3(list_dict_pair_,name,_it)(it, *list_ptr);                  \
        !CAT3(list_dict_pair_,name,_end_p)(it);                         \
        CAT3(list_dict_pair_,name,_next)(it)) {                         \
      CAT3(dict_pair_,name,_t) *ref = CAT3(list_dict_pair_,name,_ref)(it); \
      if (M_GET_EQUAL key_oplist ((*ref)->key, key))                      \
        return &(*ref)->value;                                          \
    }                                                                   \
    return NULL;                                                        \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dicti_,name,_resize_up)(CAT3(dict_,name,_t) map)                 \
  {                                                                     \
    size_t old_size = CAT3(array_list_dict_pair_,name,_size)(map->table); \
    size_t new_size = old_size * 2;                                     \
    /* Resize the dictionnary */                                        \
    CAT3(array_list_dict_pair_,name,_resize)(map->table, new_size);     \
    /* Move the items to the new upper part */                          \
    for(size_t i = 0; i < old_size; i++) {                              \
      CAT3(list_dict_pair_,name,_t) *list =                             \
        CAT3(array_list_dict_pair_,name,_get)(map->table, i);           \
      if (CAT3(list_dict_pair_,name,_empty_p)(*list))                   \
        continue;                                                       \
      CAT3(list_it_dict_pair_,name,_t) it;                              \
      CAT3(list_dict_pair_,name,_it)(it, *list);                        \
      while (!CAT3(list_dict_pair_,name,_end_p)(it)) {                  \
        CAT3(dict_pair_,name,_ptr) pair = *CAT3(list_dict_pair_,name,_ref)(it); \
        size_t hash = M_GET_HASH key_oplist (pair->key);                  \
        if ((hash & (new_size-1)) >= old_size) {                        \
          assert( (hash & (new_size-1)) == (i + old_size));             \
          CAT3(list_dict_pair_,name,_t) *new_list =                     \
            CAT3(array_list_dict_pair_,name,_get)(map->table, i + old_size); \
          CAT3(list_dict_pair_,name,_move_back)(*new_list, *list, it);  \
        } else {                                                        \
          CAT3(list_dict_pair_,name,_next)(it);                         \
        }                                                               \
      }                                                                 \
    }                                                                   \
    map->upper_limit = DICTI_UPPER_BOUND(new_size);                     \
    map->lower_limit = DICTI_LOWER_BOUND(new_size);                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dicti_,name,_resize_down)(CAT3(dict_,name,_t) map)               \
  {                                                                     \
    assert (map != NULL);                                               \
    size_t old_size = CAT3(array_list_dict_pair_,name,_size)(map->table); \
    assert ((old_size % 2) == 0);                                       \
    size_t new_size = old_size / 2;                                     \
    assert (new_size >= DICTI_INITIAL_SIZE);                            \
    /* Move all items to the lower part of the dictionnary */           \
    for(size_t i = new_size; i < old_size; i++) {                       \
      CAT3(list_dict_pair_,name,_t) *list =                             \
        CAT3(array_list_dict_pair_,name,_get)(map->table, i);           \
      if (CAT3(list_dict_pair_,name,_empty_p)(*list))                   \
        continue;                                                       \
      CAT3(list_it_dict_pair_,name,_t) it;                              \
      CAT3(list_dict_pair_,name,_it)(it, *list);                        \
      while (!CAT3(list_dict_pair_,name,_end_p)(it)) {                  \
        M_IF_DEBUG(CAT3(dict_pair_,name,_ptr) pair = *CAT3(list_dict_pair_,name,_ref)(it);) \
        M_IF_DEBUG(size_t hash = M_GET_HASH key_oplist (pair->key);)    \
        assert ((hash & (old_size -1)) == i);                           \
        assert ((hash & (new_size -1)) == i - new_size);                \
        CAT3(list_dict_pair_,name,_t) *new_list =                       \
          CAT3(array_list_dict_pair_,name,_get)(map->table, i - new_size); \
        CAT3(list_dict_pair_,name,_move_back)(*new_list, *list, it);    \
      }                                                                 \
    }                                                                   \
    /* Resize the dictionary */                                         \
    CAT3(array_list_dict_pair_,name,_resize)(map->table, new_size);     \
    map->upper_limit = DICTI_UPPER_BOUND(new_size);                     \
    map->lower_limit = DICTI_LOWER_BOUND(new_size);                     \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_,name,_set_at)(CAT3(dict_,name,_t) map,                     \
                           const key_type key, const value_type value)  \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
                                                                        \
    size_t hash = M_GET_HASH key_oplist(key);                           \
    hash = hash & (CAT3(array_list_dict_pair_,name,_size)(map->table) - 1); \
    CAT3(list_dict_pair_,name,_t) *list_ptr =                           \
      CAT3(array_list_dict_pair_,name,_get)(map->table, hash);          \
    CAT3(list_it_dict_pair_,name,_t) it;                                \
    for(CAT3(list_dict_pair_,name,_it)(it, *list_ptr);                  \
        !CAT3(list_dict_pair_,name,_end_p)(it);                         \
        CAT3(list_dict_pair_,name,_next)(it)) {                         \
      CAT3(dict_pair_,name,_ptr) ref = *CAT3(list_dict_pair_,name,_ref)(it); \
      if (M_GET_EQUAL key_oplist(ref->key, key)) {                      \
        M_GET_SET value_oplist(ref->value, value);                      \
        return;                                                         \
      }                                                                 \
    }                                                                   \
    CAT3(dict_pair_,name,_init_set2)(*CAT3(list_dict_pair_,name,_push_raw)(*list_ptr), \
                                     key, value);                       \
    map->used ++;                                                       \
    if (map->used > map->upper_limit)                                   \
      CAT3(dicti_,name,_resize_up)(map);                                \
  }                                                                     \
                                                                        \
  static inline void                                                    \
  CAT3(dict_,name,_remove)(CAT3(dict_,name,_t) map, const key_type key) \
  {                                                                     \
    DICTI_CONTRACT(name, map);                                          \
                                                                        \
    size_t hash = M_GET_HASH key_oplist(key);                           \
    hash = hash & (CAT3(array_list_dict_pair_,name,_size)(map->table) - 1); \
    CAT3(list_dict_pair_,name,_t) *list_ptr =                           \
      CAT3(array_list_dict_pair_,name,_get)(map->table, hash);          \
    CAT3(list_it_dict_pair_,name,_t) it;                                \
    for(CAT3(list_dict_pair_,name,_it)(it, *list_ptr);                  \
        !CAT3(list_dict_pair_,name,_end_p)(it);                         \
        CAT3(list_dict_pair_,name,_next)(it)) {                         \
      CAT3(dict_pair_,name,_ptr) ref = *CAT3(list_dict_pair_,name,_ref)(it); \
      if (M_GET_EQUAL key_oplist(ref->key, key)) {                      \
        CAT3(list_dict_pair_,name,_remove)(*list_ptr, it);              \
        map->used --;                                                   \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    if (map->used < map->lower_limit)                                   \
      CAT3(dicti_, name, _resize_down)(map);                            \
  }                                                                     \
                                                                        \
 static inline void                                                     \
 CAT3(dict_, name, _it)(CAT3(dict_it_, name,_t) it,                     \
                        CAT3(dict_, name,_t) d)                         \
 {                                                                      \
   CAT3(array_list_dict_pair_, name, _it)(it->array_it, d->table);      \
   CAT3(list_dict_pair_,name,_t) *ref =                                 \
     CAT3(array_list_dict_pair_, name, _ref)(it->array_it);             \
   CAT3(list_dict_pair_, name, _it)(it->list_it, *ref);                 \
   while (CAT3(list_dict_pair_, name, _end_p)(it->list_it)) {           \
     CAT3(array_list_dict_pair_, name, _next)(it->array_it);            \
     if (CAT3(array_list_dict_pair_, name, _end_p)(it->array_it))break; \
     ref = CAT3(array_list_dict_pair_, name, _ref)(it->array_it);       \
     CAT3(list_dict_pair_, name, _it)(it->list_it, *ref);               \
   }                                                                    \
 }                                                                      \
                                                                        \
 static inline void                                                     \
 CAT3(dict_, name, _it_set)(CAT3(dict_it_, name,_t) it,                 \
                            const CAT3(dict_it_, name,_t) ref)          \
 {                                                                      \
   assert (it != NULL && ref != NULL);                                  \
   CAT3(array_list_dict_pair_, name, _it_set)(it->array_it,             \
                                              ref->array_it);           \
   CAT3(list_dict_pair_, name, _it_set)(it->list_it, ref->list_it);     \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 CAT3(dict_, name, _end_p)(const CAT3(dict_it_, name,_t) it)            \
 {                                                                      \
   assert (it != NULL);                                                 \
   return CAT3(list_dict_pair_, name, _end_p)(it->list_it);             \
 }                                                                      \
                                                                        \
 static inline void                                                     \
 CAT3(dict_, name, _next)(CAT3(dict_it_, name,_t) it)                   \
 {                                                                      \
   assert(it != NULL);                                                  \
   CAT3(list_dict_pair_, name, _next)(it->list_it);                     \
   CAT3(list_dict_pair_,name,_t) *ref;                                  \
   while (CAT3(list_dict_pair_, name, _end_p)(it->list_it)) {           \
     CAT3(array_list_dict_pair_, name, _next)(it->array_it);            \
     if (CAT3(array_list_dict_pair_, name, _end_p)(it->array_it))break; \
     ref = CAT3(array_list_dict_pair_, name, _ref)(it->array_it);       \
     CAT3(list_dict_pair_, name, _it)(it->list_it, *ref);               \
   }                                                                    \
 }                                                                      \
                                                                        \
 static inline bool                                                     \
 CAT3(dict_, name, _last_p)(const CAT3(dict_it_, name,_t) it)           \
 {                                                                      \
   assert (it != NULL);                                                 \
   CAT3(dict_it_, name,_t) it2;                                         \
   CAT3(dict_, name,_it_set)(it2, it);                                  \
   CAT3(dict_, name, _next)(it2);                                       \
   return CAT3(dict_, name, _end_p)(it2);                               \
 }                                                                      \
                                                                        \
 static inline CAT3(dict_pair_,name,_t) *                               \
 CAT3(dict_, name, _ref)(CAT3(dict_it_, name,_t) it)                    \
 {                                                                      \
   assert(it != NULL);                                                  \
   return CAT3(list_dict_pair_, name, _ref)(it->list_it);               \
 }                                                                      \
                                                                        \
 static inline const CAT3(dict_pair_,name,_t) *                         \
 CAT3(dict_, name, _cref)(const CAT3(dict_it_, name,_t) it)             \
 {                                                                      \
   assert(it != NULL);                                                  \
   return CAT3(list_dict_pair_, name, _cref)(it->list_it);              \
 }                                                                      \
                                                                        \
 M_IF_METHOD(EQUAL, value_oplist)(                                      \
 static inline bool                                                     \
 CAT3(dict_, name, _equal_p)(CAT3(dict_, name,_t) dict1,                \
                             CAT3(dict_, name,_t) dict2)                \
 {                                                                      \
   assert (dict1 != NULL && dict2 != NULL);                             \
   /* NOTE: Key type has mandatory equal operator */                    \
   return dict1->used == dict2-> used &&                                \
     CAT3(array_list_dict_pair_, name, _equal_p)(dict1->table,          \
                                                 dict2->table);         \
 }                                                                      \
 , /* no value equal */ )                                               \
                                                                        \
 M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(                   \
 static inline void                                                     \
 CAT3(dict_, name, _get_str)(string_t str,                              \
                             CAT3(dict_, name,_t) dict,                 \
                             const bool append)                         \
  {                                                                     \
    assert (str != NULL && dict != NULL);                               \
    DICTI_CONTRACT(name, dict);                                         \
    (append ? string_cat_str : string_set_str) (str, "{");              \
    CAT3(dict_it_, name, _t) it;                                        \
    for (CAT3(dict_, name, _it)(it, dict) ;                             \
         !CAT3(dict_, name, _end_p)(it);                                \
         CAT3(dict_, name, _next)(it)){                                 \
      const CAT3(dict_pair_,name,_t) *item =                            \
        CAT3(dict_, name, _cref)(it);                                   \
      M_GET_GET_STR key_oplist (str, (*item)->key, true);               \
      string_push_back (str, ':');                                      \
      M_GET_GET_STR value_oplist (str, (*item)->value, true);           \
      if (!CAT3(dict_, name, _last_p)(it))                              \
        string_push_back (str, ',');                                    \
    }                                                                   \
    string_push_back (str, '}');                                        \
  }                                                                     \
 , /* no GET_STR */ )                                                   \
                                                                        \
 M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(                   \
 static inline void                                                     \
 CAT3(dict_, name, _out_str)(FILE *file,                                \
                             CAT3(dict_, name,_t) dict)                 \
  {                                                                     \
    assert (file != NULL && dict != NULL);                              \
    DICTI_CONTRACT(name, dict);                                         \
    fputc ('{', file);                                                  \
    CAT3(dict_it_, name, _t) it;                                        \
    for (CAT3(dict_, name, _it)(it, dict) ;                             \
         !CAT3(dict_, name, _end_p)(it);                                \
         CAT3(dict_, name, _next)(it)){                                 \
      const CAT3(dict_pair_,name,_t) *item =                            \
        CAT3(dict_, name, _cref)(it);                                   \
      M_GET_OUT_STR key_oplist (file, (*item)->key);                    \
      fputc (':', file);                                                \
      M_GET_OUT_STR value_oplist (file, (*item)->value);                \
      if (!CAT3(dict_, name, _last_p)(it))                              \
        fputc (',', file);                                              \
    }                                                                   \
    fputc ('}', file);                                                  \
  }                                                                     \
 , /* no OUT_STR */ )                                                   \
                                                                        \
 M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(                    \
 static inline bool                                                     \
 CAT3(dict_, name, _in_str)(CAT3(dict_, name,_t) dict, FILE *file)      \
  {                                                                     \
    assert (file != NULL && dict != NULL);                              \
    DICTI_CONTRACT(name, dict);                                         \
    CAT3(dict_, name, _clean)(dict);                                    \
    char c = fgetc(file);                                               \
    if (c != '{') return false;                                         \
    key_type key;                                                       \
    value_type value;                                                   \
    M_GET_INIT key_oplist (key);                                        \
    M_GET_INIT value_oplist (value);                                    \
    do {                                                                \
      bool b = M_GET_IN_STR key_oplist (key, file);                     \
      if (!b) { c = 0; break; }                                         \
      c = fgetc(file);                                                  \
      if (c != ':') { c = 0; break; }                                   \
      b = M_GET_IN_STR value_oplist (value, file);                      \
      if (!b) { c = 0; break; }                                         \
      CAT3(dict_, name, _set_at)(dict, key, value);                     \
      c = fgetc(file);                                                  \
    } while (c == ',' && !feof(file) && !ferror(file));                 \
    M_GET_CLEAR key_oplist (key);                                       \
    M_GET_CLEAR value_oplist (value);                                   \
    return c == '}';                                                    \
  }                                                                     \
 , /* no IN_STR */ )                                                    \
                                                                        \

/* Define the oplist of a dictionnary.
   DICT_OPLIST(name[, oplist of the key type, oplist of the value type]) */
#define DICT_OPLIST(...)                                           \
  M_IF_NARGS_EQ1(__VA_ARGS__)                                      \
  (DICTI_OPLIST(__VA_ARGS__, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST ),     \
   DICTI_OPLIST(__VA_ARGS__ ))

/********************************** INTERNAL ************************************/

/* Define the oplist of a dictionnary */
#define DICTI_OPLIST(type, key_oplist, value_oplist)                    \
  (INIT(CAT3(dict_, type, _init)),                                      \
   INIT_SET(CAT3(dict_, type, _init_set)),                              \
   SET(CAT3(dict_, type, _set)),                                        \
   CLEAR(CAT3(dict_, type, _clear)),                                    \
   INIT_MOVE(CAT3(dict_, type, _init_move)),                            \
   MOVE(CAT3(dict_, type, _move)),                                      \
   TYPE(CAT3(dict_, type, _t)),                                         \
   SUBTYPE(CAT3(dict_pair_,type,_t)),                                   \
   IT_TYPE(CAT3(dict_it_,type,_t)),                                     \
   IT_FIRST(CAT3(dict_,type,_it)),                                      \
   IT_SET(CAT3(dict_, type, _it_set)),                                  \
   IT_END_P(CAT3(dict_,type,_end_p)),                                   \
   IT_LAST_P(CAT3(dict_,type,_last_p)),                                 \
   IT_NEXT(CAT3(dict_,type,_next)),                                     \
   IT_REF(CAT3(dict_,type,_ref)),                                       \
   IT_CREF(CAT3(dict_,type,_cref))                                      \
   ,M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(GET_STR(CAT3(array_, type, _get_str)),) \
   ,M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(OUT_STR(CAT3(array_, type, _out_str)),) \
   ,M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(IN_STR(CAT3(array_, type, _in_str)),) \
   ,M_IF_METHOD(EQUAL, value_oplist)(EQUAL(CAT3(dict_, type, _equal_p)),) \
   ,KEY_OPLIST(key_oplist)                                              \
   ,VALUE_OPLIST(value_oplist)                                          \
   )

#define DICTI_LOWER_BOUND(s) ((s) <= DICTI_INITIAL_SIZE ? 0 : (s) * 1 / 4)
#define DICTI_UPPER_BOUND(s) ((s) * 2 / 3)
#define DICTI_INITIAL_SIZE   16

#define DICTI_CONTRACT(name, map) do {                                  \
    assert(map->used <= map->upper_limit);                              \
    assert(M_POWEROF2_P(CAT3(array_list_dict_pair_,name,_size)(map->table))); \
  } while (0)
    
#endif
