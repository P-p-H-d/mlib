#ifndef EX_SHARED_PTR01_C
#define EX_SHARED_PTR01_C

#include "m-shared-ptr.h"

// The header exposes only the shared handle and the generated API.
// The actual pod_t layout stays private in the .c file, so users can call
// shared_pod_new/push/pop without seeing the underlying representation.
SHARED_WEAK_PTR_DECL(shared_pod, M_OPEXTEND(M_POD_OPLIST, PUSH(1), FULL_P(1), POP(1), EMPTY_P(1), SUBTYPE(char)))

#endif
