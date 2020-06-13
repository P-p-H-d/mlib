/*
 * M*LIB - Serial BIN
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
#ifndef MSTARLIB_SERIAL_BIN_H
#define MSTARLIB_SERIAL_BIN_H

#include <stdint.h>

#include "m-core.h"
#include "m-string.h"

/* Write the boolean 'data' into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_bin_write_boolean(m_serial_write_t serial, const bool data)
{
  FILE *f = (FILE *)serial->data[0].p;
  size_t n = fwrite (M_ASSIGN_CAST(const void*, &data), sizeof (bool), 1, f);
  return n == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Write the integer 'data' of 'size_of_type' bytes into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_bin_write_integer(m_serial_write_t serial,const long long data, const size_t size_of_type)
{
  int8_t   i8;
  int16_t i16;
  int32_t i32;
  int64_t i64;
  size_t n;
  
  FILE *f = (FILE *)serial->data[0].p;
  switch (size_of_type) {
  case 1:
    i8 = data;
    n = fwrite (M_ASSIGN_CAST(const void*, &i8), sizeof i8, 1, f);
    break;
  case 2:
    i16 = data;
    n = fwrite (M_ASSIGN_CAST(const void*, &i16), sizeof i16, 1, f);
    break;
  case 4:
    i32 = data;
    n = fwrite (M_ASSIGN_CAST(const void*, &i32), sizeof i32, 1, f);
    break;
  case 8:
    i64 = data;
    n = fwrite (M_ASSIGN_CAST(const void*, &i64), sizeof i64, 1, f);
    break;
  default:
    M_ASSERT_INIT(false, "an integer of suitable size");
    break;
  }
  return n == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Write the float 'data' of 'size_of_type' bytes into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_bin_write_float(m_serial_write_t serial, const long double data, const size_t size_of_type)
{
  float   f1;
  double  f2;
  long double f3;
  size_t n;
  
  FILE *f = (FILE *)serial->data[0].p;
  if (size_of_type == sizeof f1) {
    f1 = data;
    n = fwrite (M_ASSIGN_CAST(const void*, &f1), sizeof f1, 1, f);
  } else if (size_of_type == sizeof f2) {
    f2 = data;
    n = fwrite (M_ASSIGN_CAST(const void*, &f2), sizeof f2, 1, f);
  } else if (size_of_type == sizeof f3) {
    f3 = data;
    n = fwrite (M_ASSIGN_CAST(const void*, &f3), sizeof f3, 1, f);
  } else {
    M_ASSERT_INIT(false, "a float of suitable size");
  }
  return n == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Write the null-terminated string 'data'into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_bin_write_string(m_serial_write_t serial, const char data[])
{
  FILE *f = (FILE *)serial->data[0].p;
  size_t l = strlen(data);
  size_t n = fwrite (M_ASSIGN_CAST(const void*, data), 1, l+1, f);
  return n == l+1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Start writing an array of 'number_of_elements' objects into the serial stream 'serial'.
   If 'number_of_elements' is 0, then either the array has no data,
   or the number of elements of the array is unkown.
   Initialize 'local' so that it can be used to serialize the array 
   (local is an unique serialization object of the array).
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline m_serial_return_code_t
m_serial_bin_write_array_start(m_serial_local_t local, m_serial_write_t serial, const size_t number_of_elements)
{
  FILE *f = (FILE *)serial->data[0].p;
  size_t n = fwrite (M_ASSIGN_CAST(const void*, &number_of_elements), sizeof number_of_elements, 1, f);
  local->data[0].b = (number_of_elements == 0);
  return n == 1 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

/* Write an array separator between elements of an array into the serial stream 'serial' if needed.
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_write_array_next(m_serial_local_t local, m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0].p;
  // Need separator if we don't know the real size 
  if (local->data[0].b) {
    size_t n = 0xABCDEF;
    n = fwrite (M_ASSIGN_CAST(const void*, &n), sizeof n, 1, f);
    return n == 1 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;    
  } else {
    return M_SERIAL_OK_CONTINUE;
  }
}

/* End the writing of an array into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_bin_write_array_end(m_serial_local_t local, m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0].p;
  // Need mark if we don't know the real size 
  if (local->data[0].b) {
    size_t n = 0x12345678;
    n = fwrite (M_ASSIGN_CAST(const void*, &n), sizeof n, 1, f);
    return n == 1 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;    
  } else {
    return M_SERIAL_OK_CONTINUE;
  }
}

/* Write a value separator between element of the same pair of a map into the serial stream 'serial' if needed.
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_bin_write_map_value(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  (void) serial;
  return M_SERIAL_OK_CONTINUE;
}

/* Start writing a tuple into the serial stream 'serial'.
   Initialize 'local' so that it can serial the tuple 
   (local is an unique serialization object of the tuple).
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_bin_write_tuple_start(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  (void) serial;
  return M_SERIAL_OK_CONTINUE;
}

/* Start writing the field named field_name[index] of a tuple into the serial stream 'serial'.
   Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_bin_write_tuple_id(m_serial_local_t local, m_serial_write_t serial, const char *const field_name[], const int max, const int index)
{
  (void) local; // argument not used
  (void) serial;
  (void) field_name;
  (void) max;
  (void) index; // Assume index are write in order from 0 to max.
  return M_SERIAL_OK_CONTINUE;
}

/* End the write of a tuple into the serial stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_bin_write_tuple_end(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  (void) serial;
  return M_SERIAL_OK_DONE;
}

/* Start writing a variant into the serial stream 'serial'.
   If index <= 0, the variant is empty.
     Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise 
   Otherwise, the field 'field_name[index]' will be filled.
     Return M_SERIAL_OK_CONTINUE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_bin_write_variant_start(m_serial_local_t local, m_serial_write_t serial, const char *const field_name[], const int max, const int index)
{
  (void) field_name;
  (void) max;
  (void) local;
  FILE *f = (FILE *)serial->data[0].p;
  size_t n = fwrite (M_ASSIGN_CAST(const void*, &index), sizeof index, 1, f);
  return n == 1 ? ((index < 0) ? M_SERIAL_OK_DONE : M_SERIAL_OK_CONTINUE) : M_SERIAL_FAIL;
}

/* End Writing a variant into the serial stream 'serial'. 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline   m_serial_return_code_t
m_serial_bin_write_variant_end(m_serial_local_t local, m_serial_write_t serial)
{
  (void) local; // argument not used
  (void) serial;
  return M_SERIAL_OK_DONE;
}

/* The exported interface. */
static const m_serial_write_interface_t m_serial_write_bin_interface = {
  m_serial_bin_write_boolean,
  m_serial_bin_write_integer,
  m_serial_bin_write_float,
  m_serial_bin_write_string, 
  m_serial_bin_write_array_start,
  m_serial_bin_write_array_next,
  m_serial_bin_write_array_end,
  m_serial_bin_write_array_start,
  m_serial_bin_write_map_value,
  m_serial_bin_write_array_next,
  m_serial_bin_write_array_end,
  m_serial_bin_write_tuple_start,
  m_serial_bin_write_tuple_id,
  m_serial_bin_write_tuple_end,
  m_serial_bin_write_variant_start,
  m_serial_bin_write_variant_end
};

