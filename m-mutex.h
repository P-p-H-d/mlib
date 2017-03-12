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
#include "m-core.h"

//FIXME: static inline versions could be cleaner?

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L    \
  && !defined(__STDC_NO_THREADS__)

/****************************** C11 version ********************************/

#include <threads.h>

#define M_MUTEX_T              mtx_t
#define M_MUTEX_INIT(_mutex)   do {             \
    int rc = mtx_init(&(_mutex), mtx_plain);    \
    M_ASSERT_INIT (rc == 0);                    \
  } while (0)
#define M_MUTEX_CLEAR(_mutex)  mtx_destroy(&(_mutex))
#define M_MUTEX_LOCK(_mutex)   mtx_lock(&(_mutex))
#define M_MUTEX_UNLOCK(_mutex) mtx_unlock(&(_mutex))

#define M_COND_T               cnd_t
#define M_COND_INIT(_cond)     do {             \
    int rc = cnd_init(&(_cond));                \
    M_ASSERT_INIT (rc == 0);                    \
  } while (0)
#define M_COND_CLEAR(_cond)    cnd_destroy(&(_cond))
#define M_COND_SIGNAL(_cond)   cnd_signal(&(_cond))
#define M_COND_WAIT(_cond, _mutex) cnd_wait(&(_cond),&(_mutex))

#define M_THREAD_T                    thrd_t
#define M_THREAD_CREATE(h, func, arg) do {                              \
    int rc = thrd_create(&(h), ((int)(*)(void*))func, arg);             \
    M_ASSERT_INIT (rc == thrd_success);                                 \
  } while (0)
#define M_THREAD_JOIN(h)              do {                             \
    int rc = thrd_join((h), NULL);                                     \
    assert (rc == thrd_success);                                       \
  } while (0)
#define M_THREAD_EXIT(arg)            thrd_exit(arg)

#define M_ONCEI_T                     once_flag
#define M_ONCEI_INIT_VALUE            ONCE_FLAG_INIT
#define M_ONCEI_CALL(once,func)       call_once(&(once),(func))

#elif defined(WIN32) || defined(_WIN32)

/****************************** WIN32 version ******************************/

#include <window.h>
  
#define M_MUTEX_T              HANDLE
#define M_MUTEX_INIT(_mutex)   do {                    \
    (_mutex) = CreateMutex(NULL, FALSE, NULL);         \
    M_ASSERT_INIT((_mutex) != NULL);                   \
  } while (0)
#define M_MUTEX_CLEAR(_mutex)  CloseHandle(_mutex)
#define M_MUTEX_LOCK(_mutex)   do  {                                    \
    DWORD dwWaitResult = WaitForSingleObject((_mutex), INFINITE);       \
    assert (dwWaitResult == WAIT_OBJECT_0);                             \
  } while(0)
#define M_MUTEX_TRYLOCK(m)     (WaitForSingleObject((m), 0) == WAIT_OBJECT_0)
#define M_MUTEX_UNLOCK(_mutex) ReleaseMutex(_mutex)
#define M_MUTEXI_INIT_VALUE      NULL
#define M_MUTEXI_LAZY_LOCK(_mutex)   do  {                              \
    volatile HANDLE *addr = &(_mutex);                                  \
    if (*addr == NULL) {                                                \
      /* Try to create one, affect it atomicaly and otherwise clears it */ \
      HANDLE h = CreateMutex(NULL, FALSE, NULL);                        \
      if (InterlockedCompareExchangePointer((PVOID*)addr, (PVOID)h, NULL) != NULL) \
        CloseHandle(h);                                                 \
      /* FIXME: there is no way to clean the mutex at program exit... */ \
    }                                                                   \
    DWORD dwWaitResult = WaitForSingleObject(*addr, INFINITE);          \
    assert (dwWaitResult == WAIT_OBJECT_0);                             \
  } while(0)

#define M_COND_T               HANDLE
#define M_COND_INIT(_cond)     do {                            \
    (_cond) = = CreateEvent (NULL,  FALSE, FALSE, NULL);       \
    M_ASSERT_INIT ((_cond) != NULL);                           \
  } while (0)
#define M_COND_CLEAR(_cond)    CloseHandle(_cond)
#define M_COND_SIGNAL(_cond)   PulseEvent(_cond)
#define M_COND_WAIT(_cond, _mutex) do {                                 \
    DWORD dwWaitResult = SignalObjectAndWait((_mutex),(_cond),INFINITE,FALSE); \
    assert (dwWaitResult == WAIT_OBJECT_0);                             \
    dwWaitResult = WaitForSingleObject((_mutex), INFINITE);             \
    assert (dwWaitResult == WAIT_OBJECT_0);                             \
  } while(0)

