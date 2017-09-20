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
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

/***************************************************************/
/************************ Compiler Macro ***********************/
/***************************************************************/

/* M_ASSUME is equivalent to assert, but gives hints to compiler
   about how to optimize the code if NDEBUG is defined. */
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

/* M_LIKELY / M_UNLIKELY gives hints on the compiler of the likehood
   of the given condition */
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
   Can be increased for future release of this header.
   Some macros have been port to a maximum of 52, but not all.
*/
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

/* Increment the number given as argument (from [0..29[) */
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
   M_RET_ARG53
 */
#define M_RETI_ARG1(a, ...)     a
#define M_RET_ARG1(...)         M_RETI_ARG1(__VA_ARGS__)

#define M_RETI_ARG2(a, b, ...)  b
#define M_RET_ARG2(...)         M_RETI_ARG2(__VA_ARGS__)

#define M_RETI_ARG3(a, b, c, ...)  c
#define M_RET_ARG3(...)         M_RETI_ARG3(__VA_ARGS__)

#define M_RETI_ARG27(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa, ...)    aa
#define M_RET_ARG27(...)        M_RETI_ARG27(__VA_ARGS__)

#define M_RETI_ARG53(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z, aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az, ba, ...)    ba
#define M_RET_ARG53(...)        M_RETI_ARG53(__VA_ARGS__)

/* Convert an integer or a symbol into 0 (if 0) or 1 (if not 0).
   1 if symbol unknown */
#define M_TOBOOLI_0                 1, 0,
#define M_BOOL(x)                   M_RET_ARG2(M_C(M_TOBOOLI_, x), 1, useless)

/* Inverse 0 into 1 and 1 into 0 */
#define M_INVI_0                    1
#define M_INVI_1                    0
#define M_INV(x)                    M_C(M_INVI_, x)

/* Perform a AND between the inputs */
#define M_ANDI_00                   0
#define M_ANDI_01                   0
#define M_ANDI_10                   0
#define M_ANDI_11                   1
#define M_AND(x,y)                  M_C3(M_ANDI_, x, y)

/* Perform a OR between the inputs */
#define M_ORI_00                    0
#define M_ORI_01                    1
#define M_ORI_10                    1
#define M_ORI_11                    1
#define M_OR(x,y)                   M_C3(M_ORI_, x, y)

/* M_IF Macro :
   M_IF(condition)(Execute if true, execute if false)
   Example: M_IF(0)(true_action, false_action) --> false_action */
#define M_IFI_0(true_macro, ...)    __VA_ARGS__
#define M_IFI_1(true_macro, ...)    true_macro
#define M_IF(c)                     M_C(M_IFI_, M_BOOL(c))

/* Return 1 if there is a comma inside the argument list, 0 otherwise */
#define M_COMMA_P(...)              M_RET_ARG53(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, useless)

/* Return the string representation of the evaluated x.
   NOTE: Need to be used with M_APPLY to defer the evaluation  */
#define M_AS_STR(x)                 #x

/* Return 1 if the argument is 'empty', 0 otherwise.
   Handle: EMPTY_P(), EMPTY_P(x), EMPTY_P(()) and EMPTY_P(,) and EMPTY_P(f) with #define f() 2,3 */
#define M_EMPTYI_DETECT(...)        0, 1,
#define M_EMPTYI_P_C1(...)          M_COMMA_P(M_EMPTYI_DETECT __VA_ARGS__ () )
#define M_EMPTYI_P_C2(...)          M_COMMA_P(M_EMPTYI_DETECT __VA_ARGS__)
#define M_EMPTYI_P_C3(...)          M_COMMA_P(__VA_ARGS__ () )
#define M_EMPTY_P(...)              M_AND(M_EMPTYI_P_C1(__VA_ARGS__), M_INV(M_OR(M_OR(M_EMPTYI_P_C2(__VA_ARGS__), M_COMMA_P(__VA_ARGS__)),M_EMPTYI_P_C3(__VA_ARGS__))))

/* Generate a comma later in the next evaluation pass. */
#define M_DEFERRED_COMMA            ,

/* M_IF macro for empty arguments:
    M_IF_EMPTY(arguments)(action if empty, action if not empty) */
#define M_IF_EMPTY(...)             M_IF(M_EMPTY_P(__VA_ARGS__))

/* Return 1 if argument is "()" or "(x)" */
#define M_PARENTHESISI_DETECT(...)  0, 1,
#define M_PARENTHESIS_P(...)        M_AND(M_COMMA_P(M_PARENTHESISI_DETECT __VA_ARGS__), M_INV(M_COMMA_P(__VA_ARGS__)))

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

/* Perform 3^5 evaluation 
 NOTE: There can be only one eval per complete macro-evaluation pass.
 If there is multiple macro using recursivity, only one M_EVAL
 can exist.*/
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
  M_IF(M_EMPTY_P(arg))(, func(arg))

/* Apply 'func' to arg1,arg2 if 'arg2' is not empty */
/* Example: M_APPLY_FUNC2(f, 2, 3) ==> 'f(2, 3)' and M_APPLY_FUNC2(f, 2, ) ==> '' */
#define M_APPLY_FUNC2(func, arg1, arg2)                 \
  M_IF(M_EMPTY_P(arg2))(,func(arg1, arg2))

/* Map: apply the given macro to all arguments (non recursive version) */
/* Example: M_MAP(f,a, b, c) ==> f(a) f(b) f(c) */
#define M_MAPI_0(func, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, ...) \
  M_APPLY_FUNC(func, a) M_APPLY_FUNC(func, b) M_APPLY_FUNC(func, c) M_APPLY_FUNC(func, d) M_APPLY_FUNC(func, e) \
  M_APPLY_FUNC(func, f) M_APPLY_FUNC(func, g) M_APPLY_FUNC(func, h) M_APPLY_FUNC(func, i) M_APPLY_FUNC(func, j) \
  M_APPLY_FUNC(func, k) M_APPLY_FUNC(func, l) M_APPLY_FUNC(func, m) M_APPLY_FUNC(func, n) M_APPLY_FUNC(func, o) \
  M_APPLY_FUNC(func, p) M_APPLY_FUNC(func, q) M_APPLY_FUNC(func, r) M_APPLY_FUNC(func, s) M_APPLY_FUNC(func, t) \
  M_APPLY_FUNC(func, u) M_APPLY_FUNC(func, v) M_APPLY_FUNC(func, w) M_APPLY_FUNC(func, x) M_APPLY_FUNC(func, y) \
  M_APPLY_FUNC(func, z)
#define M_MAP(f, ...) M_MAPI_0(f, __VA_ARGS__, , , , , , , , , , , , , , , , , , , , , , , , , , )

// TODO: M_MAP_C(f, a, b, c) ==> f(a), f(b), f(c)
// TODO: M_MAP2_C(f, d, a, b, c) ==> f(d, a), f(d, b), f(d, c)

/* Map a macro to all given arguments with one additional fixed data (non recursive version) */
/* Example: M_MAP2(f, data, a, b, c) ==> f(data,a) f(data,b) f(data,c) */
#define M_MAP2I_0(func, data, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, ...) \
  M_APPLY_FUNC2(func, data, a) M_APPLY_FUNC2(func, data, b) M_APPLY_FUNC2(func, data, c) \
  M_APPLY_FUNC2(func, data, d) M_APPLY_FUNC2(func, data, e) M_APPLY_FUNC2(func, data, f) \
  M_APPLY_FUNC2(func, data, g) M_APPLY_FUNC2(func, data, h) M_APPLY_FUNC2(func, data, i) \
  M_APPLY_FUNC2(func, data, j) M_APPLY_FUNC2(func, data, k) M_APPLY_FUNC2(func, data, l) \
  M_APPLY_FUNC2(func, data, m) M_APPLY_FUNC2(func, data, n) M_APPLY_FUNC2(func, data, o) \
  M_APPLY_FUNC2(func, data, p) M_APPLY_FUNC2(func, data, q) M_APPLY_FUNC2(func, data, r) \
  M_APPLY_FUNC2(func, data, s) M_APPLY_FUNC2(func, data, t) M_APPLY_FUNC2(func, data, u) \
  M_APPLY_FUNC2(func, data, v) M_APPLY_FUNC2(func, data, w) M_APPLY_FUNC2(func, data, x) \
  M_APPLY_FUNC2(func, data, y) M_APPLY_FUNC2(func, data, z)
#define M_MAP2(f, ...) M_MAP2I_0(f, __VA_ARGS__, , , , , , , , , , , , , , , , , , , , , , , , , , , )


