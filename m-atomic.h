/*
 * M*LIB - Thin stdatomic wrapper for C++ compatibility
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
#ifndef __M_ATOMIC_H
#define __M_ATOMIC_H

/* NOTE: Due to the C++ not having recognized stdatomic.h officialy,
   it is hard to use this header directly with a C++ compiler like 
   g++ or MSVC.
   (See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60932 ).
   clang has no issue with this header.
   GCC 4.9 doesn't have a working implementation of 'atomic'
*/
#if defined(__cplusplus) && __cplusplus >= 201103L	\
  && !defined(__clang__) && !(defined(__GNUC__) && __GNUC__ < 5)

/* NOTE: This is what the stdatomic.h header shall do in C++ mode. */
#include <atomic>

using std::memory_order;

using std::atomic_bool;
using std::atomic_char;
using std::atomic_short;
using std::atomic_int;
using std::atomic_long;
using std::atomic_llong;
using std::atomic_uchar;
using std::atomic_schar;
using std::atomic_ushort;
using std::atomic_uint;
using std::atomic_ulong;
using std::atomic_ullong;
using std::atomic_intptr_t;
using std::atomic_uintptr_t;
using std::atomic_size_t;
using std::atomic_ptrdiff_t;
using std::atomic_intmax_t;
using std::atomic_uintmax_t;
using std::atomic_flag;

using std::kill_dependency;
using std::atomic_thread_fence;
using std::atomic_signal_fence;
using std::atomic_is_lock_free;
using std::atomic_store_explicit;
using std::atomic_store;
using std::atomic_load_explicit;
using std::atomic_load;
using std::atomic_exchange_explicit;
using std::atomic_exchange;
using std::atomic_compare_exchange_strong_explicit;
using std::atomic_compare_exchange_strong;
using std::atomic_compare_exchange_weak_explicit;
using std::atomic_compare_exchange_weak;
using std::atomic_fetch_add;
using std::atomic_fetch_add_explicit;
using std::atomic_fetch_sub;
using std::atomic_fetch_sub_explicit;
using std::atomic_fetch_or;
using std::atomic_fetch_or_explicit;
using std::atomic_fetch_xor;
using std::atomic_fetch_xor_explicit;
using std::atomic_fetch_and;
using std::atomic_fetch_and_explicit;
using std::atomic_flag_test_and_set;
using std::atomic_flag_test_and_set_explicit;
using std::atomic_flag_clear;
using std::atomic_flag_clear_explicit;

#define _Atomic(T) std::atomic< T >

#elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)	\
  || ( defined(__GNUC__) && !defined(__clang__) && !defined(__cplusplus)) \
  || (defined(__clang__) && __clang_major__ >= 4)

/* CLANG 3.5 has issues with GCC's stdatomic.h
   and doesn't provide its own stdatomic.h */
#include <stdatomic.h>

#else

/* No working atomic.h, nor working stdatomic.h
   Write a compatible slin layer using mutex. */
#include "m-mutex.h"

/* The structure is quite large... */
#define	_Atomic(T)                              \
  struct {                                      \
    volatile T __val;                           \
    T          __zero;                          \
    T          __previous;                      \
    bool       __cmp;                           \
    m_mutex_t  __lock;                          \
  }

/* Even memory order is defined, only the strongest constraint exist */
typedef enum {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
} memory_order;

typedef _Atomic(bool)               atomic_bool;
typedef _Atomic(char)               atomic_char;
typedef _Atomic(short)              atomic_short;
typedef _Atomic(int)                atomic_int;
typedef _Atomic(long)               atomic_long;
typedef _Atomic(long long)          atomic_llong;
typedef _Atomic(unsigned char)      atomic_uchar;
typedef _Atomic(signed char)        atomic_schar;
typedef _Atomic(unsigned short)     atomic_ushort;
typedef _Atomic(unsigned int)       atomic_uint;
typedef _Atomic(unsigned long)      atomic_ulong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(intptr_t)           atomic_intptr_t;
typedef _Atomic(uintptr_t)          atomic_uintptr_t;
typedef _Atomic(size_t)             atomic_size_t;
typedef _Atomic(ptrdiff_t)          atomic_ptrdiff_t;
/* TODO: Test if stdint.h was included?
   typedef _Atomic(intmax_t)           atomic_intmax_t;
   typedef _Atomic(uintmax_t)          atomic_uintmax_t;*/