static inline void m_serial_bin_write_init(m_serial_write_t serial, FILE *f)
{
  serial->m_interface = &m_serial_write_bin_interface;
  serial->data[0].p = M_ASSIGN_CAST(void*, f);
}

static inline void m_serial_bin_write_clear(m_serial_write_t serial)
{
  (void) serial; // Nothing to do
}


/* Define a synonym of m_serial_read_t to the BIN serializer with its proper OPLIST */
typedef m_serial_write_t m_serial_bin_write_t;
#define M_OPL_m_serial_bin_write_t()                                                   \
  (INIT_WITH(m_serial_bin_write_init), CLEAR(m_serial_bin_write_clear), TYPE(m_serial_bin_write_t) )


/* Read from the stream 'serial' a boolean.
   Set '*b' with the boolean value if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_boolean(m_serial_read_t serial, bool *b){
  FILE *f = (FILE*) serial->data[0].p;
  size_t n = fread (M_ASSIGN_CAST(void*, b), sizeof (bool), 1, f);
  return n == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Read from the stream 'serial' an integer that can be represented with 'size_of_type' bytes.
   Set '*i' with the integer value if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_integer(m_serial_read_t serial, long long *i, const size_t size_of_type){
  int8_t   i8;
  int16_t i16;
  int32_t i32;
  int64_t i64;
  size_t n;
  FILE *f = (FILE *)serial->data[0].p;
  switch (size_of_type) {
  case 1:
    n = fread (M_ASSIGN_CAST(void*, &i8), sizeof i8, 1, f);
    *i = i8;
    break;
  case 2:
    n = fread (M_ASSIGN_CAST(void*, &i16), sizeof i16, 1, f);
    *i = i16;
    break;
  case 4:
    n = fread (M_ASSIGN_CAST(void*, &i32), sizeof i32, 1, f);
    *i = i32;
    break;
  case 8:
    n = fread (M_ASSIGN_CAST(void*, &i64), sizeof i64, 1, f);
    *i = i64;
    break;
  default:
    M_ASSERT_INIT(false, "an integer of suitable size");
    break;
  }
  return n == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Read from the stream 'serial' a float that can be represented with 'size_of_type' bytes.
   Set '*r' with the boolean value if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_float(m_serial_read_t serial, long double *r, const size_t size_of_type){
  float   f1;
  double  f2;
  long double f3;
  size_t n;
  FILE *f = (FILE *)serial->data[0].p;
  if (size_of_type == sizeof f1) {
    n = fread (M_ASSIGN_CAST(void*, &f1), sizeof f1, 1, f);
    *r = f1;
  } else if (size_of_type == sizeof f2) {
    n = fread (M_ASSIGN_CAST(void*, &f2), sizeof f2, 1, f);
    *r = f2;
  } else if (size_of_type == sizeof f3) {
    n = fread (M_ASSIGN_CAST(void*, &f3), sizeof f3, 1, f);
    *r = f3;
  } else {
    M_ASSERT_INIT(false, "a float of suitable size");
  }
  return n == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Read from the stream 'serial' a string.
   Set 's' with the string if succeeds 
   Return M_SERIAL_OK_DONE if it succeeds, M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_string(m_serial_read_t serial, struct string_s *s){
  FILE *f = (FILE*) serial->data[0].p;
  string_clean(s);
  int c;
  c = fgetc(f);
  while (c != 0 && c != EOF) {
    string_push_back(s, c);
    c = fgetc(f);
  }
  return c != EOF ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

/* Start reading from the stream 'serial' an array.
   Set '*num' with the number of elements, or 0 if it is not known.
   Initialize 'local' so that it can be used to serialize the array 
   (local is an unique serialization object of the array).
   Return M_SERIAL_OK_CONTINUE if it succeeds and the array continue,
   M_SERIAL_OK_DONE if it succeeds and the array ends (the array is empty),
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_array_start(m_serial_local_t local, m_serial_read_t serial, size_t *num)
{
  FILE *f = (FILE*) serial->data[0].p;
  size_t n = fread (M_ASSIGN_CAST(void*, num), sizeof *num, 1, f);
  if (n != 1)
    return M_SERIAL_FAIL;
  local->data[0].b = (*num == 0);
  local->data[1].s = *num;
  if (local->data[0].b) {
    // Size not know ==> use of marker in the stream.
    size_t p;
    n = fread (M_ASSIGN_CAST(void*, &p), sizeof p, 1, f);
    if (n != 1)
      return M_SERIAL_FAIL;
    return p == 0xABCDEF ? M_SERIAL_OK_CONTINUE : p == 0x12345678 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
  }
  return M_SERIAL_OK_CONTINUE;
}

/* Continue reading from the stream 'serial' an array.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the array continue,
   M_SERIAL_OK_DONE if it succeeds and the array ends,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_array_next(m_serial_local_t local, m_serial_read_t serial)
{
  FILE *f = (FILE*) serial->data[0].p;
  if (local->data[0].b) {
    // Size not know ==> use of marker in the stream.
    size_t p, n;
    n = fread (M_ASSIGN_CAST(void*, &p), sizeof p, 1, f);
    if (n != 1)
      return M_SERIAL_FAIL;
    return p == 0xABCDEF ? M_SERIAL_OK_CONTINUE : p == 0x12345678 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
  } else {
    assert(local->data[1].s > 0);
    local->data[1].s --;
    return local->data[1].s == 0 ? M_SERIAL_OK_DONE : M_SERIAL_OK_CONTINUE;
  }
}


/* Continue reading from the stream 'serial' the value separator
   Return M_SERIAL_OK_CONTINUE if it succeeds and the map continue,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_map_value(m_serial_local_t local, m_serial_read_t serial)
{
  (void) local; // argument not used
  (void) serial;
  return M_SERIAL_OK_CONTINUE;
}

/* Start reading a tuple from the stream 'serial'.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the tuple continues,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_tuple_start(m_serial_local_t local, m_serial_read_t serial)
{
  (void) serial;
  local->data[1].s = 0;
  return M_SERIAL_OK_CONTINUE;
}

/* Continue reading a tuple from the stream 'serial'.
   Set '*id' with the corresponding index of the table 'field_name[max]'
   associated to the parsed field in the stream.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the tuple continues,
   Return M_SERIAL_OK_DONE if it succeeds and the tuple ends,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_tuple_id(m_serial_local_t local, m_serial_read_t serial, const char *const field_name [], const int max, int *id)
{
  (void) serial;
  (void) field_name;
  (void) max;
  *id = local->data[1].s;
  local->data[1].s ++;
  return (*id == max) ? M_SERIAL_OK_DONE : M_SERIAL_OK_CONTINUE;
}

/* Start reading a variant from the stream 'serial'.
   Set '*id' with the corresponding index of the table 'field_name[max]'
   associated to the parsed field in the stream.
   Return M_SERIAL_OK_CONTINUE if it succeeds and the variant continues,
   Return M_SERIAL_OK_DONE if it succeeds and the variant ends(variant is empty),
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_variant_start(m_serial_local_t local, m_serial_read_t serial, const char *const field_name[], const int max, int*id)
{
  (void) field_name;
  (void) max;
  (void) local; // argument not used
  FILE *f = (FILE*) serial->data[0].p;
  size_t n = fread (M_ASSIGN_CAST(void*, id), sizeof *id, 1, f);
  return n == 1 ? ((*id < 0) ? M_SERIAL_OK_DONE : M_SERIAL_OK_CONTINUE) : M_SERIAL_FAIL;
}

/* End reading a variant from the stream 'serial'.
   Return M_SERIAL_OK_DONE if it succeeds and the variant ends,
   M_SERIAL_FAIL otherwise */
