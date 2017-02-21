/*
 * M*LIB - Extended Pre-processing macros module
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
#ifndef __M_MACRO_H
#define __M_MACRO_H

#include <stddef.h>
#include <assert.h>

/***************************************************************/
/************************ Compiler Macro ***********************/
/***************************************************************/

#if !defined(NDEBUG)
# define M_ASSUME(x) assert(x)
#elif defined(__GNUC__)                         \
  && (__GNUC__ * 100 + __GNUC_MINOR__) >= 480
# define M_ASSUME(x)                                    \
  (! __builtin_constant_p (!!(x) || !(x)) || (x) ?      \
   (void) 0 : __builtin_unreachable())
#elif defined(_MSC_VER)
# define M_ASSUME(x) __assume(x)
#else
# define M_ASSUME(x) assert(x)
#endif

#ifdef __GNUC__
# define M_LIKELY(cond)   __builtin_expect(!!(cond), 1)
# define M_UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#else
# define M_LIKELY(cond)   (cond)
# define M_UNLIKELY(cond) (cond)
#endif


/***************************************************************/
/****************** Preprocessing Times Macro ******************/
/***************************************************************/

/* Maximum number of argument which can be handled by this header.
   Can be increased for future release of this header. */
#define M_MAX_NB_ARGUMENT 26

/* Basic handling of concatenation of symbols:
 * M_C, M_C3, M_C4
 */
#define M_C2I(a, ...)       a ## __VA_ARGS__
#define M_C(a, ...)         M_C2I(a, __VA_ARGS__)
#define M_C3I(a, b, ...)    a ## b ## __VA_ARGS__
#define M_C3(a, b, ...)     M_C3I(a ,b, __VA_ARGS__)
#define M_C4I(a, b, c, ...) a ## b ## c ## __VA_ARGS__
#define M_C4(a, b, c, ...)  M_C4I(a ,b, c, __VA_ARGS__)

/* Increment the number given in argument (from [0..29[) */
#define M_INC(x)          M_C(M_INC_, x)
#define M_INC_0 1
#define M_INC_1 2
#define M_INC_2 3
#define M_INC_3 4
#define M_INC_4 5
#define M_INC_5 6
#define M_INC_6 7
#define M_INC_7 8
#define M_INC_8 9
#define M_INC_9 10
#define M_INC_10 11
#define M_INC_11 12
#define M_INC_12 13
#define M_INC_13 14
#define M_INC_14 15
#define M_INC_15 16
#define M_INC_16 17
#define M_INC_17 18
#define M_INC_18 19
#define M_INC_19 20
#define M_INC_20 21
#define M_INC_21 22
#define M_INC_22 23
#define M_INC_23 24
#define M_INC_24 25
#define M_INC_25 26
#define M_INC_26 27
#define M_INC_27 28
#define M_INC_28 29
#define M_INC_29 OVERFLOW

/* Decrement the number given in argument (from [0..29[) */
#define M_DEC(x)          M_C(M_DEC_, x)
#define M_DEC_0 overflow
#define M_DEC_1 0
#define M_DEC_2 1
#define M_DEC_3 2
#define M_DEC_4 3
#define M_DEC_5 4
#define M_DEC_6 5
#define M_DEC_7 6
#define M_DEC_8 7
#define M_DEC_9 8
#define M_DEC_10 9
#define M_DEC_11 10
#define M_DEC_12 11
#define M_DEC_13 12
#define M_DEC_14 13
#define M_DEC_15 14
#define M_DEC_16 15
#define M_DEC_17 16
#define M_DEC_18 17
#define M_DEC_19 18
#define M_DEC_20 19
#define M_DEC_21 20
#define M_DEC_22 21
#define M_DEC_23 22
#define M_DEC_24 23
#define M_DEC_25 24
#define M_DEC_26 25
#define M_DEC_27 26
#define M_DEC_28 27
#define M_DEC_29 28

/* Return the nth argument:
   M_RET_ARG1
   M_RET_ARG2
   M_RET_ARG3
   M_RET_ARG27
 */
#define M_RETI_ARG1(a, ...)     a
#define M_RET_ARG1(...)         M_RETI_ARG1(__VA_ARGS__)

#define M_RETI_ARG2(a, b, ...)  b
#define M_RET_ARG2(...)         M_RETI_ARG2(__VA_ARGS__)

#define M_RETI_ARG3(a, b, c, ...)  c
#define M_RET_ARG3(...)         M_RETI_ARG3(__VA_ARGS__)

#define M_RETI_ARG27(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa, ...)    aa
#define M_RET_ARG27(...)        M_RETI_ARG27(__VA_ARGS__)


/* Convert an integer or a symbol into 0 (if 0) or 1 (if not 0).
   1 if symbol unknown */
#define M_TOBOOL_0                  1, 0,
#define M_BOOL(x)                   M_RET_ARG2(M_C(M_TOBOOL_, x), 1, useless)

/* Inverse 0 into 1 and 1 into 0 */
#define M_INV_0                     1
#define M_INV_1                     0
#define M_INV(x)                    M_C(M_INV_, x)

/* Perform a AND between the inputs */
#define M_AND_00                    0
#define M_AND_01                    0
#define M_AND_10                    0
#define M_AND_11                    1
#define M_AND(x,y)                  M_C3(M_AND_, x, y)

/* Perform a OR between the inputs */
#define M_OR_00                     0
#define M_OR_01                     1
#define M_OR_10                     1
#define M_OR_11                     1
#define M_OR(x,y)                   M_C3(M_OR_, x, y)

/* M_IF Macro :
   M_IF(42)(Execute if true, execute if false)
   Example: M_IF(0)(true_action, false_action) --> false_action */
#define M_IF_0(true_macro, ...)     __VA_ARGS__
#define M_IF_1(true_macro, ...)     true_macro
#define M_IF(c)                     M_C(M_IF_, M_BOOL(c))

/* Return 1 if comma inside the argument list, 0 otherwise */
#define M_COMMA_P(...)              M_RET_ARG27(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, useless)

