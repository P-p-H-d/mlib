#include "m-try.h"
#include "m-array.h"

// Define & register an array of int
ARRAY_DEF(array, int)
#define M_OPL_array_t() ARRAY_OPLIST(array, M_BASIC_OPLIST)

// This shall be defined once per program to define the global variables needed
// to handle the exceptions.
M_TRY_DEF_ONCE()

static void f(void)
{
    M_TRY(f_exception) {
        // Let's define a variable and do stuff:
        // Using M_LET allows the register of destructors for the exception handler.
        M_LET(a, array_t) {
            array_push_back(a, 5);
            array_push_back(a, 4);
            array_push_back(a, 3);
            array_push_back(a, 2);
            array_push_back(a, 1);
            // Throw a M_ERROR_MEMORY exception
            M_THROW(M_ERROR_MEMORY);
            printf("This should never be printed!\n");
        }
    } M_CATCH(f_exception, M_ERROR_MEMORY) {
        // Exception handler than catches the M_ERROR_MEMORY errors.
        // Here the destructor of variable a has been called to free memory
        printf("Memory exception %d received in f function, thrown by %s:%d\n",
                f_exception->error_code, f_exception->filename, f_exception->line);
    }
}

static void g(void)
{
    M_TRY(f_exception) {
        // Let's define a variable and do stuff:
        // Using M_LET allows the register of destructors for the exception handler.
        M_LET(a, array_t) {
            array_push_back(a, 5);
            array_push_back(a, 4);
            array_push_back(a, 3);
            array_push_back(a, 2);
            array_push_back(a, 1);
            // Throw an unkown exception
            M_THROW(42);
            printf("This should never be printed!\n");
        }
    } M_CATCH(f_exception, M_ERROR_MEMORY) {
        printf("Memory exception %d received in f function, thrown by %s:%d\n",
                f_exception->error_code, f_exception->filename, f_exception->line);
        printf("This should never be printed!\n");
    }
}

int main(void)
{
    // Let's do a global exception handler
    M_TRY(main_exception) {
        f();
        g();
    } M_CATCH(main_exception, 0) {
        // Exception handler than catches all errors.
        // Here the destructor of variable a has been called to free memory
        printf("Exception %d received in main function, thrown by %s:%d\n",
                main_exception->error_code, main_exception->filename, main_exception->line);
    }
    printf("You should run it under valgrind to check that all memory allocations are freed.\n");
    exit(0);
}
