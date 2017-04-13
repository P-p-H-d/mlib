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
   clang has no issue with this header.*/
#if defined(__cplusplus) && __cplusplus >= 201103L && !defined(__clang__)

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

#else

#include <stdatomic.h>

#endif

#endif
