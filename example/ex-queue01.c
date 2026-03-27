#include <stdio.h>
#include "m-queue.h"

/* QUEUE_DEF(name, type, capacity) generates a FIFO queue type named
 * 'float_queue_t'.  The third argument (20) is the compile-time fixed
 * capacity: the ring buffer holds at most 20 elements.  Use 0 instead
 * to have the capacity supplied at init() time. */
QUEUE_DEF(float_queue, float, 20)
/* Register the oplist so that generic macros (M_LET, M_EACH, ...)
 * can infer the oplist from the type name alone. */
#define M_OPL_float_queue_t() QUEUE_OPLIST(float_queue, M_BASIC_OPLIST)

/* STACK_DEF generates a LIFO stack type named 'stack_int_t'.
 * Passing 0 as the capacity means it is NOT fixed at compile time —
 * the actual capacity is supplied as a parameter at init() time. */
STACK_DEF(stack_int, int, 0)
#define M_OPL_stack_int_t() QUEUE_OPLIST(stack_int, M_BASIC_OPLIST)

int main(void)
{
    /* --- FIFO Queue (float, fixed capacity 20) --- */

    /* Declare and initialize the queue.  Because the capacity was fixed
     * at 20 in QUEUE_DEF, the argument here must be exactly 20. */
    float_queue_t queue;
    float_queue_init(queue, 20);

    /* push() is a non-blocking, best-effort operation: it returns true
     * on success and false when the queue is full. */
    for (int i = 0; i < 20; i++) {
        if (!float_queue_push(queue, (float)i)) {
            printf("Failed to push %d into queue\n", i);
        }
    }
    printf("Queue size after pushing 20 elements: %zu\n", float_queue_size(queue));

    /* pop() removes and returns the oldest element (FIFO order).
     * It writes the value through the pointer and returns false when
     * the queue is empty, so a while loop drains it completely. */
    float value;
    while (float_queue_pop(&value, queue)) {
        printf("Popped from queue: %f\n", value);
    }

    /* Release internal resources.  Always call clear() before the
     * variable goes out of scope. */
    float_queue_clear(queue);

    /* --- LIFO Stack (int, runtime-defined capacity) ---
     * Unlike the queue above, the capacity here is given at init()
     * time (10).  It is still a hard limit — the stack will NOT
     * grow dynamically if it becomes full. */
    stack_int_t stack;
    stack_int_init(stack, 10); /* initial capacity of 10, and it won't grow as needed! */

    /* Attempting to push 11 elements into a capacity-10 stack: the
     * 11th push will fail and print the error message. */
    for(int i = 0 ; i < 11; i++) {
        if (!stack_int_push(stack, i)) {
            printf("Failed to push %d into stack\n", i);
        }
    }
    printf("Stack size after pushing 11 elements: %zu\n", stack_int_size(stack));

    /* pop() removes and returns the most recently pushed element
     * (LIFO order), which is the reverse of the queue above. */
    int ivalue;
    while (stack_int_pop(&ivalue, stack)) {
        printf("Popped from stack: %d\n", ivalue);
    }

    stack_int_clear(stack);

    return 0;
}
