/*
 * M*LIB - Wrap API to other Module
 *
 * Copyright (c) 2017-2021, Patrick Pelissier
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

// TODO: Support of user suffix.
#ifndef MSTARLIB_WRAP_H
#define MSTARLIB_WRAP_H

#include "m-core.h"

#define WRAP_FULL_DECL(name, subtype_t, type_oplist)                          \
    WRAP_FULL_DECL_AS(name, M_C(name, _t), M_C(name, _it_t) , subtype_t, type_oplist)

#define WRAP_FULL_DECL_AS(name, name_t, name_it_t, subtype_t, type_oplist)    \
    M_WR4P_FULL_DECL_AS_P3 ((name, name_t, name_it_t, subtype_t, size_t, subtype_t, type_oplist, M_GET_LIMITS type_oplist, 0))

#define WRAP_FULL_DEF(name, subtype_t, type_oplist)                           \
    WRAP_FULL_DEF_AS(name, M_C(name, _t), M_C(name, _it_t) , subtype_t, type_oplist)

#define WRAP_FULL_DEF_AS(name, name_t, name_it_t, subtype_t, type_oplist)     \
    M_WR4P_FULL_DEF_AS_P3( (name, name_t, name_it_t, subtype_t, size_t, subtype_t, type_oplist, 0) )

#define WRAP_FULL_DECL2(name, keytype_t, valuetype_t, type_oplist)            \
    WRAP_FULL_DECL2_AS(name, M_C(name, _t), M_C(name, _it_t), M_C(name, _itref_t), keytype_t, valuetype_t, type_oplist)

#define WRAP_FULL_DECL2_AS(name, name_t, name_it_t, name_itref_t, keytype_t, valuetype_t, type_oplist) \
    M_WR4P_FULL_DECL_AS_P3A((name, name_t, name_it_t, name_itref_t, keytype_t, valuetype_t, type_oplist, M_GET_LIMITS type_oplist, 1)) \
    M_WR4P_FULL_DECL_AS_P3 ((name, name_t, name_it_t, name_itref_t, keytype_t, valuetype_t, type_oplist, M_GET_LIMITS type_oplist, 1))

#define WRAP_FULL_DEF2(name, keytype_t, valuetype_t, type_oplist)             \
    WRAP_FULL_DEF2_AS(name, M_C(name, _t), M_C(name, _it_t), M_C(name, _itref_t), keytype_t, valuetype_t, type_oplist)

#define WRAP_FULL_DEF2_AS(name, name_t, name_it_t, name_itref_t, keytype_t, valuetype_t, type_oplist) \
    M_WR4P_FULL_DEF_AS_P3( (name, name_t, name_it_t, name_itref_t, keytype_t, valuetype_t, type_oplist, 1) )

/* PARTIAL(name, type_oplist, inline_needed_oplist, encapsulated_needed_oplist )

    Type is used as it is (through typedef)
    DECL:
        INLINE: redefine with name the inline_oplist
        ENCAPSULATE: redefine as extern inline the 
    DEF:
        ENCAPSULATE: encapsulate then.
*/

#define WRAP_PARTIAL_DECL(name, subtype_t, subtype_oplist, inline_oplist, wrapped_oplist) \
    WRAP_PARTIAL_DECL_AS(name, M_C(name, _t), M_C(name, _it_t) , subtype_t, subtype_oplist, inline_oplist, wrapped_oplist)

#define WRAP_PARTIAL_DECL_AS(name, name_t, name_it_t, subtype_t, subtype_oplist, inline_oplist, wrapped_oplist) \
    M_WR4P_PARTIAL_DECL_AS_P3 ( (name, name_t, name_it_t, subtype_t, size_t, subtype_t, subtype_oplist, inline_oplist, wrapped_oplist) )

#define WRAP_PARTIAL_DEF(name, subtype_t, subtype_oplist, inline_oplist, wrapped_oplist) \
    WRAP_PARTIAL_DEF_AS(name, M_C(name, _t), M_C(name, _it_t) , subtype_t, subtype_oplist, inline_oplist, wrapped_oplist)

#define WRAP_PARTIAL_DEF_AS(name, name_t, name_it_t, subtype_t, subtype_oplist, inline_oplist, wrapped_oplist) \
    M_WR4P_PARTIAL_DEF_AS_P3 ( (name, name_t, name_it_t, subtype_t, size_t, subtype_t, subtype_oplist, inline_oplist, wrapped_oplist) )


/********************************* INTERNAL ***********************************/

/* Intermediate wrapper so that all arguments are properly expanded before
 * going further */
