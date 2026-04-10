#include "m-array.h"
#include "m-deque.h"
#include "m-list.h"
#include "m-generic.h"

// M*LIB generates strongly-typed container APIs from macros.
// Here we request 3 different containers that store the same element type.
// Each macro creates both a type alias and a full set of functions.
LIST_DEF(list_uint, unsigned int)
ARRAY_DEF(array_uint, unsigned int)
DEQUE_DEF(deque_uint, unsigned int)

// An oplist describes how a type behaves (copy, move, print, iter, ...).
// Registering these oplists enables generic helpers like let/for each/print
// to call the right concrete function with compile-time type safety.
#define M_OPL_list_uint_t()  LIST_OPLIST(list_uint, M_BASIC_OPLIST)
#define M_OPL_array_uint_t() ARRAY_OPLIST(array_uint, M_BASIC_OPLIST)
#define M_OPL_deque_uint_t() DEQUE_OPLIST(deque_uint, M_BASIC_OPLIST)

// Generic registration is organized by namespaces:
// organization -> component -> oplists.
// The numbers (_2 here) are user-chosen unique indexes in this program.
// Parentheses are mandatory for macro values.
#define M_GENERIC_ORG_2() (USER)
#define M_GENERIC_ORG_USER_COMP_2() (SEQUENCE)

// Export the 3 container types to the generic framework so expressions like
// push(seq, x), print(seq), and for each(item, seq) are resolved automatically
// to list/array/deque implementations depending on the actual type of seq.
#define M_GENERIC_ORG_USER_COMP_SEQUENCE_OPLIST_1() M_OPL_list_uint_t()
#define M_GENERIC_ORG_USER_COMP_SEQUENCE_OPLIST_2() M_OPL_array_uint_t()
#define M_GENERIC_ORG_USER_COMP_SEQUENCE_OPLIST_3() M_OPL_deque_uint_t()

// One block of code, reused for several concrete container types.
// This is the key point: in plain C we write type-agnostic code while keeping
// generated, efficient, strongly-typed container operations.
#define RUN_DEMO(TYPE_NAME, LABEL) do {                                       \
    print(LABEL, " = ");                                                      \
    /* Construct a container initialized with 1, 2, 3. */                     \
    let ((seq, 1U, 2U, 3U), TYPE_NAME) {                                      \
        /* Same generic push for list/array/deque. */                         \
        push(seq, 5U);                                                        \
        push(seq, 8U);                                                        \
        unsigned int sum = 0U;                                                \
        unsigned int count = 0U;                                              \
        /* Same generic iteration API for every registered container. */      \
        for each (item, seq) {                                                \
            print(*item, " ");                                                \
            sum += *item;                                                     \
            count++;                                                          \
        }                                                                     \
        print("| count=", count, " sum=", sum, "\n");                         \
    }                                                                         \
} while (0)

int main(void)
{
    // The same high-level code runs on three different data structures.
    // This demonstrates how far modern macro metaprogramming can push C.
    RUN_DEMO(list_uint_t,  "LIST");
    RUN_DEMO(array_uint_t, "ARRAY");
    RUN_DEMO(deque_uint_t, "DEQUE");
    return 0;
}
