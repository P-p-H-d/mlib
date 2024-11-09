#include "m-shared-ptr.h"

#include "m-string.h"
#include "m-array.h"
#include "m-bptree.h"
#include "m-deque.h"
#include "m-dict.h"
#include "m-list.h"
#include "m-prioqueue.h"
#include "m-rbtree.h"
#include "m-tuple.h"
#include "m-variant.h"

// TEST WITH STRING

SHARED_PTR_DECL(shared_string, STRING_OPLIST)

static void test_string(void)
{
    shared_string_t *p = shared_string_new();
    shared_string_t *q = shared_string_make("Hello world");
    shared_string_t *r = shared_string_acquire(p);
    assert(p == r);
    assert(shared_string_equal_p(r, p));
    assert(!shared_string_equal_p(r, q));
    shared_string_t *p2 = shared_string_new_copy(p);
    assert(p2 != p);
    assert(shared_string_equal_p(p2, p));
    bool b = shared_string_empty_p(p);
    assert(b);
    shared_string_clear(p);
    b = shared_string_empty_p(r);
    assert(b);
    shared_string_copy(q, r);
    assert(shared_string_equal_p(r, q));
    shared_string_set(&q, r);
    assert(shared_string_equal_p(r, q));
    shared_string_clear(q);
    shared_string_release(r);
    shared_string_clear(p2);
}

SHARED_PTR_DEF_EXTERN(shared_string, string_t, STRING_OPLIST)


// TEST WITH ARRAY
ARRAY_DEF(array, int)
// Provide a FULL_P operator
static inline bool array_full_p(const array_t a) { return array_size(a) >= 10; }
#define M_OPL_array_t() M_OPEXTEND( ARRAY_OPLIST(array, M_BASIC_OPLIST), SUBTYPE(int), FULL_P(array_full_p) )
SHARED_PTR_DECL(shared_array, M_OPL_array_t() )

static int callback1(void *data, const int *el)
{
    int *s_ptr = (int *) data;
    *s_ptr += *el;
    return 0;
}

static int callback2(void *data, const int *el)
{
    int *s_ptr = (int *) data;
    return *s_ptr == *el;
}

static int callback3(void *data, int *el)
{
    int *s_ptr = (int *) data;
    *el *= *s_ptr;
    return 0;
}

static void test_array(void)
{
    shared_array_t *p = shared_array_new();
    bool b;
    int j;

    for(int i = 0 ; i < 10; i++) {
        b = shared_array_try_push(p, i);
        assert(b);
    }
    // We can only push 10 elements before being full.
    b = shared_array_try_push(p, 11);
    assert(!b);

    shared_array_get(&j, p, 4);
    assert( j == 4);

    shared_array_set_at(p, 4, 5);
    shared_array_get(&j, p, 4);
    assert( j == 5);

    shared_array_set_at(p, 4, 4);
    shared_array_get(&j, p, 4);
    assert( j == 4);

    int s = 0;
    int r = shared_array_for_each(p, callback1, &s);
    assert(r == 0);
    assert(s == 9*10/2);

    s = 4;
    r = shared_array_for_each(p, callback2, &s);
    assert(r == 1);

    s = 2;
    r = shared_array_apply(p, callback3, &s);
    assert(r == 0);

    s = 0;
    r = shared_array_for_each(p, callback1, &s);
    assert(r == 0);
    assert(s == 9*10);

    for(int i = 9 ; i >= 0; i--) {
        b = shared_array_try_pop(&j, p);
        assert(b);
        assert(i * 2== j);
    }
    b = shared_array_try_pop(&j, p);
    assert(!b);

    shared_array_release(p);
}

SHARED_PTR_DEF_EXTERN(shared_array, array_t)

ARRAY_DEF(array_str, string_t)
SHARED_PTR_DEF(shared_array_str, array_str_t, ARRAY_OPLIST(array_str, M_STRING_OPLIST))