#define M_WR4P_FULL_DECL_AS_P3A(list)                                         \
    M_WR4P_FULL_DECL_AS_P4A list

/* Define the subtype structure for associative array.
   We are defining as two pointers to the key and the value respectively
   as it is the most flexible form.
   For encapsulation reason, we don't want to allow modification of the 
   values (it makes the following code more complex and it breaks
   strict encapsulation)
 */
#define M_WR4P_FULL_DECL_AS_P4A(name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, type_oplist, limits, isMap) \
                                                                              \
typedef struct M_C(name, _itref_s) {                                          \
    key_type_t const *key_ptr;                                                \
    value_type_t const *value_ptr;                                            \
} subtype_t;                                                                  \


/* Intermediate wrapper so that all arguments are properly expanded before
 * going further */
#define M_WR4P_FULL_DECL_AS_P3(list)                                          \
    M_WR4P_FULL_DECL_AS_P4 list

#define M_WR4P_FULL_DECL_AS_P4(name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, type_oplist, limits, isMap) \
                                                                              \
/* Define a structure which size is an upper bound of the real object         \
   This structure shall be suitably aligned for any kind of object */         \
typedef struct M_C(name, _s) {                                                \
    m_serial_ll_ct buffer[M_RET_ARG1 (M_OPFLAT limits, ,)];                   \
} name_t[1];                                                                  \
                                                                              \
/* Define a structure which size is an upper bound of the real object         \
   This structure shall be suitably aligned for any kind of object */         \
typedef struct M_C(name, _it_s) {                                             \
    m_serial_ll_ct buffer[M_RET_ARG2 (M_OPFLAT limits, ,)];                   \
    M_IF(isMap)(                                                              \
        subtype_t ret;                                                        \
        , /* Nothing to add */                                                \
    )                                                                         \
} name_it_t[1];                                                               \
                                                                              \
/* Helper function in case of SORT */                                         \
typedef int (*M_C(name, _cmp_func_ct))(subtype_t const *, subtype_t const *); \
                                                                              \
M_WR4P_EXPAND_LIST(M_WR4P_DECL_00, name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, type_oplist, type_oplist, isMap)

// Generate a function declaration based on the given prototype for an extern function.
#define M_WR4P_DECL_00(operator, suffix, name, name_t, name_it_t, wrap_oplist, type_oplist, rettype, ...) \
/* export method method if defined in the encapasulated object */             \
M_IF_METHOD(operator, wrap_oplist)                                            \
(                                                                             \
extern rettype M_C(name, suffix)(                                             \
    M_MAP2_C( M_WR4P_DECL_00_1, (name_t, name_it_t, ), __VA_ARGS__)           \
    );                                                                        \
, /* Nothing to do */                                                         \
)                                                                             \

// Expand to the correct type (TYPE and IT_TYPE are handled specially) for a declaration.
#define M_WR4P_DECL_00_1(tri, t)                                              \
    M_IF( M_KEYWORD_P(TYPE, t))(                                              \
        M_RET_ARG1 tri                                                        \
    ,                                                                         \
        M_IF( M_KEYWORD_P(IT_TYPE, t))(                                       \
            M_RET_ARG2 tri                                                    \
        ,                                                                     \
            t                                                                 \
        )                                                                     \
    )

/* Nothing more to do for the prototype in case if associative array */
#define M_WR4P_DECL_00_IS_MAP M_WR4P_DECL_00



/* Intermediate wrapper so that all arguments are properly expanded before
 * going further */
#define M_WR4P_FULL_DEF_AS_P3(list)                                           \
    M_WR4P_FULL_DEF_AS_P4 list

#define M_WR4P_FULL_DEF_AS_P4(name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, type_oplist, isMap) \
                                                                              \
/* The ugly cast to transform an object into another */                       \
static inline M_GET_TYPE type_oplist *                                        \
M_C(name, _cast)(name_t a)                                                    \
{                                                                             \
    /* Check if the assumption are correct */                                 \
    M_STATIC_ASSERT( sizeof (M_GET_TYPE type_oplist) <= sizeof (name_t) ,     \
        MLIB_INTERNAL, "M*LIB: Export limit of oplist if below than actual size"); \
    /* TODO: static assert of alignment if C11 */                             \
    return (M_GET_TYPE type_oplist *) a;                                      \
}                                                                             \
                                                                              \
