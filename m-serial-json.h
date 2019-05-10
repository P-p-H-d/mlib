/*
 * Copyright (c) 2017-2019, Patrick Pelissier
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

static inline m_serial_return_code_t
m_serial_json_write_boolean(m_serial_write_t serial, const bool data)
{
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "%s", data ? "true" : "false");
  return n > 0 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline m_serial_return_code_t
m_serial_json_write_integer(m_serial_write_t serial,const long long data, const size_t size_of_type)
{
  (void) size_of_type; // Ignored
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "%lld", data);
  return n > 0 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;  
}

static inline m_serial_return_code_t
m_serial_json_write_float(m_serial_write_t serial, const long double data, const size_t size_of_type)
{
  (void) size_of_type; // Ignored
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "%Lf", data);
  return n > 0 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline m_serial_return_code_t
m_serial_json_write_string(m_serial_write_t serial, const char data[])
{
  FILE *f = (FILE *)serial->data[0];
  /* Build dummy string to reuse string_out_str */
  string_t v2;
  v2->u.heap.size = strlen(data);
  v2->u.heap.alloc = v2->u.heap.size + 1;
  v2->ptr = (char*)data;
  string_out_str(f, v2);
  return M_SERIAL_OK_DONE;
}

static inline m_serial_return_code_t
m_serial_json_write_array_start(m_serial_write_t serial, const size_t number_of_elements)
{
  (void) number_of_elements; // Ignored
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "[");
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_write_array_next(m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, ",");
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t
m_serial_json_write_array_end(m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "]");
  return n > 0 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t
m_serial_json_write_map_start(m_serial_write_t serial, const size_t number_of_elements)
{
  (void) number_of_elements; // Ignored
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "{");
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t
m_serial_json_write_map_value(m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, ":");
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t
m_serial_json_write_map_next(m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, ",");
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t
m_serial_json_write_map_end(m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "}");
  return n > 0 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t m_serial_json_write_tuple_start(m_serial_write_t serial){
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "{");
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t m_serial_json_write_tuple_id(m_serial_write_t serial, const char *const field_name[], const int max, const int index){
  (void) max;
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "%c\"%s\":", index == 0 ? ' ' : ',', field_name[index]);
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t m_serial_json_write_tuple_end(m_serial_write_t serial){
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "}");
  return n > 0 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t
