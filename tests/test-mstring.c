/*
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

#define M_USE_ADDITIONAL_CHECKS 1
#include "m-string.h"

BOUNDED_STRING_DEF(string16, 16)

static void test_utf8_basic(void)
{
  string_t s;
  string_init (s);

  assert (M_P(string, utf8)(s) == true);
  assert (M_F(string, length_u)(s) == 0);
  
  /* Test internal encode + decode for all characters */
  for (string_unicode_t i = 1; i < 0x10ffff; i++) {
    if (i < 0xD800U || i > 0xDFFFU) {
      string_it_t it;
      char buf[5] = {0};
      stringi_utf8_encode (buf, i);
      /* Low level access for internal testing */
      it->ptr = buf;
      bool b = string_end_p(it);
      assert (b == false);
      assert (it->u == i);
      assert (M_F(string, get_cref)(it) == i);
      assert (*it->next_ptr == 0);
      assert (stringi_utf8_length(buf) == 1);
      assert (stringi_utf8_valid_str_p(buf) == true);
      /* Higher level access */
      string_clean(s);
      M_F(string, push_u)(s, i);
      assert(M_P(string, utf8)(s) == true);
      assert(M_F(string, length_u)(s) == 1); 
    }
  }
  
  /* Reject out of range value */
  for (string_unicode_t i = 0x110000; i < 0x1fffff; i++) {
    string_clean(s);
    M_F(string, push_u)(s, i);
    assert(M_P(string, utf8)(s) == false);
  }
  /* Test rejection of surrogate halves */
  for (string_unicode_t i = 0xd800; i <= 0xdfff; i++) {
    string_clean(s);
    M_F(string, push_u)(s, i);
    assert(M_P(string, utf8)(s) == false);
  }
  /* NOTE: Non-canonical representation are not rejected */
  string_clear(s);
}

static void test_utf8_it(void)
{
  string_t s;
  string_it_t it;
  string_it_t it2;

  string_init(s);
  string_it_end(it2, s);

  for(M_F(string, M_NAMING_IT_FIRST)(it, s) ; !string_end_p(it); M_F(string, next)(it)) {
    assert(0); // Shall not be called
  }
  
  const unsigned int tab[] = { 45, 1458, 25623, 129, 24 };
  for(int i = 0; i < 5; i++)
    M_F(string, push_u) (s, tab[i]);
  assert (M_F(string, length_u)(s) == 5);
  int i = 0;
  for(M_F(string, M_NAMING_IT_FIRST)(it, s) ; !string_end_p(it); M_F(string, next)(it), i++) {
    assert (i < 5);
    assert (M_F(string, get_cref)(it) == tab[i]);
  }

  M_F3(name, set, str) (s, "H€llo René Chaînôr¬");
  assert(M_F(string, length_u)(s) == 19);
  string_unicode_t tab2[19];
  i = 0;
  for(M_F(string, M_NAMING_IT_FIRST)(it, s) ; !string_end_p(it); M_F(string, next)(it), i++) {
    assert (i < 19);
    string_it_set(it2, it);
    tab2[i] = M_F(string, get_cref)(it2);
    assert(string_it_equal_p(it, it2));
    assert(tab2[i] == *M_F(string, cref)(it2));
  }
  assert (i == 19);
  string_clean(s);
  for(i = 0 ; i < 19; i++)
    M_F(string, push_u)(s, tab2[i]);
  assert (string_equal_str_p(s, "H€llo René Chaînôr¬"));

  string_it_end(it, s);
  assert(string_end_p(it));
  assert(!string_it_equal_p(it, it2));

  string_clear(s);
}

