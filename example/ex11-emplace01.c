/* This example demonstrates M*LIB's "emplace" feature using GMP's mpz_t
   (arbitrary-precision integers) as the element type.

   "Emplacing" means constructing an element in-place directly inside the
   container from a source value of a *different* type (int, double, string…),
   avoiding a temporary object and an extra copy.  M*LIB generates one
   typed emplace function per registered source type (see EMPLACE_TYPE below).

   Requires the GMP library (-lgmp).  The build system sets HAVE_GMP when it
   is available; the #else branch below provides a no-op fallback. */
#if HAVE_GMP

#include <stdio.h>

#include <gmp.h>

#include "m-array.h"

/* GMP doesn't provide a fast hash function (it is needed for being able to
   use mpz_t as a key in a hash-map).  We compute it by feeding each limb
   (machine word) of the internal representation into M*LIB's hash mixer. */
static inline size_t
mpz_fast_hash(const mpz_t z)
{
  mp_size_t  n = z->_mp_size; /* number of limbs; negative when z < 0 */
  mp_limb_t *p = z->_mp_d;   /* pointer to the limb array              */
  M_HASH_DECL(hash);          /* declare and initialise the hash state  */

  if (n <= 0) {
    /* Mix a sentinel so that 0 and negative numbers hash differently */
    M_HASH_UP (hash, 0x7F5C1458);
    if (n == 0) {
      return M_HASH_FINAL (hash); /* zero has no limbs to iterate over */
    }
    n = -n; /* iterate over the absolute value */
  }
  while (n-- > 0) {
    M_HASH_UP (hash, *p++); /* fold each limb into the hash */
  }
  return M_HASH_FINAL (hash);
}

/* Define the OPLIST of mpz_t and register it globally as M_OPL_mpz_t().
   An OPLIST is a compile-time dictionary that maps generic M*LIB operator
   names (INIT, SET, EQUAL, …) to the actual GMP functions.

   Key points:
   - API(...) adapts the calling convention when GMP's argument order or
     return value differs from what M*LIB expects.
   - INIT_WITH uses M_INIT_WITH_THROUGH_EMPLACE_TYPE so that generic
     initialisation (e.g. inside M_LET) can delegate to any emplace variant.
   - EMPLACE_TYPE lists every source type from which a mpz_t can be
     constructed in-place.  Each entry is:
       (suffix, init_function, source_c_type)
     M*LIB will generate array_mpz_emplace_back_<suffix>() for each one. */
#define M_OPL_mpz_t()                                                   \
  (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear), \
   INIT_WITH(API_1(M_INIT_WITH_THROUGH_EMPLACE_TYPE)),                  \
   INIT_MOVE(M_COPY_A1_DEFAULT)                                       \
   EQUAL( API( mpz_cmp, EQ(0), ARG1, ARG2)),                            \
   CMP(mpz_cmp),                                                        \
   HASH(mpz_fast_hash),                                                 \
   OUT_STR( API( mpz_out_str, VOID, ARG1, 10, ARG2)),                   \
   IN_STR( API( mpz_inp_str, GT(0), ARG1, ARG2, 10)),                   \
   ADD(mpz_add), SUB(mpz_sub), MUL(mpz_mul), DIV(mpz_div),              \
   EMPLACE_TYPE( LIST( (_si,   mpz_init_set_si,  int),                  \
                       (_ui,   mpz_init_set_ui,  unsigned),             \
                       (_d,    mpz_init_set_d,   double),               \
                       (_cstr, API(mpz_init_set_str, VOID, ARG1, ARG2, 10), const char *), \
                       (_str,  API(mpz_init_set_str, VOID, ARG1, ARG2, 10), char *) )), \
   TYPE(mpz_t) )

/* Instantiate a dynamic array of mpz_t elements.
   ARRAY_DEF generates array_mpz_push_back, array_mpz_emplace_back_*, etc.
   The companion M_OPL_array_mpz_t() macro lets other M*LIB containers
   (or M_LET) recognise array_mpz_t as a managed type. */
ARRAY_DEF(array_mpz, mpz_t)
#define M_OPL_array_mpz_t() ARRAY_OPLIST(array_mpz, M_OPL_mpz_t())

int main(void)
{
  /* M_LET declares a variable, calls its INIT operator on entry, and
     automatically calls its CLEAR operator on exit — no manual cleanup. */
  M_LET(z, mpz_t) {
    mpz_set_str(z, "25446846874687468746874687468746874686874", 10);
    printf("z = %s\n", mpz_get_str(NULL, 10, z));
  }

  /* M_LET also accepts an optional initializer for each variable.
     The syntax (varname, (arg)) means: declare varname and construct it
     via INIT_WITH(arg), which — thanks to INIT_WITH_THROUGH_EMPLACE_TYPE —
     picks the right emplace constructor based on the argument's C type:
       "2544684687468746874"  → const char*  → _cstr emplace variant
       23                     → int          → _si   emplace variant
       24.5                   → double       → _d    emplace variant
     All variables are still cleared automatically at the end of the block. */
  M_LET( (z1, ("2544684687468746874")), (z2, (23)), (z4, (24.5)), mpz_t) {
    printf("z1 = %s\n", mpz_get_str(NULL, 10, z1));
    printf("z2 = %s\n", mpz_get_str(NULL, 10, z2));
    printf("z4 = %s\n", mpz_get_str(NULL, 10, z4));
  }

  /* Demonstrate all emplace variants: each call constructs a new mpz_t
     element directly at the back of the array from the given source value,
     without ever creating a temporary mpz_t in the caller's scope. */
  M_LET(array, array_mpz_t) {
    /* _cstr: construct from a const char* (decimal string literal) */
    array_mpz_emplace_back_cstr(array, "1234567890123456789012345678901234567890");
    array_mpz_emplace_back_cstr(array, "9876543210987654321098765432109876543210");

    /* _si: construct from a signed int */
    array_mpz_emplace_back_si(array, -42);

    /* _ui: construct from an unsigned int */
    array_mpz_emplace_back_ui(array, 100U);

    /* _d: construct from a double (fractional part is truncated by GMP) */
    array_mpz_emplace_back_d(array, 3.14);

    /* _str: construct from a char* (same underlying function as _cstr,
       but accepts a non-const pointer — useful when the string is mutable) */
    char str[] = "271828182845904523536";
    array_mpz_emplace_back_str(array, str);

    /* Print every element using GMP's base-10 string conversion */
    for(size_t i = 0; i < array_mpz_size(array); i++) {
      printf("array[%zu] = %s\n", i, mpz_get_str(NULL, 10, *array_mpz_get(array, i)));
    }
  }

  /* The same initializer mechanism works for containers too.
     Here the initializer is a parenthesised list of values:
       ("4568888")  → const char*  → emplace_back_cstr
       (245)        → int          → emplace_back_si
       (789999U)    → unsigned     → emplace_back_ui
       (-25.6)      → double       → emplace_back_d
     M*LIB iterates over the list and emplaces each element in turn,
     again entirely at compile time through EMPLACE_TYPE dispatch. */
  M_LET( (array, ( ("4568888"), (245), (789999U), (-25.6))), array_mpz_t) {
    printf("Array is: ");
    array_mpz_out_str(stdout, array);
    printf("\n");
  }

  return 0;
}

#else
/* GMP is not available on this platform: provide a no-op main so that
   the build still succeeds and tests can run. */
int main(void) { return 0; }
#endif