/* Return 1 if the argument is 'empty', 0 otherwise.
    Handle: EMPTY_P(), EMPTY_P(x), EMPTY_P(()) and EMPTY_P(,) and EMPTY_P(f) with #define f() 2,3 */
#define M_EMPTY_DETECT(...)         0, 1,
#define M_EMPTY_P_C1(...)           M_COMMA_P(M_EMPTY_DETECT __VA_ARGS__ () )
#define M_EMPTY_P_C2(...)           M_COMMA_P(M_EMPTY_DETECT __VA_ARGS__)
#define M_EMPTY_P_C3(...)           M_COMMA_P(__VA_ARGS__ () )
#define M_EMPTY_P(...)              M_AND(M_EMPTY_P_C1(__VA_ARGS__), M_INV(M_OR(M_OR(M_EMPTY_P_C2(__VA_ARGS__), M_COMMA_P(__VA_ARGS__)),M_EMPTY_P_C3(__VA_ARGS__))))

/* Generate a comma later in the next evaluation pass. */
#define M_DEFERRED_COMMA            ,

/* M_IF macro for empty arguments:
    M_IF_EMPTY(arguments)(action if empty, action if not empty) */
#define M_IF_EMPTY(...)             M_IF(M_EMPTY_P(__VA_ARGS__))

/* Return 1 if argument is "()" or "(x)" */
#define M_PARENTHESIS_DETECT(...)   0, 1,
#define M_PARENTHESIS_P(x)          M_RET_ARG2(M_PARENTHESIS_DETECT x, 0, useless)

/* Necessary macros to handle recursivity */
/* Delay the evaluation by one level or two or three or ... */
/* The argument is a macro-function which has to be deferred */
#define M_DELAY0()
#define M_DELAY1(...)               __VA_ARGS__ M_DELAY0 ()
#define M_DELAY2(...)               __VA_ARGS__ M_DELAY1 (M_DELAY0) ()
#define M_DELAY3(...)               __VA_ARGS__ M_DELAY2 (M_DELAY0) ()
#define M_DELAY4(...)               __VA_ARGS__ M_DELAY3 (M_DELAY0) ()
#define M_DELAY5(...)               __VA_ARGS__ M_DELAY4 (M_DELAY0) ()
#define M_DELAY6(...)               __VA_ARGS__ M_DELAY5 (M_DELAY0) ()
#define M_DELAY7(...)               __VA_ARGS__ M_DELAY6 (M_DELAY0) ()
#define M_DELAY8(...)               __VA_ARGS__ M_DELAY7 (M_DELAY0) ()
#define M_DELAY9(...)               __VA_ARGS__ M_DELAY8 (M_DELAY0) ()

/* Perform 3^5 evaluation */
#define M_EVAL(...)                 M_EVAL1(M_EVAL1(M_EVAL1(__VA_ARGS__)))
#define M_EVAL1(...)                M_EVAL2(M_EVAL2(M_EVAL2(__VA_ARGS__)))
#define M_EVAL2(...)                M_EVAL3(M_EVAL3(M_EVAL3(__VA_ARGS__)))
#define M_EVAL3(...)                M_EVAL4(M_EVAL4(M_EVAL4(__VA_ARGS__)))
#define M_EVAL4(...)                M_EVAL0(M_EVAL0(M_EVAL0(__VA_ARGS__)))
#define M_EVAL0(...)                __VA_ARGS__

/* Apply Macro :
   It allows that a won't be evaluated until all arguments of 'a' have been already evaluated */
#define M_APPLY(a, ...) a (__VA_ARGS__)

/* Apply 'func' to arg if 'arg' is not empty */
/* Example: M_APPLY_FUNC(f, 2) ==> 'f(2)' and M_APPLY_FUNC(f, ) ==> '' */
#define M_APPLY_FUNC(func, arg)                 \
  M_IF(M_INV(M_EMPTY_P(arg)))(func(arg),)

/* Apply 'func' to arg1,arg2 if 'arg2' is not empty */
/* Example: M_APPLY_FUNC2(f, 2, 3) ==> 'f(2, 3)' and M_APPLY_FUNC2(f, 2, ) ==> '' */
#define M_APPLY_FUNC2(func, arg1, arg2)                 \
  M_IF(M_INV(M_EMPTY_P(arg2)))(func(arg1, arg2),)

/* Map a macro to all given arguments (non recursive version) */
/* Example: M_MAP(f,a, b, c) ==> f(a) f(b) f(c) */
#define M_MAP_0(func, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, ...) \
  M_APPLY_FUNC(func, a) M_APPLY_FUNC(func, b) M_APPLY_FUNC(func, c) M_APPLY_FUNC(func, d) M_APPLY_FUNC(func, e) \
  M_APPLY_FUNC(func, f) M_APPLY_FUNC(func, g) M_APPLY_FUNC(func, h) M_APPLY_FUNC(func, i) M_APPLY_FUNC(func, j) \
  M_APPLY_FUNC(func, k) M_APPLY_FUNC(func, l) M_APPLY_FUNC(func, m) M_APPLY_FUNC(func, n) M_APPLY_FUNC(func, o) \
  M_APPLY_FUNC(func, p) M_APPLY_FUNC(func, q) M_APPLY_FUNC(func, r) M_APPLY_FUNC(func, s) M_APPLY_FUNC(func, t) \
  M_APPLY_FUNC(func, u) M_APPLY_FUNC(func, v) M_APPLY_FUNC(func, w) M_APPLY_FUNC(func, x) M_APPLY_FUNC(func, y) \
  M_APPLY_FUNC(func, z)
#define M_MAP(f, ...) M_MAP_0(f, __VA_ARGS__, , , , , , , , , , , , , , , , , , , , , , , , , , )