m_serial_json_write_variant_start(m_serial_write_t serial, const char *const field_name[], const int max, const int index)
{
  FILE *f = (FILE *)serial->data[0];
  int n;
  if (index >= 0) {
    n = fprintf(f, "{\"%s\":", field_name[index]);
  } else {
    n = fprintf(f, "{}");
  }
  return n > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline   m_serial_return_code_t
m_serial_json_write_variant_end(m_serial_write_t serial)
{
  FILE *f = (FILE *)serial->data[0];
  int n = fprintf(f, "}");
  return n > 0 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

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

static inline void m_serial_json_write_init(m_serial_write_t serial, FILE *f)
{
  serial->interface = &m_serial_write_json_interface;
  serial->data[0] = (void*) f;
}

static inline void m_serial_json_write_clear(m_serial_write_t serial)
{
  (void) serial; // Nothing to do
}

static inline int
m_serial_json_read_skip (FILE *f)
{
  int c;
  do {
    c = fgetc(f);
  } while (isspace(c));
  return c;
}

static inline  m_serial_return_code_t
m_serial_json_read_boolean(m_serial_read_t serial, bool *b){
  FILE *f = (FILE*) serial->data[0];  
  int c = m_serial_json_read_skip(f);
  if (c == 't') {
    *b = true;
    c = fgetc(f);
    if (c != 'r') return M_SERIAL_FAIL;
    c = fgetc(f);
    if (c != 'u') return M_SERIAL_FAIL;
    c = fgetc(f);
    if (c != 'e') return M_SERIAL_FAIL;
    return M_SERIAL_OK_DONE;
  } else if (c == 'f') {
    *b = false;
    c = fgetc(f);
    if (c != 'a') return M_SERIAL_FAIL;
    c = fgetc(f);
    if (c != 'l') return M_SERIAL_FAIL;
    c = fgetc(f);
    if (c != 's') return M_SERIAL_FAIL;
    c = fgetc(f);
    if (c != 'e') return M_SERIAL_FAIL;
    return M_SERIAL_OK_DONE;
  } else {
    return M_SERIAL_FAIL;
  }
}

static inline  m_serial_return_code_t
m_serial_json_read_integer(m_serial_read_t serial, long long *i, const size_t size_of_type){
  (void) size_of_type; // Ignored
  FILE *f = (FILE*) serial->data[0];
  return fscanf(f, " %lld", i) == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_float(m_serial_read_t serial, long double *r, const size_t size_of_type){
  (void) size_of_type; // Ignored
  FILE *f = (FILE*) serial->data[0];  
  return fscanf(f, " %Lf", r) == 1 ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_string(m_serial_read_t serial, struct string_s *s){
  FILE *f = (FILE*) serial->data[0];
  // NOTE: string_in_str doesnt parse space characters before the \"... Is it needed?
  fscanf(f, " ");
  return string_in_str(s, f) ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_array_start(m_serial_read_t serial, size_t *num)
{
  FILE *f = (FILE*) serial->data[0];
  int final = -1;
  fscanf(f, " {%n", &final);
  *num = -1U;
  return final > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_array_next(m_serial_read_t serial)
{
  FILE *f = (FILE*) serial->data[0];
  char c = 0;
  fscanf(f, " %c", &c);
  return c == ',' ? M_SERIAL_OK_CONTINUE : c == ']' ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_map_start(m_serial_read_t serial, size_t *num)
{
  FILE *f = (FILE*) serial->data[0];
  int final = -1;
  fscanf(f, " {%n", &final);
  *num = -1U;
  return final > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_map_value(m_serial_read_t serial)
{
  FILE *f = (FILE*) serial->data[0];
  int final = -1;
  fscanf(f, " :%n", &final);
  return final > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_map_next(m_serial_read_t serial)
{
  FILE *f = (FILE*) serial->data[0];
  char c = 0;
  fscanf(f, " %c", &c);
  return c == ',' ? M_SERIAL_OK_CONTINUE : c == '}' ? M_SERIAL_OK_DONE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_tuple_start(m_serial_read_t serial)
{
  FILE *f = (FILE*) serial->data[0];
  int final = -1;
  fscanf(f, " {%n", &final);
  return final > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_tuple_id(m_serial_read_t serial, const char *const field_name [], const int max, int *id){
  FILE *f = (FILE*) serial->data[0];
  int c = m_serial_json_read_skip(f);
  if (c == '}') return M_SERIAL_OK_DONE;
  if (c == ',') {
     // If first call of read_tuple_id, it is a failure
     if (*id == -1) return M_SERIAL_FAIL;
  } else {
    // c should be \" but let fscanf parse it.
    ungetc(c, f);
  }
  /* Read the field in the JSON */
  c = -1;
  char field[M_MAX_IDENTIFIER_LENGTH+1]; // TODO: fix error detection below.
  fscanf(f, " \"%" M_APPLY(M_AS_STR, M_MAX_IDENTIFIER_LENGTH) "[^ \t\n\"]\":%n", field, &c);
  if (c <= 0)
    return M_SERIAL_FAIL;
  /* Search for field in field_name */
  for(int n = 0; n < max; n++) {
    if (strcmp(field, field_name[n]) == 0) {
      *id = n;
      return M_SERIAL_OK_CONTINUE;
    }
  }
  return M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_variant_start(m_serial_read_t serial, const char *const field_name[], const int max, int*id)
{
  FILE *f = (FILE*) serial->data[0];
  int c = m_serial_json_read_skip(f);
  if (c == '}') return M_SERIAL_OK_DONE;
  // c should be \" but let fscanf parse it.
  ungetc(c, f);
  /* Read the field in the JSON */
  c = -1;
  char field[M_MAX_IDENTIFIER_LENGTH+1];
  fscanf(f, " \"%" M_APPLY(M_AS_STR, M_MAX_IDENTIFIER_LENGTH) "[^ \t\n\"]\":%n", field, &c);
  if (c == -1)
    return M_SERIAL_FAIL;
  /* Search for field in field_name */
  for(int n = 0; n < max; n++) {
    if (strcmp(field, field_name[n]) == 0) {
      *id = n;
      return M_SERIAL_OK_CONTINUE;
    }
  }
  return M_SERIAL_FAIL;
}

static inline  m_serial_return_code_t
m_serial_json_read_variant_end(m_serial_read_t serial)
{
  FILE *f = (FILE*) serial->data[0];
  int final = -1;
  fscanf(f, " }%n", &final);
  return final > 0 ? M_SERIAL_OK_CONTINUE : M_SERIAL_FAIL;
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

static inline void m_serial_json_read_init(m_serial_read_t serial, FILE *f)
{
  serial->interface = &m_serial_json_read_interface;
  serial->data[0] = (void*) f;
}

static inline void m_serial_json_read_clear(m_serial_read_t serial)
{
  (void) serial; // Nothing to do
}

#endif
