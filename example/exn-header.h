#ifndef MY_HEADER
#define MY_HEADER

// Request M*LIB to not always inline functions
#define M_USE_EXTERN_FINE_GRAINED 1
#include "m-string.h"
#include "m-array.h"

// The following functions will not be inlined:
#undef M_USE_EXTERN_FINE_SELECT
#ifdef EXN_LIB
#define M_USE_EXTERN_FINE_SELECT 3
#else
#define M_USE_EXTERN_FINE_SELECT 2
#endif

ARRAY_DEF(array_str, string_t)

#endif
