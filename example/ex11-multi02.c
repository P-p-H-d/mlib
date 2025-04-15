#if HAVE_GMP && HAVE_MPFR

#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>

#include "m-variant.h"
#include "m-string.h"
#include "m-dict.h"
#include "m-array.h"
#include "m-algo.h"

// GMP doesn't provide a fast hash function (It is needed for being able to use mpz_t as key in a hashmap)
static inline size_t
mpz_fast_hash(const mpz_t z)
{
  mp_size_t  n = z->_mp_size;
  mp_limb_t *p = z->_mp_d;
  M_HASH_DECL(hash);

  if (n <= 0) {
    M_HASH_UP (hash, 0x7F5C1458);
    if (n == 0) {
      return M_HASH_FINAL (hash);
    }
    n = -n;
  }
  while (n-- > 0) {
    M_HASH_UP (hash, *p++);
  }
  return M_HASH_FINAL (hash);
}

/* Define the OPLIST of a mpz_t and register it globally */
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
   EMPLACE_TYPE( LIST( (_si, mpz_init_set_si, int),                     \
                       (_ui, mpz_init_set_ui, unsigned),                \
                       (_d,  mpz_init_set_d,  double),                  \
                       (_cstr, API(mpz_init_set_str, VOID, ARG1, ARG2, 10), const char *), \
                       (_str, API(mpz_init_set_str, VOID, ARG1, ARG2, 10), char *) )), \
   TYPE(mpz_t) )



// MPFR doesn't provide a fast hash function (It is needed for being able to use mpz_t as key in a hashmap)
static size_t
mpfr_fast_hash (const mpfr_t f)
{
  mp_limb_t *p = f->_mpfr_d;
  mp_size_t  n = (f->_mpfr_prec-1)/(sizeof(mp_limb_t)*CHAR_BIT)+1;
  M_HASH_DECL (h);

  M_HASH_UP (h, f->_mpfr_exp);

  if (MPFR_SIGN (f) < 0)
    M_HASH_UP (h, 1);

  /* Special value: NAN, INF and ZERO */
  if (mpfr_nan_p (f) || mpfr_inf_p (f) || mpfr_zero_p (f))
    return M_HASH_FINAL (h);

  /* Mantissa */
  do {
    M_HASH_UP (h, *p++);
  } while (--n > 0);
  return M_HASH_FINAL (h);
}

/* Define the OPLIST of a mpfr_t and register it globally */
#define M_OPL_mpfr_t()                                                   \
  (INIT(mpfr_init), INIT_SET(API(mpfr_init_set, VOID, ARG1, ARG2, MPFR_RNDN)), SET(API(mpfr_set, VOID, ARG1, ARG2, MPFR_RNDN)), CLEAR(mpfr_clear), \
   INIT_WITH(API_1(M_INIT_WITH_THROUGH_EMPLACE_TYPE)),                   \
   INIT_MOVE(M_COPY_A1_DEFAULT)                                        \
   EQUAL( API( mpfr_cmp, EQ(0), ARG1, ARG2)),                            \
   CMP(mpfr_cmp),                                                        \
   HASH(mpfr_fast_hash),                                                 \
   OUT_STR( API( mpfr_out_str, VOID, ARG1, 10, 0, ARG2, MPFR_RNDN)),     \
   IN_STR( API( mpfr_inp_str, GT(0), ARG1, ARG2, 10, MPFR_RNDN)),        \
   ADD( API(mpfr_add, VOID, ARG1, ARG2, ARG3, MPFR_RNDN)),               \
   SUB( API(mpfr_sub, VOID, ARG1, ARG2, ARG3, MPFR_RNDN)),               \
   MUL( API(mpfr_mul, VOID, ARG1, ARG2, ARG3, MPFR_RNDN)),               \
   DIV( API(mpfr_div, VOID, ARG1, ARG2, ARG3, MPFR_RNDN)),               \
   EMPLACE_TYPE( LIST( (_si, API(mpfr_init_set_si, VOID, ARG1, ARG2, MPFR_RNDN), int), \
                       (_ui, API(mpfr_init_set_ui, VOID, ARG1, ARG2, MPFR_RNDN), unsigned), \
                       (_d,  API(mpfr_init_set_d,  VOID, ARG1, ARG2, MPFR_RNDN), double), \
                       (_ld, API(mpfr_init_set_ld, VOID, ARG1, ARG2, MPFR_RNDN), long double), \
                       (_z,  API(mpfr_init_set_z,  VOID, ARG1, ARG2, MPFR_RNDN), mpz_t), \
                       (_zptr,  API(mpfr_init_set_z,  VOID, ARG1, ARG2, MPFR_RNDN), mpz_ptr), \
                       (_cstr, API(mpfr_init_set_str, VOID, ARG1, ARG2, 10, MPFR_RNDN), const char *), \
                       (_str, API(mpfr_init_set_str, VOID, ARG1, ARG2, 10, MPFR_RNDN), char *) )), \
   TYPE(mpfr_t) )


