#ifndef MY_NOINLINE_HEADER
#define MY_NOINLINE_HEADER

// Header shared by both TUs:
// - one TU will only see declarations (M_USE_DECL)
// - one TU will emit the real function bodies (M_USE_DEF)
// This keeps generated container functions out-of-line.
#define M_USE_DECL
#include "m-string.h"
#include "m-array.h"

// Generate the type and API for an array of strings.
ARRAY_DEF(array_str, string_t)

#endif

