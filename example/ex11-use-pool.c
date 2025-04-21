#include <stdlib.h>

// Define the memory context. 
typedef int *context_t;

// Define the realloc / free that accepts the memory context.
static void *my_realloc(void *ptr, size_t old, size_t new, size_t base, context_t context)
{
    (void) context;
    (void) old;
    return realloc(ptr, new*base);
}

static void my_free(void *ptr, size_t old, size_t base, context_t context)
{
    (void) context;
    (void) old;
    (void) base;
    free(ptr);
}

// Request M*LIB to generate functions accepting a memory context parameter that will be passed to the memory functions
# define M_USE_CONTEXT context_t

// Overloaded memory functions to use the custom ones.
// m_context is the name of the memory context parameter
# define M_MEMORY_REALLOC(type, ptr, o, n) my_realloc(ptr, o, n, sizeof (type), m_context)
# define M_MEMORY_FREE(type, ptr, o) my_free(ptr, o, sizeof (*ptr), m_context)

# define M_MEMORY_ALLOC(type) my_realloc(NULL, 0, 1, sizeof (type), m_context)
# define M_MEMORY_DEL(ptr) my_free(ptr, 1, sizeof (*ptr), m_context)

// Now we can include M*LIB functions.
#include "m-string.h"
#include "m-bitset.h"
#include "m-bstring.h"
#include "m-genint.h"
#include "m-algo.h"
context_t my_pool = NULL;
extern int f(string_t,  bitset_t b, bstring_t y);

int f(string_t x, bitset_t b, bstring_t y)
{
    string_init(x);
    string_set_str(my_pool, x, "Hello world");
    string_clear(my_pool, x);

    bitset_init(b);
    bitset_push_back(my_pool, b, true);
    bitset_clear(my_pool, b);

    bstring_init(y);
    bstring_push_back(my_pool, y, 0);
    bstring_clear(my_pool, y);

    return 0;
}

#include "m-array.h"

ARRAY_DEF(array, int)
ALGO_DEF(array, ARRAY_OPLIST(array, M_BASIC_OPLIST))
ARRAY_DEF(array_str, string_t)
ALGO_DEF(array_str, ARRAY_OPLIST(array_str, STRING_OPLIST))

#include "m-prioqueue.h"

PRIOQUEUE_DEF(prio, int)
PRIOQUEUE_DEF(prio_str, string_t)

#include "m-rbtree.h"

RBTREE_DEF(rbtree, int)
RBTREE_DEF(rbtree_str, string_t)

#include "m-list.h"

LIST_DEF(list, int)
ALGO_DEF(list, LIST_OPLIST(list, M_BASIC_OPLIST))
LIST_DEF(list_str, string_t)
ALGO_DEF(list_str, LIST_OPLIST(list_str, M_STRING_OPLIST))

LIST_DUAL_PUSH_DEF(list2, int)
LIST_DUAL_PUSH_DEF(list2_str, string_t)

#include "m-bptree.h"

BPTREE_DEF2(bptree, 5, int, int)

#include "m-dict.h"

DICT_DEF2(dict, int, int)
DICT_DEF2(dict_str, string_t, string_t)

DICT_SET_DEF(set, int)
DICT_SET_DEF(set_str, string_t)

static inline bool oor_equal_p(int k, char n)
{
  return k == INT_MIN + n;
}

static inline int  oor_set(char n)
{
  return INT_MIN + n;
}

DICT_OA_DEF2(dict_oa,
    int, M_OPEXTEND(M_BASIC_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(API_4(oor_set))),
    int, M_BASIC_OPLIST)
DICT_OA_DEF2(dict2_str, string_t, string_t)

DICT_OASET_DEF(set_oa,
    int, M_OPEXTEND(M_BASIC_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(API_4(oor_set))) )
DICT_OASET_DEF(set2_str, string_t)

#include "m-buffer.h"

BUFFER_DEF(buffer1, int, 10, M_BUFFER_QUEUE)
BUFFER_DEF(buffer2, int, 0, M_BUFFER_STACK)

M_QUEUE_MPMC_DEF(queue1, int, M_BUFFER_STACK)
M_QUEUE_SPSC_DEF(queue2, int, M_BUFFER_STACK)

BUFFER_DEF(buffer11, string_t, 10, M_BUFFER_QUEUE)
BUFFER_DEF(buffer22, string_t, 0, M_BUFFER_STACK)

M_QUEUE_MPMC_DEF(queue11, string_t, M_BUFFER_STACK)
M_QUEUE_SPSC_DEF(queue22, string_t, M_BUFFER_STACK)

#include "m-deque.h"

M_DEQUE_DEF(deque, int)
M_DEQUE_DEF(deque_str, string_t)

#include "m-snapshot.h"

M_SNAPSHOT_SPSC_DEF(snap1, int)
M_SNAPSHOT_SPMC_DEF(snap2, int)
M_SNAPSHOT_MPMC_DEF(snap3, int)

#include "m-tree.h"

M_TREE_DEF(tree, int)
M_TREE_DEF(tree_str, string_t)

#include "m-tuple.h"

M_TUPLE_DEF2(tuple1, (num, int) )
M_TUPLE_DEF2(tuple2, (num, int), (name, string_t) )

#include "m-variant.h"

M_VARIANT_DEF2(variant1, (num, int) )
M_VARIANT_DEF2(variant2, (num, int), (name, string_t) )

#include "m-worker.h"

M_WORKER_SPAWN_DEF2(worker_int, (in, int) )
M_WORKER_SPAWN_DEF2(worker_str, (str, string_t) )

#include "m-shared-ptr.h"

M_SHARED_PTR_DEF(shared_int, int)
M_SHARED_PTR_DEF(shared_str, string_t)

#include "m-queue.h"

M_QUEUE_DEF(queue_10, int, 10 )
M_STACK_DEF(stack_20, string_t, 20)

#include "m-serial-json.h"

#include "m-serial-bin.h"

int main(void)
{
    return 0;
}