static void test_array_string(void)
{
    shared_array_str_t *p = shared_array_str_new();

    M_LET( (x, ("Hello")), string_t)
        shared_array_str_push(p, x);

    string_t tmp;
    string_init_set_str(tmp, "World");
    shared_array_str_push_move(p, &tmp); // No need to clear tmp

    shared_array_str_emplace(p, "!");

    bool b = shared_array_str_try_emplace(p, " ");
    assert(b);

    string_init_set_str(tmp, "!");
    b = shared_array_str_try_push_move(p, &tmp); // No need to clear tmp
    assert(b);

    shared_array_str_release(p);
}

// TEST WITH LIST
LIST_DEF(list, float)
#define M_OPL_list_t() LIST_OPLIST(list, M_BASIC_OPLIST)
SHARED_WEAK_PTR_DECL(shared_list, M_OPL_list_t())
SHARED_WEAK_PTR_DEF_EXTERN(shared_list, list_t)

LIST_DUAL_PUSH_DEF(list2, int)
SHARED_WEAK_PTR_DEF(shared_list2, list2_t, LIST_OPLIST(list2))


// TEST WITH DICT
DICT_DEF2(dict, float, float)
#define M_OPL_dict_t() DICT_OPLIST(dict, M_BASIC_OPLIST, M_BASIC_OPLIST)
SHARED_WEAK_PTR_DECL_AS(shared_dict, shared_dict_t, M_OPL_dict_t())
SHARED_WEAK_PTR_DEF_EXTERN_AS(shared_dict, shared_dict_t, dict_t)

DICT_DEF2(string_pool, string_t, string_t)
#define M_OPL_string_pool_t() DICT_OPLIST(string_pool, STRING_OPLIST, STRING_OPLIST)
SHARED_PTR_DEF(string_pool_ts, string_pool_t, M_OPL_string_pool_t() )

DICT_SET_DEF(dict2, int)
SHARED_PTR_DEF(shared_dict2, dict2_t, DICT_SET_OPLIST(dict2))


// TEST WITH TUPLE
TUPLE_DEF2(point, (x, int), (y, int))
#define M_OPL_point_t() TUPLE_OPLIST(point, M_BASIC_OPLIST, M_BASIC_OPLIST)
SHARED_PTR_DEF(shared_point, point_t)

//SHARED_PTR_DECL(shared_point, ARRAY_OPLIST(array_str, M_STRING_OPLIST))
//M_OPL_point_t())

// TEST WITH VARIANT
VARIANT_DEF2(dimension, (x, int), (y, float))
#define M_OPL_dimension_t() VARIANT_OPLIST(dimension, M_BASIC_OPLIST, M_BASIC_OPLIST)
SHARED_PTR_DEF(shared_dimension, dimension_t)


// TEST WITH DEQUE
DEQUE_DEF(deque1, int)
SHARED_PTR_DEF(shared_deque1, deque1_t, DEQUE_OPLIST(deque1))

PRIOQUEUE_DEF(prio1, int)
SHARED_PTR_DEF(shared_prio1, prio1_t, PRIOQUEUE_OPLIST(prio1))

// TEST WITH B+TREE
BPTREE_DEF2(bptree1, 10, int, int)
SHARED_PTR_DEF(shared_bptree1, bptree1_t, BPTREE_OPLIST2(bptree1, M_BASIC_OPLIST, M_BASIC_OPLIST))

BPTREE_DEF(bptree2, 10, int)
SHARED_PTR_DEF(shared_bptree2, bptree2_t, BPTREE_OPLIST(bptree2))

// TEST WITH R/B TREE
RBTREE_DEF(rbtree1, int)
SHARED_PTR_DEF(shared_rbtree1, rbtree1_t, RBTREE_OPLIST(rbtree1))


int main(void)
{
    test_string();
    test_array();
    test_array_string();
    exit(0);
}