/* Map a macro to all given arguments with one additional fixed data*/
/* Example: M_MAP2(f,data,a, b, c) ==> f(data,a) f(data,b) f(data,c) */
#define M_MAP2_0(func, data, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, ...) \
  M_APPLY_FUNC2(func, data, a) M_APPLY_FUNC2(func, data, b) M_APPLY_FUNC2(func, data, c) \
  M_APPLY_FUNC2(func, data, d) M_APPLY_FUNC2(func, data, e) M_APPLY_FUNC2(func, data, f) \
  M_APPLY_FUNC2(func, data, g) M_APPLY_FUNC2(func, data, h) M_APPLY_FUNC2(func, data, i) \
  M_APPLY_FUNC2(func, data, j) M_APPLY_FUNC2(func, data, k) M_APPLY_FUNC2(func, data, l) \
  M_APPLY_FUNC2(func, data, m) M_APPLY_FUNC2(func, data, n) M_APPLY_FUNC2(func, data, o) \
  M_APPLY_FUNC2(func, data, p) M_APPLY_FUNC2(func, data, q) M_APPLY_FUNC2(func, data, r) \
  M_APPLY_FUNC2(func, data, s) M_APPLY_FUNC2(func, data, t) M_APPLY_FUNC2(func, data, u) \
  M_APPLY_FUNC2(func, data, v) M_APPLY_FUNC2(func, data, w) M_APPLY_FUNC2(func, data, x) \
  M_APPLY_FUNC2(func, data, y) M_APPLY_FUNC2(func, data, z)
#define M_MAP2(f, ...) M_MAP2_0(f, __VA_ARGS__, , , , , , , , , , , , , , , , , , , , , , , , , , , )

/* Duplicate of macros for GET_METHOD as it may be called in context where a M_MAP2 is in progress */
#define M_APPLY_FUNC2B(func, arg1, arg2)           \
  M_IF(M_INV(M_EMPTY_P(arg2)))(func(arg1, arg2),)
#define M_MAP2B_0(func, data, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, ...) \
  M_APPLY_FUNC2B(func, data, a) M_APPLY_FUNC2B(func, data, b) M_APPLY_FUNC2B(func, data, c) \
  M_APPLY_FUNC2B(func, data, d) M_APPLY_FUNC2B(func, data, e) M_APPLY_FUNC2B(func, data, f) \
  M_APPLY_FUNC2B(func, data, g) M_APPLY_FUNC2B(func, data, h) M_APPLY_FUNC2B(func, data, i) \
  M_APPLY_FUNC2B(func, data, j) M_APPLY_FUNC2B(func, data, k) M_APPLY_FUNC2B(func, data, l) \
  M_APPLY_FUNC2B(func, data, m) M_APPLY_FUNC2B(func, data, n) M_APPLY_FUNC2B(func, data, o) \
  M_APPLY_FUNC2B(func, data, p) M_APPLY_FUNC2B(func, data, q) M_APPLY_FUNC2B(func, data, r) \
  M_APPLY_FUNC2B(func, data, s) M_APPLY_FUNC2B(func, data, t) M_APPLY_FUNC2B(func, data, u) \
  M_APPLY_FUNC2B(func, data, v) M_APPLY_FUNC2B(func, data, w) M_APPLY_FUNC2B(func, data, x) \
  M_APPLY_FUNC2B(func, data, y) M_APPLY_FUNC2B(func, data, z)
#define M_MAP2B(f, ...) M_MAP2B_0(f, __VA_ARGS__, , , , , , , , , , , , , , , , , , , , , , , , , , , )


/* Map a macro to all given pair of arguments (Using recursivity) */
/* Example: M_MAP_PAIR(f, a, b, c, d) ==> f(a,b) f(c,d) */
#define M_MAP_PAIR_L0_INDIRECT()      M_MAP_PAIR_L0
#define M_MAP_PAIR_L0(f, ...)         M_IF_NARGS_EQ2(__VA_ARGS__)( f(__VA_ARGS__) , M_MAP_PAIR_L1(f, __VA_ARGS__))
#define M_MAP_PAIR_L1(f, a, b, ...)   f(a,b) M_DELAY3(M_MAP_PAIR_L0_INDIRECT) () (f, __VA_ARGS__)
#define M_MAP_PAIR(f, ...)            M_IF_EMPTY(__VA_ARGS__)( /* end */, M_EVAL(M_MAP_PAIR_L0(f, __VA_ARGS__)))

/* Map a macro to all given arguments and reduce all theses computation
   with another reduce macro (using recursivity) */
/* Example: M_REDUCE(f, g, a, b, c) ==> g( f(a), g( f(b), f(c)) */
#define M_REDUCE_L0_INDIRECT()         M_REDUCE_L0
#define M_REDUCE_L1(f, g, cur, a, ...) M_DELAY3(M_REDUCE_L0_INDIRECT) () (f, g, g(cur, f(a)), __VA_ARGS__)
#define M_REDUCE_L0(f, g, cur, ...)    M_IF_NARGS_EQ1(__VA_ARGS__)( g(cur, f(__VA_ARGS__)) , M_REDUCE_L1(f, g, cur, __VA_ARGS__))
#define M_REDUCE1(f, g, a, ...)        M_EVAL(M_REDUCE_L0(f, g, f(a), __VA_ARGS__))
#define M_REDUCE(f, g, ...)            M_IF_NARGS_EQ1(__VA_ARGS__)( f(__VA_ARGS__), M_REDUCE1(f, g, __VA_ARGS__))

/* Map a macro to all given arguments and reduce all theses computation
   with another reduce macro with an argument (non-recursive way - should be way faster):
   M_REDUCE2(f, g, data, a, b, c) ==> g( f(data,a), g( f(data, b), f(data, c))) */
#define M_REDUCE2(f,g,d,...)                                            \
  M_RET_ARG27(__VA_ARGS__, M_REDUCEI_26, M_REDUCEI_25, M_REDUCEI_24,          \
              M_REDUCEI_23, M_REDUCEI_22, M_REDUCEI_21, M_REDUCEI_20,           \
              M_REDUCEI_19, M_REDUCEI_18, M_REDUCEI_17, M_REDUCEI_16,           \
              M_REDUCEI_15, M_REDUCEI_14, M_REDUCEI_13, M_REDUCEI_12,           \
              M_REDUCEI_11, M_REDUCEI_10, M_REDUCEI_9, M_REDUCEI_8,             \
              M_REDUCEI_7, M_REDUCEI_6, M_REDUCEI_5, M_REDUCEI_4,               \
              M_REDUCEI_3, M_REDUCEI_2, M_REDUCEI_1, useless)(f,g,d,__VA_ARGS__)
