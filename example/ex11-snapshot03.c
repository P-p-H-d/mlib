#include <stdio.h>
#include <assert.h>

#include "m-snapshot.h"
#include "m-thread.h"
#include "m-array.h"

// Define a dynamic array type that will be exchanged through snapshots.
ARRAY_DEF(int_array, int)
#define M_OPL_int_array_t() ARRAY_OPLIST(int_array, M_BASIC_OPLIST)

// Build a Multi-Producer/Multiple-Consumer snapshot type named "snapshot".
// Snapshots are technically shared registers of any type between threads designed to exchange data quickly and atomically 
// Technically, MPMC snapshots are 1+N_WRITER+N_READER buffers.
// Multiple threads can write complete arrays, multiple threads can read the latest published one.
SNAPSHOT_MPMC_DEF(snapshot, int_array_t)

// Global shared register between the read and the write threads
snapshot_t g_snapshot;

static void thread_write(void *p)
{
    (void) p;
    // Producer thread: repeatedly prepare data in a private write buffer,
    // then publish it atomically for the reader.
    // Get a reference to the current write buffer in the snapshot
    int_array_t *a = snapshot_write_start(g_snapshot);
    for(int j = 0 ; j < 10; j++) {
        // and fill it with some data.
        int_array_reset(*a);
        for(int i = 0; i < 10; i++) {
            int_array_push_back(*a, j+i);
        }
        // Publish the buffer to the reader thread, and get a new one for the next iteration.
        snapshot_write_end(g_snapshot, a);
        a = snapshot_write_start(g_snapshot);
        printf("Publish array %d done\n", j);
        m_thread_sleep(1000);
    }
    // End-of-stream marker: publish an empty array so the reader can stop.
    int_array_reset(*a);
    snapshot_write_end(g_snapshot, a);

    return;
}

static void thread_read(void *p)
{
    (void) p;
    const int_array_t *a = NULL;

    a = snapshot_read_start(g_snapshot);
    // Consumer thread: wait for first non-empty publication.
    // Wait until something has been published (array is not empty)
    do {
       snapshot_read_end(g_snapshot, a);
       a = snapshot_read_start(g_snapshot);
    } while (int_array_size(*a) == 0);

    // Keep reading until the writer publishes the empty-array sentinel.
    while (int_array_size(*a) != 0) {
        printf("Array is :");
        int_array_out_str(stdout, *a);
        printf("\n");
        // Frequency between reader and write are not the same: not an issue!
        m_thread_sleep(500);
        // Read a new read buffer from the snapshot, potentially updating the one we are reading.
        snapshot_read_end(g_snapshot, a);
        a = snapshot_read_start(g_snapshot);
    }
    snapshot_read_end(g_snapshot, a);
    return;
}

int main(void)
{
    m_thread_t t1, t2, t3, t4, t5;
    // Initialize snapshot storage, then start two producers and three consumers.
    snapshot_init(g_snapshot, 2, 3);
    m_thread_create(t1, thread_write , NULL);
    m_thread_create(t2, thread_read , NULL);
    m_thread_create(t3, thread_read , NULL);
    m_thread_create(t4, thread_write , NULL);
    m_thread_create(t5, thread_read , NULL);
    // Wait for all threads to finish before cleaning shared resources.
    m_thread_join(t1);
    m_thread_join(t2);
    m_thread_join(t3);
    m_thread_join(t4);
    m_thread_join(t5);
    snapshot_clear(g_snapshot);
    return 0;
}
