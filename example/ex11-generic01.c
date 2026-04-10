#include "m-list.h"
#include "m-generic.h"

// Classic example of generic programming. We will create a list of unsigned int and a list of mpz_t and use them in the same way without caring about the type of element they contain. The generic framework will call the correct function for each type.
LIST_DEF(list_uint, unsigned int)

// Register the created list for M_LET & M_EACH (or let and for each) generic macros
#define M_OPL_list_uint_t() LIST_OPLIST(list_uint, M_BASIC_OPLIST)

// Register the created list for the Generic framework:
// Each final index (here '1') shall be unique for the whole program
// Register our organization. Let's got with a simple one: USER 
// Parenthesis are mandatory for the value of the organization
// The name to use is M_GENERIC_ORG_<INDEX> with <INDEX> a unique index for the organization in the program.
// Up to 50 index are available for the whole program.
#define M_GENERIC_ORG_1() (USER)
// Register the component of our organization. Let's got with a simple one: CORE
// Parenthesis are mandatory for the value of the component
// Note that the component is registered in the organization, not the type. So we can register many types in the same component.
// The name to use is M_GENERIC_ORG_<ORG>_COMP_<INDEX> with <ORG> the name of the organization and <INDEX> a unique index for the component in the organization.
// Up to 50 index are available for the whole organization.
#define M_GENERIC_ORG_USER_COMP_1() (CORE)
// Register one of the oplist of our component. Let's use the one we have for list_uint_t
// Up to 50 index are available for the whole component.
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_1() M_OPL_list_uint_t()

int main(void)
{
    let(list, list_uint_t) {
        // We can push in the list. It will call the registered method.
        push(list, 42);
        push(list, 17);
        // We can iterate over the list
        for each(item, list) {
            print("ITEM=", *item, "\n");
        }
        // We can print directly the list. It will use the registered method
        print("List = ", list, "\n");
    }
    return 0;
}
