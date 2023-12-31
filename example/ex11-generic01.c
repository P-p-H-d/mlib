#include "m-list.h"
#include "m-generic.h"

LIST_DEF(list_uint, unsigned int)
// Register for M_LET & M_EACH 
#define M_OPL_list_uint_t() LIST_OPLIST(list_uint, M_BASIC_OPLIST)
// Register for Generic:
#define M_GENERIC_ORG_1() (USER)
#define M_GENERIC_ORG_USER_COMP_1() (CORE)
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_1() M_OPL_list_uint_t()

int main(void)
{
    M_LET(list, list_uint_t) {
        // We can push in the list. It will call the registered method.
        push(list, 42);
        push(list, 17);
        // We can iterate over the list
        for each(item, list) {
            M_PRINT("ITEM=", *item, "\n");
        }
        // We can print directly the list. It will use the registered method
        M_PRINT("List = ", list, "\n");
    }
}
