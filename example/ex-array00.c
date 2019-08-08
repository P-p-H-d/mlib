/* Include M*LIB header */
#include "m-array.h"
#include "m-string.h"

/* Provide several examples of basic types usage with array.
   Each time it creates and initializes an array,
   fill it with some number,
   and then compute its sum.

   This is done for several basic C types:
   - integers,
   - floats, 
   - pointers, 
   - structures.
*/


/* Generate prototypes and inline functions
   for basic types.
   For integer and floats, there is no need to specify
   an oplist. M_DEFAULT_OPLIST will be used.
 */
ARRAY_DEF(r_bool, bool)

/* Basic usage of the type */
static int
test_bool(int n)
{
  r_bool_t array;

  r_bool_init(array);
  for(int i = 0; i < n; i++) {
    r_bool_push_back(array, i & 1);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_bool_get(array, i);
  }
  
  r_bool_clear(array);

  return s;
}

/* Same with type char */
ARRAY_DEF(r_char, char)

static int
test_char(int n)
{
  r_char_t array;

  r_char_init(array);
  for(int i = 0; i < n; i++) {
    r_char_push_back(array, i & 255);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_char_get(array, i);
  }
  
  r_char_clear(array);

  return s;
}

/* Same with type signed int. */
ARRAY_DEF(r_int, signed int)

static int
test_int(int n)
{
  r_int_t array;

  r_int_init(array);
  for(int i = 0; i < n; i++) {
    r_int_push_back(array, i);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_int_get(array, i);
  }
  
  r_int_clear(array);

  return s;
}

/* Same with type signed int.
   But with an explicit oplist.
   This generates exactly the same code than r_int
*/
ARRAY_DEF(r_int2, signed int, M_DEFAULT_OPLIST)

static int
test_int2(int n)
{
  r_int2_t array;

  r_int2_init(array);
  for(int i = 0; i < n; i++) {
    r_int2_push_back(array, i);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_int2_get(array, i);
  }
  
  r_int2_clear(array);

  return s;
}

/* Same with type unsigned long long */
ARRAY_DEF(r_ullong, unsigned long long)

static int
test_ullong(int n)
{
  r_ullong_t array;

  r_ullong_init(array);
  for(int i = 0; i < n; i++) {
    r_ullong_push_back(array, i);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_ullong_get(array, i);
  }
  
  r_ullong_clear(array);

  return s;
}

/* Same with type float */
ARRAY_DEF(r_float, float)

static int
test_float(int n)
{
  r_float_t array;

  r_float_init(array);
  for(int i = 0; i < n; i++) {
    r_float_push_back(array, i/4.0);
  }

  float s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_float_get(array, i);
  }
  
  r_float_clear(array);

  return s;
}

/* Same with type double and an explicit oplist*/
ARRAY_DEF(r_double, double, M_DEFAULT_OPLIST)

static int
test_double(int n)
{
  r_double_t array;

  r_double_init(array);
  for(int i = 0; i < n; i++) {
    r_double_push_back(array, i/4.0);
  }

  float s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_double_get(array, i);
  }
  
  r_double_clear(array);

  return s;
}

static char *my_strdup(const char *p)
{
  size_t s = strlen(p);
  char *d= malloc(s+1);
  if(!d) abort();
  strcpy(d, p);
  return d;
}

/* Same with type const char *, representing a contant string.
   We *** need *** an explicit oplist to tell M*LIB
   to consider the type as a constant string.
   The oplist to use is M_CSTR_OPLIST. */
ARRAY_DEF(r_cstring, const char *, M_CSTR_OPLIST)

static int
test_cstring(int n)
{
  r_cstring_t array;

  r_cstring_init(array);
  for(int i = 0; i < n; i++) {
    char buffer[10];
    // Create a string from the integer
    sprintf(buffer, "%d", i);
    // We need to make an explicit copy of the string
    // since it is not supported by the basic type.
    char *p = my_strdup(buffer);
    // Push the copy in the container.
    r_cstring_push_back(array, p);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get back the string
    const char *p = *r_cstring_get(array, i);
    // Convert it to integer
    int j = atoi(p);
    s += j;
  }

  // We also need a special unalloc pass
  // to free all constant strings we have allocated.
  for(int i = 0; i < n; i++) {
    const char *p = *r_cstring_get(array, i);
    free((void*)(uintptr_t)p);
  }
  
  r_cstring_clear(array);

  return s;
}

/* Same with type string_t of M*LIB, representing a variable string.
   We don't need explicit oplist to tell M*LIB!
   The oplist to use is STRING_OPLIST,
   but it has been registered globaly, so we don't need to explicit
   give it to array_def! */
