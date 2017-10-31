/*
 * M*LIB - Thin Mutex & Thread wrapper
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
#ifndef __M_MUTEX_H
#define __M_MUTEX_H

#include <assert.h>
#include <stdbool.h>
#include "m-core.h"

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L    \
  && !defined(__STDC_NO_THREADS__)

/****************************** C11 version ********************************/

#include <threads.h>

typedef mtx_t                  m_mutex_t[1];
typedef cnd_t                  m_cond_t[1];
typedef thrd_t                 m_thread_t[1];

static inline void m_mutex_init(m_mutex_t m)
{
  int rc = mtx_init(m, mtx_plain);
  M_ASSERT_INIT (rc == 0);
}

static inline void m_mutex_clear(m_mutex_t m)
{
  mtx_destroy(m);
}

static inline void m_mutex_lock(m_mutex_t m)
{
  mtx_lock(m);
}

static inline void m_mutex_unlock(m_mutex_t m)
{
  mtx_unlock(m);
}

static inline void m_cond_init(m_cond_t c)
{
  int rc = cnd_init(c);
  M_ASSERT_INIT (rc == 0);
}

static inline void m_cond_clear(m_cond_t c)
{
  cnd_destroy(c);
}

static inline void m_cond_clear(m_cond_t c)
{
  cnd_signal(c);
}

static inline void m_cond_wait(m_cond_t c, m_mutex_t m)
{
  cnd_wait(c, m);
}

static inline void m_thread_create(m_thread_t t, void (*func)(void*), void* arg)
{
  int rc = thrd_create(t, ((int)(*)(void*))func, arg);
  M_ASSERT_INIT (rc == thrd_success);
}

static inline void m_thread_join(m_thread_t t)
{
  int rc = thrd_join(t, NULL);
  assert (rc == thrd_success);
}

// Internal type, not exported.
typedef once_flag                     m_oncei_t[1];
#define M_ONCEI_INIT_VALUE            { ONCE_FLAG_INIT }
static inline void m_oncei_call(m_oncei_t o, void (*func)(void))
{
  call_once(o,func);
}




#elif defined(WIN32) || defined(_WIN32)

/****************************** WIN32 version ******************************/

#include <windows.h>

typedef HANDLE                 m_mutex_t[1];
typedef HANDLE                 m_cond_t[1];
typedef HANDLE                 m_thread_t[1];

static inline void m_mutex_init(m_mutex_t m)
{
  *m = CreateMutex(NULL, FALSE, NULL);
  M_ASSERT_INIT(*m != NULL);
}

static inline void m_mutex_clear(m_mutex_t m)
{
  CloseHandle(*m);
}

static inline void m_mutex_lock(m_mutex_t m)
{
  DWORD dwWaitResult = WaitForSingleObject(*m, INFINITE);
  assert (dwWaitResult == WAIT_OBJECT_0);
}

static inline void m_mutex_unlock(m_mutex_t m)
{
  ReleaseMutex(*m);
}

// Internal function, not exported
#define M_MUTEXI_INIT_VALUE      { NULL }
static inline void m_mutexi_lazy_lock (m_mutex_t m)
{
  HANDLE *addr = m;
  if (*addr == NULL) {
    /* Try to create one, affect it atomicaly and otherwise clears it */
    HANDLE h = CreateMutex(NULL, FALSE, NULL);
    if (InterlockedCompareExchangePointer((PVOID*)addr, (PVOID)h, NULL) != NULL)
      CloseHandle(h);
    /* FIXME: there is no way to clean the mutex at program exit... */
  }
  DWORD dwWaitResult = WaitForSingleObject(*addr, INFINITE);
  assert (dwWaitResult == WAIT_OBJECT_0);
}

static inline void m_cond_init(m_cond_t c)
{
  *c = CreateEvent (NULL,  FALSE, FALSE, NULL);
  M_ASSERT_INIT (*c != NULL);
}

static inline void m_cond_clear(m_cond_t c)
{
  CloseHandle(*c);
}

static inline void m_cond_signal(m_cond_t c)
{
  PulseEvent(*c);
}

static inline void m_cond_wait(m_cond_t c, m_mutex_t m)
{
  DWORD dwWaitResult = SignalObjectAndWait(*m, *c, INFINITE,FALSE);
  assert (dwWaitResult == WAIT_OBJECT_0);
  dwWaitResult = WaitForSingleObject(*m, INFINITE);
  assert (dwWaitResult == WAIT_OBJECT_0);
}

