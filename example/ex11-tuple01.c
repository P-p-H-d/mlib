#include <stdio.h>

#include "m-tuple.h"
#include "m-string.h"

// Declare two tuple types.
// IntPair holds only POD fields, while Record also owns dynamic strings.
// Tuples are defined with a list of fields, each field is a tuple of (field_name, field_type[, field_oplist]).
// They are pretty much the same as a struct, but with some differences:
// - They are designed to be used with generic macros, so they automatically create a consistent API for initialization, assignment, comparison, hashing, etc. This is achieved through the use of oplists (see M_OPL_* macros below).
// - They can be initialized with a convenient syntax using the _emplace functions, which allow to initialize all fields in one call. This is especially useful for tuples with many fields or with complex types (e.g. string_t).
TUPLE_DEF2(IntPair, (a, int), (b, int))

TUPLE_DEF2(Record, (name, string_t), (surname, string_t), (age, int))

// All tests subsequently call the same code, but with different ways to initialize the tuple and print it.
// using more and more of the features of the tuple type (initialization, output, oplist, etc).
static void test1(void)
{
    // Explicit lifecycle style: init -> fill fields -> use -> clear.
    // This is the most direct way to understand tuple ownership rules.
    IntPair_t p;
    IntPair_init(p);
    p->a = 1;
    p->b = 2;
    printf("p: %d, %d\n", p->a, p->b);
    IntPair_clear(p);

    Record_t r;
    Record_init(r);
    string_set_str(r->name, "John");
    string_set_str(r->surname, "Smith");
    r->age = 30;
    printf("r: %s %s, %d\n",
           string_get_cstr(r->name),
           string_get_cstr(r->surname),
           r->age);
    // Record contains string_t fields, so clear releases their internal buffers.
    Record_clear(r);
}

static void test2(void)
{
    // Convenience constructor: init + assign all fields in one call.
    IntPair_t p;
    IntPair_init_emplace(p, 1, 2);
    printf("p: %d, %d\n", p->a, p->b);
    IntPair_clear(p);

    Record_t r;
    // Use of STRING_CTE macro to create temporary string_t objects from C strings.
    Record_init_emplace(r, STRING_CTE("John"), STRING_CTE("Smith"), 30);
    printf("r: %s %s, %d\n",
           string_get_cstr(r->name),
           string_get_cstr(r->surname),
           r->age);
    Record_clear(r);
}

// Register oplists for both tuple types so generic helpers (M_LET, M_PRINT, *_out_str)
// know how to initialize, print, and clear each field.
#define M_OPL_IntPair_t() TUPLE_OPLIST(IntPair, M_BASIC_OPLIST, M_BASIC_OPLIST)
#define M_OPL_Record_t() TUPLE_OPLIST(Record, STRING_OPLIST, STRING_OPLIST, M_BASIC_OPLIST)

static void test3(void)
{
    // M_LET creates an automatic scope: object is initialized on entry and
    // automatically cleared on scope exit (RAII-like behavior in C macros).
    // M_LET also allows to initialize the tuple with a literal syntax, using parenthesis to group fields.
    M_LET( (p, (1, 2)), IntPair_t) {
        printf("p:");
        /*
         * *_out_str is a generic output function that uses the tuple oplist to print all fields.
         * It is equivalent to:
         *   printf("p: %d, %d\n", p->a, p->b);
         */
        IntPair_out_str(stdout, p);
        printf("\n");
    }

    M_LET( (r, (STRING_CTE("John"), STRING_CTE("Smith"), 30)), Record_t) {
        printf("r:");
        Record_out_str(stdout, r);
        printf("\n");
    }
}

static void test4(void)
{
    // Same scoped construction, but using M_PRINT for generic formatted output.
    M_LET( (p, (1, 2)), IntPair_t) {
        M_PRINT("p:", (p, IntPair_t), "\n");
    }

    M_LET( (r, (("John"), ("Smith"), 30)), Record_t) {
        M_PRINT("r:", (r, Record_t), "\n");
    }
}

int main(void)
{
    test1();
    test2();
    test3();
    test4();
    return 0;
}