#define M_THREAD_T                    HANDLE
#define M_THREAD_CREATE(h, func, arg) do {                              \
    (h) = CreateThread(NULL, 0, (unsigned int (*)(void*)) func, arg,    \
                       0, NULL);                                        \
    M_ASSERT_INIT ((h) != NULL);                                        \
  } while (0)
#define M_THREAD_JOIN(h)              do {                      \
    DWORD dwWaitResult = WaitForSingleObject((h), INFINITE);    \
    assert (dwWaitResult == WAIT_OBJECT_0);                     \
  } while (0)
#define M_THREAD_EXIT(arg)            ExitThread(arg)

#else

/**************************** PTHREAD version ******************************/

#include <pthread.h>

#define M_MUTEX_T              pthread_mutex_t
#define M_MUTEX_INIT(_mutex)   do {                     \
    int _rc = pthread_mutex_init(&(_mutex), NULL);      \
    M_ASSERT_INIT (_rc == 0);                                  \
  } while (0)
#define M_MUTEX_CLEAR(_mutex)  pthread_mutex_destroy(&(_mutex))
#define M_MUTEX_LOCK(_mutex)   pthread_mutex_lock(&(_mutex))
#define M_MUTEX_UNLOCK(_mutex) pthread_mutex_unlock(&(_mutex))
#define M_MUTEXI_INIT_VALUE    PTHREAD_MUTEX_INITIALIZER
#define M_MUTEXI_LAZY_LOCK(_mutex) M_MUTEX_LOCK(_mutex)

#define M_COND_T               pthread_cond_t
#define M_COND_INIT(_cond)     do {                     \
    int _rc = pthread_cond_init(&(_cond), NULL);        \
    M_ASSERT_INIT (_rc == 0);                           \
  } while (0)
#define M_COND_CLEAR(_cond)    pthread_cond_destroy(&(_cond))
#define M_COND_SIGNAL(_cond)   pthread_cond_signal(&(_cond))
#define M_COND_WAIT(_cond, _mutex) pthread_cond_wait(&(_cond),&(_mutex))
  
#define M_THREAD_T                    pthread_t
#define M_THREAD_CREATE(h, func, arg) do {                              \
    int _rc = pthread_create(&(h), NULL, (void*(*)(void*))func, arg);   \
    M_ASSERT_INIT (_rc == 0);                                           \
  } while (0)
#define M_THREAD_JOIN(h)              do {                      \
    int _rc = pthread_join((h), NULL);                          \
    assert (_rc == 0);                                          \
  } while (0)
#define M_THREAD_EXIT(arg)            pthread_exit((void*)arg)

#define M_ONCEI_T                     pthread_once_t
#define M_ONCEI_INIT_VALUE            PTHREAD_ONCE_INIT
#define M_ONCEI_CALL(once,func)       pthread_once(&(once),(func))

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
#ifdef M_MUTEXI_LAZY_LOCK
# define M_LOCK_DECL(name) M_MUTEX_T name = M_MUTEXI_INIT_VALUE
# define M_LOCK(name)                                                   \
  M_LOCKI_DO(name, M_C(local_cont_, __LINE__), M_MUTEXI_LAZY_LOCK, M_MUTEX_UNLOCK)
#else
# define M_LOCK_DECL(name)                                      \
  M_MUTEX_T name;                                               \
  static void M_C(m_mutex_init_, name)(void) {                  \
    M_MUTEX_INIT(name);                                         \
  }                                                             \
  M_ONCEI_T M_C(m_once_, name) = M_ONCEI_INIT_VALUE
# define M_LOCKI_BY_ONCE(name)                                          \
  (M_ONCEI_CALL(M_C(m_once_, name), M_C(m_mutex_init_, name)),          \
   M_MUTEX_LOCK(name), (void) 0 )
# define M_LOCK(name)                                                   \
  M_LOCKI_DO(name, M_C(local_cont_, __LINE__), M_LOCKI_BY_ONCE, M_MUTEX_UNLOCK)
#endif

#define M_LOCKI_DO(name, cont, lock, unlock)                          \
  for(bool cont = true                                                \
        ; cont && (lock (name), true);                                \
      (unlock (name), cont = false))

#endif