static void test0(void)
{
  string_t s1;
  string_t s2;
  size_t i, j;
  bool b;

  string_init (s1);
  string_init (s2);

  M_F3(name, set, str) (s1, "Hello, world!");
  assert (string_size(s1) == 13);
  assert (M_F(string, get_char)(s1, 1) == 'e');

  string_clean(s1);
  assert (string_size(s1) == 0);
  assert (string_empty_p(s1) == true);
  assert (strcmp(M_F(string, get_cstr)(s1), "") == 0);

  M_F(string, set_cstrn)(s1, "Hello, world!", 14);
  assert (strcmp(M_F(string, get_cstr)(s1), "Hello, world!") == 0);
  string_clean(s1);
  M_F(string, set_cstrn)(s1, "Hello, world!", 5);
  assert (strcmp(M_F(string, get_cstr)(s1), "Hello") == 0);
  string_clean(s1);
  
  M_F3(name, set, str) (s1, "Hello");
  M_F3(name, set, str) (s2, "Hello, world!");
  string_set_n (s1, s2, 7, 5);
  assert (strcmp(M_F(string, get_cstr)(s1), "world") == 0);
  
  M_F3(name, set, str) (s1, "Hello beautiful world!");
  assert (string_size(s1) == 22);
  assert (strcmp(M_F(string, get_cstr)(s1), "Hello beautiful world!") == 0);

  string_clean(s1);
  string_set_n (s2, s1, 0, 5);
  assert(string_empty_p(s2));
  
  M_F3(name, set, str)(s1, "Hello");
  M_F(string, cat_cstr)(s1, ", world!");
  assert (string_size(s1) == 13);
  assert (string_cmp_str(s1, "Hello, world!") == 0);
  M_F3(name, set, str)(s2, "Hello, world!");
  assert (string_cmp(s1, s2) == 0);
  
  M_F3(name, set, str)(s2, "hELLO, world!");
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

  string_set (s1, s1);
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

  string_set_n (s1, s2, j, 2);
  assert (string_cmp_str (s1, ", ") == 0);

  string_set (s1, s2);
  i = string_replace_str (s1, "world", "Paul");
  assert (i != STRING_FAILURE);
  assert (string_cmp_str (s1, "Hello, Paul!") == 0);

  i = string_replace_str (s1, "world", "Paul");
  assert (i == STRING_FAILURE);
  assert (string_cmp_str (s1, "Hello, Paul!") == 0);

  i = string_replace_str (s1, "Paul", "Juliette");
  assert (i != STRING_FAILURE);
  assert (string_cmp_str (s1, "Hello, Juliette!") == 0);

  i = string_replace_str (s1, "Juliette", "");
  assert (i != STRING_FAILURE);
  assert (string_cmp_str (s1, "Hello, !") == 0);

  M_F3(name, set, str)(s1, "Hello world 4 life world is real another world");
  M_F3(string, replace_all, cstr)(s1, "world", "earth");
  assert( string_equal_str_p(s1, "Hello earth 4 life earth is real another earth"));

  M_F3(name, set, str)(s1, "Hello world 4 life world is real another world");
  M_F3(string, replace_all, cstr)(s1, "world", "Wrld");
  assert( string_equal_str_p(s1, "Hello Wrld 4 life Wrld is real another Wrld"));

  M_F3(name, set, str)(s1, "world Hello world 4 life world is real another planet");
  M_F3(string, replace_all, cstr)(s1, "world", "Wrld");
  assert( string_equal_str_p(s1, "Wrld Hello Wrld 4 life Wrld is real another planet"));

  M_F3(name, set, str)(s1, "Hello world 4 life world is real another world");
  M_F3(string, replace_all, cstr)(s1, "world", "");
  assert( string_equal_str_p(s1, "Hello  4 life  is real another "));

  M_F3(name, set, str)(s1, "Hello world 4 life world is real another world");
  M_F3(string, replace_all, cstr)(s1, "world", "**WORLD**");
  assert( string_equal_str_p(s1, "Hello **WORLD** 4 life **WORLD** is real another **WORLD**"));

  M_F3(name, set, str)(s1, "world Hello world 4 life world is real another planet");
  M_F3(string, replace_all, cstr)(s1, "world", "**WORLD**");
  assert( string_equal_str_p(s1, "**WORLD** Hello **WORLD** 4 life **WORLD** is real another planet"));

  M_F3(name, set, str)(s1, "HELLO");
  M_F3(name, set, str)(s2, "WORLD");
  M_F(string, replace_all)(s1, s1, s2);
  assert( string_equal_str_p(s1, "WORLD"));

  M_F(string, printf) (s1, "");
  assert (string_cmp_str (s1, "") == 0);
  assert (string_size (s1) == 0);

  M_F(string, printf) (s1, "There is %d Paul!", 2);
  assert (string_cmp_str (s1, "There is 2 Paul!") == 0);
  assert (string_size (s1) == 16);

  // Illegal format conversion(assign empty string)
  M_F(string, printf) (s1, "%#");
  assert (string_cmp_str (s1, "") == 0);

  M_F3(name, set, str)(s1, "Hello, world()\n");
  i = string_cspn(s1, ",()");
  assert (i == 5);
  M_F3(name, set, str)(s1, "Hello (world)\n");
  i = string_cspn(s1, ",()");
  assert (i == 6);
  M_F3(name, set, str)(s1, "Hello (world)\n");
  i = string_spn(s1, "Hel");
  assert (i == 4);

  string_clear (s1);
  string_init (s1);
  string_strim(s1);
  
  assert (string_cmp_str (s1, "") == 0);
  assert (string_cmpi (s1, s1) == 0);
 
  M_F3(name, set, str)(s2, "Hello");
  string_cat (s2, s1);
  assert (string_cmp_str (s2, "Hello") == 0);

  M_F3(name, set, str)(s2, "Hello");
  string_cat (s1, s2);
  assert (string_cmp_str (s1, "Hello") == 0);

  M_F3(name, set, str)(s2, " World!");
  string_cat (s1, s2);
  assert (string_cmp_str (s1, "Hello World!") == 0);

  string_clear (s1);
  string_init (s1);

  string_left(s1, 100);
  assert (string_empty_p (s1));
  assert (string_size (s1) == 0);
  assert (string_cmp_str (s1, "") == 0);
  
  string_right(s1, 100);
  assert (string_empty_p (s1));
  assert (string_size (s1) == 0);
  assert (string_cmp_str (s1, "") == 0);

  M_F3(name, set, str)(s2, "Hello");
  string_set (s2, s1);
  assert (string_empty_p (s2));
  assert (string_size (s2) == 0);
  assert (string_cmp_str (s2, "") == 0);

  M_F3(name, set, str)(s1, "Hello, world!");
  assert(M_P(string, start_with_cstr)(s1, "Hello") == true);
  assert(M_P(string, start_with_cstr)(s1, "Help") == false);
  assert(M_P(string, end_with_cstr)(s1, "world!") == true);
  assert(M_P(string, end_with_cstr)(s1, "worldX") == false);
  assert(M_P(string, end_with_cstr)(s1, "Hello, world!!") == false);
  M_F3(name, set, str)(s2, "H");
  assert(M_P(string, start_with_string)(s1, s2) == true);
  M_F3(name, set, str)(s2, "!");
  assert(M_P(string, end_with_string)(s1, s2) == true);

  M_F(string, cat_printf)(s1, " %d little %s.", 10, "suns");
  M_F3(name, set, str)(s2, "Hello, world! 10 little suns.");
  assert(string_equal_p(s1, s2) == true);

  M_F3(name, set, str)(s1, "X:");
  M_F(string, cat_printf)(s1, "");
  assert(string_equal_str_p(s1, "X:") == true);

  // Illegal format char
  M_F3(name, set, str)(s1, "X:");
  M_F(string, cat_printf)(s1, "%#");
  assert(string_equal_str_p(s1, "X:") == true);

  M_F3(name, set, str)(s1, " \r\n\t HELLO  \n\r\t");
  string_strim(s1);
  assert (string_cmp_str (s1, "HELLO") == 0);
  
  M_F3(name, set, str)(s1, " \r\n\t   \n\r\t");
  string_strim(s1);
  assert (string_cmp_str (s1, "") == 0);

  M_F3(name, set, str)(s1, " \r\n\t++\t YES  ++ \n\r\t");
  string_strim(s1, "\r\n\t +");
  assert (string_cmp_str (s1, "YES") == 0);
  
  M_F3(name, set, str)(s1, "Hello world");
  M_F(string, get_str)(s2, s1, false);
  assert (string_cmp_str (s2, "\"Hello world\"") == 0);
  M_F3(name, set, str)(s1, "Hello \"world\"");
  M_F(string, get_str)(s2, s1, true);
  assert (string_cmp_str (s2, "\"Hello world\"\"Hello \\\"world\\\"\"") == 0);

  const char *sp = M_F(string, get_cstr)(s2);
  b = M_F(string, parse_cstr)(s1, sp, &sp);
  assert (b == true);
  assert (strcmp(sp, "\"Hello \\\"world\\\"\"") == 0);
  assert (string_cmp_str(s1, "Hello world") == 0);
  b = M_F(string, parse_cstr)(s1, sp, &sp);
  assert (b == true);
  assert (strcmp(sp, "") == 0);
  assert (string_cmp_str(s1, "Hello \"world\"") == 0);
  
  b = M_F(string, parse_cstr)(s1, "Hop", NULL);
  assert(b == false);
  b = M_F(string, parse_cstr)(s1, "\"\\x\"", NULL);
  assert(b == false);
  b = M_F(string, parse_cstr)(s1, "\"\\0 \"", NULL);
  assert(b == false);
  b = M_F(string, parse_cstr)(s1, "\"\\01.\"", NULL);
  assert(b == false);
  b = M_F(string, parse_cstr)(s1, "\"\\012\"", NULL);
  assert(b == true);
  assert (string_cmp_str(s1, "\012") == 0);
  b = M_F(string, parse_cstr)(s1, "\"ANSWER:\\n\\012\"", &sp);
  assert(b == true);
  assert(*sp == 0);
  assert (string_cmp_str(s1, "ANSWER:\n\012") == 0);

  M_F3(name, set, str)(s1, "Hello \"world\"");
  FILE *f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  M_F(string, out_str) (f, s1);
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  b = string_in_str(s2, f);
  assert(b);
  fclose(f);
  assert (string_equal_p(s1, s2));

  M_F3(name, set, str)(s1, "\tHell\\o\n\"World\"\r\001");
  M_F(string, get_str)(s2, s1, false);
  assert (string_cmp_str(s2, "\"\\tHell\\\\o\\n\\\"World\\\"\\r\\001\"") == 0);

  f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  M_F(string, out_str) (f, s1);
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  b = string_in_str(s2, f);
  assert(b);
  fclose(f);
  assert (string_equal_p(s1, s2));

  f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  fputs ("\"\\8\"", f);
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  b = string_in_str(s2, f);
  assert(!b);
  fclose(f);

  f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  fputs ("\"\\7 \"", f);
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  b = string_in_str(s2, f);
  assert(!b);
  fclose(f);

  f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  fputs ("\"\\01A\"", f);
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  b = string_in_str(s2, f);
  assert(!b);
  fclose(f);

  M_F3(name, set, str)(s1, "AZERTY");
  M_F3(name, set, str)(s2, "QWERTY");
  string_swap (s1, s2);
  assert(string_equal_str_p (s1, "QWERTY"));
  assert(string_equal_str_p (s2, "AZERTY"));

  string_cat(s1, s1);
  assert (string_equal_str_p(s1, "QWERTYQWERTY"));

  string_clear (s2);
  char *s = M_F3(string, M_NAMING_CLEAR, get_cstr) (s1);
  assert(strcmp(s, "QWERTYQWERTY") == 0);
  free(s);

  string_t s3;
  string_init(s3);
  M_F(string, cat_cstr)(s3, "ABC");
  s = M_F3(string, M_NAMING_CLEAR, get_cstr)(s3);
  assert(strcmp(s, "ABC") == 0);
  free(s);
  
  string_init_set_str(s1, "RESTART");
  assert (string_equal_str_p(s1, "RESTART"));
  
  string_init_set(s2, s1);
  assert (string_equal_str_p(s2, "RESTART"));

  M_F3(name, set, str)(s2, "DUMMY");
  string_clear (s2);
  string_init_move(s2, s1);
  assert (string_equal_str_p(s2, "RESTART"));
  string_reserve(s2, 0);
  assert (string_equal_str_p(s2, "RESTART"));
  string_init(s1);
  string_move(s1, s2);
  assert (string_equal_str_p(s1, "RESTART"));

  string_init_set_str(s2, "START");
  size_t n = string_search(s1, s2);
  assert(n == 2);

  string_set(s1, s2);
  int r = string_strcoll (s1, s2);
  assert (r == 0);
  
  string_right (s1, 100);
  assert (string_empty_p(s1));

  M_F3(name, set, str)(s1, "RESTART");
  M_F3(name, set, str)(s2, "START");
  string_replace (s1, s2, s2);
  assert (string_equal_str_p(s1, "RESTART"));
  
  string_clear (s1);
  string_clear (s2);

  string_init (s1);
  string_init (s2);
  
  size_t h1 = M_F(string, hash)(s1);
  assert (h1 != 0);

  M_F(string, printf) (s1, "Hello %d worlds. How do you do? I'm fine. Thank you! The weather is bad today. I should had brought my umbrella. Oh! You can lend me one! Thank you very much! No really thank you. I wouldn't be able to get in time for my job.", 2);
  assert (string_equal_str_p(s1, "Hello 2 worlds. How do you do? I'm fine. Thank you! The weather is bad today. I should had brought my umbrella. Oh! You can lend me one! Thank you very much! No really thank you. I wouldn't be able to get in time for my job."));
  
  f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  M_F(string, fputs) (f, s1);
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  M_F(string, fgets)(s2, f, STRING_READ_FILE);
  fclose(f);
  assert (string_equal_p(s1, s2));

  string_clear(s2);
  string_init(s2);
  M_F3(name, set, str)(s2, "I'm ok");

  f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  M_F(string, fputs) (f, s1);
  fprintf (f, "\n");
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  M_F(string, fgets)(s2, f, STRING_READ_PURE_LINE);
  fclose(f);
  assert (string_equal_p(s1, s2));

  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  M_F(string, fgets)(s2, f, STRING_READ_LINE);
  fclose(f);
  M_F(string, push_back)(s1, '\n');
  assert (string_equal_p(s1, s2));

  string_clear(s1);
  string_init(s1);
  f = m_core_fopen("a-mstring.dat", "wt");
  assert(f != NULL);
  fprintf(f, "hello world...\n\tHowwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww do you do?");
  fclose(f);
  f = m_core_fopen("a-mstring.dat", "rt");
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b);
  assert(string_equal_str_p(s1, "hello"));
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b);
  assert(string_equal_str_p(s1, "world"));
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b);
  assert(string_equal_str_p(s1, "Howwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"));
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b);
  assert(string_equal_str_p(s1, "do"));
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b);
  assert(string_equal_str_p(s1, "you"));
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b);
  assert(string_equal_str_p(s1, "do"));
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b == false);
  fclose(f);

  f = m_core_fopen("a-mstring.dat", "wt");
  assert(f != NULL);
  fprintf(f, "word");
  fclose(f);
  f = m_core_fopen("a-mstring.dat", "rt");
  b = M_F(string, fget_word)(s1, " \t.\n?", f);
  assert(b);
  assert(string_equal_str_p(s1, "word"));
  fclose(f);
  
  size_t h = M_F(string, hash)(s1);
  assert (h != 0);

  M_F3(name, set, str) (s1, "");
  assert (M_F(string, capacity)(s1) > 0);
  string_reserve (s1, 0);
  assert (M_F(string, capacity)(s1) == sizeof (string_heap_ct) - 1);
  
  M_F3(name, set, str)(s1, "HELLO XXX!");
  string_replace_at(s1, 6, 3, "World");
  assert(string_equal_str_p(s1, "HELLO World!"));
  string_replace_at(s1, 6, 5, "WORLD");
  assert(string_equal_str_p(s1, "HELLO WORLD!"));

  h = string_search_pbrk(s1, "AB");
  assert(h==STRING_FAILURE);
  h = string_search_pbrk(s1, "oO");
  assert(h==4);

  string_clean(s1);
  string_reserve(s1, 0);
  M_F(string, cat_printf)(s1, "%d little %s.", 42, "suns");
  M_F3(name, set, str)(s2, "42 little suns.");
  assert(string_equal_p(s1, s2) == true);

  string_clear (s1);
  string_clear (s2);

  string_init(s1);
  M_F3(name, set, str)(s1, "Hello");
  string_reserve(s1, 128);
  assert(string_equal_str_p(s1, "Hello"));
  string_clear(s1);
}

