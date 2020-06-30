/*
 * M*LIB - Serial JSON
 *
 * Copyright (c) 2017-2020, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef MSTARLIB_SERIAL_JSON_H
#define MSTARLIB_SERIAL_JSON_H

#include "m-core.h"
#include "m-string.h"

/* Write the boolean 'data' into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_json_write_boolean(m_serial_write_t serial, const bool data)
{
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "%s", data ? "true" : "false");
  return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Write the integer 'data' of 'size_of_type' bytes into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_json_write_integer(m_serial_write_t serial,const long long data, const size_t size_of_type)
{
  (void) size_of_type; // Ignored
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "%lld", data);
  return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();  
}

/* Write the float 'data' of 'size_of_type' bytes into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_json_write_float(m_serial_write_t serial, const long double data, const size_t size_of_type)
{
  (void) size_of_type; // Ignored
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "%Lf", data);
  return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Write the null-terminated string 'data'into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_json_write_string(m_serial_write_t serial, const char data[], size_t length)
{
  STRINGI_ASSUME(length == strlen(data) );
  FILE *f = (FILE *)serial->data[0].p;
  assert(f != NULL && data != NULL);
  /* HACK: Build dummy string to reuse string_out_str */
  string_t v2;
  uintptr_t ptr = (uintptr_t) data;
  v2->u.heap.size = length;
  v2->u.heap.alloc = length + 1;
  v2->ptr = (char*)ptr;
  string_out_str(f, v2);
  return M_SERIAL_OK_DONE;
}

/* Start writing an array of 'number_of_elements' objects into the serial stream 'serial'.
   If 'number_of_elements' is 0, then either the array has no data,
   or the number of elements of the array is unkown.
   Initialize 'local' so that it can be used to serialize the array 
   (local is an unique serialization object of the array).
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_json_write_array_start(m_serial_local_t local, m_serial_write_t serial, const size_t number_of_elements)
{
  (void) local; // argument not used
  (void) number_of_elements; // Ignored
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "[");
  return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* Write an array separator between elements of an array into the serial stream 'serial' if needed.
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_write_array_next(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, ",");
  return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* End the writing of an array into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_array_end(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "]");
  return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Start writing a map of 'number_of_elements' pairs of objects into the serial stream 'serial'.
   If 'number_of_elements' is 0, then either the map has no data,
   or the number of elements is unkown.
   Initialize 'local' so that it can be used to serialize the map 
   (local is an unique serialization object of the map).
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_map_start(m_serial_local_t local, m_serial_write_t serial, const size_t number_of_elements)
{
  (void) local; // argument not used
  (void) number_of_elements; // Ignored
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "{");
  return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* Write a value separator between element of the same pair of a map into the serial stream 'serial' if needed.
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_map_value(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, ":");
  return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* Write a map separator between elements of a map into the serial stream 'serial' if needed.
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_map_next(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, ",");
  return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* End the writing of a map into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_map_end(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "}");
  return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Start writing a tuple into the serial stream 'serial'.
   Initialize 'local' so that it can serial the tuple 
   (local is an unique serialization object of the tuple).
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_tuple_start(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "{");
  return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* Start writing the field named field_name[index] of a tuple into the serial stream 'serial'.
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_tuple_id(m_serial_local_t local, m_serial_write_t serial, const char *const field_name[], const int max, const int index)
{
  (void) local; // argument not used
  (void) max; // Ignored
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "%c\"%s\":", index == 0 ? ' ' : ',', field_name[index]);
  return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* End the write of a tuple into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_tuple_end(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "}");
  return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Start writing a variant into the serial stream 'serial'.
   If index <= 0, the variant is empty.
     Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise 
   Otherwise, the field 'field_name[index]' will be filled.
     Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_variant_start(m_serial_local_t local, m_serial_write_t serial, const char *const field_name[], const int max, const int index)
{
  (void) local; // argument not used
  (void) max; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n;
  if (index >= 0) {
    assert (index < max);
    n = fprintf(f, "{\"%s\":", field_name[index]);
    return n > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
  } else {
    n = fprintf(f, "{}");
    return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
  }
}

/* End Writing a variant into the serial stream 'serial'. 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_json_write_variant_end(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE *)serial->data[0].p;
  int n = fprintf(f, "}");
  return n > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* The internal exported interface of m_serial_write_json. */
