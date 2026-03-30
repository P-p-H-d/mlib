#ifndef MY_HEADER
#define MY_HEADER

// Enable per-section linkage control instead of all-inline behavior.
#define M_USE_FINE_GRAINED_LINKAGE
#include "m-string.h"
#include "m-array.h"

// Only the code generated while M_USE_DECL is active will be emitted
// out-of-line in another TU (the lib.c file).
#define M_USE_DECL
ARRAY_DEF(array_str, string_t)
#undef M_USE_DECL

// m-string APIs remain in their default linkage mode in this example.

#endif