#define M_REDUCEI_1(f1,g1,d1,a)           f1(d1, a)
#define M_REDUCEI_2(f1,g1,d1,a,b)         g1(f1(d1, a),f1(d1,b))
#define M_REDUCEI_3(f1,g1,d1,a,b,c)       g1(g1(f1(d1, a),f1(d1,b)),f1(d1,c))
#define M_REDUCEI_4(f1,g1,d1,a,b,c,d)     g1(g1(f1(d1, a),f1(d1,b)),g1(f1(d1,c),f1(d1,d)))
#define M_REDUCEI_5(f1,g1,d1,a,b,c,d,e)   g1(g1(g1(f1(d1, a),f1(d1,b)),g1(f1(d1,c),f1(d1,d))),f1(d1,e))
#define M_REDUCEI_6(f1,g1,d1,a,b,c,d,e,f) g1(g1(g1(f1(d1, a),f1(d1,b)),f1(d1,c)),g1(g1(f1(d1,d),f1(d1,e)),f1(d1,f)))
#define M_REDUCEI_7(f1,g1,d1,a,b,c,d,e,f,g) g1(M_REDUCEI_6(f1,g1,d1,a,b,c,d,e,f),f1(d1,g))
#define M_REDUCEI_8(f1,g1,d1,a,b,c,d,e,f,g,h) g1(M_REDUCEI_4(f1,g1,d1,a,b,c,d),M_REDUCEI_4(f1,g1,d1,e,f,g,h))
#define M_REDUCEI_9(f1,g1,d1,a,b,c,d,e,f,g,h,i) g1(M_REDUCEI_8(f1,g1,d1,a,b,c,d,e,f,g,h),f1(d1,i))
#define M_REDUCEI_10(f1,g1,d1,a,b,c,d,e,f,g,h,i,j) g1(M_REDUCEI_5(f1,g1,d1,a,b,c,d,e),M_REDUCEI_5(f1,g1,d1,f,g,h,i,j))
#define M_REDUCEI_11(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k) g1(M_REDUCEI_10(f1,g1,d1,a,b,c,d,e,f,g,h,i,j),f1(d1,k))
#define M_REDUCEI_12(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l) g1(M_REDUCEI_6(f1,g1,d1,a,b,c,d,e,f),M_REDUCEI_6(f1,g1,d1,g,h,i,j,k,l))
#define M_REDUCEI_13(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m) g1(M_REDUCEI_12(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l),f1(d1,m))
#define M_REDUCEI_14(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n) g1(M_REDUCEI_7(f1,g1,d1,a,b,c,d,e,f,g),M_REDUCEI_7(f1,g1,d1,h,i,j,k,l,m,n))
#define M_REDUCEI_15(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) g1(M_REDUCEI_14(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n),f1(d1,o))
#define M_REDUCEI_16(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) g1(M_REDUCEI_8(f1,g1,d1,a,b,c,d,e,f,g,h),M_REDUCEI_8(f1,g1,d1,i,j,k,l,m,n,o,p))
#define M_REDUCEI_17(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) g1(M_REDUCEI_16(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p),f1(d1,q))
#define M_REDUCEI_18(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) g1(M_REDUCEI_9(f1,g1,d1,a,b,c,d,e,f,g,h,i),M_REDUCEI_9(f1,g1,d1,j,k,l,m,n,o,p,q,r))
#define M_REDUCEI_19(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) g1(M_REDUCEI_18(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r),f1(d1,s))
#define M_REDUCEI_20(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) g1(M_REDUCEI_10(f1,g1,d1,a,b,c,d,e,f,g,h,i,j),M_REDUCEI_9(f1,g1,d1,k,l,m,n,o,p,q,r,s,t))
#define M_REDUCEI_21(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) g1(M_REDUCEI_20(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t),f1(d1,u))
#define M_REDUCEI_22(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) g1(M_REDUCEI_11(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k),M_REDUCEI_9(f1,g1,d1,l,m,n,o,p,q,r,s,t,u,v))
#define M_REDUCEI_23(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) g1(M_REDUCEI_22(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v),f1(d1,w))
#define M_REDUCEI_24(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) g1(M_REDUCEI_11(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l),M_REDUCEI_11(f1,g1,d1,m,n,o,p,q,r,s,t,u,v,w,x))
#define M_REDUCEI_25(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) g1(M_REDUCEI_24(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x),f1(d1,y))
#define M_REDUCEI_26(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) g1(M_REDUCEI_12(f1,g1,d1,a,b,c,d,e,f,g,h,i,j,k,l,m),M_REDUCEI_12(f1,g1,d1,n,o,p,q,r,s,t,u,v,w,x,y,z))


/* Sequence of numerical (using recursivity) */
/* Example: M_SEQ(init,end, macro)==>macro(init), macro(init+1), ...macro(end) */
#define M_SEQ_L0_INDIRECT()           M_SEQ_L0
#define M_SEQ_L0(current,end,f)       M_IF(M_EQUAL(current, end))( f(current) , M_SEQ_L1(current, end, f))
#define M_SEQ_L1(current,end,f)       f(current) , M_DELAY3(M_SEQ_L0_INDIRECT) () (M_INC(current), end, f)
#define M_SEQ(init,end,f)             M_EVAL(M_SEQ_L0(init,end,f))

/* Sequence of numerical with data (using recursivity) */
/* Example: M_SEQ2(init, end, macro, data)==>macro(data, init), macro(data, init+1), ...macro(data, end) */
#define M_SEQ2_L0_INDIRECT()           M_SEQ2_L0
#define M_SEQ2_L0(current,end,f,d)     M_IF(M_EQUAL(current, end))( f(d, current) , M_SEQ2_L1(current, end, f, d))
#define M_SEQ2_L1(current,end,f,d)     f(d,current) , M_DELAY3(M_SEQ2_L0_INDIRECT) () (M_INC(current), end, f, d)
#define M_SEQ2(init,end,f,data)        M_EVAL(M_SEQ2_L0(init,end,f,data))