ARRAY_DEF(r_string, string_t)

static int
test_string(int n)
{
  r_string_t array;
  string_t str;
  
  r_string_init(array);
  string_init (str);
  
  for(int i = 0; i < n; i++) {
    // create a string from an integer
    string_printf(str, "%d", i);
    // No need to make a copy in the container!
    r_string_push_back(array, str);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get back the string and convert it back to an integer
    int j = atoi(string_get_cstr(*r_string_get(array, i)));
    s += j;
  }

  // No need to explicit dealloc !
  string_clear(str);
  r_string_clear(array);

  return s;
}

/* Same with type 'volatile unsigned int *'
   The array stores pointers to another integer,
   allocated somewhere else.
   We need to tell M*LIB to handle the basic type
   as pointers without caring for what the value of
   the pointer is.
   So here, we **need** to specify the oplist of the type
   as M_PTR_OPLIST.
 */
ARRAY_DEF(r_vintptr, volatile unsigned int *, M_PTR_OPLIST)

static int
test_vintptr(int n)
{
  r_vintptr_t array;

  // Allocate a secondary table to store the value.
  unsigned *tab = calloc(n, sizeof(volatile unsigned int));
  if (!tab) abort();
  
  r_vintptr_init(array);
  for(int i = 0; i < n; i++) {
    tab[i] = i*i - i;
    // Push the pointer to this integer in the array
    r_vintptr_push_back(array, &tab[i]);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the integer.
    volatile unsigned *p = *r_vintptr_get(array, i);
    s += *p;
  }
  
  r_vintptr_clear(array);
  free(tab);
  
  return s;
}


/* Same with a structure type.
   We need to tell M*LIB to handle the basic type
   as POD;
   So here, we **need** to specify the oplist of the type
   as M_POD_OPLIST.
 */
struct rock_me_out {
  int n;
  float other;
};

ARRAY_DEF(r_rockme, struct rock_me_out, M_POD_OPLIST)

static int
test_rockme(int n)
{
  r_rockme_t array;

  // Allocate a secondary table to store the structure
  // for the purpose of this test.
  struct rock_me_out *tab = calloc(n, sizeof(struct rock_me_out));
  if (!tab) abort();
  
  r_rockme_init(array);
  for(int i = 0; i < n; i++) {
    tab[i].n = i*i - i;
    // Push the pointer to this integer in the array
    r_rockme_push_back(array, tab[i]);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the integer.
    struct rock_me_out *p = r_rockme_get(array, i);
    s += p->n;
  }
  
  r_rockme_clear(array);
  free(tab);
  
  return s;
}


/* Same with a structure type defined with [1]:
   We need to tell M*LIB to handle the basic type
   as a special array;
   So here, we **need** to specify the oplist of the type
   as M_A1_OPLIST.
 */
typedef struct rock_me_in {
  int n;
  float other;
} rock_me_in[1];

ARRAY_DEF(r_rockme2, rock_me_in, M_A1_OPLIST)

static int
test_rockme2(int n)
{
  r_rockme2_t array;

  // Allocate a secondary table to store the structure
  // for the purpose of this test.
  rock_me_in *tab = calloc(n, sizeof(struct rock_me_in));
  if (!tab) abort();
  
  r_rockme2_init(array);
  for(int i = 0; i < n; i++) {
    tab[i]->n = i*i - i;
    // Push the pointer to this integer in the array
    r_rockme2_push_back(array, tab[i]);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the integer.
    rock_me_in *p = r_rockme2_get(array, i);
    s += (*p)->n;
  }
  
  r_rockme2_clear(array);
  free(tab);
  
  return s;
}


int main (int argc, const char *argv[])
{
  int n = argc == 1 ? 10 : atoi(argv[1]);
  int s;

  s = test_bool(n);
  printf ("S[bool] = %d\n", s);

  s = test_char(n);
  printf ("S[char] = %d\n", s);

  s = test_int(n);
  printf ("S[int] = %d\n", s);

  s = test_int2(n);
  printf ("S[int2] = %d\n", s);

  s = test_ullong(n);
  printf ("S[ullong] = %d\n", s);

  s = test_float(n);
  printf ("S[float] = %d\n", s);

  s = test_double(n);
  printf ("S[double] = %d\n", s);

  s = test_cstring(n);
  printf ("S[cstring] = %d\n", s);

  s = test_string(n);
  printf ("S[string] = %d\n", s);

  s = test_vintptr(n);
  printf ("S[vintptr] = %d\n", s);

  s = test_rockme(n);
  printf ("S[rockme] = %d\n", s);

  s = test_rockme2(n);
  printf ("S[rockme2] = %d\n", s);

  exit(0);
}