static const m_serial_write_interface_t m_serial_write_json_interface = {
  m_serial_json_write_boolean,
  m_serial_json_write_integer,
  m_serial_json_write_float,
  m_serial_json_write_string, 
  m_serial_json_write_array_start,
  m_serial_json_write_array_next,
  m_serial_json_write_array_end,
  m_serial_json_write_map_start,
  m_serial_json_write_map_value,
  m_serial_json_write_map_next,
  m_serial_json_write_map_end,
  m_serial_json_write_tuple_start,
  m_serial_json_write_tuple_id,
  m_serial_json_write_tuple_end,
  m_serial_json_write_variant_start,
  m_serial_json_write_variant_end
};

/* Initialize the JSON serial object for writing any object to JSON format in the given FILE */
static inline void m_serial_json_write_init(m_serial_write_t serial, FILE *f)
{
  serial->m_interface = &m_serial_write_json_interface;
  serial->data[0].p = M_ASSIGN_CAST(void*, f);
}

/* CLear the JSON serial object for writing*/
static inline void m_serial_json_write_clear(m_serial_write_t serial)
{
  (void) serial; // Nothing to do
}

/* Define a synonym to the JSON serializer with a proper OPLIST */
typedef m_serial_write_t m_serial_json_write_t;
#define M_OPL_m_serial_json_write_t()                                   \
  (INIT_WITH(m_serial_json_write_init), CLEAR(m_serial_json_write_clear), TYPE(m_serial_json_write_t) )



/* Helper function to skip a space */
static inline int
m_serial_json_read_skip (FILE *f)
{
  int c;
  do {
    c = fgetc(f);
  } while (isspace(c));
  return c;
}