/* Duplicate of macros for GET_METHOD as it may be called in context where a M_MAP2 is in progress.
   NOTE: Increase number of arguments to 52 due to the number of available methods.
   NOTE: Rewrite with another approach much more verbose but also much faster for the compiler.
   This kind of approach seems ugly, but is in fact the fast one.
   NOTE: Do not call recursively the macro (example: M_MAP2B_10 expands f(d,e) then call M_MAP2B_9 on what remains to break as soon as possible the macro "chain".
   Generated by the following command:
   for i in $(seq 1 52) ; do printf "#define M_MAP2B_%d(f, d" $i ; for j in $(seq 1 $i) ; do printf ", _%d" $j ; done ; printf ") " ; for j in $(seq 1 $i) ; do printf "f(d, _%d) " $j ; done ; printf "\n"; done
*/
#define M_MAP2B(f, d, ...)  M_MAP2B0(M_C(M_MAP2B_, M_NARGS(__VA_ARGS__)),  f, d, __VA_ARGS__)
#define M_MAP2B0(F,...)     F (__VA_ARGS__)
#define M_MAP2B_1(f, d, _1) f(d, _1) 
#define M_MAP2B_2(f, d, _1, _2) f(d, _1) f(d, _2) 
#define M_MAP2B_3(f, d, _1, _2, _3) f(d, _1) f(d, _2) f(d, _3) 
#define M_MAP2B_4(f, d, _1, _2, _3, _4) f(d, _1) f(d, _2) f(d, _3) f(d, _4) 
#define M_MAP2B_5(f, d, _1, _2, _3, _4, _5) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) 
#define M_MAP2B_6(f, d, _1, _2, _3, _4, _5, _6) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) 
#define M_MAP2B_7(f, d, _1, _2, _3, _4, _5, _6, _7) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) 
#define M_MAP2B_8(f, d, _1, _2, _3, _4, _5, _6, _7, _8) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) 
#define M_MAP2B_9(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) 
#define M_MAP2B_10(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) 
#define M_MAP2B_11(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) 
#define M_MAP2B_12(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) 
#define M_MAP2B_13(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) 
#define M_MAP2B_14(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) 
#define M_MAP2B_15(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) 
#define M_MAP2B_16(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) 
#define M_MAP2B_17(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) 
#define M_MAP2B_18(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) 
#define M_MAP2B_19(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) 
#define M_MAP2B_20(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) 
#define M_MAP2B_21(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) 
#define M_MAP2B_22(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) 
#define M_MAP2B_23(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) 
#define M_MAP2B_24(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) 
#define M_MAP2B_25(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) 
#define M_MAP2B_26(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) 
#define M_MAP2B_27(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) 
#define M_MAP2B_28(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) 
#define M_MAP2B_29(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) 
#define M_MAP2B_30(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) 
#define M_MAP2B_31(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) 
#define M_MAP2B_32(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) 
#define M_MAP2B_33(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) 
#define M_MAP2B_34(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) 
#define M_MAP2B_35(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) 
#define M_MAP2B_36(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) 
#define M_MAP2B_37(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) 
#define M_MAP2B_38(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) 
#define M_MAP2B_39(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) 
#define M_MAP2B_40(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) 
#define M_MAP2B_41(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) 
#define M_MAP2B_42(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) 
#define M_MAP2B_43(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) 
#define M_MAP2B_44(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) 
#define M_MAP2B_45(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) 
#define M_MAP2B_46(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) f(d, _46) 
#define M_MAP2B_47(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) f(d, _46) f(d, _47) 
#define M_MAP2B_48(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) f(d, _46) f(d, _47) f(d, _48) 
#define M_MAP2B_49(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) f(d, _46) f(d, _47) f(d, _48) f(d, _49) 
#define M_MAP2B_50(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) f(d, _46) f(d, _47) f(d, _48) f(d, _49) f(d, _50) 
#define M_MAP2B_51(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) f(d, _46) f(d, _47) f(d, _48) f(d, _49) f(d, _50) f(d, _51) 
#define M_MAP2B_52(f, d, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52) f(d, _1) f(d, _2) f(d, _3) f(d, _4) f(d, _5) f(d, _6) f(d, _7) f(d, _8) f(d, _9) f(d, _10) f(d, _11) f(d, _12) f(d, _13) f(d, _14) f(d, _15) f(d, _16) f(d, _17) f(d, _18) f(d, _19) f(d, _20) f(d, _21) f(d, _22) f(d, _23) f(d, _24) f(d, _25) f(d, _26) f(d, _27) f(d, _28) f(d, _29) f(d, _30) f(d, _31) f(d, _32) f(d, _33) f(d, _34) f(d, _35) f(d, _36) f(d, _37) f(d, _38) f(d, _39) f(d, _40) f(d, _41) f(d, _42) f(d, _43) f(d, _44) f(d, _45) f(d, _46) f(d, _47) f(d, _48) f(d, _49) f(d, _50) f(d, _51) f(d, _52)



/* Map a macro to all given pair of arguments (Using recursivity) */
/* Example: M_MAP_PAIR(f, a, b, c, d) ==> f(a,b) f(c,d) */
#define M_MAP_PAIR_L0_INDIRECT()      M_MAP_PAIR_L0
#define M_MAP_PAIR_L0(f, ...)         M_IF_NARGS_EQ2(__VA_ARGS__)( f(__VA_ARGS__) , M_MAP_PAIR_L1(f, __VA_ARGS__))
#define M_MAP_PAIR_L1(f, a, b, ...)   f(a,b) M_DELAY3(M_MAP_PAIR_L0_INDIRECT) () (f, __VA_ARGS__)
#define M_MAP_PAIR(f, ...)            M_IF_EMPTY(__VA_ARGS__)( /* end */, M_EVAL(M_MAP_PAIR_L0(f, __VA_ARGS__)))


/* Map a macro to all given arguments and reduce all theses computation
   with another reduce macro */
/* Example: M_REDUCE(f, g, a, b, c) ==> g( f(a), g( f(b), f(c)) */
/* Generated by:
   for i in $(seq 1 26) ; do printf "#define M_REDUCEI1_%d(f, g" $i ; for j in $(seq 1 $i) ; do printf ", _%d" $j ; done ; printf ") "; for j in $(seq 1 $(( $i - 1 )) ) ; do printf "g(f(_%d), " $j; done ; printf "f(_%d" $i; for j in $(seq 1 $i); do printf ")"; done ; printf "\n";  done
*/
#define M_REDUCE(f, g, ...)            M_REDUCEI1_0(M_C(M_REDUCEI1_, M_NARGS(__VA_ARGS__)), f, g, __VA_ARGS__)
#define M_REDUCEI1_0(F, ...)           F(__VA_ARGS__)
#define M_REDUCEI1_1(f, g, _1) f(_1)
#define M_REDUCEI1_2(f, g, _1, _2) g(f(_1), f(_2))
#define M_REDUCEI1_3(f, g, _1, _2, _3) g(f(_1), g(f(_2), f(_3)))
#define M_REDUCEI1_4(f, g, _1, _2, _3, _4) g(f(_1), g(f(_2), g(f(_3), f(_4))))
#define M_REDUCEI1_5(f, g, _1, _2, _3, _4, _5) g(f(_1), g(f(_2), g(f(_3), g(f(_4), f(_5)))))
#define M_REDUCEI1_6(f, g, _1, _2, _3, _4, _5, _6) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), f(_6))))))
#define M_REDUCEI1_7(f, g, _1, _2, _3, _4, _5, _6, _7) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), f(_7)))))))
#define M_REDUCEI1_8(f, g, _1, _2, _3, _4, _5, _6, _7, _8) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), f(_8))))))))
#define M_REDUCEI1_9(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), f(_9)))))))))
#define M_REDUCEI1_10(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), f(_10))))))))))
#define M_REDUCEI1_11(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), f(_11)))))))))))
#define M_REDUCEI1_12(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), f(_12))))))))))))
#define M_REDUCEI1_13(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), f(_13)))))))))))))
#define M_REDUCEI1_14(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), f(_14))))))))))))))
#define M_REDUCEI1_15(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), f(_15)))))))))))))))
#define M_REDUCEI1_16(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), f(_16))))))))))))))))
#define M_REDUCEI1_17(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), f(_17)))))))))))))))))
#define M_REDUCEI1_18(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), f(_18))))))))))))))))))
#define M_REDUCEI1_19(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), f(_19)))))))))))))))))))
#define M_REDUCEI1_20(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), g(f(_19), f(_20))))))))))))))))))))
#define M_REDUCEI1_21(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), g(f(_19), g(f(_20), f(_21)))))))))))))))))))))
#define M_REDUCEI1_22(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), g(f(_19), g(f(_20), g(f(_21), f(_22))))))))))))))))))))))
#define M_REDUCEI1_23(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), g(f(_19), g(f(_20), g(f(_21), g(f(_22), f(_23)))))))))))))))))))))))
#define M_REDUCEI1_24(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), g(f(_19), g(f(_20), g(f(_21), g(f(_22), g(f(_23), f(_24))))))))))))))))))))))))
#define M_REDUCEI1_25(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), g(f(_19), g(f(_20), g(f(_21), g(f(_22), g(f(_23), g(f(_24), f(_25)))))))))))))))))))))))))
#define M_REDUCEI1_26(f, g, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) g(f(_1), g(f(_2), g(f(_3), g(f(_4), g(f(_5), g(f(_6), g(f(_7), g(f(_8), g(f(_9), g(f(_10), g(f(_11), g(f(_12), g(f(_13), g(f(_14), g(f(_15), g(f(_16), g(f(_17), g(f(_18), g(f(_19), g(f(_20), g(f(_21), g(f(_22), g(f(_23), g(f(_24), g(f(_25), f(_26))))))))))))))))))))))))))


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


