#include "m-array.h"
#include "m-string.h"

/* This example show how to use complex structure with array 
   embedding another library */

/* This is a trivial library */
typedef struct lib_ext_struct {
  int id;
  // Other data
} lib_ext_struct;

static lib_ext_struct *lib_ext_struct_Duplicate(const lib_ext_struct *obj)
{
  lib_ext_struct *p = malloc(sizeof(lib_ext_struct));
  if (!p) abort();
  p->id = obj->id;
  return p;
}

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

static void data_node_init(data_node *obj)
{
  obj->id = 0;
  string_init(obj->type);
  obj->properties = NULL;
}

static void data_node_init_set(data_node *obj, const data_node *src)
{
  obj->id = src->id;
  string_init_set(obj->type, src->type);
  if (src->properties)
    obj->properties = lib_ext_struct_Duplicate(src->properties);
  else
    obj->properties = NULL;
}

static void data_node_set(data_node *obj, const data_node *src)
{
  obj->id = src->id;
  string_set(obj->type, src->type);
  if (obj->properties)
    lib_ext_struct_Delete(obj->properties);    
  if (src->properties)
    obj->properties = lib_ext_struct_Duplicate(src->properties);
  else
    obj->properties = NULL;
}

static void data_node_clear(data_node *obj)
{
  string_clear(obj->type);
  if (obj->properties)
    lib_ext_struct_Delete(obj->properties);
}


ARRAY_DEF(array_data_node, data_node, (INIT(API_2(data_node_init)),SET(API_6(data_node_set)),INIT_SET(API_6(data_node_init_set)),CLEAR(API_2(data_node_clear))))

array_data_node_t global_array;

int main(void)
{
  array_data_node_init(global_array);
  array_data_node_clear(global_array);
}
