#include <stdio.h>
#include "m-variant.h"
#include "m-list.h"

// Define the variant with the given basic types
// A variant is an union of all its type, with the information needed to identify which type it holds
VARIANT_DEF2(obj,
             (boolean, bool, M_BOOL_OPLIST),
             (integer,  int,  M_BASIC_OPLIST),
             (real, double, M_BASIC_OPLIST),
             (cstr, const char*, M_CSTR_OPLIST))
// Associate obj_t to its oplist, so that we can only use 'obj_t'
// and M*LIB knows how to handle this type properly
// We need to extend the oplist with FIELD so that M*LIB is able to generate 'emplace' functions
#define M_OPL_obj_t()                                                   \
  VARIANT_OPLIST(obj, M_OPEXTEND(M_BOOL_OPLIST, FIELD(boolean)),        \
                 M_OPEXTEND(M_BASIC_OPLIST, FIELD(integer)),            \
                 M_OPEXTEND(M_BASIC_OPLIST, FIELD(real)),               \
                 M_OPEXTEND(M_CSTR_OPLIST, FIELD(cstr)))

// Define a list over the variant
LIST_DEF(list, obj_t)
// Associate list_t to its oplist, so that we can only use 'list_t'
#define M_OPL_list_t() LIST_OPLIST(list, M_OPL_obj_t())

/* Print the type of the object */
static void print_obj(obj_t o, const char name[])
{
  if (obj_empty_p(o)) {
    printf("Object %s is empty\n", name);
  }
  if (obj_boolean_p(o)) {
    printf("Object %s is a boolean (%d)\n", name, *obj_get_boolean(o));
  }
  if (obj_integer_p(o)) {
    printf("Object %s is an integer (%d)\n", name, *obj_get_integer(o));
  }
  if (obj_real_p(o)) {
    printf("Object %s is a float (%f)\n", name, *obj_get_real(o));
  }
  if (obj_cstr_p(o)) {
    printf("Object %s is C string (%s)\n", name, *obj_get_cstr(o));
  }
}


static void f(void)
{
  // Initialize & afterward clear the object
  M_LET(o, obj_t) {
    print_obj(o, "1st init");
    obj_set_real(o, 23.4);
    print_obj(o, "1st+set");
  } // Now the object is out of scope and is cleared.

  // Initialize, set as an integer of value '12' & afterward clear the object
  // () around 12 means that we want to 'emplace' object using the input '12' to construct the object in-place,
  // rather than setting it from an already existing 'obj_t'.
  // It checks for the type of 12, find it is an integer, and initialize o with this value
  // It won't work if there is multiple compatible type in the variant
  M_LET( (o, (12)), obj_t) {
    print_obj(o, "2nd init");
    obj_set_cstr(o, "Hello");
    print_obj(o, "2nd+set");
  }

  M_LET( (o, (true)), obj_t) {
    // In C11, a 'true' value is an 'int' type, not a bool!
    print_obj(o, "3rd init");
    obj_set_boolean(o, true);
    print_obj(o, "3rd+set");
    // Without "()" around the source, it will set the value using the object builtin copy.
    M_LET( (p, o), obj_t) {
      print_obj(o, "3rd+rec");
      // We can also print the object directly:
      printf("OBJ=");
      obj_out_str(stdout, o);
      printf("\n");
    }
  }
}

static void g(void)
{
  // Initialize a list of obj_t and clear it afterwards
  M_LET(l, list_t){
    // Let's emplace some type in the list
    list_emplace_back_boolean(l, true);
    list_emplace_back_integer(l, 12);
    list_emplace_back_cstr(l, "World");
    list_emplace_back_real(l, 17.42);
    // We can iterate on the list
    for M_EACH(item, l, list_t) {
        print_obj(*item, "loop");
    }
    // We can also print the object directly:
    printf("LIST=");
    list_out_str(stdout, l);
    printf("\n");
  } // Clear & free the list
}

int main(void)
{
  f();
  g();
  exit(0);
}
