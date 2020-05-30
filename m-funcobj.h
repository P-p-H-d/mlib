/*
 * M*LIB - Function Object module
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
#ifndef MSTARLIB_FUNCOBJ_H
#define MSTARLIB_FUNCOBJ_H

#include "m-core.h"

/* Define a function object interface of name 'name'
 * with a function like retcode, type of param1, type of param 2, ...
 * USAGE:
 *    FUNC_OBJ_ITF_DEF(name, retcode type, type of param1, type of param 2, ...)
 */
#define FUNC_OBJ_ITF_DEF(name, ...)                                     \
  M_IF_NARGS_EQ1(__VA_ARGS__)(FUNCOBJI_ITF_NO_PARAM_DEF, FUNCOBJI_ITF_PARAM_DEF)(name, __VA_ARGS__)


/* Define a function object instance of name 'name' based on the interface 'base_name'
 * The function is defined as per :
 * - the prototype of the inherited interface
 * - the parameters of the function are named as per the list param_list
 * - the core of the function shall be defined in callback_core
 * - optionals member attributes of the function object can be defined after the core
 * (just like for tuple & variant: (name, type [, oplist])
 * 
 * In the core of the function, parameters are accessible just like a normal like
 * and a special variable 'self' which refers to the function object itself
 * can be used to access member attributes.
 * 
 * There shall be **exactly** the same number of parameters in 'param_list' than
 * the number of parameters of the interface 'base_name'
 *
 * USAGE/EXAMPLE:
 *   FUNC_OBJ_INS_DEF(name, base_name, (param1, ...), { return param1 * self->member1 }, (member1, int), ...)
 */
#define FUNC_OBJ_INS_DEF(name, base_name, param_list, ...)              \
  M_IF_NARGS_EQ1(__VA_ARGS__)(FUNCOBJI_INS_NO_ATTR_DEF, FUNCOBJI_INS_ATTR_DEF)(name, base_name, param_list, __VA_ARGS__)


/* OPLIST of the instanced function object
 * USAGE:
 *  FUNC_OBJ_INS_OPLIST(name, oplist of the attr1, ...)
 */
#define FUNC_OBJ_INS_OPLIST(...)                                        \
  M_IF_NARGS_EQ1(__VA_ARGS__)(FUNCOBJI_INS_NO_ATTR_OPLIST, FUNCOBJI_INS_ATTR_OPLIST_P1)( __VA_ARGS__)



/********************************************************************************/
/********************************** INTERNAL ************************************/
/********************************************************************************/

/* To be used by M_IF_FUNCOBJ macro.
   NOTE: It is reversed (0 instead of 1) so that it can be used in M_IF reliabely.
*/
#define M_FUNCOBJ_IS_NOT_DEFINED 0

/* Design Constraints:
 *  callback SHALL be the first member of the structures in all the definitions.
 *
 * Structure definitions are specialized in function of the presence or not
 * of parameters and/or attributes 
 * FIXME: How to factorize reasonnably well between the definitions?
 */

/* Specialization of the OPLIST in function if there is at least one member or not */
#define FUNCOBJI_INS_NO_ATTR_OPLIST(name)				\
  (NAME(name), TYPE(M_C(name, _t)),                                     \
   CLEAR(M_C(name, _clear)),                                            \
   INIT(M_C(name,_init))                                                \
   )

/* Validate the oplist before going further */
#define FUNCOBJI_INS_ATTR_OPLIST_P1(name, ...)				\
  M_IF(M_REDUCE(M_OPLIST_P, M_AND, __VA_ARGS__))(FUNCOBJI_INS_ATTR_OPLIST_P3, FUNCOBJI_INS_ATTR_OPLIST_FAILURE)(name, __VA_ARGS__)

/* Prepare a clean compilation failure */
#define FUNCOBJI_INS_ATTR_OPLIST_FAILURE(name, ...)			\
  ((M_LIB_ERROR(ONE_ARGUMENT_OF_FUNC_OBJ_INS_OPLIST_IS_NOT_AN_OPLIST, name, __VA_ARGS__)))