M_IF_METHOD(IT_TYPE, type_oplist) (                                           \
    /* The ugly cast to transform an object into another */                   \
    static inline M_GET_IT_TYPE type_oplist *                                 \
    M_C(name, _cast_it)(name_it_t a)                                          \
    {                                                                         \
        /* Check if the assumption are correct */                             \
        M_STATIC_ASSERT( sizeof (M_GET_TYPE type_oplist) <= sizeof (name_it_t) , \
            MLIB_INTERNAL, "M*LIB: Export limit of oplist if below than actual size"); \
    /* TODO: static assert of alignment if C11 */                             \
        return (M_GET_IT_TYPE type_oplist *) a;                               \
    }                                                                         \
, /* Nothing to do */                                                         \
)                                                                             \
                                                                              \
M_WR4P_EXPAND_LIST(M_WR4P_DEF_00, name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, type_oplist, type_oplist, isMap)

// Generate a function definition based on the given prototype
// The function doesn't use itref with key_ptr / value_ptr.
#define M_WR4P_DEF_00(operator, suffix, name, name_t, name_it_t, wrap_oplist, type_oplist, rettype, ...) \
M_IF_METHOD(operator, wrap_oplist)                                            \
(                                                                             \
rettype M_C(name, suffix)(                                                    \
    M_MAP3_C( M_WR4P_DEF_00_1, (name_t, name_it_t, ), __VA_ARGS__)            \
    )                                                                         \
{                                                                             \
    M_IF(M_KEYWORD_P(void, rettype))(/*nothing*/, return)                     \
    M_C(M_CALL_, operator)(type_oplist,                                       \
        M_MAP3_C( M_WR4P_DEF_00_2, name, __VA_ARGS__)                         \
    );                                                                        \
}                                                                             \
, /* Nothing to do */                                                         \
)                                                                             \

/* Test if the container object returns an itref which is based on
   key_ptr / value_ptr (true) or key / value (false) */
#define WRAP_IS_PTR_REF(limits)                                               \
    M_NOTEQUAL( M_RET_ARG3 (M_OPFLAT limits, ), 2)

/* Define the wrapper in case of associative array and _cref or _ref */
#define M_WR4P_DEF_00_IS_MAP(operator, suffix, name, name_t, name_it_t, wrap_oplist, type_oplist, rettype, ...) \
M_IF_METHOD(operator, type_oplist)                                            \
(                                                                             \
rettype M_C(name, suffix)(                                                    \
    M_MAP3_C( M_WR4P_DEF_00_1, (name_t, name_it_t, ), __VA_ARGS__)            \
    )                                                                         \
{                                                                             \
    /* Get the reference of the iterator */                                   \
    M_GET_SUBTYPE type_oplist const *ret =                                    \
    M_C(M_CALL_, operator)(type_oplist,                                       \
        M_MAP3_C( M_WR4P_DEF_00_2, name, __VA_ARGS__)                         \
    );                                                                        \
    /* Set the return values in the iterator so that it is not destroyed */   \
    M_IF(WRAP_IS_PTR_REF(M_GET_LIMITS type_oplist) )(                         \
        _1->ret.key_ptr = &ret->key;                                          \
        _1->ret.value_ptr = &ret->value;                                      \
    , /* else */                                                              \
        _1->ret.key_ptr = ret->key_ptr;                                       \
        _1->ret.value_ptr = ret->value_ptr;                                   \
    )                                                                         \
    /* Return this reference */                                               \
    return &_1->ret;                                                          \
}                                                                             \
, /* Nothing to do */                                                         \
)                                                                             \

// Expand to 'type var' to be used in function definition
// Replacing TYPE & IT_TYPE into the right types.
#define M_WR4P_DEF_00_1(tri, num, t)                                          \
    M_IF( M_KEYWORD_P(TYPE, t))(                                              \
        M_RET_ARG1 tri M_C(_, num)                                            \
    ,                                                                         \
        M_IF( M_KEYWORD_P(IT_TYPE, t))(                                       \
            M_RET_ARG2 tri M_C(_, num)                                        \
        ,                                                                     \
            t M_C(_, num)                                                     \
        )                                                                     \
    )

// Expand to 'var' to be used in function call. 
// Apply cast if needed for TYPE & IT_TYPE
#define M_WR4P_DEF_00_2(name, num, t)                                         \
    M_IF( M_KEYWORD_P(TYPE, t))(                                              \
        *M_C(name, _cast)(M_C(_, num))                                        \
    ,                                                                         \
        M_IF( M_KEYWORD_P(IT_TYPE, t))(                                       \
            *M_C(name, _cast_it)(M_C(_, num))                                 \
        ,                                                                     \
            M_C(_, num)                                                       \
        )                                                                     \
    )