static inline  m_serial_return_code_t
m_serial_bin_read_variant_end(m_serial_local_t local, m_serial_read_t serial)
{
  (void) local; // argument not used
  (void) serial;
  return M_SERIAL_OK_DONE;
}

static const m_serial_read_interface_t m_serial_bin_read_interface = {
  m_serial_bin_read_boolean,
  m_serial_bin_read_integer,
  m_serial_bin_read_float,
  m_serial_bin_read_string,
  m_serial_bin_read_array_start,
  m_serial_bin_read_array_next,
  m_serial_bin_read_array_start,
  m_serial_bin_read_map_value,
  m_serial_bin_read_array_next,
  m_serial_bin_read_tuple_start,
  m_serial_bin_read_tuple_id,
  m_serial_bin_read_variant_start,
  m_serial_bin_read_variant_end
};

static inline void m_serial_bin_read_init(m_serial_read_t serial, FILE *f)
{
  serial->m_interface = &m_serial_bin_read_interface;
  serial->data[0].p = M_ASSIGN_CAST(void*, f);
}

static inline void m_serial_bin_read_clear(m_serial_read_t serial)
{
  (void) serial; // Nothing to do
}

/* Define a synonym of m_serial_read_t to the BIN serializer with its proper OPLIST */
typedef m_serial_read_t m_serial_bin_read_t;
#define M_OPL_m_serial_bin_read_t()                                                   \
  (INIT_WITH(m_serial_bin_read_init), CLEAR(m_serial_bin_read_clear), TYPE(m_serial_bin_read_t) )

#endif
