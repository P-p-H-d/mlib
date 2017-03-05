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
#include "m-array.h"

// Let's define an array of 'char *' named 'charp'
ARRAY_DEF (charp, char *)

int main(void)
{
  // Create an array list
  array_charp_t al;
  // Initialize it
  array_charp_init(al);
      
  // add elements to the array list
  array_charp_push_back (al,"C");
  array_charp_push_back (al,"A");
  array_charp_push_back (al,"E");
  array_charp_push_back (al,"B");
  array_charp_push_back (al,"D");
  array_charp_push_back (al,"F");

  // Use iterator to display contents of al
  printf("Original contents of al: ");
  array_it_charp_t itr;
  for (array_charp_it(itr, al) ; !array_charp_end_p (itr); array_charp_next(itr)) {
    char *element = *array_charp_ref(itr);
    printf ("%s ", element);
  }
  printf("\n");
      
  // Now, display the list backwards
  printf("List backwards: ");
  for (array_charp_previous (itr) ; !array_charp_end_p (itr); array_charp_previous(itr)) {
    char *element = *array_charp_ref(itr);
    printf ("%s ", element);
  }
  printf("\n");

  // Clear the array
  array_charp_clear(al);
  return 0;
}
