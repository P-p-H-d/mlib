/*
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

#include "m-core.h"

#ifndef M_CLASS_CPT

// TODO: Integrate default case with dynamic polymorphisme
// TODO: WIP...

// Global counter
#define M_CLASS_CPT 0

// In case
#define M_REGISTER_TYPE "m-register.h"

// TODO: Iterator shall also be considered as key
#define M_METHOD_EXPAND(method, id)                                     \
  CAT3(CLASS_,id,_TYPE)*: CAT3(CLASS_,id,_DEFINITION).method,           \
    const CAT3(CLASS_,id,_TYPE)*: CAT3(CLASS_,id,_DEFINITION).method

// TODO: How to define Default method if there is no registered method?
// the current way may produce syntaxically invalid C due to macro expanding to
// wrong term...
#define M_CALL_METHOD(method, def, ...)                                 \
  _Generic(&(M_RET_ARG1(__VA_ARGS__, )),                                \
           M_SEQ2(0,DEC(CLASS_CPT),M_METHOD_EXPAND, method))(__VA_ARGS__) \
  
#define M_CALL_METHOD2(method, def, ...)                                \
  _Generic(&(M_RET_ARG2(__VA_ARGS__, )),                                \
           M_SEQ2(0,DEC(CLASS_CPT),M_METHOD_EXPAND, method))(__VA_ARGS__) \

#define M_CLASS_DEFINE_FIELD(a)     M_APPLY(M_CLASS_DEFINE_FIELD0 , CAT(M_PROTO_TUPLE_, a))
#define M_CLASS_M_GET_FUNCPTR(a)      M_APPLY(M_CLASS_M_GET_FUNCPTR0,   CAT(M_PROTO_TUPLE_, a))
#define M_CLASS_DEFINE_TYPE(a)      M_APPLY(M_CLASS_DEFINE_TYPE0 ,  CAT(M_PROTO_TUPLE_, a))
#define M_CLASS_DEFINE_FIELD0(a, b) M_IF(M_BOOL(CAT(M_PROTO_ISMETHOD_, a)))(CAT(M_PROTO_METHOD_, a)(TYPE, a);,/*nothing*/)
#define M_CLASS_M_GET_FUNCPTR0(a,b)   M_IF(M_BOOL(CAT(M_PROTO_ISMETHOD_, a)))(.a = b M_DEFERRED_COMMA, /*nothing*/)
#define M_CLASS_DEFINE_TYPE0(a, b)  M_IF(M_BOOL(CAT(M_PROTO_ISMETHOD_, a)))(/*nothing*/, typedef b CAT(TYPE_, a);)

/* Define basic supported methods and types */
#define M_PROTO_TUPLE_INIT(value)            INIT, value
#define M_PROTO_TUPLE_INIT_SET(value)        INIT_SET, value
#define M_PROTO_TUPLE_SET(value)             SET, value
#define M_PROTO_TUPLE_CLEAR(value)           CLEAR, value
#define M_PROTO_TUPLE_EQUAL(value)           EQUAL, value
#define M_PROTO_TUPLE_IT_TYPE(value)         IT_TYPE, value
#define M_PROTO_TUPLE_PUSH(value)            PUSH, value
#define M_PROTO_TUPLE_POP(value)             POP, value
#define M_PROTO_TUPLE_PRINT(value)           PRINT, value
#define M_PROTO_TUPLE_TOSTR(value)           TOSTR, value

// Define methods
#define M_PROTO_METHOD_INIT(type, alias)     void (*alias)(type)
#define M_PROTO_METHOD_INIT_SET(type, alias) void (*alias)(type, const type)
#define M_PROTO_METHOD_SET(type, alias)      void (*alias)(type, const type)
#define M_PROTO_METHOD_CLEAR(type, alias)    void (*alias)(type)
#define M_PROTO_METHOD_EQUAL(type, alias)    bool (*alias)(const type, const type)
#define M_PROTO_METHOD_PUSH(type, alias)     void (*alias)(type, const CAT(type, _TYPE))
#define M_PROTO_METHOD_POP(type, alias)      void (*alias)(CAT(type, _TYPE)*, type)
#define M_PROTO_METHOD_PRINT(type, alias)    void (*alias)(const type)
#define M_PROTO_METHOD_TOSTR(type, alias)    void (*alias)(string_t, const type)

