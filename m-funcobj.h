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
#define MSTARLIB_FUNCOBJ_H 1

#include "m-core.h"

/* Define a function object interface of name 'name'
 * with a function like retcode, type of param1, type of param 2, ...
 * USAGE:
 *    FUNC_OBJ_ITF_DEF(name, retcode type, type of param1, type of param 2, ...)
 */
#define FUNC_OBJ_ITF_DEF(name, ...)                                     \
  M_IF_NARGS_EQ1(__VA_ARGS__)(FUNC_OBJ_ITF_NO_PARAM_DEF, FUNC_OBJ_ITF_PARAM_DEF)(name, __VA_ARGS__)


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
  M_IF_NARGS_EQ1(__VA_ARGS__)(FUNC_OBJ_INS_NO_ATTR_DEF, FUNC_OBJ_INS_ATTR_DEF)(name, base_name, param_list, __VA_ARGS__)

/* OPLIST of the instanced function object
 * USAGE:
 *  FUNC_OBJ_INS_OPLIST(name, oplist of the attr1, ...)
 */
#define FUNC_OBJ_INS_OPLIST(...)                                        \
  M_IF_NARGS_EQ1(__VA_ARGS__)(FUNC_OBJ_INS_NO_ATTR_OPLIST, FUNC_OBJ_INS_ATTR_OPLIST)( __VA_ARGS__)



/********************************************************************************/
/********************************** INTERNAL ************************************/
/********************************************************************************/

/* Design Constraints:
 *  callback SHALL be the first member of the structures in all the definitions.
 *
 * Structure definitions are specialized in function of the presence or not
 * of parameters and/or attributes 
 * FIXME: How to factorize reasonnably well between definition?
 */


/* Specialization of the OPLIST in function if there is at least one member or not */
#define FUNC_OBJ_INS_NO_ATTR_OPLIST(name, ...)                          \
  (NAME(name), TYPE(M_C(name, _t)),                                     \
   CLEAR(M_C(name, _clear)),                                            \
   INIT(M_C(name,_init))                                                \
   )

#define FUNC_OBJ_INS_ATTR_OPLIST(name, ...)                             \
  (NAME(name), TYPE(M_C(name, _t)),                                     \
   INIT_WITH(M_C(name, _init_with)),                                    \
   CLEAR(M_C(name, _clear)),                                            \
   M_IF_METHOD_ALL(INIT, __VA_ARGS__)(INIT(M_C(name,_init)),),          \
   )


/* Specialization of the definition a function object interface of name 'name'
 * with a function like retcode () that doesn't have any input parameters.
 */
#define FUNC_OBJ_ITF_NO_PARAM_DEF(name, retcode)                        \
  typedef retcode M_C(name, _retcode_t);                                \
  struct M_C(name, _s);                                                 \
  typedef retcode(*M_C(name, _callback_t))(struct M_C(name, _s) *);     \
  typedef struct M_C(name, _s) {                                        \
     M_C(name, _callback_t) callback;                                   \
  } M_C(name, _t)[1];                                                   \
                                                                        \
  static inline retcode                                                 \
  M_C(name, _call)(M_C(name, _t) funcobj)                               \
  {                                                                     \
    M_IF(M_KEYWORD_P(void, retcode)) ( ,return)                         \
      funcobj->callback(funcobj);                                       \
  }


/* Specialization of the definition a function object interface of name 'name'
 * with a function like retcode, type of param1, type of param 2, ...
 * with mandatory input parameters.
 */
#define FUNC_OBJ_ITF_PARAM_DEF(name, retcode, ...)                      \
  typedef retcode M_C(name, _retcode_t);                                \
  M_MAP3(FUNC_OBJ_BASE_TYPE, name, __VA_ARGS__)                         \
  struct M_C(name, _s);                                                 \
  typedef retcode(*M_C(name, _callback_t))(struct M_C(name, _s) *, __VA_ARGS__); \
  typedef struct M_C(name, _s) {                                        \
     M_C(name, _callback_t) callback;                                   \
  } M_C(name, _t)[1];                                                   \
                                                                        \
  static inline retcode                                                 \
  M_C(name, _call)(M_C(name, _t) funcobj                                \
                   M_MAP3(FUNC_OBJ_BASE_ARGLIST, name, __VA_ARGS__) )   \
  {                                                                     \
    M_IF(M_KEYWORD_P(void, retcode)) ( ,return)                         \
      funcobj->callback(funcobj M_MAP3(FUNC_OBJ_BASE_ARGCALL, name, __VA_ARGS__) ); \
  }


/* Specialization of the definition a function object instance of name 'name'
 * with no member attribute.
 */
