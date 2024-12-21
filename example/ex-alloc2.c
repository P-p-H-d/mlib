#include <stdio.h>
#include <stdlib.h>

#include "m-list.h"
#include "m-array.h"

////////////////////////////////////////////////////////////

static size_t allocated_size = 0;

static void* my_alloc(size_t s)
{
    printf("Alloc called s=%lu!\n", s);
    allocated_size += s;
    return malloc(s);
}

static void my_free(void *ptr, size_t s)
{
    printf("Free called s=%lu!\n", s);
    allocated_size -= s;
    free(ptr);
}

// Allocate for single object
# define MY_MEMORY_ALLOC(type) my_alloc (sizeof (type))
# define MY_MEMORY_DEL(ptr)    my_free(ptr, sizeof *ptr)

////////////////////////////////////////////////////////////

// Override only for this container the memory allocation function
LIST_DEF(list_int, int, M_OPEXTEND(M_BASIC_OPLIST, NEW(MY_MEMORY_ALLOC), DEL(MY_MEMORY_DEL)))

static void test_list(void)
{
    printf("List demo\n");

    list_int_t list;
    list_int_init(list);

    list_int_push_back(list, 34);
    list_int_push_back(list, 34);
    
    if (allocated_size == 0) {
        printf("Nothing was allocated... Very strange...\n");
        exit(1);
    }
    list_int_clear(list);

    if (allocated_size != 0) {
        printf("Nothing was freed (%lu)... Very strange...\n", allocated_size);
        exit(2);
    }
    printf("Global custom allocator for list works!\n");
}

////////////////////////////////////////////////////////////

static void *last_alloc;

static void* my_array_alloc(size_t s, void *org, size_t n)
{
    printf("Array Alloc called org=%p of n=%zu elements of size s=%lu!\n", org, n, s);
    last_alloc = realloc(org, s*n);
    return last_alloc;
}

static void my_array_free(void *ptr)
{
    printf("Array Free called ptr=%p last_alloc=%p!\n", ptr, last_alloc);
    if (ptr != last_alloc) {
        printf("Somthing went wrong.\n");
        exit(4);
    }
    free(ptr);
}

// Allocate for array of objects
# define MY_MEMORY_REALLOC(type, ptr, o, n) my_array_alloc (sizeof (type), ptr, n)
# define MY_MEMORY_FREE(type, ptr, n)       my_array_free(ptr)

////////////////////////////////////////////////////////////

// Override only for this container the memory allocation function
ARRAY_DEF(array_int, int, M_OPEXTEND(M_BASIC_OPLIST, REALLOC(MY_MEMORY_REALLOC), FREE(MY_MEMORY_FREE)))

static void test_array(void)
{
    printf("Array demo\n");

    array_int_t array;
    array_int_init(array);

    array_int_push_back(array, 34);
    array_int_push_back(array, 34);
    
    array_int_clear(array);

    printf("Global custom allocator for array works!\n");
}

////////////////////////////////////////////////////////////

int main(void)
{
    test_list();
    test_array();
    exit(0);
}
