#include <stdio.h>
#include "m-deque.h"

/* Create a new deque of int */
DEQUE_DEF(int_deque, int)
/* Register the oplist for the deque of int by associating its type to its oplist */
#define M_OPL_int_deque_t() DEQUE_OPLIST(int_deque, M_BASIC_OPLIST)

/* Helper to display the deque from front to back using an iterator. */
static void print_deque(const char *label, int_deque_t deque)
{
    printf("%s", label);
    int_deque_it_t it;
    /* Start at the first element and walk until the end sentinel. */
    for (int_deque_it(it, deque); !int_deque_end_p(it); int_deque_next(it)) {
        printf("%d ", *int_deque_cref(it));
    }
    printf("\n");
}

int main(void)
{
    /* M_LET initializes and automatically clears the deque at scope exit. */
    M_LET(deque, int_deque_t) {
        /* Add elements at both ends. */
        int_deque_push_back(deque, 20);
        int_deque_push_back(deque, 30);
        int_deque_push_front(deque, 10);
        int_deque_push_front(deque, 5);
        print_deque("After push_front/push_back: ", deque);

        /* front/back are O(1) access to both extremities of the deque. */
        printf("front=%d back=%d size=%zu\n",
               *int_deque_front(deque),
               *int_deque_back(deque),
               int_deque_size(deque));

        /* Random access is available with get/set_at style APIs. */
        int_deque_set_at(deque, 1, 15);
        printf("Element at index 2: %d\n", *int_deque_get(deque, 2));
        print_deque("After set_at(index=1, value=15): ", deque);

        /* Remove elements from both ends. */
        int first = 0;
        int last = 0;
        int_deque_pop_front(&first, deque);
        int_deque_pop_back(&last, deque);
        printf("pop_front=%d pop_back=%d\n", first, last);
        print_deque("After pop_front/pop_back: ", deque);

        /* Iterate from the back toward the front. */
        printf("Backward iteration: ");
        int_deque_it_t it;
        for(int_deque_it_last(it, deque); !int_deque_last_p(it); int_deque_previous(it)) {
            printf("%d ", *int_deque_cref(it));
        }
        printf("\n");

        /* The container still owns its elements until leaving M_LET scope. */
        printf("Queue is empty: %s\n", int_deque_empty_p(deque) ? "true" : "false");
    }

    return 0;
}