/* Return the input (delay evaluation) */
#define M_ID(...)                 __VA_ARGS__

/* M_NARGS: Return number of argument.
   (don't work for empty arg) */
#define M_NARGS(...)                                                    \
  M_RET_ARG27(__VA_ARGS__, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,  \
              15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, useless)

/* If the number of arguments is 1 */
#define M_IF_NARGS_EQ1(...)           M_IF(M_EQUAL(M_NARGS(__VA_ARGS__), 1))

/* If the number of arguments is 2 */
#define M_IF_NARGS_EQ2(...)           M_IF(M_EQUAL(M_NARGS(__VA_ARGS__), 2))

/* If NDEBUG macro is defined
	M_IF_DEBUG(code if NDEBUG is not defined)
   Note: not 100% robust */
#define M_TEST_NDEBUG_P()           M_C3(M_, NDEBUG, _TEST)
#define M_NDEBUG_TEST               0
#define M_IF_DEBUG(a)               M_IF(M_TEST_NDEBUG_P())(,a)

/* NOTEQUAL(val1,val2) with val from [0 to 30[
   Return 1 or 0 if val1=val2
 */
#define M_NOTEQUAL_0_0 0
#define M_NOTEQUAL_1_1 0
#define M_NOTEQUAL_2_2 0
#define M_NOTEQUAL_3_3 0
#define M_NOTEQUAL_4_4 0
#define M_NOTEQUAL_5_5 0
#define M_NOTEQUAL_6_6 0
#define M_NOTEQUAL_7_7 0
#define M_NOTEQUAL_8_8 0
#define M_NOTEQUAL_9_9 0
#define M_NOTEQUAL_10_10 0
#define M_NOTEQUAL_11_11 0
#define M_NOTEQUAL_12_12 0
#define M_NOTEQUAL_13_13 0
#define M_NOTEQUAL_14_14 0
#define M_NOTEQUAL_15_15 0
#define M_NOTEQUAL_16_16 0
#define M_NOTEQUAL_17_17 0
#define M_NOTEQUAL_18_18 0
#define M_NOTEQUAL_19_19 0
#define M_NOTEQUAL_20_20 0
#define M_NOTEQUAL_21_21 0
#define M_NOTEQUAL_22_22 0
#define M_NOTEQUAL_23_23 0
#define M_NOTEQUAL_24_24 0
#define M_NOTEQUAL_25_25 0
#define M_NOTEQUAL_26_26 0
#define M_NOTEQUAL_27_27 0
#define M_NOTEQUAL_28_28 0
#define M_NOTEQUAL_29_29 0
#define M_NOTEQUAL(x,y) M_BOOL(M_C4(M_NOTEQUAL_, x, _, y))

/* ADD: Through recursivity.
   Example: ADD(4,5) --> 9  */
#define M_ADD_L0_INDIRECT()         M_ADD_L0
#define M_ADD_L1(x,y)               M_DELAY3(M_ADD_L0_INDIRECT) () (M_INC(x), M_DEC(y))
#define M_ADD_L0(x,y)               M_IF(M_BOOL(y)) (M_ADD_L1(x,y) , x)
#define M_ADD(x,y)                  M_EVAL(M_ADD_L0(x,y))

/* SUB: Through recursivity
   Example: SUB(15,6) --> 11 */
#define M_SUB_L0_INDIRECT()         M_SUB_L0
#define M_SUB_L1(x,y)               M_DELAY3(M_SUB_L0_INDIRECT) () (M_DEC(x), M_DEC(y))
#define M_SUB_L0(x,y)               M_IF(M_BOOL(y)) (M_SUB_L1(x,y) , x)
#define M_SUB(x,y)                  M_EVAL(M_SUB_L0(x,y))

/* EQUAL(val1,val2) with valN from [0 to 30[
   Return 1 if val1=val2 or 0
   Example: M_EQUAL(1,2) --> 0
 */
#define M_EQUAL(x,y) M_INV(M_NOTEQUAL(x,y))

/* Revert the list given as parameter
   Example: M_INVERT(a,b,c) --> (c,b,a)  */
/* Generated by:
   for i in $(seq 1 26) ; do printf "#define M_INVERT_%d(" $i ; for j in $(seq 1 $i) ; do printf "_%d" $j ; if test $i -ne $j ; then printf "," ; fi ; done; printf ") (" ; for j in $(seq $i -1 1) ; do printf "_%d" $j ; if test 1 -ne $j ; then printf "," ; fi ; done ; printf ")\n" ; done */