static void test_bounded1(void)
{
  string16_t s, d;
  string16_init(s);
  assert (string16_empty_p(s));
  assert (string16_size(s) == 0);
  assert (string16_capacity(s) == 17);
  string16_set_str(s, "Hello");
  assert (!string16_empty_p(s));
  assert (string16_size(s) == 5);
  assert (string16_equal_str_p(s, "Hello"));
  assert (string16_cmp_str(s, "Hello") == 0);
  assert (string16_get_char(s, 1) == 'e');
  assert (strcmp(string16_get_cstr(s), "Hello") == 0);
  string16_clean(s);
  assert (string16_empty_p(s));
  assert (string16_size(s) == 0);
  string16_set_str(s, "Hello, world! How do you do?");
  assert (!string16_empty_p(s));
  assert (string16_size(s) == 16);
  assert (string16_equal_str_p(s, "Hello, world! Ho"));
  string16_set_strn(s, "Hello, world! How do you do?", 17);
  assert (!string16_empty_p(s));
  assert (string16_size(s) == 16);
  assert (string16_equal_str_p(s, "Hello, world! Ho"));
  string16_set_strn(s, "Hello, world! How do you do?", 15);
  assert (!string16_empty_p(s));
  assert (string16_size(s) == 15);
  assert (string16_equal_str_p(s, "Hello, world! H"));
  string16_cat_str(s, "ow do you do?");
  assert (string16_size(s) == 16);
  assert (!string16_equal_str_p(s, "Hello, world! H"));
  assert (string16_equal_str_p(s, "Hello, world! Ho"));
  string16_printf(s, "HeH:%d", 16);
  assert (string16_size(s) == 6);
  assert (string16_equal_str_p(s, "HeH:16"));
  string16_cat_printf(s, " GeG:%d/%d FRE:%d", 17, 42, 13);
  assert (string16_size(s) == 16);
  assert (string16_equal_str_p(s, "HeH:16 GeG:17/42"));
  assert (string16_hash(s) != 0);
  
  string16_set_strn(s, "Hello, world! How do you do?", 15);
  string16_init_set(d, s);
  string16_clean(s);
  assert (string16_equal_str_p(d, "Hello, world! H"));
  
  string16_clear(d);
  string16_clear(s);
}

