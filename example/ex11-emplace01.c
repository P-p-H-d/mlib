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

// There are unfortunately no standard mpq_init_set_* functions, so we define them ourselves as wrappers around mpq_init + mpq_set_*.
#ifndef mpq_init_set
#define mpq_init_set(mpq, other) do { mpq_init(mpq); mpq_set(mpq, other); } while(0)
#endif
#ifndef mpq_init_set_si
#define mpq_init_set_si(mpq, num, den) do { mpq_init(mpq); mpq_set_si(mpq, num, den); } while(0)
#endif
#ifndef mpq_init_set_ui
#define mpq_init_set_ui(mpq, num, den) do { mpq_init(mpq); mpq_set_ui(mpq, num, den); } while(0)
#endif
#ifndef mpq_init_set_str
#define mpq_init_set_str(mpq, str) do { mpq_init(mpq); mpq_set_str(mpq, str, 10); } while(0)
#endif

/* Define an oplist for mpq_t (rationals) and register multi-argument
  emplace constructors.
  Unlike mpz_t, a rational may naturally need two values at construction
  time (numerator + denominator), so _si and _ui emplace variants each
  accept two arguments and forward both to mpq_init_set_* wrappers. */
#define M_OPL_mpq_t()                                                   \
  (INIT(mpq_init), INIT_SET(mpq_init_set), SET(mpq_set), CLEAR(mpq_clear), \
   INIT_WITH(API_1(M_INIT_WITH_THROUGH_EMPLACE_TYPE)),                  \
   INIT_MOVE(M_COPY_A1_DEFAULT)                                       \
   EQUAL( API( mpq_cmp, EQ(0), ARG1, ARG2)),                            \
   CMP(mpq_cmp),                                                        \
   OUT_STR( API( mpq_out_str, VOID, ARG1, 10, ARG2)),                   \
   IN_STR( API( mpq_inp_str, GT(0), ARG1, ARG2, 10)),                   \
   ADD(mpq_add), SUB(mpq_sub), MUL(mpq_mul), DIV(mpq_div),              \
   EMPLACE_TYPE( LIST( (_si,   mpq_init_set_si,  long, unsigned long),                  \
                       (_ui,   mpq_init_set_ui,  unsigned long, unsigned long),             \
                       (_cstr, mpq_init_set_str, const char *), \
                       (_str,  mpq_init_set_str, char *) )), \
   TYPE(mpq_t) )

ARRAY_DEF(array_mpq, mpq_t)
#define M_OPL_array_mpq_t() ARRAY_OPLIST(array_mpq, M_OPL_mpq_t())


int main(void)
{
  /* M_LET declares a variable, calls its INIT operator on entry, and
     automatically calls its CLEAR operator on exit — no manual cleanup.
     However you cannot return from the function or break from the block, as the variable would not be cleared,
     but you can throw exceptions (e.g. with M_THROW) as the variable will be cleared in this case.
     Basic usage: M_LET(var1, ...,varN, type) { ... } */
  M_LET(z, mpz_t) {
    mpz_set_str(z, "25446846874687468746874687468746874686874", 10);
    gmp_printf("z = %Zd\n", z);
  }

  /* M_LET also accepts an optional initializer for each variable.
     The syntax (varname, (arg)) means: declare varname and construct it
     via INIT_WITH(arg), which — thanks to INIT_WITH_THROUGH_EMPLACE_TYPE —
     picks the right emplace constructor based on the argument's C type:
       "2544684687468746874"  → const char*  → _cstr emplace variant
       23                     → int          → _si   emplace variant
       24.5                   → double       → _d    emplace variant
     All variables are still cleared automatically at the end of the block.
     Medium usage: M_LET( (var1, value1), ..., (varN, valueN), type) { ... }
                   with value1 the same type as var1.
     Advanced usage: M_LET( (var1, (value1_1, ...) ), ..., (varN, (valueN_1, ...) ), type) { ... }
                  with value1_1 to value1_N1 used to build var1 with an emplace constructor, and so on for var2 to varN.
                  It is the INIT_WITH operator that is used to build var1 from value1_1 to value1_N1, and so on for var2 to varN.
   */
  M_LET( (z1, ("2544684687468746874")), (z2, (23)), (z4, (24.5)), mpz_t) {
    gmp_printf("z1 = %Zd\n", z1);
    gmp_printf("z2 = %Zd\n", z2);
    gmp_printf("z4 = %Zd\n", z4);
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
      gmp_printf("array[%zu] = %Zd\n", i, *array_mpz_get(array, i));
    }
  }

  /* The same initializer mechanism works for containers too.
     Here the initializer is a parenthesised list of values:
       ("4568888")  → const char*  → emplace_back_cstr
       (245)        → int          → emplace_back_si
       (789999U)    → unsigned     → emplace_back_ui
       (-25.6)      → double       → emplace_back_d
     M*LIB iterates over the list and emplaces each element in turn,
     again entirely at compile time through EMPLACE_TYPE dispatch.

     Very Advanced usage: M_LET( (var1, ( (value1_1), ...) ), ..., (varN, ( (valueN_1), ...) ), type) { ... }
                  with value1_1 to value1_N1 used to build the intermediary variable that will be used to build var1 with two emplace constructors, and so on for var2 to varN.
                  It is the INIT_WITH operators that are used to build var1 from value1_1 to value1_N1, and so on for var2 to varN.
    
    This example really shows the power of M*LIB's compile-time introspection and code generation:
    the correct emplace constructor is chosen for each variable based on its type, without any runtime
    overhead or manual boilerplate and then the result of theses constructions are given at another 
    constructor to build the final array!
    And it doesn't show that it can also build multiple variables at the same time.
    */
  M_LET( (array, ( ("4568888"), (245), (789999U), (-25.6))), array_mpz_t) {
    printf("Array is: ");
    array_mpz_out_str(stdout, array);
    printf("\n");
  }

  /* Basic mpq_t example: M_LET handles init/clear automatically,
     and gmp_printf with %Qd prints the rational as num/den. */
  M_LET(q, mpq_t) {
    mpq_set_str(q, "25446846874687468746874687468746874686874", 10);
    gmp_printf("q = %Qd\n", q);
  }

  /* mpq_t emplace supports both one-argument and two-argument constructors.
     The two-argument forms (_si, _ui) map directly to (numerator, denominator). */
  M_LET(array, array_mpq_t) {
    /* _si/_ui take two parameters: numerator then denominator. */
    array_mpq_emplace_back_si(array, 1, 2); // 1/2
    array_mpq_emplace_back_ui(array, 3, 4); // 3/4
    array_mpq_emplace_back_cstr(array, "5/6"); // 5/6

    /* Print every element using GMP's base-10 string conversion */
    for(size_t i = 0; i < array_mpq_size(array); i++) {
      gmp_printf("array[%zu] = %Qd\n", i, *array_mpq_get(array, i));
    }
  }

  return 0;
}

#else
/* GMP is not available on this platform: provide a no-op main so that
   the build still succeeds and tests can run. */
int main(void) { return 0; }
#endif
