#ifndef MY_HEADER
#define MY_HEADER

// Request M*LIB to not inline functions if not the library source
#define M_USE_DECL
#include "m-string.h"
#include "m-array.h"

ARRAY_DEF(array_str, string_t)

#endif

