/*
 * Copyright (c) 2017, Patrick Pelissier
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

#include "m-string.h"

int main(void)
{
  string_t s1;
  STRING_DECL_INIT (s2);
  size_t i, j;
  bool b;

  string_init (s1);

  string_set_str (s1, "Hello, world!");
  assert (string_get_length(s1) == 13);
  assert (string_get_char(s1, 1) == 'e');

  string_clean(s1);
  assert (string_get_length(s1) == 0);
  assert (string_empty_p(s1) == true);
  assert (strcmp(string_get_cstr(s1), "") == 0);

  string_set_strn(s1, "Hello, world!", 14);
  assert (strcmp(string_get_cstr(s1), "Hello, world!") == 0);
  string_clean(s1);
  string_set_strn(s1, "Hello, world!", 5);
  assert (strcmp(string_get_cstr(s1), "Hello") == 0);
  string_clean(s1);
  
  string_set_str (s1, "Hello beautiful world!");
  assert (string_get_length(s1) == 22);
  assert (strcmp(string_get_cstr(s1), "Hello beautiful world!") == 0);

  string_clean(s1);
  string_set_str(s1, "Hello");
  string_cat_str(s1, ", world!");
  assert (string_get_length(s1) == 13);
  assert (string_cmp_str(s1, "Hello, world!") == 0);
  string_set_str(s2, "Hello, world!");
  assert (string_cmp(s1, s2) == 0);
  
  string_set_str(s2, "hELLO, world!");
  assert (string_cmpi(s1, s2) == 0);

  i = string_search_char (s1, ',');
  assert (i == 5);
  i = string_search_char (s1, 'Z');
  assert (i == STRING_FAILURE);
  
  i = string_search_rchar (s1, 'o');
  assert (i == 8);
  i = string_search_rchar (s1, 'Z');
  assert (i == STRING_FAILURE);

  i = string_search_str (s1, "world");
  assert (i == 7);
  i = string_search_str (s1, "World");
  assert (i == STRING_FAILURE);
  
  string_set (s2, s1);

  j = string_search_char (s1, ',');
  string_left (s1, j);
  assert (string_cmp_str (s1, "Hello") == 0);

  string_set (s1, s2);
  j = string_search_char (s1, ',');
  string_right (s1, j);
  assert (string_cmp_str (s1, ", world!") == 0);

  string_set (s1, s2);
  j = string_search_char (s1, ',');
  string_mid (s1, j, 2);
  assert (string_cmp_str (s1, ", ") == 0);

  string_set (s1, s2);
  b = string_replace_str (s1, "world", "Paul");
  assert (b == true);
  assert (string_cmp_str (s1, "Hello, Paul!") == 0);

  b = string_replace_str (s1, "world", "Paul");
  assert (b == false);
  assert (string_cmp_str (s1, "Hello, Paul!") == 0);

  b = string_replace_str (s1, "Paul", "Juliette");
  assert (b == true);
  assert (string_cmp_str (s1, "Hello, Juliette!") == 0);

  string_printf (s1, "There is %d Paul!", 2);
  assert (string_cmp_str (s1, "There is 2 Paul!") == 0);
  assert (string_get_length (s1) == 16);

  string_set_str(s1, "Hello, world()\n");
  i = string_cspn(s1, ",()");
  assert (i == 5);
  string_set_str(s1, "Hello (world)\n");
  i = string_cspn(s1, ",()");
  assert (i == 6);
  string_set_str(s1, "Hello (world)\n");
  i = string_spn(s1, "Hel");
  assert (i == 4);

  string_clear (s1);
  string_init (s1);
  
  assert (string_cmp_str (s1, "") == 0);
  assert (string_cmpi (s1, s1) == 0);
 
  string_set_str(s2, "Hello");
  string_cat (s2, s1);
  assert (string_cmp_str (s2, "Hello") == 0);

  string_set_str(s2, "Hello");
  string_cat (s1, s2);
  assert (string_cmp_str (s1, "Hello") == 0);

  string_clear (s1);
  string_init (s1);

  string_left(s1, 100);
  assert (string_empty_p (s1));
  assert (string_get_length (s1) == 0);
  assert (string_cmp_str (s1, "") == 0);
  
  string_right(s1, 100);
  assert (string_empty_p (s1));
  assert (string_get_length (s1) == 0);
  assert (string_cmp_str (s1, "") == 0);

  string_set_str(s2, "Hello");
  string_set (s2, s1);
  assert (string_empty_p (s2));
  assert (string_get_length (s2) == 0);
  assert (string_cmp_str (s2, "") == 0);

  string_set_str(s1, "Hello, world!");
  assert(string_start_with_str_p(s1, "Hello") == true);
  assert(string_start_with_str_p(s1, "Help") == false);
  string_set_str(s2, "H");
  assert(string_start_with_string_p(s1, s2) == true);

  string_cat_printf(s1, " %d little %s.", 10, "suns");
  string_set_str(s2, "Hello, world! 10 little suns.");
  assert(string_equal_p(s1, s2) == true);

  string_set_str(s1, " \r\n\t HELLO  \n\r\t");
  string_strim(s1);
  assert (string_cmp_str (s1, "HELLO") == 0);
  
  string_set_str(s1, " \r\n\t   \n\r\t");
  string_strim(s1);
  assert (string_cmp_str (s1, "") == 0);

  string_set_str(s1, "Hello world");
  string_get_str(s2, s1, false);
  assert (string_cmp_str (s2, "\"Hello world\"") == 0);
  string_set_str(s1, "Hello \"world\"");
  string_get_str(s2, s1, true);
  assert (string_cmp_str (s2, "\"Hello world\"\"Hello \\\"world\\\"\"") == 0);

  string_set_str(s1, "Hello \"world\"");
  FILE *f = fopen ("a.dat", "wt");
  assert (f != NULL);
  string_out_str (f, s1);
  fclose (f);
  f = fopen("a.dat", "rt");
  assert (f != NULL);
  b = string_in_str(s2, f);
  assert(b);
  fclose(f);
  assert (string_equal_p(s1, s2));

  string_clear (s1);
  exit(0);
}
