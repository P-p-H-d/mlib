// Emit out-of-line bodies only for APIs that were declared under M_USE_DECL
// in ex_noinline02.h (here: the array_str-generated functions).
#define M_USE_DEF
#include "ex_noinline02.h"