#define M_WR4P_PARTIAL_DECL_AS_P3(xlist)                                      \
    M_WR4P_PARTIAL_DECL_AS_P4 xlist

#define M_WR4P_PARTIAL_DECL_AS_P4(name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, subtype_oplist, inline_oplist, wrapped_oplist) \
    /* Define synonymous */                                                   \
    typedef M_GET_TYPE subtype_oplist name_t;                                 \
    typedef M_GET_IT_TYPE subtype_oplist name_it_t;                           \
    /* Export synonymous inline function */                                   \
    M_WR4P_EXPAND_LIST(M_WR4P_DECL_01, name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, inline_oplist, subtype_oplist, 0) \
    /* Export extern wrapped function */                                      \
    M_WR4P_EXPAND_LIST(M_WR4P_DECL_00, name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, wrapped_oplist, subtype_oplist,0) \

// Generate a function declaration based on the given prototype for an extern function.
#define M_WR4P_DECL_01(operator, suffix, name, name_t, name_it_t, wrap_oplist, type_oplist, rettype, ...) \
/* export method method if defined in the encapasulated object */             \
M_IF_METHOD(operator, wrap_oplist)                                            \
(                                                                             \
static inline rettype M_C(name, suffix)(                                      \
    M_MAP3_C( M_WR4P_DEF_00_1, (name_t, name_it_t, ), __VA_ARGS__)            \
    ) {                                                                       \
        M_IF(M_KEYWORD_P(void, rettype))(/*nothing*/, return)                 \
        M_C(M_CALL_, operator)(type_oplist,                                   \
            M_MAP3_C( M_WR4P_DECL_01_2, name, __VA_ARGS__)                    \
        );                                                                    \
    }                                                                         \
, /* Nothing to do */                                                         \
)                                                                             \

#define M_WR4P_DECL_01_2(name, num, t)                                        \
            M_C(_, num)                                                       \



#define M_WR4P_PARTIAL_DEF_AS_P3(xlist)                                      \
    M_WR4P_PARTIAL_DEF_AS_P4 xlist

#define M_WR4P_PARTIAL_DEF_AS_P4(name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, subtype_oplist, inline_oplist, wrapped_oplist) \
    /* Define extern wrapped function */                                      \
    M_WR4P_EXPAND_LIST(M_WR4P_DEF_01, name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, wrapped_oplist, subtype_oplist,0) \

// Generate a function declaration based on the given prototype for an extern function.
#define M_WR4P_DEF_01(operator, suffix, name, name_t, name_it_t, wrap_oplist, type_oplist, rettype, ...) \
/* export method method if defined in the encapasulated object */             \
M_IF_METHOD(operator, wrap_oplist)                                            \
(                                                                             \
rettype M_C(name, suffix)(                                      \
    M_MAP3_C( M_WR4P_DEF_00_1, (name_t, name_it_t, ), __VA_ARGS__)            \
    ) {                                                                       \
        M_IF(M_KEYWORD_P(void, rettype))(/*nothing*/, return)                 \
        M_C(M_CALL_, operator)(type_oplist,                                   \
            M_MAP3_C( M_WR4P_DECL_01_2, name, __VA_ARGS__)                    \
        );                                                                    \
    }                                                                         \
, /* Nothing to do */                                                         \
)                                                                             \



/* Define the list of operators to be expanded
 * - macro is the function expansion in function of the given argument
 * - name is the prefix user to wrap the container into another
 * - name_t is the type of the wrapper container
 * - name_it_t it the type of the iterator of the wrapper container
 * - subtype_t is the type of the element of the wrapped container
 * - key_type_t is the key type of the iterator of the wrapped container.
 * - value_type_t is the key type of the iterator of the wrapped container.
 * - isMap: 1 if associative array, 0 otherwise */