/* Read from the stream 'serial' a boolean.
   Set '*b' with the boolean value if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_boolean(m_serial_read_t serial, bool *b){
  FILE *f = (FILE*) serial->data[0].p;
  int c = m_serial_json_read_skip(f);
  if (c == 't') {
    *b = true;
    c = fgetc(f);
    if (c != 'r') return m_core_serial_fail();
    c = fgetc(f);
    if (c != 'u') return m_core_serial_fail();
    c = fgetc(f);
    if (c != 'e') return m_core_serial_fail();
    return M_SERIAL_OK_DONE;
  } else if (c == 'f') {
    *b = false;
    c = fgetc(f);
    if (c != 'a') return m_core_serial_fail();
    c = fgetc(f);
    if (c != 'l') return m_core_serial_fail();
    c = fgetc(f);
    if (c != 's') return m_core_serial_fail();
    c = fgetc(f);
    if (c != 'e') return m_core_serial_fail();
    return M_SERIAL_OK_DONE;
  } else {
    return m_core_serial_fail();
  }
}

/* Read from the stream 'serial' an integer that can be represented with 'size_of_type' bytes.
   Set '*i' with the integer value if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_integer(m_serial_read_t serial, long long *i, const size_t size_of_type){
  (void) size_of_type; // Ignored
  FILE *f = (FILE*) serial->data[0].p;
  return m_core_fscanf(f, " %lld", i) == 1 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Read from the stream 'serial' a float that can be represented with 'size_of_type' bytes.
   Set '*r' with the boolean value if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_float(m_serial_read_t serial, long double *r, const size_t size_of_type){
  (void) size_of_type; // Ignored
  FILE *f = (FILE*) serial->data[0].p;
  return m_core_fscanf(f, " %Lf", r) == 1 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Read from the stream 'serial' a string.
   Set 's' with the string if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_string(m_serial_read_t serial, struct string_s *s){
  FILE *f = (FILE*) serial->data[0].p;
  m_core_fscanf(f, " "); // Skip any leading spaces.
  return string_in_str(s, f) ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Start reading from the stream 'serial' an array.
   Set '*num' with the number of elements, or 0 if it is not known.
   Initialize 'local' so that it can be used to serialize the array 
   (local is an unique serialization object of the array).
   Return M_SERIAL_OK_CONTINUE if it succeeds and the array continue,
   M_SERIAL_OK_DONE if it succeeds and the array ends (the array is empty),
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_array_start(m_serial_local_t local, m_serial_read_t serial, size_t *num)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  int final1 = -1, final2 = -1;
  m_core_fscanf(f, " [%n ]%n", &final1, &final2);
  *num = 0; // don't know the size of the array.
  // Test how much the parsing succeed.
  if (final1 < 0) m_core_serial_fail();
  return (final2 < 0) ? M_SERIAL_OK_CONTINUE : M_SERIAL_OK_DONE;
}

/* Continue reading from the stream 'serial' an array.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the array continue,
   M_SERIAL_OK_DONE if it succeeds and the array ends,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_array_next(m_serial_local_t local, m_serial_read_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  char c = 0;
  m_core_fscanf(f, " %c", m_core_arg_size(&c, 1) );
  return c == ',' ? M_SERIAL_OK_CONTINUE : c == ']' ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Start reading from the stream 'serial' a map.
   Set '*num' with the number of elements, or 0 if it is not known.
   Initialize 'local' so that it can be used to serialize the array 
   (local is an unique serialization object of the array).
   Return M_SERIAL_OK_CONTINUE if it succeeds and the map continue,
   M_SERIAL_OK_DONE if it succeeds and the map ends (the map is empty),
   m_core_serial_fail() otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_map_start(m_serial_local_t local, m_serial_read_t serial, size_t *num)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  int final1 = -1, final2 = -1;
  m_core_fscanf(f, " {%n }%n", &final1, &final2);
  *num = 0; // don't know the size of the map.
  // Test how much the parsing succeed.
  if (final1 < 0) m_core_serial_fail();
  return (final2 < 0) ? M_SERIAL_OK_CONTINUE : M_SERIAL_OK_DONE;
}

/* Continue reading from the stream 'serial' the value separator
   Return M_SERIAL_OK_CONTINUE if it succeeds and the map continue,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_map_value(m_serial_local_t local, m_serial_read_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  int final = -1;
  m_core_fscanf(f, " :%n", &final);
  return final > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* Continue reading from the stream 'serial' a map.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the map continue,
   M_SERIAL_OK_DONE if it succeeds and the map ends,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_map_next(m_serial_local_t local, m_serial_read_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  char c = 0;
  m_core_fscanf(f, " %c", m_core_arg_size(&c, 1) );
  return c == ',' ? M_SERIAL_OK_CONTINUE : c == '}' ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

/* Start reading a tuple from the stream 'serial'.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the tuple continues,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_tuple_start(m_serial_local_t local, m_serial_read_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  int final = -1;
  m_core_fscanf(f, " {%n", &final);
  return final > 0 ? M_SERIAL_OK_CONTINUE : m_core_serial_fail();
}

/* Continue reading a tuple from the stream 'serial'.
   Set '*id' with the corresponding index of the table 'field_name[max]'
   associated to the parsed field in the stream.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the tuple continues,
   Return M_SERIAL_OK_DONE if it succeeds and the tuple ends,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_tuple_id(m_serial_local_t local, m_serial_read_t serial, const char *const field_name [], const int max, int *id)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  int c = m_serial_json_read_skip(f);
  if (c == EOF) return m_core_serial_fail();
  if (c == '}') return M_SERIAL_OK_DONE;
  if (c == ',') {
     // If first call of read_tuple_id, it is a failure
     if (*id == -1) return m_core_serial_fail();
  } else {
    // c should be \" but let fscanf parse it.
    ungetc(c, f);
  }
  /* Read the field in the JSON */
  c = -1;
  char field[M_MAX_IDENTIFIER_LENGTH+1];
  m_core_fscanf(f, " \"%" M_APPLY(M_AS_STR, M_MAX_IDENTIFIER_LENGTH) "[^ \t\n\"]\":%n",
                m_core_arg_size(field, M_MAX_IDENTIFIER_LENGTH+1), &c);
  if (c <= 0)
    return m_core_serial_fail();
  /* Search for field in field_name */
  for(int n = 0; n < max; n++) {
    if (strcmp(field, field_name[n]) == 0) {
      *id = n;
      return M_SERIAL_OK_CONTINUE;
    }
  }
  return m_core_serial_fail();
}