/* Sequence of numerical */
/* Example: 
   M_SEQ(init,end)==>init, init+1, ...end 
   M_MAP(f, M_SEQ(init, end)) ==> f(init) f(init+1) .... f(end) 
   M_MAP2(f, d, M_SEQ(init, end))
*/
/* Generated by:
   for i in $(seq 0 26); do for j in $(seq $i 26) ; do printf "#define M_SEQI_%d_%d " $i $j; for k in $(seq $i $(($j - 1))) ; do printf "%d," $k ;done ; printf "%d\n" $j; done ; done */
#define M_SEQ(first, last) M_C4(M_SEQI_, first, _, last)
#define M_SEQI_0_0 0
#define M_SEQI_0_1 0,1
#define M_SEQI_0_2 0,1,2
#define M_SEQI_0_3 0,1,2,3
#define M_SEQI_0_4 0,1,2,3,4
#define M_SEQI_0_5 0,1,2,3,4,5
#define M_SEQI_0_6 0,1,2,3,4,5,6
#define M_SEQI_0_7 0,1,2,3,4,5,6,7
#define M_SEQI_0_8 0,1,2,3,4,5,6,7,8
#define M_SEQI_0_9 0,1,2,3,4,5,6,7,8,9
#define M_SEQI_0_10 0,1,2,3,4,5,6,7,8,9,10
#define M_SEQI_0_11 0,1,2,3,4,5,6,7,8,9,10,11
#define M_SEQI_0_12 0,1,2,3,4,5,6,7,8,9,10,11,12
#define M_SEQI_0_13 0,1,2,3,4,5,6,7,8,9,10,11,12,13
#define M_SEQI_0_14 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
#define M_SEQI_0_15 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
#define M_SEQI_0_16 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
#define M_SEQI_0_17 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_0_18 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_0_19 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_0_20 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_0_21 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_0_22 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_0_23 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_0_24 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_0_25 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_0_26 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_1_1 1
#define M_SEQI_1_2 1,2
#define M_SEQI_1_3 1,2,3
#define M_SEQI_1_4 1,2,3,4
#define M_SEQI_1_5 1,2,3,4,5
#define M_SEQI_1_6 1,2,3,4,5,6
#define M_SEQI_1_7 1,2,3,4,5,6,7
#define M_SEQI_1_8 1,2,3,4,5,6,7,8
#define M_SEQI_1_9 1,2,3,4,5,6,7,8,9
#define M_SEQI_1_10 1,2,3,4,5,6,7,8,9,10
#define M_SEQI_1_11 1,2,3,4,5,6,7,8,9,10,11
#define M_SEQI_1_12 1,2,3,4,5,6,7,8,9,10,11,12
#define M_SEQI_1_13 1,2,3,4,5,6,7,8,9,10,11,12,13
#define M_SEQI_1_14 1,2,3,4,5,6,7,8,9,10,11,12,13,14
#define M_SEQI_1_15 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
#define M_SEQI_1_16 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
#define M_SEQI_1_17 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_1_18 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_1_19 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_1_20 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_1_21 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_1_22 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_1_23 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_1_24 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_1_25 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_1_26 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_2_2 2
#define M_SEQI_2_3 2,3
#define M_SEQI_2_4 2,3,4
#define M_SEQI_2_5 2,3,4,5
#define M_SEQI_2_6 2,3,4,5,6
#define M_SEQI_2_7 2,3,4,5,6,7
#define M_SEQI_2_8 2,3,4,5,6,7,8
#define M_SEQI_2_9 2,3,4,5,6,7,8,9
#define M_SEQI_2_10 2,3,4,5,6,7,8,9,10
#define M_SEQI_2_11 2,3,4,5,6,7,8,9,10,11
#define M_SEQI_2_12 2,3,4,5,6,7,8,9,10,11,12
#define M_SEQI_2_13 2,3,4,5,6,7,8,9,10,11,12,13
#define M_SEQI_2_14 2,3,4,5,6,7,8,9,10,11,12,13,14
#define M_SEQI_2_15 2,3,4,5,6,7,8,9,10,11,12,13,14,15
#define M_SEQI_2_16 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
#define M_SEQI_2_17 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_2_18 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_2_19 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_2_20 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_2_21 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_2_22 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_2_23 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_2_24 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_2_25 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_2_26 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_3_3 3
#define M_SEQI_3_4 3,4
#define M_SEQI_3_5 3,4,5
#define M_SEQI_3_6 3,4,5,6
#define M_SEQI_3_7 3,4,5,6,7
#define M_SEQI_3_8 3,4,5,6,7,8
#define M_SEQI_3_9 3,4,5,6,7,8,9
#define M_SEQI_3_10 3,4,5,6,7,8,9,10
#define M_SEQI_3_11 3,4,5,6,7,8,9,10,11
#define M_SEQI_3_12 3,4,5,6,7,8,9,10,11,12
#define M_SEQI_3_13 3,4,5,6,7,8,9,10,11,12,13
#define M_SEQI_3_14 3,4,5,6,7,8,9,10,11,12,13,14
#define M_SEQI_3_15 3,4,5,6,7,8,9,10,11,12,13,14,15
#define M_SEQI_3_16 3,4,5,6,7,8,9,10,11,12,13,14,15,16
#define M_SEQI_3_17 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_3_18 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_3_19 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_3_20 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_3_21 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_3_22 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_3_23 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_3_24 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_3_25 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_3_26 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_4_4 4
#define M_SEQI_4_5 4,5
#define M_SEQI_4_6 4,5,6
#define M_SEQI_4_7 4,5,6,7
#define M_SEQI_4_8 4,5,6,7,8
#define M_SEQI_4_9 4,5,6,7,8,9
#define M_SEQI_4_10 4,5,6,7,8,9,10
#define M_SEQI_4_11 4,5,6,7,8,9,10,11
#define M_SEQI_4_12 4,5,6,7,8,9,10,11,12
#define M_SEQI_4_13 4,5,6,7,8,9,10,11,12,13
#define M_SEQI_4_14 4,5,6,7,8,9,10,11,12,13,14
#define M_SEQI_4_15 4,5,6,7,8,9,10,11,12,13,14,15
#define M_SEQI_4_16 4,5,6,7,8,9,10,11,12,13,14,15,16
#define M_SEQI_4_17 4,5,6,7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_4_18 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_4_19 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_4_20 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_4_21 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_4_22 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_4_23 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_4_24 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_4_25 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_4_26 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_5_5 5
#define M_SEQI_5_6 5,6
#define M_SEQI_5_7 5,6,7
#define M_SEQI_5_8 5,6,7,8
#define M_SEQI_5_9 5,6,7,8,9
#define M_SEQI_5_10 5,6,7,8,9,10
#define M_SEQI_5_11 5,6,7,8,9,10,11
#define M_SEQI_5_12 5,6,7,8,9,10,11,12
#define M_SEQI_5_13 5,6,7,8,9,10,11,12,13
#define M_SEQI_5_14 5,6,7,8,9,10,11,12,13,14
#define M_SEQI_5_15 5,6,7,8,9,10,11,12,13,14,15
#define M_SEQI_5_16 5,6,7,8,9,10,11,12,13,14,15,16
#define M_SEQI_5_17 5,6,7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_5_18 5,6,7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_5_19 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_5_20 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_5_21 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_5_22 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_5_23 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_5_24 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_5_25 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_5_26 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_6_6 6
#define M_SEQI_6_7 6,7
#define M_SEQI_6_8 6,7,8
#define M_SEQI_6_9 6,7,8,9
#define M_SEQI_6_10 6,7,8,9,10
#define M_SEQI_6_11 6,7,8,9,10,11
#define M_SEQI_6_12 6,7,8,9,10,11,12
#define M_SEQI_6_13 6,7,8,9,10,11,12,13
#define M_SEQI_6_14 6,7,8,9,10,11,12,13,14
#define M_SEQI_6_15 6,7,8,9,10,11,12,13,14,15
#define M_SEQI_6_16 6,7,8,9,10,11,12,13,14,15,16
#define M_SEQI_6_17 6,7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_6_18 6,7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_6_19 6,7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_6_20 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_6_21 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_6_22 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_6_23 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_6_24 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_6_25 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_6_26 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_7_7 7
#define M_SEQI_7_8 7,8
#define M_SEQI_7_9 7,8,9
#define M_SEQI_7_10 7,8,9,10
#define M_SEQI_7_11 7,8,9,10,11
#define M_SEQI_7_12 7,8,9,10,11,12
#define M_SEQI_7_13 7,8,9,10,11,12,13
#define M_SEQI_7_14 7,8,9,10,11,12,13,14
#define M_SEQI_7_15 7,8,9,10,11,12,13,14,15
#define M_SEQI_7_16 7,8,9,10,11,12,13,14,15,16
#define M_SEQI_7_17 7,8,9,10,11,12,13,14,15,16,17
#define M_SEQI_7_18 7,8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_7_19 7,8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_7_20 7,8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_7_21 7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_7_22 7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_7_23 7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_7_24 7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_7_25 7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_7_26 7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_8_8 8
#define M_SEQI_8_9 8,9
#define M_SEQI_8_10 8,9,10
#define M_SEQI_8_11 8,9,10,11
#define M_SEQI_8_12 8,9,10,11,12
#define M_SEQI_8_13 8,9,10,11,12,13
#define M_SEQI_8_14 8,9,10,11,12,13,14
#define M_SEQI_8_15 8,9,10,11,12,13,14,15
#define M_SEQI_8_16 8,9,10,11,12,13,14,15,16
#define M_SEQI_8_17 8,9,10,11,12,13,14,15,16,17
#define M_SEQI_8_18 8,9,10,11,12,13,14,15,16,17,18
#define M_SEQI_8_19 8,9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_8_20 8,9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_8_21 8,9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_8_22 8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_8_23 8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_8_24 8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_8_25 8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_8_26 8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_9_9 9
#define M_SEQI_9_10 9,10
#define M_SEQI_9_11 9,10,11
#define M_SEQI_9_12 9,10,11,12
#define M_SEQI_9_13 9,10,11,12,13
#define M_SEQI_9_14 9,10,11,12,13,14
#define M_SEQI_9_15 9,10,11,12,13,14,15
#define M_SEQI_9_16 9,10,11,12,13,14,15,16
#define M_SEQI_9_17 9,10,11,12,13,14,15,16,17
#define M_SEQI_9_18 9,10,11,12,13,14,15,16,17,18
#define M_SEQI_9_19 9,10,11,12,13,14,15,16,17,18,19
#define M_SEQI_9_20 9,10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_9_21 9,10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_9_22 9,10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_9_23 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_9_24 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_9_25 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_9_26 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_10_10 10
#define M_SEQI_10_11 10,11
#define M_SEQI_10_12 10,11,12
#define M_SEQI_10_13 10,11,12,13
#define M_SEQI_10_14 10,11,12,13,14
#define M_SEQI_10_15 10,11,12,13,14,15
#define M_SEQI_10_16 10,11,12,13,14,15,16
#define M_SEQI_10_17 10,11,12,13,14,15,16,17
#define M_SEQI_10_18 10,11,12,13,14,15,16,17,18
#define M_SEQI_10_19 10,11,12,13,14,15,16,17,18,19
#define M_SEQI_10_20 10,11,12,13,14,15,16,17,18,19,20
#define M_SEQI_10_21 10,11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_10_22 10,11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_10_23 10,11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_10_24 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_10_25 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_10_26 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_11_11 11
#define M_SEQI_11_12 11,12
#define M_SEQI_11_13 11,12,13
#define M_SEQI_11_14 11,12,13,14
#define M_SEQI_11_15 11,12,13,14,15
#define M_SEQI_11_16 11,12,13,14,15,16
#define M_SEQI_11_17 11,12,13,14,15,16,17
#define M_SEQI_11_18 11,12,13,14,15,16,17,18
#define M_SEQI_11_19 11,12,13,14,15,16,17,18,19
#define M_SEQI_11_20 11,12,13,14,15,16,17,18,19,20
#define M_SEQI_11_21 11,12,13,14,15,16,17,18,19,20,21
#define M_SEQI_11_22 11,12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_11_23 11,12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_11_24 11,12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_11_25 11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_11_26 11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_12_12 12
#define M_SEQI_12_13 12,13
#define M_SEQI_12_14 12,13,14
#define M_SEQI_12_15 12,13,14,15
#define M_SEQI_12_16 12,13,14,15,16
#define M_SEQI_12_17 12,13,14,15,16,17
#define M_SEQI_12_18 12,13,14,15,16,17,18
#define M_SEQI_12_19 12,13,14,15,16,17,18,19
#define M_SEQI_12_20 12,13,14,15,16,17,18,19,20
#define M_SEQI_12_21 12,13,14,15,16,17,18,19,20,21
#define M_SEQI_12_22 12,13,14,15,16,17,18,19,20,21,22
#define M_SEQI_12_23 12,13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_12_24 12,13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_12_25 12,13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_12_26 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_13_13 13
#define M_SEQI_13_14 13,14
#define M_SEQI_13_15 13,14,15
#define M_SEQI_13_16 13,14,15,16
#define M_SEQI_13_17 13,14,15,16,17
#define M_SEQI_13_18 13,14,15,16,17,18
#define M_SEQI_13_19 13,14,15,16,17,18,19
#define M_SEQI_13_20 13,14,15,16,17,18,19,20
#define M_SEQI_13_21 13,14,15,16,17,18,19,20,21
#define M_SEQI_13_22 13,14,15,16,17,18,19,20,21,22
#define M_SEQI_13_23 13,14,15,16,17,18,19,20,21,22,23
#define M_SEQI_13_24 13,14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_13_25 13,14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_13_26 13,14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_14_14 14
#define M_SEQI_14_15 14,15
#define M_SEQI_14_16 14,15,16
#define M_SEQI_14_17 14,15,16,17
#define M_SEQI_14_18 14,15,16,17,18
#define M_SEQI_14_19 14,15,16,17,18,19
#define M_SEQI_14_20 14,15,16,17,18,19,20
#define M_SEQI_14_21 14,15,16,17,18,19,20,21
#define M_SEQI_14_22 14,15,16,17,18,19,20,21,22
#define M_SEQI_14_23 14,15,16,17,18,19,20,21,22,23
#define M_SEQI_14_24 14,15,16,17,18,19,20,21,22,23,24
#define M_SEQI_14_25 14,15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_14_26 14,15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_15_15 15
#define M_SEQI_15_16 15,16
#define M_SEQI_15_17 15,16,17
#define M_SEQI_15_18 15,16,17,18
#define M_SEQI_15_19 15,16,17,18,19
#define M_SEQI_15_20 15,16,17,18,19,20
#define M_SEQI_15_21 15,16,17,18,19,20,21
#define M_SEQI_15_22 15,16,17,18,19,20,21,22
#define M_SEQI_15_23 15,16,17,18,19,20,21,22,23
#define M_SEQI_15_24 15,16,17,18,19,20,21,22,23,24
#define M_SEQI_15_25 15,16,17,18,19,20,21,22,23,24,25
#define M_SEQI_15_26 15,16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_16_16 16
#define M_SEQI_16_17 16,17
#define M_SEQI_16_18 16,17,18
#define M_SEQI_16_19 16,17,18,19
#define M_SEQI_16_20 16,17,18,19,20
#define M_SEQI_16_21 16,17,18,19,20,21
#define M_SEQI_16_22 16,17,18,19,20,21,22
#define M_SEQI_16_23 16,17,18,19,20,21,22,23
#define M_SEQI_16_24 16,17,18,19,20,21,22,23,24
#define M_SEQI_16_25 16,17,18,19,20,21,22,23,24,25
#define M_SEQI_16_26 16,17,18,19,20,21,22,23,24,25,26
#define M_SEQI_17_17 17
#define M_SEQI_17_18 17,18
#define M_SEQI_17_19 17,18,19
#define M_SEQI_17_20 17,18,19,20
#define M_SEQI_17_21 17,18,19,20,21
#define M_SEQI_17_22 17,18,19,20,21,22
#define M_SEQI_17_23 17,18,19,20,21,22,23
#define M_SEQI_17_24 17,18,19,20,21,22,23,24
#define M_SEQI_17_25 17,18,19,20,21,22,23,24,25
#define M_SEQI_17_26 17,18,19,20,21,22,23,24,25,26
#define M_SEQI_18_18 18
#define M_SEQI_18_19 18,19
#define M_SEQI_18_20 18,19,20
#define M_SEQI_18_21 18,19,20,21
#define M_SEQI_18_22 18,19,20,21,22
#define M_SEQI_18_23 18,19,20,21,22,23
#define M_SEQI_18_24 18,19,20,21,22,23,24
#define M_SEQI_18_25 18,19,20,21,22,23,24,25
#define M_SEQI_18_26 18,19,20,21,22,23,24,25,26
#define M_SEQI_19_19 19
#define M_SEQI_19_20 19,20
#define M_SEQI_19_21 19,20,21
#define M_SEQI_19_22 19,20,21,22
#define M_SEQI_19_23 19,20,21,22,23
#define M_SEQI_19_24 19,20,21,22,23,24
#define M_SEQI_19_25 19,20,21,22,23,24,25
#define M_SEQI_19_26 19,20,21,22,23,24,25,26
#define M_SEQI_20_20 20
#define M_SEQI_20_21 20,21
#define M_SEQI_20_22 20,21,22
#define M_SEQI_20_23 20,21,22,23
#define M_SEQI_20_24 20,21,22,23,24
#define M_SEQI_20_25 20,21,22,23,24,25
#define M_SEQI_20_26 20,21,22,23,24,25,26
#define M_SEQI_21_21 21
#define M_SEQI_21_22 21,22
#define M_SEQI_21_23 21,22,23
#define M_SEQI_21_24 21,22,23,24
#define M_SEQI_21_25 21,22,23,24,25
#define M_SEQI_21_26 21,22,23,24,25,26
#define M_SEQI_22_22 22
#define M_SEQI_22_23 22,23
#define M_SEQI_22_24 22,23,24
#define M_SEQI_22_25 22,23,24,25
#define M_SEQI_22_26 22,23,24,25,26
#define M_SEQI_23_23 23
#define M_SEQI_23_24 23,24
#define M_SEQI_23_25 23,24,25
#define M_SEQI_23_26 23,24,25,26
#define M_SEQI_24_24 24
#define M_SEQI_24_25 24,25
#define M_SEQI_24_26 24,25,26
#define M_SEQI_25_25 25
#define M_SEQI_25_26 25,26
#define M_SEQI_26_26 26