#define M_WR4P_EXPAND_LIST(macro, name, name_t, name_it_t, subtype_t, key_type_t, value_type_t, wrap_oplist, type_oplist, isMap) \
macro(INIT, _init, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE) \
macro(INIT_SET, _init_set, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE) \
macro(SET, _set, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE) \
macro(INIT_MOVE, _init_move, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE) \
macro(MOVE, _move, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE) \
macro(CLEAR, _clear, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE) \
macro(CLEAN, _clean, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE) \
macro(SWAP, _swap, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE) \
macro(HASH, _hash, name, name_t, name_it_t, wrap_oplist, type_oplist, size_t, TYPE) \
macro(EQUAL, _equal_p, name, name_t, name_it_t, wrap_oplist, type_oplist, bool, TYPE, TYPE) \
macro(CMP, _cmp, name, name_t, name_it_t, wrap_oplist, type_oplist, int, TYPE, TYPE) \
macro(EMPTY_P, _empty_p, name, name_t, name_it_t, wrap_oplist, type_oplist, bool, TYPE) \
macro(GET_SIZE, _size, name, name_t, name_it_t, wrap_oplist, type_oplist, size_t, TYPE) \
macro(SORT, _special_sort, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, M_C(name, _cmp_func_ct) ) \
macro(UPDATE, _update, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, subtype_t) \
macro(SPLICE_BACK, _splice_back, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE, IT_TYPE) \
macro(SPLICE_AT, _splice_at, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, IT_TYPE, TYPE, IT_TYPE) \
macro(IT_FIRST, _it, name, name_t, name_it_t, wrap_oplist, type_oplist, void, IT_TYPE, TYPE) \
macro(IT_LAST, _it_last, name, name_t, name_it_t, wrap_oplist, type_oplist, void, IT_TYPE, TYPE) \
macro(IT_END, _it_end, name, name_t, name_it_t, wrap_oplist, type_oplist, void, IT_TYPE, TYPE) \
macro(IT_SET, _it_set, name, name_t, name_it_t, wrap_oplist, type_oplist, void, IT_TYPE, IT_TYPE) \
macro(IT_END_P, _end_p, name, name_t, name_it_t, wrap_oplist, type_oplist, bool, IT_TYPE) \
macro(IT_LAST_P, _last_p, name, name_t, name_it_t, wrap_oplist, type_oplist, bool, IT_TYPE) \
macro(IT_EQUAL_P, _it_equal_p, name, name_t, name_it_t, wrap_oplist, type_oplist, bool, IT_TYPE, IT_TYPE) \
macro(IT_NEXT, _next, name, name_t, name_it_t, wrap_oplist, type_oplist, void, IT_TYPE) \
macro(IT_PREVIOUS, _previous, name, name_t, name_it_t, wrap_oplist, type_oplist, void, IT_TYPE) \
M_IF(isMap)(M_C(macro, _IS_MAP), macro)(IT_REF, _ref, name, name_t, name_it_t, wrap_oplist, type_oplist, subtype_t const *, IT_TYPE) \
M_IF(isMap)(M_C(macro, _IS_MAP), macro)(IT_CREF, _cref, name, name_t, name_it_t, wrap_oplist, type_oplist, subtype_t const *, IT_TYPE) \
macro(IT_REMOVE, _remove, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, IT_TYPE) \
macro(IT_INSERT, _insert, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, IT_TYPE, subtype_t) \
macro(ADD, _add, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE, TYPE) \
macro(SUB, _sub, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE, TYPE) \
macro(MUL, _mul, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE, TYPE) \
macro(DIV, _div, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, TYPE, TYPE) \
macro(GET_KEY, _get, name, name_t, name_it_t, wrap_oplist, type_oplist, value_type_t *, TYPE, key_type_t) \
macro(GET_SET_KEY, _get_at, name, name_t, name_it_t, wrap_oplist, type_oplist, value_type_t *, TYPE, key_type_t) \
macro(SET_KET, _set_at, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, key_type_t, value_type_t) \
macro(ERASE_KEY, _erase, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, key_type_t) \
macro(PUSH, _push, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, subtype_t) \
macro(POP, _pop, name, name_t, name_it_t, wrap_oplist, type_oplist, void, subtype_t *, TYPE) \
macro(PUSH_MOVE, _push_move, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, subtype_t *) \
macro(POP_MOVE, _pop_move, name, name_t, name_it_t, wrap_oplist, type_oplist, void, subtype_t *, TYPE) \
macro(REVERSE, _reverse, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE) \
macro(GET_STR, _get_str, name, name_t, name_it_t, wrap_oplist, type_oplist, void, string_t, TYPE) \
macro(PARSE_STR, _parse_str, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, const char *, const char **) \
macro(OUT_STR, _out_str, name, name_t, name_it_t, wrap_oplist, type_oplist, void, FILE *, TYPE) \
macro(IN_STR, _in_str, name, name_t, name_it_t, wrap_oplist, type_oplist, void, TYPE, FILE *) \
macro(OUT_SERIAL, _out_serial, name, name_t, name_it_t, wrap_oplist, type_oplist, m_serial_return_code_t, m_serial_write_t, TYPE) \
macro(IN_SERIAL, _in_serial, name, name_t, name_it_t, wrap_oplist, type_oplist, m_serial_return_code_t, TYPE, m_serial_read_t) \

#endif