#define FUNC_OBJ_INS_NO_ATTR_DEF(name, base_name, param_list, callback_core) \
  typedef struct M_C(name, _s) {                                        \
    M_C(base_name, _callback_t) callback;                               \
  } M_C(name, _t)[1];                                                   \
                                                                        \
   static inline M_C(base_name, _retcode_t)                             \
   M_C(name, _callback)(M_C(base_name, _t) obj                          \
                        M_IF_EMPTY(M_OPFLAT param_list)(                \
                           /* No param */,                              \
                           M_MAP3(FUNC_OBJ_INS_ARGLIST, base_name, M_OPFLAT param_list)\
                                                                      ) \
                       )                                                \
   {                                                                    \
     struct M_C(name, _s) *self = (struct M_C(name, _s) *)obj;          \
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
     /* nothing to do */                                                \
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
 */
#define FUNC_OBJ_INS_ATTR_DEF(name, base_name, param_list, callback_core, ...) \
  typedef struct M_C(name, _s) {                                        \
    M_C(base_name, _callback_t) callback;                               \
    M_MAP(FUNC_OBJ_INS_ATTR_STRUCT, __VA_ARGS__)                        \
   } M_C(name, _t)[1];                                                  \
                                                                        \
   static inline M_C(base_name, _retcode_t)                             \
   M_C(name, _callback)(M_C(base_name, _t) obj                          \
                        M_IF_EMPTY(M_OPFLAT param_list)(                \
                          /* No param */,                               \
                          M_MAP3(FUNC_OBJ_INS_ARGLIST, base_name, M_OPFLAT param_list)\
                                                                      ) \
                       )                                                \
   {                                                                    \
     struct M_C(name, _s) *self = (struct M_C(name, _s) *)obj;          \
     callback_core;                                                     \
   }                                                                    \
                                                                        \
   static inline void                                                   \
   M_C(name, _init_with)(M_C(name, _t) obj M_MAP(FUNC_OBJ_INS_ATTR_LIST, __VA_ARGS__)) \
   {                                                                    \
     obj->callback = M_C(name, _callback);                              \
     M_MAP(FUNC_OBJ_INS_ATTR_INIT_SET, __VA_ARGS__);                    \
   }                                                                    \
                                                                        \
   static inline void                                                   \
   M_C(name, _clear)(M_C(name, _t) obj)                                 \
   {                                                                    \
     M_MAP(FUNC_OBJ_INS_ATTR_CLEAR, __VA_ARGS__);                       \
   }                                                                    \
                                                                        \
   static inline struct M_C(base_name, _s) *                            \
   M_C(name, _as_interface)(M_C(name, _t) obj)                          \
   {                                                                    \
     return (struct M_C(base_name, _s) *) obj;                          \
   }                                                                    \
                                                                        \
   M_IF(FUNC_OBJ_TEST_METHOD_P(INIT, __VA_ARGS))                        \
   (                                                                    \
    static inline void                                                  \
    M_C(name, _init)(M_C(name, _t) obj)                                 \
    {                                                                   \
      obj->callback = M_C(name, _callback);                             \
      M_MAP(FUNC_OBJ_INS_ATTR_INIT, __VA_ARGS__);                       \
    }                                                                   \
    , /* END OF INIT METHOD */ )                                        \


/* Helper macros */
#define FUNC_OBJ_BASE_TYPE(name, num, type)     \
  typedef type M_C4(name, _param_, num, _t);

#define FUNC_OBJ_BASE_ARGLIST(name, num, type)  \
  M_DEFERRED_COMMA type M_C(param_, num)

#define FUNC_OBJ_BASE_ARGCALL(name, num, type) \
  M_DEFERRED_COMMA M_C(param_, num)


/* Helper macros */
/* arg = (name, type [, oplist]) */
#define FUNC_OBJ_INS_ATTR_STRUCT(arg)                  \
  M_RET_ARG2 arg M_RET_ARG1 arg;

#define FUNC_OBJ_INS_ATTR_LIST(arg)             \
  M_DEFERRED_COMMA M_RET_ARG2 arg M_RET_ARG1 arg

#define FUNC_OBJ_INS_ATTR_INIT(arg)                                     \
  M_CALL_INIT(FUNC_OBJ_INS_ATTR_GET_OPLIST(arg), obj -> M_RET_ARG1 arg);

#define FUNC_OBJ_INS_ATTR_INIT_SET(arg)                                 \
  M_CALL_INIT_SET(FUNC_OBJ_INS_ATTR_GET_OPLIST(arg), obj -> M_RET_ARG1 arg, M_RET_ARG1 arg);

#define FUNC_OBJ_INS_ATTR_CLEAR(arg)                                    \
  M_CALL_CLEAR(FUNC_OBJ_INS_ATTR_GET_OPLIST(arg), obj -> M_RET_ARG1 arg);
  
#define FUNC_OBJ_INS_ATTR_GET_OPLIST(arg)                                   \
  M_GLOBAL_OPLIST_OR_DEF(M_RET_ARG(M_NARGS arg, M_OPFLAT arg))()

#define FUNC_OBJ_INS_ARGLIST(name, num, param)          \
  M_DEFERRED_COMMA M_C4(name, _param_, num, _t) param


/* Macros for testing for a method presence */
#define FUNC_OBJ_TEST_METHOD2_P(method, op)     \
  M_TEST_METHOD_P(method, op)
#define FUNC_OBJ_TEST_METHOD1_P(method, arg)                            \
  M_APPLY(FUNC_OBJ_TEST_METHOD2_P, method, FUNC_OBJ_INS_ATTR_GET_OPLIST arg)
#define FUNC_OBJ_TEST_METHOD_P(method, ...)                             \
  M_IF(M_REDUCE2(FUNC_OBJ_TEST_METHOD1_P, M_AND, method, __VA_ARGS__))

#endif
