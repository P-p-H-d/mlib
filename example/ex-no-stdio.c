/*
 * Micro example to test if M*LIB supports not using stdio.h
 */
#define M_USE_STDIO 0
#define M_RAISE_FATAL(...) abort()
#include "m-array.h"

ARRAY_DEF(arr, int)

int main(void) {
    arr_t a;
    arr_init(a);
    arr_push_back(a, 4);
    arr_clear(a);
    return 0;
}