/* Return the input (delay evaluation) */
#define M_ID(...)                 __VA_ARGS__

/* Globber the input */
#define M_EAT(...)

/* M_NARGS: Return number of argument.
   (don't work for empty arg) */
#define M_NARGS(...)                                                    \
  M_RET_ARG53(__VA_ARGS__, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,  \
              41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, \
              26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,               \
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

/* Helper macro to redefine a function with a default value :
   If there is only one variable as the __VA_ARGS__, print
   __VA_ARGS__ then ', value', instead only __VA_ARGS__.
   Example:
    int f(int a, int b);
    #define f(...) M_APPLY(f, M_IF_DEFAULT1(0, __VA_ARGS__))
   This need to be called within a M_APPLY macro.
*/
#define M_IF_DEFAULT1(value, ...)					\
  __VA_ARGS__ M_IF_NARGS_EQ1(__VA_ARGS__)(M_DEFERRED_COMMA value, )

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

/* Within a C11 _Generic statement, all expressions shall be valid C
   expression even if the case if always false, and is not executed.
   This can seriously limit the _Generic statement.
   This macro overcomes this limitation by returning :
   * either the input 'x' if it is of type 'type',
   * or the value 0 view as a type 'type'. */
#define M_AS_TYPE(type, x) _Generic((x)+0, type: (x), default: (type) 0)

/* Return the minimum between x and y (computed in compile time) */
#define M_MIN(x, y) ((x) < (y) ? (x) : (y))

/* Return the maximum between x and y (computed in compile time) */
#define M_MAX(x, y) ((x) > (y) ? (x) : (y))

/* Is the number a power of 2? (computed in compile time) */
#define M_POWEROF2_P(n) (!((n)&((n)-1)))

/* Swap two types */
#define M_SWAP(type, x, y) do {                                 \
  type tmp = (x);                                               \
  (x) = (y);                                                    \
  (y) = tmp;                                                    \
  } while (0)