#define M_INVERT_0() ()
#define M_INVERT_1(_1) (_1)
#define M_INVERT_2(_1,_2) (_2,_1)
#define M_INVERT_3(_1,_2,_3) (_3,_2,_1)
#define M_INVERT_4(_1,_2,_3,_4) (_4,_3,_2,_1)
#define M_INVERT_5(_1,_2,_3,_4,_5) (_5,_4,_3,_2,_1)
#define M_INVERT_6(_1,_2,_3,_4,_5,_6) (_6,_5,_4,_3,_2,_1)
#define M_INVERT_7(_1,_2,_3,_4,_5,_6,_7) (_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_8(_1,_2,_3,_4,_5,_6,_7,_8) (_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_9(_1,_2,_3,_4,_5,_6,_7,_8,_9) (_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) (_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_11(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11) (_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_12(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12) (_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_13(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13) (_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_14(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14)\
  (_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_15(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15)\
  (_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_16(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16)\
  (_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_17(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17)\
  (_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_18(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18)\
  (_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_19(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19)\
  (_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_20(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20)\
  (_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_21(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21)\
  (_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_22(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22)\
  (_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_23(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23)\
  (_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_24(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24)\
  (_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_25(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25)\
  (_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT_26(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26)\
  (_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1)
#define M_INVERT(...) M_C(M_INVERT_, M_NARGS(__VA_ARGS__))(__VA_ARGS__)


/***************************************************************/
/******************** Compile Times Macro **********************/
/***************************************************************/

/* C11 MACROS */

/* Return the string format of a variable */
#define M_PRINTF_FORMAT(x)                                              \
  _Generic(((void)0,x),                                                 \
           char: "%c",                                                  \
           signed char: "%hhd",                                         \
           unsigned char: "%hhu",                                       \
           signed short: "%hd",                                         \
           unsigned short: "%hu",                                       \
           signed int: "%d",                                            \
           unsigned int: "%u",                                          \
           long int: "%ld",                                             \
           unsigned long int: "%lu",                                    \
           long long int: "%lld",                                       \
           unsigned long long int: "%llu",                              \
           float: "%f",                                                 \
           double: "%f",                                                \
           long double: "%Lf",                                          \
           const char *: "%s",                                          \
           char *: "%s",                                                \
           const void *: "%p",                                          \
           void *: "%p")

/* Print a C variable if it is a standard type to stdout.
   Note: final ';' is needed to be called within a M_MAP */
#define M_PRINT_ARG(x) printf(M_PRINTF_FORMAT(x), x);

/* Print a C variable if it is a standard type to the given file 'f'
   Note: final ';' is needed to be called within a M_MAP2 */
#define M_FPRINT_ARG(f, x) fprintf(f, M_PRINTF_FORMAT(x), x);

/* Generic PRINT macro: print all its inputs regardless of the type
   provided it is a generic "non-struct" type. */
#define M_PRINT(...)   do { M_MAP(M_PRINT_ARG, __VA_ARGS__) } while (0)

/* Generic FPRINT macro: print all its inputs regardless of the type
   provided it is a generic "non-struct" type into the file 'f'. */
#define M_FPRINT(f,...)  do { M_MAP2(M_FPRINT_ARG, f, __VA_ARGS__) } while (0)

/* Return the minimum between x and y (computed in compile time) */
#define M_MIN(x, y) ((x) < (y) ? (x) : (y))

/* Return the maximum between x and y (computed in compile time) */
#define M_MAX(x, y) ((x) > (y) ? (x) : (y))

/* Is the number a power of 2? (computed in compile time) */
#define M_POWEROF2_P(n) (!((n)&((n)-1)))


/************************************************************/
/******************** METHODS handling **********************/
/************************************************************/

/* List of supported methods */
#define M_INIT_INIT(a)           ,a,
#define M_INIT_SET_INIT_SET(a)   ,a,
#define M_INIT_MOVE_INIT_MOVE(a) ,a,
#define M_SET_SET(a)             ,a,
#define M_MOVE_MOVE(a)           ,a,
#define M_CLEAR_CLEAR(a)         ,a,
#define M_DEL_DEL(a)             ,a,
#define M_HASH_HASH(a)           ,a,
#define M_EQUAL_EQUAL(a)         ,a,
#define M_CMP_CMP(a)             ,a,
#define M_TYPE_TYPE(a)           ,a,
#define M_SUBTYPE_SUBTYPE(a)     ,a,
#define M_OPLIST_OPLIST(a)       ,a,
#define M_SORT_SORT(a)           ,a,
#define M_UPDATE_UPDATE(a)       ,a,
#define M_IT_TYPE_IT_TYPE(a)     ,a,
#define M_IT_FIRST_IT_FIRST(a)   ,a,
#define M_IT_LAST_IT_LAST(a)     ,a,
#define M_IT_SET_IT_SET(a)       ,a,
#define M_IT_END_P_IT_END_P(a)   ,a,
#define M_IT_LAST_P_IT_LAST_P(a) ,a,
#define M_IT_EQUAL_P_IT_EQUAL_P(a) ,a,
#define M_IT_NEXT_IT_NEXT(a)     ,a,
#define M_IT_PREVIOUS_IT_PREVIOUS(a)     ,a,
#define M_IT_REF_IT_REF(a)       ,a,
#define M_IT_CREF_IT_CREF(a)     ,a,
#define M_ADD_ADD(a)             ,a,
#define M_SUB_SUB(a)             ,a,
#define M_MUL_MUL(a)             ,a,
#define M_DIV_DIV(a)             ,a,
#define M_CLEAN_CLEAN(a)         ,a,
#define M_PUSH_PUSH(a)           ,a,
#define M_POP_POP(a)             ,a,
#define M_REVERSE_REVERSE(a)     ,a,
#define M_GET_STR_GET_STR(a)     ,a,
#define M_OUT_STR_OUT_STR(a)     ,a,
#define M_SET_STR_SET_STR(a)     ,a,
#define M_IN_STR_IN_STR(a)       ,a,
#define M_SEPARATOR_SEPARATOR(a) ,a,
#define M_EXT_ALGO_EXT_ALGO(a)   ,a,

/* From an oplist - an unorded list of methods : like "INIT(mpz_init),CLEAR(mpz_clear),SET(mpz_set)" -
   Return the given method in the oplist or the default method.
   Example: 
   M_GET_METHOD(INIT, my_default, INIT(mpz_init),CLEAR(mpz_clear),SET(mpz_set)) --> mpz_init
   M_GET_METHOD(INIT, my_default, CLEAR(mpz_clear),SET(mpz_set)) --> my_default */
#define M_GET_METHOD(method, method_default, ...)                        \
  M_RET_ARG2 (M_MAP2B(M_C, M_C3(M_, method, _), __VA_ARGS__), method_default,)

/* Get the given method */
#define M_GET_INIT(...)      M_GET_METHOD(INIT,        M_INIT_DEFAULT,     __VA_ARGS__)
#define M_GET_INIT_SET(...)  M_GET_METHOD(INIT_SET,    M_SET_DEFAULT,      __VA_ARGS__)
#define M_GET_INIT_MOVE(...) M_GET_METHOD(INIT_MOVE,   M_MOVE_DEFAULT,     __VA_ARGS__)
#define M_GET_SET(...)       M_GET_METHOD(SET,         M_SET_DEFAULT,      __VA_ARGS__)
#define M_GET_MOVE(...)      M_GET_METHOD(MOVE,        M_MOVE_DEFAULT,     __VA_ARGS__)
#define M_GET_CLEAR(...)     M_GET_METHOD(CLEAR,       M_CLEAR_DEFAULT,    __VA_ARGS__)
#define M_GET_DEL(...)       M_GET_METHOD(DEL,         M_DEL_DEFAULT,      __VA_ARGS__)
#define M_GET_HASH(...)      M_GET_METHOD(HASH,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_EQUAL(...)     M_GET_METHOD(EQUAL,       M_EQUAL_DEFAULT,    __VA_ARGS__)
#define M_GET_CMP(...)       M_GET_METHOD(CMP,         M_CMP_DEFAULT,      __VA_ARGS__)
#define M_GET_UPDATE(...)    M_GET_METHOD(UPATE,       M_MEMCPY_DEFAULT,   __VA_ARGS__)
#define M_GET_TYPE(...)      M_GET_METHOD(TYPE,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_SUBTYPE(...)   M_GET_METHOD(SUBTYPE,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_OPLIST(...)    M_GET_METHOD(OPLIST,      (),                 __VA_ARGS__)
#define M_GET_SORT(...)      M_GET_METHOD(SORT,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_TYPE(...)   M_GET_METHOD(IT_TYPE,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_FIRST(...)  M_GET_METHOD(IT_FIRST,    M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_LAST(...)   M_GET_METHOD(IT_LAST,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_SET(...)    M_GET_METHOD(IT_SET,      M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_END_P(...)  M_GET_METHOD(IT_END_P,    M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_LAST_P(...) M_GET_METHOD(IT_LAST_P,   M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_EQUAL_P(...) M_GET_METHOD(IT_EQUAL_P, M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_NEXT(...)   M_GET_METHOD(IT_NEXT,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_PREVIOUS(...) M_GET_METHOD(IT_PREVIOUS, M_NO_DEFAULT,     __VA_ARGS__)
#define M_GET_IT_REF(...)    M_GET_METHOD(IT_REF,      M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_CREF(...)   M_GET_METHOD(IT_CREF,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_ADD(...)       M_GET_METHOD(ADD,         M_ADD_DEFAULT,      __VA_ARGS__)
#define M_GET_SUB(...)       M_GET_METHOD(SUB,         M_SUB_DEFAULT,      __VA_ARGS__)
#define M_GET_MUL(...)       M_GET_METHOD(MUL,         M_MUL_DEFAULT,      __VA_ARGS__)
#define M_GET_DIV(...)       M_GET_METHOD(DIV,         M_DIV_DEFAULT,      __VA_ARGS__)
#define M_GET_CLEAN(...)     M_GET_METHOD(CLEAN,       M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_PUSH(...)      M_GET_METHOD(PUSH,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_POP(...)       M_GET_METHOD(POP,         M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_REVERSE(...)   M_GET_METHOD(REVERSE,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_GET_STR(...)   M_GET_METHOD(GET_STR,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_OUT_STR(...)   M_GET_METHOD(OUT_STR,     M_FPRINT_ARG,       __VA_ARGS__)
#define M_GET_IN_STR(...)    M_GET_METHOD(IN_STR,      M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_SEPARATOR(...) M_GET_METHOD(SEPARATOR,   ',',                __VA_ARGS__)
#define M_GET_EXT_ALGO(...)  M_GET_METHOD(EXT_ALGO,    M_NO_EXT_ALGO,      __VA_ARGS__)

/* Define the default method */
#define M_INIT_DEFAULT(a)       ((a) = 0)
#define M_MEMSET_DEFAULT(a)     (memset(&(a), 0, sizeof (a)))
#define M_SET_DEFAULT(a,b)      ((a) = (b))
#define M_MOVE_DEFAULT(a,b)     (M_MEMCPY_DEFAULT(a, b), M_MEMSET_DEFAULT(b))
#define M_MEMCPY_DEFAULT(a,b)   (memcpy(&(a), &(b), sizeof (a)))
#define M_CLEAR_DEFAULT(a)      (void)a
#define M_DEL_DEFAULT(a)        free(a)
#define M_NO_DEFAULT(...)       m_no_default_function
#define M_EQUAL_DEFAULT(a,b)    ((a) == (b))
#define M_CMP_DEFAULT(a,b)      ((a) < (b) ? -1 : (a) > (b))
#define M_ADD_DEFAULT(a,b,c)    ((a) = (b) + (c))
#define M_SUB_DEFAULT(a,b,c)    ((a) = (b) - (c))
#define M_MUL_DEFAULT(a,b,c)    ((a) = (b) * (c))
#define M_DIV_DEFAULT(a,b,c)    ((a) = (b) / (c))
#define M_NO_EXT_ALGO(n,co,to)

/* Default hash: perform a hash of the memory pattern of the object */
// TODO: Better hash function?
static inline size_t
m_core_hash(const void *ptr, size_t s)
{
  const unsigned char *p = (const unsigned char *) ptr;
  size_t hash = 0;
  for(size_t i = 0 ; i < s; i++) {
    unsigned int c = p[i];
    hash = hash * 31421 + c + 6297;
  }
  return hash;
}
#define M_HASH_DEFAULT(a)       m_core_hash((const void*) &(a), sizeof (a))

#define M_DEFAULT_OPLIST                                                \
  (INIT(M_INIT_DEFAULT), INIT_SET(M_SET_DEFAULT), SET(M_SET_DEFAULT),   \
   CLEAR(M_CLEAR_DEFAULT), EQUAL(M_EQUAL_DEFAULT), CMP(M_CMP_DEFAULT),  \
   INIT_MOVE(M_MOVE_DEFAULT), MOVE(M_MOVE_DEFAULT) ,                    \
   HASH(M_HASH_DEFAULT)                                                 \
   /*ADD(M_ADD_DEFAULT), SUB(M_SUB_DEFAULT), MUL(M_MUL_DEFAULT),*/      \
   /*DIV(M_DIV_DEFAULT)*/ )

/* From an oplist (...) return ... */
#define M_OPLIST_FLAT(...)     __VA_ARGS__

/* Concat two oplists in one. op1 will have higher priority to op2 */
#define M_OPLIST_CAT(op1,op2) (M_OPLIST_FLAT op1, M_OPLIST_FLAT op2)

/* Appl an oplist */
#define M_OPLIST_APPLY(a, oplist)  a oplist

/* Test if a method is present in an oplist.
   Return 0 or 1 */
#define M_TEST_METHOD_P(method, oplist)                   \
  M_BOOL(M_GET_METHOD (method, 0, M_OPLIST_FLAT oplist))

/* Perfom a preprocessing M_IF, if the method is present in the oplist.
   Example: M_IF_METHOD(HASH, oplist)(define function with HASH method, ) */
#define M_IF_METHOD(method, oplist) M_IF(M_TEST_METHOD_P(method, oplist))

/* Perform a preprocessing M_IF if the method exists in both oplist.
   Example: M_IF_METHOD_BOTH(HASH, oplist1, oplist2) (define function with HASH method, ) */
#define M_IF_METHOD_BOTH(method, oplist1, oplist2)                      \
  M_IF(M_AND(M_TEST_METHOD_P(method, oplist1), M_TEST_METHOD_P(method, oplist2)))

/* Perform a preprocessing M_IF if the method exists for all oplist.
   Example: M_IF_METHOD_ALL(HASH, oplist1, oplist2) (define function with HASH method, ) */
#define M_IF_METHOD_ALL(method, ...)                            \
  M_IF(M_REDUCE2(M_TEST_METHOD_P, M_AND, method, __VA_ARGS__))

/* Define M_EACH macro allowing to iterate over a container
   First argument will be a created pointer to the underlying type.
   Example: for M_EACH(item, list, LIST_OPLIST) { action; } */
#define M_EACH(item, container, oplist)                                 \
  M_EACHI(item, container, oplist, M_C(local_iterator_, __LINE__),      \
          M_C(local_cont_, __LINE__))

/* Internal for M_EACH */
#define M_EACHI(item,container,oplist, iterator, cont)                  \
  (bool cont = true; cont; cont = false)                                \
  for(M_GET_SUBTYPE oplist *item; cont ; cont = false)                  \
    for(M_GET_IT_TYPE oplist iterator; cont ; cont = false)             \
      for(M_GET_IT_FIRST oplist (iterator, container) ;                 \
          !M_GET_IT_END_P oplist (iterator) && (item = M_GET_IT_REF oplist (iterator), true) ; \
          M_GET_IT_NEXT oplist (iterator))

/* Define M_LET macro allowing to define, auto init & auto clear an object
   within the next bracket.
   Example:
     M_LET(a, STRING_OPLIST) { do something with a }  or
     M_LET(a, b, c, STRING_OPLIST) { do something with a, b & c } */
#define M_LET(...)                                                      \
  M_ID(M_LETI M_INVERT( __VA_ARGS__, M_C(local_cont_, __LINE__) ))

#define M_LETI(cont, oplist, ...)                                       \
  for(bool cont = true; cont ; cont = false)                            \
    M_MAP2(M_LETI_SINGLE, (cont, oplist,), __VA_ARGS__)

#define M_LETI_SINGLE(data, name)                               \
  M_LETI_SINGLE2(M_RET_ARG1 data, M_RET_ARG2 data, name)

#define M_LETI_SINGLE2(cont, oplist, name)                              \
  for(M_GET_TYPE oplist name;                                           \
      (M_GET_INIT oplist (name), cont);                                 \
      (M_GET_CLEAR oplist (name), cont = false))

/* Check if 'n' is assignable to an object of type 'type'
   Return the object. */
#define M_ASSIGN_CAST(type, n)                  \
  ((type) { 0 } = (n))


/* By putting this after a method, we transform the argument list
   so that the first argument becomes a pointer to the destination. */
#define M_IPTR(...) ( & __VA_ARGS__ )


/************************************************************/
/********************* MEMORY handling **********************/
/************************************************************/

#ifndef M_MEMORY_ALLOC
#ifdef __cplusplus
# include <cstdlib>
# define M_MEMORY_ALLOC(type) ((type*)std::malloc (sizeof (type)))
# define M_MEMORY_REALLOC(type, ptr, n)         \
  ((type*) std::realloc ((ptr), (n)*sizeof (type)))
# define M_MEMORY_FREE(ptr) std::free(ptr)
#else
# include <stdlib.h>
# define M_MEMORY_ALLOC(type) malloc (sizeof (type))
# define M_MEMORY_REALLOC(type, ptr, n) realloc ((ptr), (n)*sizeof (type))
# define M_MEMORY_FREE(ptr) free(ptr)
#endif
#endif

// Basic ERROR handling macros.
// Note: Can be overloaded by used code to:
// * throw an exception if needed.
// * set a global error variable and return
// * abort
#ifndef M_MEMORY_FULL
#include <stdio.h>
#include <stdlib.h>
#define M_MEMORY_FULL(size) do {                                        \
    fprintf(stderr, "ERROR: Can not allocate memory (%lu bytes).\n",    \
            (unsigned long) size);                                      \
    abort();                                                            \
  } while (0)
#endif

#ifndef M_INIT_FAILURE
#include <stdio.h>
#include <stdlib.h>
#define M_INIT_FAILURE() do {                                           \
    fprintf(stderr, "ERROR: Can not initialize data.\n");               \
    abort();                                                            \
  } while (0)
#endif

#ifndef M_ASSERT_INIT
#define M_ASSERT_INIT(expr)                     do {                    \
  if (!(expr)) {                                                        \
    fprintf(stderr, "ERROR: Can not initialize data: %s\n", #expr);     \
    abort();                                                            \
  } } while (0)
#endif

#endif
