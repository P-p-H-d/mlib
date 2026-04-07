#include "m-array.h"
#include "m-string.h"

/* This example show how to use complex structure with array 
   embedding another library, showing how to handle complex objects
   and create custom oplists using API adaptator */

/* This is a typical trivial library with its interface */
typedef struct lib_ext_struct {
  int id;
  // Other data may be added
} lib_ext_struct;

/* Method to create a duplicate of the object.
   The new object is heap allocated */
static lib_ext_struct *lib_ext_struct_Duplicate(const lib_ext_struct *obj)
{
  assert(o);
  lib_ext_struct *p = malloc(sizeof(lib_ext_struct));
  if (!p) abort();
  p->id = obj->id;
  return p;
}

/* Method to delete any object */
static void lib_ext_struct_Delete(lib_ext_struct *obj)
{
  free(obj);
}

/* This is the complex structure */
typedef struct  {
  uint32_t id;
  string_t type;
  lib_ext_struct* properties;
} data_node;

/* This method shall initialize the object data_node to
   the init state.
   It takes as first argument the object to initialize,
   and since it takes by reference through a pointer,
   it is compatible with 'API_2' convention for oplist */
static void data_node_init(data_node *obj)
{
  assert(obj);
  obj->id = 0;
  string_init(obj->type);
  obj->properties = NULL;
}

/* This method shall initialize the object data_node to
   the same state as the 'src' object (creating a copy, a duplicate) .
   It takes as first argument the object to initialize,
   and as second argument the source object.
   It takes both object by reference through pointers,
   as such it is compatible with 'API_6' convention for oplist */
static void data_node_init_set(data_node *obj, const data_node *src)
{
  assert(obj);
  assert(src);
  obj->id = src->id;
  string_init_set(obj->type, src->type);
  if (src->properties)
    obj->properties = lib_ext_struct_Duplicate(src->properties);
  else
    obj->properties = NULL;
}

/* This method shall set the object data_node to
   the same state as the 'src' object (creating a copy, a duplicate) .
   THe object was already initialized.
   It takes as first argument the object to initialize,
   and as second argument the source object.
   It takes both object by reference through pointers,
   as such it is compatible with 'API_6' convention for oplist */
static void data_node_set(data_node *obj, const data_node *src)
{
  assert(obj);
  assert(src);
  obj->id = src->id;
  string_set(obj->type, src->type);
  if (obj->properties)
    lib_ext_struct_Delete(obj->properties);    
  if (src->properties)
    obj->properties = lib_ext_struct_Duplicate(src->properties);
  else
    obj->properties = NULL;
}

/* This method shall clear the object data_node to
   the cleared state (aka it shall free any resource).
   It takes as first argument the object to initialize,
   and since it takes by reference through a pointer,
   it is compatible with 'API_2' convention for oplist */
static void data_node_clear(data_node *obj)
{
  assert(obj);
  string_clear(obj->type);
  if (obj->properties)
    lib_ext_struct_Delete(obj->properties);
}

/*
 How an oplist is defined?
 An oplist is an ordered map that associates a method to an operator (the name of the method in the API convention).
 It starts with '(' and ends with ')', and each method is defined as 'METHOD_NAME(API_X(method))' where:
   - METHOD_NAME is the name of the method in the API convention (e.g. INIT, CLEAR, SET, etc.)
   - API_X is the API convention that the method follows (e.g. API_0, API_2, API_6, etc.)
   - method is the name of the function that implements the method.
 Each item is separated by a comma.
 If it uses API_0, it can be omitted (it means there is no adaptation needed, the method is directly compatible with the API convention). 

 API_x are standard API conventions defined in the Manual, that specify how the method takes its arguments (by value, by reference, etc.) and how it returns (void, a new object, etc.).
  - API_0: no adaptation (can be omitted)
  - API_2: transforms the first argument to be passed by pointer, ie. the destination object to operate on
  - API_6: transforms the two first arguments to be passed by pointer, ie. the destination object and the source object.
  - etc.
 There is also the generic API adaptation layer, but it will be explained in another example, and is not needed here since we can directly use the standard API conventions.

 If there is no method for a given operator, it can be omitted, and M*LIB will use a default behavior if possible (or raises a compiler error if it is not possible).
 If there are two (or more) methods for a given operator, the first one will be used (the latests ones are ignored), 
 ensuring inheritance of methods if the oplist is defined as an extension of another oplist (see the Manual for more details on this point).
 */

/* Create a dynamic array of 'data_node' named array_data_node_t
   and gives the minimum oplist allowing M*LIB to handle
   properly the object 'data_node' with the API convention
   to respect for calling the methods.
   The Manual defines the different supported API.
*/
ARRAY_DEF(/* the array prefix */ array_data_node,
	  /* the object type */ data_node,
	  /* the object oplist */ (INIT(API_2(data_node_init)),
				   SET(API_6(data_node_set)),
				   INIT_SET(API_6(data_node_init_set)),
				   CLEAR(API_2(data_node_clear))))

/* Define a global array with the created type */
array_data_node_t global_array;

int main(void)
{
  /* Call the created method */
  array_data_node_init(global_array);
  /* Do stuff... */
  array_data_node_clear(global_array);
}