/* Define at least the oplist */
#define FUNCOBJI_INS_ATTR_OPLIST_P3(name, ...)				\
  (NAME(name), TYPE(M_C(name, _t)),                                     \
   INIT_WITH(M_C(name, _init_with)),                                    \
   CLEAR(M_C(name, _clear)),                                            \
   M_IF_METHOD_ALL(INIT, __VA_ARGS__)(INIT(M_C(name,_init)),),          \
   )


/* Specialization of the definition a function object interface of name 'name'
 * with a function like retcode () that doesn't have any input parameters.
 */
#define FUNCOBJI_ITF_NO_PARAM_DEF(name, retcode)                        \
  typedef retcode M_C(name, _retcode_t);                                \
                                                                        \
  struct M_C(name, _s);                                                 \
                                                                        \
  /* No parameters to the callback */                                   \
  typedef retcode(*M_C(name, _callback_t))(struct M_C(name, _s) *);     \
                                                                        \
  typedef struct M_C(name, _s) {                                        \
     M_C(name, _callback_t) callback;                                   \
  } M_C(name, _t)[1];                                                   \
                                                                        \
  static inline retcode                                                 \
  M_C(name, _call)(M_C(name, _t) funcobj)                               \
  {                                                                     \
    M_IF(M_KEYWORD_P(void, retcode)) ( /* nothing */,return)            \
      funcobj->callback(funcobj);                                       \
  }


/* Specialization of the definition a function object interface of name 'name'
 * with a function like retcode, type of param1, type of param 2, ...
 * with mandatory input parameters.
 */
#define FUNCOBJI_ITF_PARAM_DEF(name, retcode, ...)                      \
  typedef retcode M_C(name, _retcode_t);                                \
                                                                        \
  /* Define types for all paremeters */                                 \
  M_MAP3(FUNCOBJI_BASE_TYPE, name, __VA_ARGS__)                         \
                                                                        \
  struct M_C(name, _s);                                                 \
                                                                        \
  typedef retcode(*M_C(name, _callback_t))(struct M_C(name, _s) *, __VA_ARGS__); \
                                                                        \
  typedef struct M_C(name, _s) {                                        \
     M_C(name, _callback_t) callback;                                   \
  } M_C(name, _t)[1];                                                   \
                                                                        \
  static inline retcode                                                 \
  M_C(name, _call)(M_C(name, _t) funcobj                                \
                   M_MAP3(FUNCOBJI_BASE_ARGLIST, name, __VA_ARGS__) )   \
  {                                                                     \
    M_IF(M_KEYWORD_P(void, retcode)) ( /* nothing */,return)            \
      funcobj->callback(funcobj M_MAP3(FUNCOBJI_BASE_ARGCALL, name, __VA_ARGS__) ); \
  }


/* Specialization of the definition a function object instance of name 'name'
 * with no member attribute.
 */
#define FUNCOBJI_INS_NO_ATTR_DEF(name, base_name, param_list, callback_core) \
  typedef struct M_C(name, _s) {                                        \
    M_C(base_name, _callback_t) callback;                               \
  } M_C(name, _t)[1];                                                   \
                                                                        \
   static inline M_C(base_name, _retcode_t)                             \
   M_C(name, _callback)(M_C(base_name, _t) _self                        \
                        M_IF_EMPTY(M_OPFLAT param_list)(                \
                           /* No param */,                              \
                           M_MAP3(FUNCOBJI_INS_ARGLIST, base_name, M_OPFLAT param_list)\
                                                                      ) \
                       )                                                \
   {                                                                    \
     struct M_C(name, _s) *self = (struct M_C(name, _s) *)_self;        \
     (void) self; /* maybe unused */                                    \
     callback_core;                                                     \
   }                                                                    \
                                                                        \
   static inline void                                                   \
   M_C(name, _init_with)(M_C(name, _t) obj)                             \
   {                                                                    \
     obj->callback = M_C(name, _callback);                              \
   }                                                                    \
                                                                        \
   static inline void                                                   \
   M_C(name, _clear)(M_C(name, _t) obj)                                 \
   {                                                                    \
     (void) obj; /* nothing to do */                                    \
   }                                                                    \
                                                                        \
   static inline struct M_C(base_name, _s) *                            \
   M_C(name, _as_interface)(M_C(name, _t) obj)                          \
   {                                                                    \
     return (struct M_C(base_name, _s) *) obj;                          \
   }                                                                    \
                                                                        \
   static inline void                                                   \
   M_C(name, _init)(M_C(name, _t) obj)                                  \
   {                                                                    \
     obj->callback = M_C(name, _callback);                              \
   }                                                                    \
   

