#include <stdio.h>
#include <stdlib.h>

// Define context memory functions
struct context_s;
extern void* my_alloc(struct context_s *context, size_t s);
extern void my_free(struct context_s *context, void *ptr, size_t s);

// Make M*LIB use this context for memory allocation functions (single object):
// It shall be done once before including M*LIB header.
#define M_USE_CONTEXT struct context_s *
#define M_MEMORY_ALLOC(ctx, type) my_alloc (ctx, sizeof (type))
#define M_MEMORY_DEL(ctx, ptr)    my_free(ctx, ptr, sizeof *ptr)

////////////////////////////////////////////////////////////
#include "m-list.h"

////////////////////////////////////////////////////////////

struct context_s {
    size_t allocated_size;
};

void* my_alloc(struct context_s *context, size_t s)
{
    printf("Alloc called s=%zu!\n", s);
    context->allocated_size += s;
    return malloc(s);
}

void my_free(struct context_s *context, void *ptr, size_t s)
{
    printf("Free called s=%zu!\n", s);
    context->allocated_size -= s;
    free(ptr);
}

////////////////////////////////////////////////////////////

// Define the list container with the context sensitive memory allocation function
LIST_DEF(list_int, int)

static void test_list(struct context_s *context)
{
    printf("List demo with context allocator\n");

    list_int_t list;
    list_int_init(list);

    list_int_push_back(context, list, 34);
    list_int_push_back(context, list, 34);

    if (context->allocated_size == 0) {
        printf("Nothing was allocated... Very strange...\n");
        exit(1);
    }
    list_int_clear(context, list);

    if (context->allocated_size != 0) {
        printf("Nothing was freed (%zu)... Very strange...\n", context->allocated_size);
        exit(2);
    }
    printf("Global custom allocator for list with context works!\n");
}

int main(void)
{
    struct context_s context = { 0 };
    test_list(&context);
    exit(0);
}