static inline void m_thread_create(m_thread_t t, void (*func)(void*), void *arg)
{
  *t = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, NULL);
  M_ASSERT_INIT (*t != NULL);
}

static inline void m_thread_join(m_thread_t t)
{
  DWORD dwWaitResult = WaitForSingleObject(*t, INFINITE);
  assert (dwWaitResult == WAIT_OBJECT_0);
}




#else

/**************************** PTHREAD version ******************************/

#include <pthread.h>

typedef pthread_mutex_t        m_mutex_t[1];
typedef pthread_cond_t         m_cond_t[1];
typedef pthread_t              m_thread_t[1];

static inline void m_mutex_init(m_mutex_t m)
{
  int _rc = pthread_mutex_init(m, NULL);
  M_ASSERT_INIT (_rc == 0);
}

static inline void m_mutex_clear(m_mutex_t m)
{
  pthread_mutex_destroy(m);
}

static inline void m_mutex_lock(m_mutex_t m)
{
  pthread_mutex_lock(m);
}

static inline void m_mutex_unlock(m_mutex_t m)
{
  pthread_mutex_unlock(m);
}

#define M_MUTEXI_INIT_VALUE    { PTHREAD_MUTEX_INITIALIZER }
static inline void m_mutexi_lazy_lock(m_mutex_t m)
{
  pthread_mutex_lock(m);
}

static inline void m_cond_init(m_cond_t c)
{
  int _rc = pthread_cond_init(c, NULL);
  M_ASSERT_INIT (_rc == 0);
}

static inline void m_cond_clear(m_cond_t c)
{
  pthread_cond_destroy(c);
}

static inline void m_cond_signal(m_cond_t c)
{
  pthread_cond_signal(c);
}

static inline void m_cond_wait(m_cond_t c, m_mutex_t m)
{
  pthread_cond_wait(c, m);
}

static inline void m_thread_create(m_thread_t t, void (*func)(void*), void *arg)
{
  int _rc = pthread_create(t, NULL, (void*(*)(void*))func, arg);
  M_ASSERT_INIT (_rc == 0);
}

static inline void m_thread_join(m_thread_t t)
{
  int _rc = pthread_join(*t, NULL);
  (void)_rc; // Avoid warning about variable unused.
  assert (_rc == 0);
}

// Internal type, not exported.
typedef pthread_once_t                m_oncei_t[1];
#define M_ONCEI_INIT_VALUE            { PTHREAD_ONCE_INIT }
static inline void m_oncei_call(m_oncei_t o, void (*func)(void))
{
  pthread_once(o,func);
}

#endif

/* M_LOCK macro. Allow simple locking encapsulation.
   USAGE:
    static M_LOCK_DECL(name);
    int f(int n) {
      M_LOCK(name) {
        // Exclusive access
      }
    }
*/
/* NOTE: Either using direct support by the OS (WIN32/PTHREAD)
   or using C11's ONCE mechanism */
#ifdef M_MUTEXI_INIT_VALUE
# define M_LOCK_DECL(name) m_mutex_t name = M_MUTEXI_INIT_VALUE
# define M_LOCK(name)                                                   \
  M_LOCKI_DO(name, M_C(local_cont_, __LINE__), m_mutexi_lazy_lock, m_mutex_unlock)
#else
# define M_LOCK_DECL(name)                                      \
  m_mutex_t name;                                               \
  static void M_C(m_mutex_init_, name)(void) {                  \
    m_mutex_init(name);                                         \
  }                                                             \
  m_oncei_t M_C(m_once_, name) = M_ONCEI_INIT_VALUE
# define M_LOCKI_BY_ONCE(name)                                          \
  (m_oncei_call(M_C(m_once_, name), M_C(m_mutex_init_, name)),          \
   m_mutex_lock(name), (void) 0 )
# define M_LOCK(name)                                                   \
  M_LOCKI_DO(name, M_C(local_cont_, __LINE__), M_LOCKI_BY_ONCE, m_mutex_unlock)
#endif

#define M_LOCKI_DO(name, cont, lock_func, unlock_func)                \
  for(bool cont = true                                                \
        ; cont && (lock_func (name), true);                           \
      (unlock_func (name), cont = false))

#endif