static void test_bounded_io(void)
{
  string16_t s, d;
  bool b;
  FILE *f;
  
  string16_init(s);
  string16_init(d);
  
  string16_set_str(s, "Hello \"world\"");
  f = m_core_fopen ("a-mstring.dat", "wt");
  assert (f != NULL);
  string16_out_str (f, s);
  fclose (f);
  f = m_core_fopen("a-mstring.dat", "rt");
  assert (f != NULL);
  b = string16_in_str(d, f);
  assert(b);
  fclose(f);
  assert (string16_equal_p(s, d));

  const char *end;
  string_t str;
  string_init(str);
  string16_get_str(str, s, false);
  assert(string_equal_str_p(str, "\"Hello \\\"world\\\"\""));
  b = string16_parse_str(d, M_F(string, get_cstr)(str), &end);
  assert (b);
  assert (string16_equal_p(s, d));

  string_clear(str);
  string16_clear(s);
  string16_clear(d);
}

static void test_bounded_M_LET(void)
{
  M_LET( x, BOUNDED_STRING_OPLIST(string16)) {
    assert (string16_empty_p(x));
  }
  
  M_LET( (x,"tree"), BOUNDED_STRING_OPLIST(string16)) {
    assert (string16_equal_str_p(x, "tree"));
  }

  M_LET( (x,"tree%d", 78), BOUNDED_STRING_OPLIST(string16)) {
    assert (string16_equal_str_p(x, "tree78"));
  }

}