/* Check if 'n' is assignable to an object of type 'type'.
   It is as if we create a temporary of type 'type' and assign 'n' to it.
   Return the object 'n' if it is possible.
   Two definitions: one with compound-literals for C, the other with static_cast for C++.
   NOTE: C definition is safer than the C++ one.
*/
#ifndef __cplusplus
# define M_ASSIGN_CAST(type, n)                 ((type) { 0 } = (n))
#else
# define M_ASSIGN_CAST(type, n)                 static_cast<type>(n)
#endif

/* Cast 'n' of type 'type*' into 'const type*'.
   This is like (const type*)p but safer as the type of 'n' is checked,
   and more robust for double arrays type.
   NOTE: Not sure if it is 100% compliant with the C standard, but
   I can't find a working implementation (or even a theorical one)
   where it fails.
*/
#ifndef __cplusplus
# define M_CONST_CAST(type, n)                          \
  (((union { type *ptr; const type *cptr; }){n}).cptr)
#else
# define M_CONST_CAST(type, n)                  const_cast<const type*>(n)
#endif

/*
 * From a pointer to a 'field_type' 'field' of a 'type'structure,
 * return pointer to the structure.
 * NOTE: Cast Inside!
 */
#define M_TYPE_FROM_FIELD(type, ptr, field_type, field)                 \
  ((type *)(void*)( (char *)M_ASSIGN_CAST(field_type*, (ptr)) - offsetof(type, field) ))



/************************************************************/
/********************* HASH selection ***********************/
/************************************************************/

/* User code shall overwrite this macro by a random seed (of type size_t)
   so that the hash are not easily predictible by an attacker.
   See https://events.ccc.de/congress/2011/Fahrplan/attachments/2007_28C3_Effective_DoS_on_web_application_platforms.pdf
*/
#ifndef M_HASH_SEED
# define M_HASH_SEED 0
#endif

#if   defined(M_USE_DJB_HASH)
#define M_HASH_INIT 5381UL
#define M_HASH_CALC(h1,h2)  (((h1) * 33UL) + (h2))
#elif defined(M_USE_JSHASH)
#define M_HASH_INIT 1315423911UL
#define M_HASH_CALC(h1,h2)  ((h1) ^ (((h1) << 5) + (h2) + ((h1) >> 2)))
#elif defined(M_USE_BKDRHASH)
#define M_HASH_INIT 0UL
#define M_HASH_CALC(h1,h2)  (((h1) * 131) + (h2))
#elif defined(M_USE_SDBMHASH)
#define M_HASH_INIT 0UL
#define M_HASH_CALC(h1,h2)  ((h2) + ((h1) << 6) + ((h1) << 16) - (h1))
#elif defined(M_USE_DEKHASH)
#define M_HASH_INIT 0UL /* should be len but not possible with interface */
#define M_HASH_CALC(h1,h2)  ( (((h1)<<5) | (h1 >> (CHAR_BIT*sizeof(size_t)-5))) ^(h2))
#elif defined(M_USE_BPHASH)
#define M_HASH_INIT 0UL
#define M_HASH_CALC(h1,h2)  (((h1) << 7) ^ (h2))
#else
//FNVHASH
#define M_HASH_INIT 0UL
#define M_HASH_CALC(h1,h2)  (((h1) * 0x811C9DC5UL) ^ (h2))
#endif

#define M_HASH_DECL(hash)   size_t hash = M_HASH_INIT ^ M_HASH_SEED
#define M_HASH_UP(hash,h)   do { hash = M_HASH_CALC(hash, (h)); } while (0)
#define M_HASH_FINAL(hash)  ( (hash) >> (sizeof(size_t)*CHAR_BIT/2) | (hash) )

/* Safe, efficient, and portable Rotate:
   It should be recognized by any compiler and generate a single roll instruction */
static inline uint32_t m_core_rotl32a (uint32_t x, uint32_t n)
{
  assert (n > 0 && n<32);
  return (x<<n) | (x>>(32-n));
}
static inline uint64_t m_core_rotl64a (uint64_t x, uint32_t n)
{
  assert (n > 0 && n<64);
  return (x<<n) | (x>>(64-n));
}

/* Round to the next highest power of 2.
   See https://web.archive.org/web/20160703165415/https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
*/
static inline uint64_t m_core_roundpow2(uint64_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  v++;
  return v;
}

/* Implement a kind of FNV1A Hash.
   Inspired by http://www.sanmayce.com/Fastest_Hash/ Jesteress and port to 64 bits.
   See https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
   NOTE: A lot of cast. Not really type nor alignement safe.
   NOTE: Can be reduced to very few instructions if constant size argument.
   FIXME: It is trivial for an attacker to generate collision and HASH_SEED doesn't prevent it.
 */