// Define types
#define M_PROTO_ISMETHOD_IT_TYPE            0
#define M_PROTO_ISMETHOD_TYPE               0
  
#define INIT_DEFLIST                                                    \
  char: "%c",                                                           \
    signed char: "%hhd",                                                \
    unsigned char: "%hhu",                                              \
    signed short: "%hd",                                                \
    unsigned short: "%hu",                                              \
    signed int: "%d",                                                   \
    unsigned int: "%u",                                                 \
    long int: "%ld",                                                    \
    unsigned long int: "%lu",                                           \
    long long int: "%lld",                                              \
    unsigned long long int: "%llu",                                     \
    float: "%f",                                                        \
    double: "%f",                                                       \
    long double: "%Lf",                                                 \
    const char *: "%s",                                                 \
    char *: "%s",                                                       \
    const void *: "%p",                                                 \
    void *: "%p"

#define new(type)      ({type *tmp = malloc(sizeof (type)); init(*tmp); tmp; })
#define init(...)      M_CALL_METHOD(INIT,     M_INIT_DEFAULT,  __VA_ARGS__)
#define init_set(...)  M_CALL_METHOD(INIT_SET, M_SET_DEFAULT,   __VA_ARGS__)
#define set(...)       M_CALL_METHOD(SET,      M_SET_DEFAULT,   __VA_ARGS__)
#define clear(...)     M_CALL_METHOD(CLEAR,    M_CLEAR_DEFAULT, __VA_ARGS__)
#define delete(obj)    (clear(*obj), free(obj))
#define equal(...)     M_CALL_METHOD(EQUAL,    M_EQUAL_DEFAULT, __VA_ARGS__)
#define push(...)      M_CALL_METHOD(PUSH,     M_NO_DEFAULT,    __VA_ARGS__)
#define pop(...)       M_CALL_METHOD2(POP,     M_NO_DEFAULT,    __VA_ARGS__)
#define print_arg(...) M_CALL_METHOD(PRINT,    M_PRINT_ARG,     __VA_ARGS__) ;
#define print(...)     do { M_MAP(print_arg, __VA_ARGS__) } while (0)

#endif

#ifdef TYPE

#ifndef OPLIST
# error "OPLIST macro not defined."
#endif
 
/* Do the stuff */
#if CLASS_CPT == 0

typedef TYPE CLASS_0_TYPE;
M_MAP(M_CLASS_DEFINE_TYPE, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
static const struct {
  M_MAP(M_CLASS_DEFINE_FIELD, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
} CLASS_0_DEFINITION = {
  M_MAP(M_CLASS_M_GET_FUNCPTR, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
};
# undef TYPE
# undef OPLIST
# undef CLASS_CPT
# define CLASS_CPT 1

#elif CLASS_CPT == 1

typedef TYPE CLASS_1_TYPE;
M_MAP(M_CLASS_DEFINE_TYPE, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
static const struct {
  M_MAP(M_CLASS_DEFINE_FIELD, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
} CLASS_1_DEFINITION = {
  M_MAP(M_CLASS_M_GET_FUNCPTR, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
};
# undef TYPE
# undef OPLIST
# undef CLASS_CPT
# define CLASS_CPT 2

#elif CLASS_CPT == 2

typedef TYPE CLASS_2_TYPE;
M_MAP(M_CLASS_DEFINE_TYPE, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
static const struct {
  M_MAP(M_CLASS_DEFINE_FIELD, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
} CLASS_2_DEFINITION = {
  M_MAP(M_CLASS_M_GET_FUNCPTR, M_OPLIST_APPLY(M_OPLIST_FLAT, OPLIST))
};
# undef TYPE
# undef OPLIST
# undef CLASS_CPT
# define CLASS_CPT 3

#else
# error "Out of scope"
#endif

#endif
