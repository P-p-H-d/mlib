#if HAVE_GMP

#include <stdio.h>
#include <gmp.h>

#include "m-array.h"
#include "m-list.h"
#include "m-generic.h"

// Register an organization for generic: USER
// Each index (here '1') shall be unique for the whole program
#define M_GENERIC_ORG_1() (USER)
// Register a component for the organization USER: CORE
// Each index (here '1') shall be unique for the whole organization
#define M_GENERIC_ORG_USER_COMP_1() (CORE)

// Register the oplist of a mpz_t. It is a classic oplist.
// Adding support for emplace and initialization
// Adding the ability to output its value for serialization/printing
// Adding type information for handling correctly the generic association
#define M_OPL_mpz_t() M_OPEXTEND(M_CLASSIC_OPLIST(mpz),         \
        INIT_WITH(mpz_init_set_ui), EMPLACE_TYPE(unsigned int), \
        OUT_STR( API( mpz_out_str, VOID, ARG1, 10, ARG2)),      \
        GENTYPE(__mpz_struct *), TYPE(mpz_t) )
// Register the oplist as a generic type of the component CORE of the organization USER
// Each index (here '1') shall be unique for the whole component
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_1() M_OPL_mpz_t()

// Define an instance of an array of mpz_t (both type and function)
ARRAY_DEF(array_mpz, mpz_t)
// Register the oplist of the created instance of array of mpz_t
#define M_OPL_array_mpz_t() ARRAY_OPLIST(array_mpz, M_OPL_mpz_t())
// Register the oplist as a generic type of the component CORE of the organization USER
// Each index (here '10') shall be unique for the whole component
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_10() M_OPL_array_mpz_t()

// Define an instance of a list of mpz_t (both type and function)
LIST_DEF(list_mpz, mpz_t)
// Register the oplist of the created instance of array of mpz_t
#define M_OPL_list_mpz_t() LIST_OPLIST(list_mpz, M_OPL_mpz_t())
// Register the oplist as a generic type of the component CORE of the organization USER
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_11() M_OPL_list_mpz_t()

// Define an instance of a list of unsigned int (both type and function)
LIST_DEF(list_uint, unsigned)
// Register the oplist of the created instance of list of unsigned
#define M_OPL_list_uint_t() LIST_OPLIST(list_uint, M_BASIC_OPLIST)
// Register the oplist as a generic type of the component CORE of the organization USER
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_12() M_OPL_list_uint_t()

// Define an instance of a array of unsigned int (both type and function)
ARRAY_DEF(array_uint, unsigned)
// Register the oplist of the created instance of list of unsigned
#define M_OPL_array_uint_t() ARRAY_OPLIST(array_uint, M_BASIC_OPLIST)
// Register the oplist as a generic type of the component CORE of the organization USER
#define M_GENERIC_ORG_USER_COMP_CORE_OPLIST_13() M_OPL_array_uint_t()

int main(void) {

    print("LIST UINT = ");
    // Initialize list as list_uint_t and push 17 & 42 in it.
    let( (list, 17, 42), list_uint_t) {
        push(list, 1742);
        // Iterate over all elements of the list of unsigned int
        for each (item, list) {
            // Print the element
            print(*item, " ");
        }
        // Print the full list too
        print("\n","List=", list, "\n");
    }
    // Now list has been cleared.

    print("Array UINT = ");
    // Initialize list as array_uint_t and push 17 & 42 in it.
    let( (list, 17, 42), array_uint_t) {
        push(list, 1742);
        // Iterate over all elements of the array of unsigned int
        for each (item, list) {
            // Print the element
            print(*item, " ");
        }
        // Print the full array too
        print("\n","List=", list, "\n");
    }
    // Now array has been cleared.

    print("LIST MPZ = ");
    // Initialize list as array_mpz_t and push_emplace mpz_t from 17 & 42 in it.
    // The parenthesis around 17 in (17) means to emplace 
    let( (list, (17), (42)), list_mpz_t) {
        list_mpz_emplace_back(list, 1742);        // Generic emplace is not supported yet
        // Iterate over all elements of the array of mpz_t
        for each (item, list) {
            // Print the element
            // Since we registered the out_str of a mpz_t, we can display it directly
            print(*item, " ");
        }
        // Print the full list too
        print("\n","List=", list, "\n");
    }

    print("ARRAY MPZ = ");
    // Same as before but for array
    let( (list, (17), (42)), array_mpz_t) {
        array_mpz_emplace_back(list, 1742);
        for each (item, list) {
            print(*item, " ");
        }
        print("\n","List=", list, "\n");
    }
    exit(0);
}

#else
int main(void) {}
#endif