#if SIZE_MAX <= 4294967295U
/* 32 bits variant with an average measured avalanche effect of 16.056 bits */
static inline uint32_t
m_core_hash (const void *str, size_t length)
{
  const uint32_t prime = 709607U;
  uint32_t hash32 = 2166136261U ^ M_HASH_SEED;
  const uint8_t *p = (const uint8_t *)str;

  assert (str != NULL);
  assert ( (( (uintptr_t)p & (sizeof(uint64_t)-1) ) == 0) || (length <= sizeof(uint32_t)));

  while (length >= 2*sizeof(uint32_t)) {
    const uint32_t *ptr = (const uint32_t *) (uintptr_t) p;
    hash32 = (hash32 ^ (m_core_rotl32a(ptr[0], 5) ^ ptr[1])) * prime;
    length -= 2*sizeof(uint32_t);
    p += 2*sizeof(uint32_t);
  }
  // Cases: 0,1,2,3,4,5,6,7
  if (length & sizeof(uint32_t)) {
    const uint32_t *ptr = (const uint32_t *) (uintptr_t) p;
    hash32 = (hash32 ^ ptr[0]) * prime;
    p += sizeof(uint32_t);
    }
  if (length & sizeof(uint16_t)) {
    const uint16_t *ptr = (const uint16_t *) (uintptr_t) p;
    hash32 = (hash32 ^ ptr[0]) * prime;
    p += sizeof(uint16_t);
  }
  if (length & 1)
    hash32 = (hash32 ^ *p) * prime;
  return hash32 ^ (hash32 >> 16);
}
#else
/* 64 bits variant with an average measured avalanche effect of 31.862 bits */
static inline uint64_t
m_core_hash (const void *str, size_t length)
{
  const uint64_t prime = 1099511628211ULL;
  uint64_t hash64 = 14695981039346656037ULL ^ M_HASH_SEED;
  const uint8_t *p = M_ASSIGN_CAST(const uint8_t *, str);

  assert (str != NULL);
  assert ( (( (uintptr_t)p & (sizeof(uint64_t)-1) ) == 0) || (length <= sizeof(uint32_t)));

  while (length >= 2*sizeof(uint64_t)) {
    const uint64_t *ptr = (const uint64_t *) (uintptr_t) p;
    hash64 = (hash64 ^ (m_core_rotl64a(ptr[0], 5) ^ ptr[1])) * prime;
    length -= 2*sizeof(uint64_t);
    p += 2*sizeof(uint64_t);
  }
  //Cases: 0 to 15.
  if (length & sizeof(uint64_t)) {
    const uint64_t *ptr = (const uint64_t *) (uintptr_t) p;
    hash64 = (hash64 ^ ptr[0]) * prime;
    p += sizeof(uint64_t);
    }
  // Cases: 0,1,2,3,4,5,6,7
  if (length & sizeof(uint32_t)) {
    const uint32_t *ptr = (const uint32_t *) (uintptr_t) p;
    hash64 = (hash64 ^ ptr[0]) * prime;
    p += sizeof(uint32_t);
    }
  if (length & sizeof(uint16_t)) {
    const uint16_t *ptr = (const uint16_t *) (uintptr_t) p;
    hash64 = (hash64 ^ ptr[0]) * prime;
    p += sizeof(uint16_t);
  }
  if (length & 1)
    hash64 = (hash64 ^ *p) * prime;
  return hash64 ^ (hash64 >> 32);
}
#endif

/* Define default HASH function.
   Macro encapsulation for C11: use specialized version of the hash function
   if the type is recognized.
   NOTE: Default case is not safe if the type is defined with the '[1]' trick. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define M_HASH_INT32(a) ( (a) ^ ((a) << 11) ^ M_HASH_SEED )
#define M_HASH_INT64(a) ( ( (a) >> 33 ) ^ (a) ^ ((a) << 11) ^ M_HASH_SEED )
#define M_HASH_DEFAULT(a)                                       \
  _Generic((a)+0,                                               \
           int32_t: M_HASH_INT32(M_AS_TYPE(int32_t, a)),        \
           uint32_t: M_HASH_INT32(M_AS_TYPE(uint32_t, a)),      \
           int64_t: M_HASH_INT64(M_AS_TYPE(int64_t, a)),        \
           uint64_t: M_HASH_INT64(M_AS_TYPE(uint64_t, a)),      \
  default:  m_core_hash((const void*) &(a), sizeof (a)) )
#else
#define M_HASH_DEFAULT(a)       m_core_hash((const void*) &(a), sizeof (a))
#endif


/************************************************************/
/******************** METHODS handling **********************/
/************************************************************/

/* List of supported methods for an oplist */
#define M_INIT_INIT(a)           ,a,
#define M_INIT_SET_INIT_SET(a)   ,a,
#define M_INIT_MOVE_INIT_MOVE(a) ,a,
#define M_SWAP_SWAP(a)           ,a,
#define M_SET_SET(a)             ,a,
#define M_MOVE_MOVE(a)           ,a,
#define M_CLEAR_CLEAR(a)         ,a,
#define M_NEW_NEW(a)             ,a,
#define M_DEL_DEL(a)             ,a,
#define M_REALLOC_REALLOC(a)     ,a,
#define M_FREE_FREE(a)           ,a,
#define M_ALIGN_ALIGN(a)         ,a,
#define M_MEMPOOL_MEMPOOL(a)     ,a,
#define M_MEMPOOL_LINKAGE_MEMPOOL_LINKAGE(a)     ,a,
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
#define M_IT_END_IT_END(a)       ,a,
#define M_IT_SET_IT_SET(a)       ,a,
#define M_IT_END_P_IT_END_P(a)   ,a,
#define M_IT_LAST_P_IT_LAST_P(a) ,a,
#define M_IT_EQUAL_P_IT_EQUAL_P(a) ,a,
#define M_IT_NEXT_IT_NEXT(a)     ,a,
#define M_IT_PREVIOUS_IT_PREVIOUS(a)     ,a,
#define M_IT_REF_IT_REF(a)       ,a,
#define M_IT_CREF_IT_CREF(a)     ,a,
#define M_IT_REMOVE_IT_REMOVE(a) ,a,
#define M_IT_INSERT_IT_INSERT(a) ,a,
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
#define M_INC_ALLOC_INC_ALLOC(a) ,a,
#define M_OOR_SET_OOR_SET(a)     ,a,
#define M_OOR_EQUAL_OOR_EQUAL(a) ,a,

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
#define M_GET_INIT_MOVE(...) M_GET_METHOD(INIT_MOVE,   M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_SET(...)       M_GET_METHOD(SET,         M_SET_DEFAULT,      __VA_ARGS__)
#define M_GET_MOVE(...)      M_GET_METHOD(MOVE,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_SWAP(...)      M_GET_METHOD(SWAP,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_CLEAR(...)     M_GET_METHOD(CLEAR,       M_NOTHING_DEFAULT,  __VA_ARGS__)
#define M_GET_NEW(...)       M_GET_METHOD(NEW,         M_NEW_DEFAULT,      __VA_ARGS__)
#define M_GET_DEL(...)       M_GET_METHOD(DEL,         M_DEL_DEFAULT,      __VA_ARGS__)
#define M_GET_REALLOC(...)   M_GET_METHOD(REALLOC,     M_REALLOC_DEFAULT,  __VA_ARGS__)
#define M_GET_FREE(...)      M_GET_METHOD(FREE,        M_FREE_DEFAULT,     __VA_ARGS__)
#define M_GET_ALIGN(...)     M_GET_METHOD(ALIGN,       M_ALIGN_DEFAULT,    __VA_ARGS__)
#define M_GET_MEMPOOL(...)   M_GET_METHOD(MEMPOOL,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_MEMPOOL_LINKAGE(...)   M_GET_METHOD(MEMPOOL_LINKAGE, ,       __VA_ARGS__)
#define M_GET_HASH(...)      M_GET_METHOD(HASH,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_EQUAL(...)     M_GET_METHOD(EQUAL,       M_EQUAL_DEFAULT,    __VA_ARGS__)
#define M_GET_CMP(...)       M_GET_METHOD(CMP,         M_CMP_DEFAULT,      __VA_ARGS__)
#define M_GET_UPDATE(...)    M_GET_METHOD(UPDATE,      M_SET_DEFAULT,      __VA_ARGS__)
#define M_GET_TYPE(...)      M_GET_METHOD(TYPE,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_SUBTYPE(...)   M_GET_METHOD(SUBTYPE,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_OPLIST(...)    M_GET_METHOD(OPLIST,      (),                 __VA_ARGS__)
#define M_GET_SORT(...)      M_GET_METHOD(SORT,        M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_TYPE(...)   M_GET_METHOD(IT_TYPE,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_FIRST(...)  M_GET_METHOD(IT_FIRST,    M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_LAST(...)   M_GET_METHOD(IT_LAST,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_END(...)    M_GET_METHOD(IT_END,      M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_SET(...)    M_GET_METHOD(IT_SET,      M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_END_P(...)  M_GET_METHOD(IT_END_P,    M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_LAST_P(...) M_GET_METHOD(IT_LAST_P,   M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_EQUAL_P(...) M_GET_METHOD(IT_EQUAL_P, M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_NEXT(...)   M_GET_METHOD(IT_NEXT,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_PREVIOUS(...) M_GET_METHOD(IT_PREVIOUS, M_NO_DEFAULT,     __VA_ARGS__)
#define M_GET_IT_REF(...)    M_GET_METHOD(IT_REF,      M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_CREF(...)   M_GET_METHOD(IT_CREF,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_REMOVE(...) M_GET_METHOD(IT_REMOVE,   M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_IT_INSERT(...) M_GET_METHOD(IT_INSERT,   M_NO_DEFAULT,       __VA_ARGS__)
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
#define M_GET_INC_ALLOC(...) M_GET_METHOD(INC_ALLOC,   M_INC_ALLOC_DEFAULT, __VA_ARGS__)
#define M_GET_OOR_SET(...)   M_GET_METHOD(OOR_SET,     M_NO_DEFAULT,       __VA_ARGS__)
#define M_GET_OOR_EQUAL(...) M_GET_METHOD(OOR_EQUAL,   M_NO_DEFAULT,       __VA_ARGS__)

