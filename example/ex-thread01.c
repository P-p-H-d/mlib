#include <stdio.h>

#include "m-thread.h"

/* m_mutex_t, m_cond_t and m_thread_t are alias for the real system types, 
   so you can encapsulate them and maximize portability. 
   They don't provide any performance it compare to the real system types.
   As a consequence, their semantics is a litle bit more strict than the real ones,
   so that semantic can apply to all the operating systems.
*/

/* Shared synchronization primitives used by all threads in this example. */
m_mutex_t mutex; /* mutex, who didn't realize it?*/
m_cond_t cond; /* conditional variable */

static void thread_func(void *arg)
{
    /* Lock before printing and waiting so output order stays predictable. */
    m_mutex_lock(mutex);
    printf("++ %s", (char *)arg);

    /*
     * Wait atomically releases the mutex, sleeps, then re-acquires the mutex
     * before returning after broadcast.
     */
    m_cond_wait(cond, mutex);

    /* We are awake and still holding the mutex at this point. */
    printf("-- %s", (char *)arg);
    m_mutex_unlock(mutex);
    return;
}

int main(void)
{
    m_thread_t thread1, thread2;

    /* Initialize synchronization objects before starting worker threads. */
    m_mutex_init(mutex);
    m_cond_init(cond);

    /* Start two threads that run the same function with different messages. */
    m_thread_create(thread1, thread_func, "Hello from thread 1!\n");
    m_thread_create(thread2, thread_func, "Hello from thread 2!\n");

    /* Give both threads time to reach the wait point for this demo. */
    sleep(1);

    /* Wake all waiting threads while holding the same mutex associated to cond. */
    m_mutex_lock(mutex);
    m_cond_broadcast(cond);
    m_mutex_unlock(mutex);

    /* Join ensures both threads are finished before cleaning resources. */
    m_thread_join(thread1);
    m_thread_join(thread2);

    /* Destroy synchronization objects after all users are done. */
    m_mutex_clear(mutex);
    m_cond_clear(cond);
    return 0;
}