static void test_M_LET(void)
{
  M_LET(s, string_t) {
    assert(string_empty_p(s));
  }
  M_LET(s1, s2, string_t) {
    assert(string_empty_p(s1));
    assert(string_empty_p(s2));
    M_F3(name, set, str)(s1, "Success!");
    string_set(s2, s1);
    assert(string_equal_p(s1, s2));
    M_F3(name, set, str)(s1, "Try to create a very, very, very, very big string!!!!!!!!!!!!!!!!!!!!!!!");
    string_set(s2, s1);
    assert(string_equal_p(s1, s2));
  }
  M_LET((s1,"OK"),string_t) {
    assert(string_equal_str_p(s1, "OK"));
  }
  M_LET((s1,"OK S=%d", 16),string_t) {
    assert(string_equal_str_p(s1, "OK S=16"));
  }
  M_LET( (s1, "Hello %s", "world"), s2, (s3, "%d-%d", 42, 17), string_t) {
    assert(string_equal_str_p(s1, "Hello world"));
    assert(string_empty_p(s2));
    assert(string_equal_str_p(s3, "42-17"));
  }
  M_LET( (s1, "Hello"), string_t) {
    assert(string_equal_str_p(s1, "Hello"));
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
    // Initalization with another string is only supported in C11 mode.
    M_LET( (s2, s1), string_t) {
      // In C11 mode, string_equal_p accept also C string
      assert(string_equal_p(s2, "Hello"));
    }
#endif
  }
}
int main(void)
{
  test0();
  test_M_LET();
  test_utf8_basic();
  test_utf8_it();
  test_bounded1();
  test_bounded_io();
  test_bounded_M_LET();
  exit(0);
}