/* Specialization of the definition a function object instance of name 'name'
 * with mandatory member attribute.
 * First inject oplist in member attributes.
 */
#define FUNCOBJI_INS_ATTR_DEF(name, base_name, param_list, callback_core, ...) \
  FUNCOBJI_INS_ATTR_DEF_P2(name, base_name, param_list, callback_core, FUNCOBJI_INJECT_GLOBAL(__VA_ARGS__) )

/* Inject the oplist within the list of arguments */
#define FUNCOBJI_INJECT_GLOBAL(...)               \
  M_MAP_C(FUNCOBJI_INJECT_OPLIST_A, __VA_ARGS__)
/* Transform (x, type) into (x, type, oplist) if there is global registered oplist 
   or (x, type, M_DEFAULT_OPLIST) if there is no global one,
   or keep (x, type, oplist) if oplist was already present */
#define FUNCOBJI_INJECT_OPLIST_A( duo_or_trio )   \
  FUNCOBJI_INJECT_OPLIST_B duo_or_trio
#define FUNCOBJI_INJECT_OPLIST_B( f, ... )                                \
  M_IF_NARGS_EQ1(__VA_ARGS__)( (f, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)()), (f, __VA_ARGS__) )

// Test if all third argument of all arguments is an oplist
#define FUNCOBJI_IF_ALL_OPLIST(...)                               \
  M_IF(M_REDUCE(FUNCOBJI_IS_OPLIST_P, M_AND, __VA_ARGS__))
// Test if the third argument is an oplist
#define FUNCOBJI_IS_OPLIST_P(a)                   \
  M_OPLIST_P(M_RET_ARG3 a)

/* Validate the oplist before going further */
#define FUNCOBJI_INS_ATTR_DEF_P2(name, base_name, param_list, callback_core, ...) \
  FUNCOBJI_IF_ALL_OPLIST(__VA_ARGS__)(FUNCOBJI_INS_ATTR_DEF_P3, FUNCOBJI_INS_ATTR_DEF_FAILURE)(name, base_name, param_list, callback_core, __VA_ARGS__)

/* Stop processing with a compilation failure */
#define FUNCOBJI_INS_ATTR_DEF_FAILURE(name, base_name, param_list, callback_core, ...) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(FUNC_OBJ_INS_DEF): at least one of the given argument is not a valid oplist: " #__VA_ARGS__)

/* Expand the Function Object with members */
#define FUNCOBJI_INS_ATTR_DEF_P3(name, base_name, param_list, callback_core, ...) \
  typedef struct M_C(name, _s) {                                        \
    /* Callback is the mandatory first argument */                      \
    M_C(base_name, _callback_t) callback;                               \
    /* All the member attribute of the Function Object */               \
    M_MAP(FUNCOBJI_INS_ATTR_STRUCT, __VA_ARGS__)                        \
   } M_C(name, _t)[1];                                                  \
                                                                        \
   FUNCOBJI_CONTROL_ALL_OPLIST(name, __VA_ARGS__)                       \
                                                                        \
   static inline M_C(base_name, _retcode_t)                             \
   M_C(name, _callback)(M_C(base_name, _t) _self                        \
                        M_IF_EMPTY(M_OPFLAT param_list)(                \
                          /* No param */,                               \
                          M_MAP3(FUNCOBJI_INS_ARGLIST, base_name, M_OPFLAT param_list)\
                                                                      ) \
                       )                                                \
   {                                                                    \
     struct M_C(name, _s) *self = (struct M_C(name, _s) *)_self;        \
     (void) self; /* maybe unused */                                    \
     callback_core;                                                     \
   }                                                                    \
                                                                        \
   static inline void                                                   \
   M_C(name, _init_with)(M_C(name, _t) obj M_MAP(FUNCOBJI_INS_ATTR_LIST, __VA_ARGS__)) \
   {                                                                    \
     obj->callback = M_C(name, _callback);                              \
     M_MAP(FUNCOBJI_INS_ATTR_INIT_SET, __VA_ARGS__);                    \
   }                                                                    \
                                                                        \
   static inline void                                                   \
   M_C(name, _clear)(M_C(name, _t) obj)                                 \
   {                                                                    \
     M_MAP(FUNCOBJI_INS_ATTR_CLEAR, __VA_ARGS__);                       \
   }                                                                    \
                                                                        \
   static inline struct M_C(base_name, _s) *                            \
   M_C(name, _as_interface)(M_C(name, _t) obj)                          \
   {                                                                    \
     return (struct M_C(base_name, _s) *) obj;                          \
   }                                                                    \
                                                                        \
   M_IF(FUNCOBJI_TEST_METHOD_P(INIT, __VA_ARGS))                        \
   (                                                                    \
    static inline void                                                  \
    M_C(name, _init)(M_C(name, _t) obj)                                 \
    {                                                                   \
      obj->callback = M_C(name, _callback);                             \
      M_MAP(FUNCOBJI_INS_ATTR_INIT, __VA_ARGS__);                       \
    }                                                                   \
    , /* END OF INIT METHOD */ )                                        \



