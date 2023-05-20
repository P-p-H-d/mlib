#ifndef MY_HEADER
#define MY_HEADER

// Request M*LIB to not always inline functions
#define M_USE_FINE_GRAINED_LINKAGE
#include "m-string.h"
#include "m-array.h"

// The following functions will not be inlined:
#define M_USE_DECL
ARRAY_DEF(array_str, string_t)
#undef M_USE_DECL

#endif
