// Header for coverage purpose
// Expand here all code that shall not be measured in coverage.
// Since the functions are not declared as static inline, their coverage is measured
SHARED_PTR_DEF_EXTERN_AS(shared_int, SharedInt, int, M_BASIC_OPLIST)
SHARED_PTR_DEF_EXTERN_AS(shared_double, SharedDouble, double, M_OPEXTEND(M_BASIC_OPLIST, TYPE(double)))
SHARED_PTR_DEF_EXTERN(shared_array, array_t)
SHARED_PTR_DEF_EXTERN(shared_array_str, array_str_t, ARRAY_OPLIST(array_str, M_STRING_OPLIST))
