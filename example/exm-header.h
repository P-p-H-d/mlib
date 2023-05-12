#ifndef MY_HEADER
#define MY_HEADER

// Request M*LIB to not inline functions if not the library source
#ifndef M_USE_EXTERN_DEF
#define M_USE_EXTERN_DECL
#endif
#include "m-string.h"
#include "m-array.h"

ARRAY_DEF(array_str, string_t)

#endif