/* Define the default method. */
#define M_INIT_DEFAULT(a)       ((a) = 0)
#define M_SET_DEFAULT(a,b)      ((a) = (b))
#define M_NOTHING_DEFAULT(...)  ((void)(__VA_ARGS__))
#define M_EMPTY_DEFAULT(...)    ((void)1)
#define M_TRUE_DEFAULT(...)     true
#define M_NEW_DEFAULT(a)        M_MEMORY_ALLOC(a)
#define M_DEL_DEFAULT(a)        M_MEMORY_DEL(a)
#define M_REALLOC_DEFAULT(t,p,s) M_MEMORY_REALLOC(t,p,s)
#define M_FREE_DEFAULT(a)       M_MEMORY_FREE(a)
#define M_ALIGN_DEFAULT         0
#define M_NO_DEFAULT(...)       m_no_default_function
#define M_EQUAL_DEFAULT(a,b)    ((a) == (b))
#define M_CMP_DEFAULT(a,b)      ((a) < (b) ? -1 : (a) > (b))
#define M_ADD_DEFAULT(a,b,c)    ((a) = (b) + (c))
#define M_SUB_DEFAULT(a,b,c)    ((a) = (b) - (c))
#define M_MUL_DEFAULT(a,b,c)    ((a) = (b) * (c))
#define M_DIV_DEFAULT(a,b,c)    ((a) = (b) / (c))
#define M_NO_EXT_ALGO(n,co,to)
#define M_INC_ALLOC_DEFAULT(n)   (M_MAX(8, (n))*2)

/* NOTE: Theses operators are NOT compatible with the '[1]' tricks
   if the variable is defined as a parameter of a function
   (sizeof (a) is not portable). */
#define M_MOVE_DEFAULT(a,b)     (M_MEMCPY_DEFAULT(a, b), M_MEMSET_DEFAULT(b))
#define M_MEMCPY_DEFAULT(a,b)   (memcpy(&(a), &(b), sizeof (a)))
#define M_MEMSET_DEFAULT(a)     (memset(&(a), 0, sizeof (a)))

/* Default oplist for C standard types (int & float) */
#define M_DEFAULT_OPLIST                                                \
  (INIT(M_INIT_DEFAULT), INIT_SET(M_SET_DEFAULT), SET(M_SET_DEFAULT),   \
   CLEAR(M_NOTHING_DEFAULT), EQUAL(M_EQUAL_DEFAULT), CMP(M_CMP_DEFAULT), \
   INIT_MOVE(M_MOVE_DEFAULT), MOVE(M_MOVE_DEFAULT) ,                    \
   HASH(M_HASH_DEFAULT)                                                 \
   /*ADD(M_ADD_DEFAULT), SUB(M_SUB_DEFAULT), MUL(M_MUL_DEFAULT),*/      \
   /*DIV(M_DIV_DEFAULT)*/ )

/* TODO: Add an option for more classic operators ? CMP ? EQUAL ? */
#define M_CLASSIC_OPLIST(name) (                    \
  INIT(M_C(name, _init)),                           \
  INIT_SET(M_C(name, _init_set)),                   \
  SET(M_C(name, _set)),                             \
  CLEAR(M_C(name, _clear)),                         \
  TYPE(M_C(name, _t)) )

/* OPLIST for 'const char *' (with NO memory allocation) */
#define M_CSTR_HASH(s) (m_core_hash((s), strlen(s)))
#define M_CSTR_EQUAL(a,b) (strcmp((a),(b)) == 0)
#define M_CSTR_OPLIST (INIT(M_INIT_DEFAULT), INIT_SET(M_SET_DEFAULT),   \
                       SET(M_SET_DEFAULT), CLEAR(M_NOTHING_DEFAULT),    \
                       HASH(M_CSTR_HASH), EQUAL(M_CSTR_EQUAL),          \
                       CMP(strcmp), TYPE(const char *) )

/* From an oplist (...) return ... */
#define M_OPFLAT(...)     __VA_ARGS__

/* Concat two oplists in one. op1 will have higher priority to op2 */
#define M_OPCAT(op1,op2) (M_OPFLAT op1, M_OPFLAT op2)

/* Apply an oplist */
#define M_OPAPPLY(a, oplist)  a oplist

/* Extend an oplist by adding some methods */
#define M_OPEXTEND(op, ...) (__VA_ARGS__, M_OPFLAT op)

/* Test if a method is present in an oplist.
   Return 0 (method is absent or disabled) or 1 (method is present and not disabled) */
#define M_TEST_METHOD_P(method, oplist)                 \
  M_BOOL(M_GET_METHOD (method, 0, M_OPFLAT oplist))

/* Test if a method is disabled in an oplist.
   To disable an oplist, just put '0' in the method like this:
   Example: (INIT(0), CLEAR(clear))
   Here INIT is disabled, whereas CLEAR is not.
   Return 1 (method is disabled) or 0 (method is not disabled - absent or present) */
#define M_TEST_DISABLED_METHOD_P(method, oplist)        \
  M_INV(M_BOOL(M_GET_METHOD (method, 1, M_OPFLAT oplist)))

/* Perfom a preprocessing M_IF, if the method is present in the oplist.
   Example: M_IF_METHOD(HASH, oplist)(define function with HASH method, ) */
#define M_IF_METHOD(method, oplist) M_IF(M_TEST_METHOD_P(method, oplist))

/* Perfom a preprocessing M_IF, if the method is disabled in the oplist.
   Example: M_IF_DISABLED_METHOD(HASH, oplist)(define function without HASH method, ) */
#define M_IF_DISABLED_METHOD(method, oplist) M_IF(M_TEST_DISABLED_METHOD_P(method, oplist))

/* Perform a preprocessing M_IF if the method exists in both oplist.
   Example: M_IF_METHOD_BOTH(HASH, oplist1, oplist2) (define function with HASH method, ) */
#define M_IF_METHOD_BOTH(method, oplist1, oplist2)                      \
  M_IF(M_AND(M_TEST_METHOD_P(method, oplist1), M_TEST_METHOD_P(method, oplist2)))

/* Perform a preprocessing M_IF if both methods exist in the oplist.
   Example: M_IF_METHOD2(HASH, CMP, oplist) (define function with HASH & CMP method, ) */
#define M_IF_METHOD2(method1, method2, oplist)				\
  M_IF(M_AND(M_TEST_METHOD_P(method1, oplist), M_TEST_METHOD_P(method2, oplist)))

/* Perform a preprocessing M_IF if the method exists for all oplist.
   Example: M_IF_METHOD_ALL(HASH, oplist1, oplist2) (define function with HASH method, ) */
#define M_IF_METHOD_ALL(method, ...)                            \
  M_IF(M_REDUCE2(M_TEST_METHOD_P, M_AND, method, __VA_ARGS__))

/* By putting this after a method, we transform the argument list
   so that the first argument becomes a pointer to the destination. */
#define M_IPTR(...) ( & __VA_ARGS__ )

/* Perform an INIT_MOVE if present, or emulate it using INIT_SET/CLEAR
   if it has been disabled. Otherwise use default move constructor
   (memcpy) */