/* Define a variant (aka a typed union) over three kind of numbers named my_number_t
   Notice than mpz_t/mpfr_t has special init & clear functions.
   This is handled by the library.
*/
VARIANT_DEF2(my_number,
             (n, long long),
             (z, mpz_t),
             (fr, mpfr_t) )
/* Register the oplist associated to this variant named my_number_t */
#define M_OPL_my_number_t() VARIANT_OPLIST(my_number, M_BASIC_OPLIST, M_OPL_mpz_t(), M_OPL_mpfr_t() )


/* Define a dictionary of string_t --> my_number_t named my_dict_t */
DICT_DEF2(my_dict, string_t, my_number_t)
/* Register the oplist associated to this dictionary named my_dict_t */
#define M_OPL_my_dict_t() DICT_OPLIST(my_dict, STRING_OPLIST, M_OPL_my_number_t() )


/* Define an array of such dictionary named my_array_t */
ARRAY_DEF(my_array, my_dict_t)
/* Register the oplist associated to this array (of dictionary of variant) named my_array_t*/
#define M_OPL_my_array_t() ARRAY_OPLIST(my_array, M_OPL_my_dict_t())


// Show how to define an array of mpz_t and some algorithms on it
ARRAY_DEF(array_mpz, mpz_t)
#define M_OPL_array_mpz_t() ARRAY_OPLIST(array_mpz, M_OPL_mpz_t())
ALGO_DEF(array_mpz, array_mpz_t)

// Show how to define an array of mpfr_t and some algorithms on it
ARRAY_DEF(array_mpfr, mpfr_t)
#define M_OPL_array_mpfr_t() ARRAY_OPLIST(array_mpfr, M_OPL_mpfr_t())
ALGO_DEF(array_mpfr, array_mpfr_t)

static void
compute_roots(array_mpfr_t roots, const array_mpz_t pol)
{
  assert( array_mpz_size(pol) == 3);
  // Initialize mpfr_t variables from the mpz_t values in the array 'pol'
  M_LET( (a, (*array_mpz_get(pol, 0))),
         (b, (*array_mpz_get(pol, 1))),
         (c, (*array_mpz_get(pol, 2))),
         delta, tmp, mpfr_t) {
      // Reset the output array
      array_mpfr_reset(roots);
      // Compute the first root
      mpfr_mul(delta, b, b, MPFR_RNDN);
      mpfr_mul_ui(tmp, a, 4, MPFR_RNDN);
      mpfr_mul(tmp, tmp, c, MPFR_RNDN);
      mpfr_sub(delta, delta, tmp, MPFR_RNDN);
      mpfr_sqrt(delta, delta, MPFR_RNDN);
      mpfr_neg(tmp, b, MPFR_RNDN);
      mpfr_add(tmp, tmp, delta, MPFR_RNDN);
      mpfr_div(tmp, tmp, a, MPFR_RNDN);
      mpfr_div_2ui(tmp, tmp, 1, MPFR_RNDN);
      // Push first root
      array_mpfr_push_back(roots, tmp);
      // Compute second root
      mpfr_neg(tmp, b, MPFR_RNDN);
      mpfr_sub(tmp, tmp, delta, MPFR_RNDN);
      mpfr_div(tmp, tmp, a, MPFR_RNDN);
      mpfr_div_2ui(tmp, tmp, 1, MPFR_RNDN);
      // Push second root
      array_mpfr_push_back(roots, tmp);
  } // Clear the mpfr_t variables after this point.
}

