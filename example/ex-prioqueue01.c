#include <stdio.h>

#include "m-prioqueue.h"

/* Define a priority queue of integers */
PRIOQUEUE_DEF(priority_queue, int)

int main(void)
{
    /* Declare & init the priority queue */
    priority_queue_t pq;
    priority_queue_init(pq);

    /* Push some elements into the priority queue. */
    priority_queue_push(pq, 5);
    priority_queue_push(pq, 2);
    priority_queue_push(pq, 8);
    priority_queue_push(pq, 1);

    /* Pop elements from the priority queue in sorted order. */
    int value;
    while (!priority_queue_empty_p(pq)) {
        value = *priority_queue_front(pq);
        printf("Top of the priority queue: %d\n", value);
        priority_queue_pop(&value, pq);
    }

    /* Clear internal resources. */
    priority_queue_clear(pq);
    return 0;
}