#define M_DO_INIT_MOVE(oplist, dest, src) do {                          \
 M_IF_METHOD(INIT_MOVE, oplist)(M_GET_INIT_MOVE oplist ((dest), (src)), \
 M_IF_DISABLED_METHOD(INIT_MOVE, oplist)(M_GET_INIT_SET oplist ((dest), (src)) ; \
                                         M_GET_CLEAR oplist (src),      \
                                         M_MOVE_DEFAULT((dest), (src)) )); \
  } while (0)

/* Perform a MOVE if present, or emulate it using CLEAR/INIT_MOVE
   otherwise */
#define M_DO_MOVE(oplist, dest, src) do {                               \
    M_IF_METHOD(MOVE, oplist)(M_GET_MOVE oplist (dest, src);,           \
                              M_GET_CLEAR oplist (dest);                \
                              M_DO_INIT_MOVE (oplist, dest, src) ; )    \
      } while (0)

/* Test if the argument is a valid oplist.
   NOTE: Imperfect test.
*/
#define M_OPLIST_P(a)                                                   \
  M_AND(M_AND(M_PARENTHESIS_P(a), M_INV(M_PARENTHESIS_P (M_OPFLAT a))), \
        M_EMPTY_P(M_EAT a))


/* Check if a is an oplist, and return a
   or if a symbol composed of M_OPL_##a() is defined as an oplist, and returns it
   otherwise return a.
   In short, if a global oplist is defined for the argument, it returns it
   otherwise it returns the argument.
   Global oplist is limited to typedef types.
*/
#define M_GLOBAL_OPLIST(a)                                              \
  M_IF( M_OPLIST_P(a))(a, M_GLOBALI_OPLIST_ELSE(a))
#define M_GLOBALI_OPLIST_ELSE(a)            M_GLOBALI_OPLIST_ELSE2(a, M_C(M_OPL_, a)())
#define M_GLOBALI_OPLIST_ELSE2(a, op)       M_IF( M_OPLIST_P (op))(op, a)


/* Check if a a symbol composed of M_OPL_##a() is defined as an oplist, and returns it
   otherwise return M_DEFAULT_OPLIST.
   In short, if a global oplist is defined for the argument, it returns it
   otherwise it returns the argument.
   Global oplist is limited to typedef types.
*/
#define M_GLOBAL_OPLIST_OR_DEF(a)                                       \
  M_IF( M_PARENTHESIS_P(a))(M_DEFAULT_OPLIST, M_GLOBAL_OPLIST_OR_DEF_ELSE(a))
#define M_GLOBAL_OPLIST_OR_DEF_ELSE(a)      M_GLOBAL_OPLIST_OR_DEF_ELSE2(M_C(M_OPL_, a)())
#define M_GLOBAL_OPLIST_OR_DEF_ELSE2(op)    M_IF( M_OPLIST_P(op))(op, M_DEFAULT_OPLIST)


/************************************************************/
/******************** Syntax Enhancing **********************/
/************************************************************/

/* Define M_EACH macro allowing to iterate over a container
   First argument will be a created pointer to the underlying type.
   Third argument can be the oplist of the list or the type of the list if a global
   oplist has been recorded.
   Example: for M_EACH(item, list, LIST_OPLIST) { action; } 
*/
#define M_EACH(item, container, oplist)                                 \
  M_EACHI_OPLIST(item, container, M_GLOBAL_OPLIST(oplist))

#define M_EACHI_OPLIST(item, container, oplist)                         \
  M_IF_METHOD(IT_REF, oplist)(M_EACHI, M_EACHI_CONST)                   \
  (item, container, oplist, M_C(local_iterator_, __LINE__),             \
   M_C(local_cont_, __LINE__))

/* Internal for M_EACH with M_GET_IT_REF operator */
#define M_EACHI(item,container,oplist, iterator, cont)                  \
  (bool cont = true; cont; cont = false)                                \
  for(M_GET_SUBTYPE oplist *item; cont ; cont = false)                  \
    for(M_GET_IT_TYPE oplist iterator; cont ; cont = false)             \
      for(M_GET_IT_FIRST oplist (iterator, container) ;                 \
          !M_GET_IT_END_P oplist (iterator)                             \
            && (item = M_GET_IT_REF oplist (iterator), true) ;          \
          M_GET_IT_NEXT oplist (iterator))

/* Internal for M_EACH with M_GET_IT_CREF operator */
#define M_EACHI_CONST(item,container,oplist, iterator, cont)            \
  (bool cont = true; cont; cont = false)                                \
  for(const M_GET_SUBTYPE oplist *item; cont ; cont = false)            \
    for(M_GET_IT_TYPE oplist iterator; cont ; cont = false)             \
      for(M_GET_IT_FIRST oplist (iterator, container) ;                 \
          !M_GET_IT_END_P oplist (iterator)                             \
            && (item = M_GET_IT_CREF oplist (iterator), true) ;         \
          M_GET_IT_NEXT oplist (iterator))


/* Define M_LET macro allowing to define, auto init & auto clear an object
   within the next bracket.
   Example:
     M_LET(a, STRING_OPLIST) { do something with a }  or
     M_LET(a, b, c, STRING_OPLIST) { do something with a, b & c }
   NOTE: The user code can not perform a return or a goto within the {}
   other wise the clear code of the object won't be called .
   Last argument can be the oplist or the type itself if a global
   oplist has been recorded for this type.
 */
#define M_LET(...)                                                      \
  M_ID(M_LETI1 M_INVERT( __VA_ARGS__, M_C(local_cont_, __LINE__) ))

#define M_LETI1(cont, oplist, ...)                                      \
  M_LETI2(cont, M_GLOBAL_OPLIST(oplist), __VA_ARGS__)

#define M_LETI2(cont, oplist, ...)                                      \
  for(bool cont = true; cont ; /* unused */)                            \
    M_MAP2(M_LETI_SINGLE, (cont, oplist,), __VA_ARGS__)

#define M_LETI_SINGLE(data, name)                               \
  M_LETI_SINGLE2(M_RET_ARG1 data, M_RET_ARG2 data, name)

#define M_LETI_SINGLE2(cont, oplist, name)                              \
  for(M_GET_TYPE oplist name;                                           \
      cont && (M_GET_INIT oplist (name), true);                         \
      (M_GET_CLEAR oplist (name), cont = false))

/* Transform the va list by adding their number as the first argument of
   the list.
   Example:   M_VA(a,b,c,d,e) ==> 5,a,b,c,d,e */
#define M_VA(...) M_NARGS(__VA_ARGS__), __VA_ARGS__


/************************************************************/
/********************* MEMORY handling **********************/
/************************************************************/

/* Note: For C build, we explicitly don't cast the return value of
   malloc, realloc as it is safer (compilers shall warn in case
   of invalid implicit cast, whereas they won't if there is an 
   explicit cast) */
// TODO: M_MEMORY_ALLOC(type, alignement)
//       M_MEMORY_DEL(type, ptr)
//       M_MEMORY_REALLOC(type, ptr, n, alignement)
//       M_MEMORY_FREE(type, ptr, n)
// alignement==0 ==> default alignement
// What about global state for memory allocator?
// Shall support effectively fixed size allocator.
// What about oplist? Can open up ALIGN & ALLOC_STATE opertors, and is
// more flexible.
// But it can be done by overloading the M_GET_NEW / M_GET_DEL macros:
// they can force an alignement for an instance.
// The main issue may be adding an object within the instance
// associated to the memory allocation.
#ifndef M_MEMORY_ALLOC
#ifdef __cplusplus
# include <cstdlib>
# define M_MEMORY_ALLOC(type) ((type*)std::malloc (sizeof (type)))
# define M_MEMORY_DEL(ptr)  std::free(ptr)
#else
# include <stdlib.h>
# define M_MEMORY_ALLOC(type) malloc (sizeof (type))
# define M_MEMORY_DEL(ptr)  free(ptr)
#endif
#endif

#ifndef M_MEMORY_REALLOC
#ifdef __cplusplus
# include <cstdlib>
# define M_MEMORY_REALLOC(type, ptr, n)         \
  ((type*) (M_UNLIKELY ((n) > SIZE_MAX / sizeof(type)) ? NULL : std::realloc ((ptr), (n)*sizeof (type))))
# define M_MEMORY_FREE(ptr) std::free(ptr)
#else
# include <stdlib.h>
# define M_MEMORY_REALLOC(type, ptr, n) (M_UNLIKELY ((n) > SIZE_MAX / sizeof(type)) ? NULL : realloc ((ptr), (n)*sizeof (type)))
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
    fprintf(stderr, "ERROR(M*LIB): Can not allocate memory in function %s of file %s:%d (%lu bytes).\n", \
            __func__, __FILE__, __LINE__, (unsigned long) size);	\
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