/* Define a numbered type of a parameter of the callback*/
#define FUNCOBJI_BASE_TYPE(name, num, type)     \
  typedef type M_C4(name, _param_, num, _t);

/* Define a list of the type of arguments for a function definition */
#define FUNCOBJI_BASE_ARGLIST(name, num, type)  \
  M_DEFERRED_COMMA type M_C(param_, num)

/* Define a list of arguments for a function call */
#define FUNCOBJI_BASE_ARGCALL(name, num, type)  \
  M_DEFERRED_COMMA M_C(param_, num)


/* Helper macros */
/* arg = (name, type [, oplist]) */
#define FUNCOBJI_INS_ATTR_STRUCT(arg)                  \
  M_RET_ARG2 arg M_RET_ARG1 arg;

#define FUNCOBJI_INS_ATTR_LIST(arg)             \
  M_DEFERRED_COMMA M_RET_ARG2 arg const M_RET_ARG1 arg

#define FUNCOBJI_INS_ATTR_INIT(arg)                                     \
  M_CALL_INIT(M_RET_ARG3 arg, obj -> M_RET_ARG1 arg);

#define FUNCOBJI_INS_ATTR_INIT_SET(arg)                                 \
  M_CALL_INIT_SET(M_RET_ARG3 arg, obj -> M_RET_ARG1 arg, M_RET_ARG1 arg);

#define FUNCOBJI_INS_ATTR_CLEAR(arg)                                    \
  M_CALL_CLEAR(M_RET_ARG3 arg, obj -> M_RET_ARG1 arg);

/* Define the list of arguments of the instance of the callback */
#define FUNCOBJI_INS_ARGLIST(name, num, param)          \
  M_DEFERRED_COMMA M_C4(name, _param_, num, _t) param


/* Macros for testing for a method presence in all the attributes */
#define FUNCOBJI_TEST_METHOD2_P(method, op)     \
  M_TEST_METHOD_P(method, op)
#define FUNCOBJI_TEST_METHOD1_P(method, arg)                            \
  M_APPLY(FUNCOBJI_TEST_METHOD2_P, method, M_RET_ARG3 arg)
#define FUNCOBJI_TEST_METHOD_P(method, ...)                             \
  M_IF(M_REDUCE2(FUNCOBJI_TEST_METHOD1_P, M_AND, method, __VA_ARGS__))

/* Macro for checking compatible type and oplist for all the attributes */
#define FUNCOBJI_CONTROL_ALL_OPLIST(name, ...)          \
  M_MAP2(FUNCOBJI_CONTROL_OPLIST, name, __VA_ARGS__)
#define FUNCOBJI_CONTROL_OPLIST(name, a)                                \
  M_CHECK_COMPATIBLE_OPLIST(name, M_RET_ARG1 a, M_RET_ARG2 a, M_RET_ARG3 a)


#endif
