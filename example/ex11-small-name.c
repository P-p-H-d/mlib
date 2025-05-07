// Disable small name usage
#define M_USE_SMALL_NAME 0

#include "m-algo.h"
#include "m-array.h"
#include "m-atomic.h"
#include "m-bitset.h"
#include "m-bptree.h"
#include "m-bstring.h"
#include "m-buffer.h"
#include "m-concurrent.h"
#include "m-deque.h"
#include "m-dict.h"
#include "m-funcobj.h"
#include "m-generic.h"
#include "m-genint.h"
#include "m-i-list.h"
#include "m-i-shared.h"
#include "m-list.h"
#include "m-mempool.h"
#include "m-prioqueue.h"
#include "m-rbtree.h"
#include "m-serial-bin.h"
#include "m-serial-json.h"
#include "m-shared-ptr.h"
#include "m-snapshot.h"
#include "m-string.h"
#include "m-thread.h"
#include "m-tree.h"
#include "m-try.h"
#include "m-tuple.h"
#include "m-variant.h"
#include "m-worker.h"

M_ARRAY_DEF(array, m_string_t)
M_ALGO_DEF(array, M_ARRAY_OPLIST(array, M_STRING_OPLIST))
M_BPTREE_DEF2(bptree, 11, m_string_t, m_string_t)
M_BUFFER_DEF(queue, m_string_t, 10, M_BUFFER_QUEUE)
M_LIST_DEF(list, m_string_t)
M_LIST_DUAL_PUSH_DEF(list2, m_string_t)
M_DEQUE_DEF(deque, m_string_t)
M_DICT_DEF2(dict, m_string_t, m_string_t)
M_DICT_OA_DEF2(dict_oa, m_string_t, m_string_t)
M_DICT_SET_DEF(dict_set, m_string_t)
M_DICT_OASET_DEF(dict_oa_set, m_string_t)
M_RBTREE_DEF(rbtree, m_string_t)
M_FUNC_OBJ_INS_DEF(compare_by /* name of the instance */,
    array_cmp_obj /* name of the interface */,
    (a, b) /* name of the input parameters of the function like object. The type are inherited from the interface. */, {
      /* code of the function object */
      if (m_string_equal_cstr_p(self->sort_field, "normal"))
        return m_string_cmp(a, b);
      else
        return -m_string_cmp(a, b);
    },
    /* Additional fields stored in the function object */
    (sort_field, m_string_t) )
M_PRIOQUEUE_DEF(prio_queue, m_string_t)
M_SHARED_PTR_DEF(shared_string, m_string_t)
M_SNAPSHOT_SPSC_DEF(snap1, m_string_t)
M_SNAPSHOT_SPMC_DEF(snap2, m_string_t)
M_SNAPSHOT_MPMC_DEF(snap3, m_string_t)
M_TREE_DEF(tree, m_string_t)
M_TUPLE_DEF2(tuple, (a, m_string_t))
M_VARIANT_DEF2(variant, (a, m_string_t))
M_WORKER_SPAWN_DEF2(worker, (filename, m_string_t) )

int main(void)
{
    exit(0);
}