/* Start reading a variant from the stream 'serial'.
   Set '*id' with the corresponding index of the table 'field_name[max]'
   associated to the parsed field in the stream.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the variant continues,
   Return M_SERIAL_OK_DONE if it succeeds and the variant ends(variant is empty),
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_variant_start(m_serial_local_t local, m_serial_read_t serial, const char *const field_name[], const int max, int*id)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;

  int final1 = -1, final2 = -1;
  m_core_fscanf(f, " {%n }%n", &final1, &final2);
  // Test how much the parsing succeed.
  if (final1 <= 0) return m_core_serial_fail();
  if (final2 > 0)  return M_SERIAL_OK_DONE;

  /* Read the field in the JSON */
  char field[M_MAX_IDENTIFIER_LENGTH+1];
  m_core_fscanf(f, " \"%" M_APPLY(M_AS_STR, M_MAX_IDENTIFIER_LENGTH) "[^ \t\n\"]\":%n", 
                m_core_arg_size(field, M_MAX_IDENTIFIER_LENGTH+1), &final2);
  if (final2 <= 0) return m_core_serial_fail();

  /* Linear Search for field in field_name */
  for(int n = 0; n < max; n++) {
    if (strcmp(field, field_name[n]) == 0) {
      *id = n;
      return M_SERIAL_OK_CONTINUE;
    }
  }
  // Field not found
  return m_core_serial_fail();
}

/* End reading a variant from the stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds and the variant ends,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_json_read_variant_end(m_serial_local_t local, m_serial_read_t serial)
{
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  int final = -1;
  m_core_fscanf(f, " }%n", &final);
  return final > 0 ? M_SERIAL_OK_DONE : m_core_serial_fail();
}

static const m_serial_read_interface_t m_serial_json_read_interface = {
  m_serial_json_read_boolean,
  m_serial_json_read_integer,
  m_serial_json_read_float,
  m_serial_json_read_string,
  m_serial_json_read_array_start,
  m_serial_json_read_array_next,
  m_serial_json_read_map_start,
  m_serial_json_read_map_value,
  m_serial_json_read_map_next,
  m_serial_json_read_tuple_start,
  m_serial_json_read_tuple_id,
  m_serial_json_read_variant_start,
  m_serial_json_read_variant_end
};

/* Initialize the JSON serial object for reading any object from JSON format in the given FILE */
static inline void m_serial_json_read_init(m_serial_read_t serial, FILE *f)
{
  serial->m_interface = &m_serial_json_read_interface;
  serial->data[0].p = M_ASSIGN_CAST(void*, f);
}

/* Clear the JSON serial object for reading from the FILE */
static inline void m_serial_json_read_clear(m_serial_read_t serial)
{
  (void) serial; // Nothing to do
}

/* Define a synonym of m_serial_read_t 
  to the JSON serializer with its proper OPLIST */
typedef m_serial_read_t m_serial_json_read_t;
#define M_OPL_m_serial_json_read_t()                                    \
  (INIT_WITH(m_serial_json_read_init), CLEAR(m_serial_json_read_clear), TYPE(m_serial_json_read_t) )

#endif