int main(void)
{
  M_LET(z, my_number_t)               /* Define & init & clear an instance of a variant of my_number_t */
    M_LET(dict, my_dict_t)            /* Define & init & clear an instance of a dictionary of variant */
    M_LET(array, my_array_t)          /* Define & init & clear an intance of an array of dictionary of variant */
    M_LET(gmp, mpz_t)                 /* M_LET allows even with non container type */
    {
      my_number_set_n (z, 17LL);                 // Set z as the long long 17
      my_dict_set_at(dict, STRING_CTE("n1"), z); // Push it in the dictionary

      mpz_set_str(gmp, "25446846874687468746874687468746874686874", 10); // Set gmp to a very long integer.
      my_number_set_z (z, gmp);                  // Set z as the mpz_t variable
      my_dict_set_at(dict, STRING_CTE("n2"), z); // Push it in the dictionary

      my_array_push_back (array, dict);          // Push the dictionary in the array

      // Let's add a new element in the array through a mpfr_t set to 42.17
      M_LET( (fr, ("42.17")), mpfr_t) {
        my_number_set_fr(z, fr);                // Set z as this variable.
      }
      my_dict_set_at(dict, STRING_CTE("n3"), z); // Push it in the dictionary

      my_number_emplace_fr_si(z, 42);            // Directly emplace the variant as a mpfr_t with a signed integer for init
      my_dict_set_at(dict, STRING_CTE("n4"), z); // Push it in the dictionary

      my_number_emplace_z_ui(z, 189);            // Directly emplace the variant as a mpz_t with an unsigned integer for init
      my_dict_set_at(dict, STRING_CTE("n5"), z); // Push it in the dictionary
      
      my_array_push_back (array, dict);          // Push the dictionary in the array
        
      // Iterate over the array container
      for M_EACH(item, array, my_array_t) {
          // Iterate over the dictionary '*item' container which is of type my_dict_t
          for M_EACH(p, *item, my_dict_t) {
              // Dictionary iterator are pair (key,value)
              printf("Key is '%s'. ", string_get_cstr(p->key));
              // In function of the type of value
              if (my_number_n_p(p->value)) {
                // Print it. 
                M_PRINT("It is a long long, value = ", *my_number_cget_n(p->value), "\n");
              }
              if (my_number_z_p(p->value)) {
                // Print it. Since mpz_t isn't a native type, M_PRINT macro doesn't know it by default
                // So we need to add the type to the value.
                // M_PRINT will then look at its globally registered oplist
                // and known how to print it.
                M_PRINT("It is a mpz_t, value = ", (*my_number_cget_z(p->value), mpz_t), "\n");
              }
              if (my_number_fr_p(p->value)) {
                // Same but with a MPFR number.
                M_PRINT("It is a mpfr_t, value = ", (*my_number_cget_fr(p->value), mpfr_t), "\n");
              }
            }
          printf("Next element of array\n");
        }
      
      // Print all the array in one step. Need to be done in C11 only
      M_PRINT("\nThe array is equal to ", (array, my_array_t), "\n");
    } /* All variables are automatically cleared beyond this point */

  // Define an array of mpz_t as { 1, 2, 1 } (with auto promotion from integer / string / double)
  // The auto-promotion uses _Generic feature and is available only in C11.
  M_LET ( (az, (1), ("2"), (1.0)), array_mpz_t)
  M_LET ( roots, array_mpfr_t) {
    // Compute roots
    compute_roots(roots, az);
    // Display of roots:
    M_PRINT("Roots of ", (az, array_mpz_t), " are ", (roots, array_mpfr_t), "\n");
  }

}

#else
// Nothing to do
int main(void) { return 0; }
#endif