/* Unlock the mutex and return the given value */
static inline long long atomic_fetch_unlock (m_mutex_t *lock, long long val)
{
  m_mutex_unlock (*lock);
  return val;
}

#define atomic_fetch_op(ptr, val, op)                                   \
  (m_mutex_lock((ptr)->__lock),                                         \
   (ptr)->__previous = (ptr)->__val,                                    \
   (ptr)->__val op (val),                                               \
   atomic_fetch_unlock(&(ptr)->__lock, (ptr)->__previous-(ptr)->__zero)+(ptr)->__zero)

#define atomic_fetch_add(ptr, val) atomic_fetch_op(ptr, val, +=)
#define atomic_fetch_sub(ptr, val) atomic_fetch_op(ptr, val, -=)
#define atomic_fetch_or(ptr, val)  atomic_fetch_op(ptr, val, |=)
#define atomic_fetch_xor(ptr, val) atomic_fetch_op(ptr, val, ^=)
#define atomic_fetch_and(ptr, val) atomic_fetch_op(ptr, val, &=)
#define atomic_exchange(ptr, val)  atomic_fetch_op(ptr, val, =)

#define ATOMIC_VAR_INIT(val) { val, 0, 0, 0, M_MUTEXI_INIT_VALUE }

#define atomic_init(ptr, val)                                           \
  (m_mutex_init((ptr)->__lock), (ptr)->__val = val, (ptr)->__zero = 0)

#define atomic_load(ptr)                        \
  (m_mutex_lock((ptr)->__lock),                                         \
   (ptr)->__previous = (ptr)->__val,                                    \
   atomic_fetch_unlock(&(ptr)->__lock, (ptr)->__previous-(ptr)->__zero)+(ptr)->__zero)

#define atomic_store(ptr, val)                                          \
  (m_mutex_lock((ptr)->__lock),                                         \
   (ptr)->__val = val,                                                  \
   m_mutex_unlock((ptr)->__lock))

#define atomic_compare_exchange_strong(ptr, exp, val)                   \
  (m_mutex_lock((ptr)->__lock),                                         \
   (ptr)->__cmp = (ptr)->__val == *(exp),                               \
   ((ptr)->__cmp != 0 && ((ptr)->__val = (val))),                       \
   ((ptr)->__cmp == 0 && (*(exp) = (ptr)->__val)),                      \
   atomic_fetch_unlock(&(ptr)->__lock, (ptr)->__cmp))

#define atomic_fetch_add_explicit(ptr, val, mem) atomic_fetch_op(ptr, val, +=)
#define atomic_fetch_sub_explicit(ptr, val, mem) atomic_fetch_op(ptr, val, -=)
#define atomic_fetch_or_explicit(ptr, val, mem)  atomic_fetch_op(ptr, val, |=)
#define atomic_fetch_xor_explicit(ptr, val, mem) atomic_fetch_op(ptr, val, ^=)
#define atomic_fetch_and_explicit(ptr, val, mem) atomic_fetch_op(ptr, val, &=)
#define atomic_exchange_explicit(ptr, val, mem)  atomic_fetch_op(ptr, val, =)
#define atomic_load_explicit(ptr, mem)           atomic_load(ptr)
#define atomic_store_explicit(ptr, val, mem)     atomic_store(ptr, val)
#define kill_dependency(ptr)                     atomic_load(ptr)
#define atomic_thread_fence(mem)                 (void) 0
#define atomic_signal_fence(mem)                 (void) 0
#define atomic_is_lock_free(ptr)                 false
#define atomic_compare_exchange_strong_explicit(ptr, exp, val, mem) atomic_compare_exchange_strong(ptr, exp, val)
#define atomic_compare_exchange_weak_explicit(ptr, exp, val, mem)   atomic_compare_exchange_strong(ptr, exp, val)
#define atomic_compare_exchange_weak(ptr, exp, val)                 atomic_compare_exchange_strong(ptr, exp, val)

/* TODO: Missing atomic_flag */

#endif

#endif
