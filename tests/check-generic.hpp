/*
 * Copyright (c) 2021-2022 Patrick Pelissier
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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

/* This test will perform random generic operations on the
   M*LIB container and compares it to the same operation on the
   C++ equivalent container.
   If there are some differences, it reports an error.
   It is therefore quite generic and based on the properties of the container
*/

/* Get the types from the container oplist if needed */
#ifndef CONT_TYPE
# define CONT_TYPE_(CONT_OPL) M_GET_TYPE CONT_OPL
# define CONT_TYPE CONT_TYPE_(CONT_OPL)
#endif

// Type used for M*LIB container. Default is the same as C++
#ifndef BASE_TYPE
# define BASE_TYPE BASE_CLASS
#endif

// Compare the M*LIB base object to the C++ base object
#ifndef CMP_BASE
# define CMP_BASE(a, b) assert( (a) == (b) )
#endif

/* Get the affectation if needed to transform an integer into C++ class (resp. M*LIB type) */
#ifndef CLASS_SET_INT
# define CLASS_SET_INT(a, i) (a) = (i)
#endif
#ifndef TYPE_SET_INT
# define TYPE_SET_INT(a, i)  (a) = (i)
#endif

/* Get the affectation if needed to transform a C++ iterator (resp. M*LIB) to an int */
#ifndef CLASS_IT_TO_INT
# define CLASS_IT_TO_INT(it) *(it)
#endif
#ifndef TYPE_IT_TO_INT
# define TYPE_IT_TO_INT(it)  *(it)
#endif

/* Global properties of the container */
// Use find to perform GET_KEY for C++
//#define HAVE_GET_KEY_THROUGH_FIND
// Container are nor ordered
//#define DONT_HAVE_SEQUENCE_IT
// associative array container
//#define HAVE_SET_KEY
// Container is in reverse
//#define HAVE_REVERSE_IT
// Have PUSH_FRONT operator
//#define HAVE_PUSH_FRONT
// Have POP_FRONT operator
//#define HAVE_POP_FRONT

/*********************************************************************************/

// Define the container name, extracting it from its oplist
#define CONT_NAME_(CONT_OPL) M_GET_NAME CONT_OPL
#define CONT_NAME            CONT_NAME_(CONT_OPL)

// Define the maximum number of tests
#define MAX_TEST 28

#ifdef HAVE_REVERSE_IT
// Scan the C++ sequence in reverse
# define begin rbegin
# define end   rend
# define IT(x) std::prev(x.base(), 1)
#else
# define IT(x) x
#endif

volatile int test_case;

void test(int select_test, int index,
          BASE_TYPE  &b0, BASE_CLASS &b1, CONT_TYPE &c0, CONT_CLASS &c1)
{
  bool p0 = false, p1 = false;
  size_t s0 = 0, s1 = 0;
  CONT_TYPE tmp;
#if M_TEST_METHOD_P(IT_TYPE, CONT_OPL)
  M_ID(M_GET_IT_TYPE CONT_OPL) it0;
  auto it1 = c1.begin();
#endif

  // Save in a volatile variable the last test for debug purpose
  test_case = select_test % MAX_TEST;
#ifdef DEBUG
  printf("Case=%d\n", test_case);
#endif
  switch (test_case) {
#if M_TEST_METHOD_P(PUSH, CONT_OPL)
  case 0:
    TYPE_SET_INT(b0, index);
    CLASS_SET_INT(b1, index);
    M_CALL_PUSH(CONT_OPL, c0, b0);
    c1.push_back(b1);
    break;
    
  case 1:
    index = index % 1000;
    for(int i = 0; i < index; i++) {
      TYPE_SET_INT(b0, i);
      CLASS_SET_INT(b1, i);
      M_CALL_PUSH(CONT_OPL, c0, b0);
      c1.push_back(b1);
    }
    break;
#endif

#if M_TEST_METHOD_P(POP, CONT_OPL)
  case 2:
   if (!c1.empty()) {
      M_CALL_POP(CONT_OPL, &b0, c0);
      b1 = c1.back();
      c1.pop_back();
    }
    break;
#endif

#if M_TEST_METHOD_P(EMPTY_P, CONT_OPL)
  case 3:
    p0 = M_CALL_EMPTY_P(CONT_OPL, c0);
    p1 = c1.empty();
    break;
#endif
    
#if M_TEST_METHOD_P(SIZE, CONT_OPL)
  case 4:
    s0 = M_CALL_SIZE(CONT_OPL, c0);
    s1 = c1.size();
    break;
#endif
    
#if M_TEST_METHOD_P(RESET, CONT_OPL)
  case 5:
    M_CALL_RESET(CONT_OPL, c0);
    c1.clear();
    break;
#endif

#if M_TEST_METHOD_P(GET_KEY, CONT_OPL)
  case 6:
    if (!c1.empty()) {
#ifndef HAVE_GET_KEY_THROUGH_FIND
      // GET_KEY takes an index which shall be within the range of the container
      index = index % c1.size();
      M_CALL_SET(M_GET_OPLIST CONT_OPL, b0, *M_CALL_GET_KEY(CONT_OPL, c0, index));
      b1 = c1[index];
#else
      // GET_KEY takes a key
      index = index % (DEFAULT_NUMBER/4);
      BASE_TYPE *p = M_CALL_GET_KEY(CONT_OPL, c0, index);
      if (p!= NULL) {
	// Key found
        M_CALL_SET(M_GET_OPLIST CONT_OPL, b0, *p);
        auto i = c1.find(index);
        assert(i != c1.end());
        b1 = CLASS_IT_TO_INT(i);
      } else {
	// Key not found
        auto i = c1.find(index);
        assert(i == c1.end());
        TYPE_SET_INT(b0, 0);
        CLASS_SET_INT(b1, 0);
      }
#endif
    } else {
      assert( M_CALL_EMPTY_P(CONT_OPL, c0) == true);
    }
    break;
#endif

#if M_TEST_METHOD_P(SET_KEY, CONT_OPL)
  case 7:
    TYPE_SET_INT(b0, index);
    CLASS_SET_INT(b1, index);
#ifndef HAVE_SET_KEY
    if (!c1.empty())
      {
	index = index % c1.size();
	M_CALL_SET_KEY(CONT_OPL, c0, index, b0);
	c1[index] = b1;
      }
#else
    index = index % (DEFAULT_NUMBER/4);
    M_CALL_SET_KEY(CONT_OPL, c0, index, b0);
    c1[index] = b1;
#endif
    break;

  case 21:
    {
      unsigned n = index % 1000;
      unsigned offset = (index / 1027) % 1000;
#ifndef HAVE_SET_KEY
      // If it is a sequence container, check it it fits, otherwise resize
      // without resize which may not be available, but push is assumed to be.
      if ((size_t) n+offset > c1.size() ) {
	TYPE_SET_INT(b0, 0);
	CLASS_SET_INT(b1, 0);
	for(unsigned i = c1.size() ; i < (size_t) n+offset; i++) {
	  M_CALL_PUSH(CONT_OPL, c0, b0);
	  c1.push_back(b1);
	}
	assert(c1.size() == (size_t) n+offset);
      }
#endif
      for(unsigned i = 0; i < n; i++) {
	TYPE_SET_INT(b0, 3*i);
	CLASS_SET_INT(b1, 3*i);
	M_CALL_SET_KEY(CONT_OPL, c0, offset + i, b0);
	c1[offset + i] = b1;
      }
    }
    break;
#endif

#if M_TEST_METHOD_P(ERASE_KEY, CONT_OPL)
  case 8:
    if (!c1.empty()) {
      index = index % c1.size();
      p0 = M_CALL_ERASE_KEY(CONT_OPL, c0, index);
#ifndef HAVE_GET_KEY_THROUGH_FIND
      c1.erase(c1.begin() + index);
      p1 = true;
#else
      auto i = c1.find(index);
      if (i != c1.end()) {
        c1.erase(i);
        p1 = true;
      } else {
        p1 = false;
      }
#endif
    }
    break;
#endif

#if M_TEST_METHOD_P(SET, CONT_OPL) && M_TEST_METHOD_P(INIT_SET, CONT_OPL)
  case 9:
    M_CALL_INIT_SET(CONT_OPL, tmp, c0);
#if M_TEST_METHOD_P(EQUAL_P, CONT_OPL)
    assert( M_CALL_EQUAL_P(CONT_OPL, tmp, c0) );
#endif
    M_CALL_SET(CONT_OPL, c0, tmp);
    M_CALL_CLEAR(CONT_OPL, tmp);
    break;
#endif

#if M_TEST_METHOD_P(INIT_MOVE, CONT_OPL)
  case 10:
    M_CALL_INIT_MOVE(CONT_OPL, tmp, c0);
    M_CALL_INIT_MOVE(CONT_OPL, c0, tmp);
    break;
#endif

#if M_TEST_METHOD_P(MOVE, CONT_OPL) && M_TEST_METHOD_P(INIT_MOVE, CONT_OPL)
  case 11:
    M_CALL_INIT_SET(CONT_OPL, tmp, c0);
    M_CALL_MOVE(CONT_OPL, tmp, c0);
    M_CALL_INIT_MOVE(CONT_OPL, c0, tmp);
    break;
#endif

#if M_TEST_METHOD_P(SWAP, CONT_OPL) && M_TEST_METHOD_P(GET_SIZE, CONT_OPL)
  case 12:
    s0 = M_CALL_GET_SIZE(CONT_OPL, c0);
    M_CALL_INIT(CONT_OPL, tmp);
    M_CALL_SWAP(CONT_OPL, tmp, c0);
    s1 = M_CALL_GET_SIZE(CONT_OPL, tmp);
    M_CALL_SWAP(CONT_OPL, tmp, c0);
    M_CALL_CLEAR(CONT_OPL, tmp);
    break;
  case 13:
    M_CALL_INIT_SET(CONT_OPL, tmp, c0);
    M_CALL_SWAP(CONT_OPL, tmp, c0);
    M_CALL_CLEAR(CONT_OPL, tmp);
    break;
#endif

#if M_TEST_METHOD_P(SAFE_GET_KEY, CONT_OPL)
  case 14:
    index = (1 + (c1.size() < 1000000)) * (index % ( c1.size() + 1) );
    M_CALL_SET(M_GET_OPLIST CONT_OPL, b0, *M_CALL_SAFE_GET_KEY(CONT_OPL, c0, index));
#ifndef HAVE_GET_KEY_THROUGH_FIND
    if ((size_t) index >= c1.size() ) {
      c1.resize(index+1);
    }
    b1 = c1[index];
#else
    {
      auto i = c1.find(index);
      if (i == c1.end()) {
#ifdef HAVE_SET_KEY
	c1[index] = 0;
#else
        c1.insert(index);
#endif
        i = c1.find(index);
	assert(i != c1.end());
      }
      b1 = CLASS_IT_TO_INT(i);
    }
#endif
    break;
#endif

#if M_TEST_METHOD_P(IT_CREF, CONT_OPL)
  case 15:
    M_CALL_IT_FIRST(CONT_OPL, it0, c0);
    it1 = c1.begin();
    while (!M_CALL_IT_END_P(CONT_OPL, it0)) {
#ifdef HAVE_KEY_PTR
      // Containers are key / value (through ptr)
      index = *M_CALL_IT_CREF(CONT_OPL, it0)->key_ptr;
      M_CALL_SET(M_GET_OPLIST CONT_OPL, b0, *M_CALL_IT_CREF(CONT_OPL, it0)->value_ptr);
#elif defined(HAVE_SET_KEY)
      // Containers are key / value
      index = M_CALL_IT_CREF(CONT_OPL, it0)->key;
      M_CALL_SET(M_GET_OPLIST CONT_OPL, b0, M_CALL_IT_CREF(CONT_OPL, it0)->value);
#else
      // Containers are single key
      M_CALL_SET(M_GET_OPLIST CONT_OPL, b0, *M_CALL_IT_CREF(CONT_OPL, it0));
      index = b0;
#endif
#ifndef DONT_HAVE_SEQUENCE_IT
      // Container are ordered
      assert(it1 != c1.end());
      b1 = CLASS_IT_TO_INT(it1);
      it1++;
#else
      // Containers are not ordered.
      it1 = c1.find(index);
      assert(it1 != c1.end());
      b1 = CLASS_IT_TO_INT(it1);
#endif
      CMP_BASE(b0, b1);
      M_CALL_IT_NEXT(CONT_OPL, it0);
    }
#ifndef DONT_HAVE_SEQUENCE_IT
    assert(it1 == c1.end());
#endif
    break;
#endif

#if M_TEST_METHOD_P(IT_REF, CONT_OPL)
  case 16:
    M_CALL_IT_FIRST(CONT_OPL, it0, c0);
    it1 = c1.begin();
    while (it1 != c1.end()) {
      assert( !M_CALL_IT_END_P(CONT_OPL, it0));
      TYPE_SET_INT(b0, index);
      CLASS_SET_INT(b1, index);
      M_CALL_SET(M_GET_OPLIST CONT_OPL, TYPE_IT_TO_INT(M_CALL_IT_REF(CONT_OPL, it0)), b0);
      *it1 = b1;
      M_CALL_IT_NEXT(CONT_OPL, it0);
      it1++;
      index = 1457857U * index + 458741U;
    }
    assert(M_CALL_IT_END_P(CONT_OPL, it0));
    break;
#endif
    
#if M_TEST_METHOD_P(IT_LAST, CONT_OPL)
  case 17:
    {
      M_CALL_IT_LAST(CONT_OPL, it0, c0);
      auto it1b = c1.rbegin();
      while (it1b != c1.rend()) {
        assert( !M_CALL_IT_END_P(CONT_OPL, it0));
        M_CALL_SET(M_GET_OPLIST CONT_OPL, b0, TYPE_IT_TO_INT(M_CALL_IT_CREF(CONT_OPL, it0)));
        b1 = CLASS_IT_TO_INT(it1b);
        CMP_BASE(b0, b1);
        M_CALL_IT_PREVIOUS(CONT_OPL, it0);
        it1b++;
      }
      assert(M_CALL_IT_END_P(CONT_OPL, it0));
    }
    break;
#endif

#if M_TEST_METHOD_P(IT_INSERT, CONT_OPL)
  case 18:
    M_CALL_IT_FIRST(CONT_OPL, it0, c0);
    it1 = c1.begin();
    TYPE_SET_INT(b0, index);
    CLASS_SET_INT(b1, index);
#ifdef HAVE_INSERT_AFTER
    while (it1 != c1.end() ) {
      if (rand() > index)
        break;
      M_CALL_IT_NEXT(CONT_OPL, it0);
      it1++;
    }
    if (it1 != c1.end() ) {
      M_CALL_IT_INSERT(CONT_OPL, c0, it0, b0);
      c1.insert_after(it1, b1);
    }
#else
    if (!c1.empty() ){
      index = index % c1.size();
      if (index == 0) {
        M_CALL_IT_END(CONT_OPL, it0, c0);
      } else {
        for(int i = 0; i < index; i++) {
          assert( it1 != c1.end());
          assert( !M_CALL_IT_END_P(CONT_OPL, it0));
          M_CALL_IT_NEXT(CONT_OPL, it0);
          it1++;
        }
        it1++;
      }
    }
    M_CALL_IT_INSERT(CONT_OPL, c0, it0, b0);
#ifdef HAVE_REVERSE_IT
    // in cases of reverse iterator, fix iterator.
    it1 = std::prev(it1, 1);
#endif
    c1.insert(IT(it1), b1);
#endif
    break;
#endif

#if M_TEST_METHOD_P(IT_REMOVE, CONT_OPL)
  case 19:
    M_CALL_IT_FIRST(CONT_OPL, it0, c0);
    it1 = c1.begin();
#ifdef HAVE_ERASE_AFTER
    while (it1 != c1.end() ) {
      M_CALL_IT_NEXT(CONT_OPL, it0);
      if (rand() > index)
        break;
      it1++;
    }
    if (it1 != c1.end() && !M_CALL_IT_END_P(CONT_OPL, it0) ) {
      M_CALL_IT_REMOVE(CONT_OPL, c0, it0);
      c1.erase_after(it1);
    }
#else
    if (!c1.empty() ){
      index = index % c1.size();
      for(int i = 0; i < index; i++) {
        assert( it1 != c1.end());
        assert( !M_CALL_IT_END_P(CONT_OPL, it0));
        M_CALL_IT_NEXT(CONT_OPL, it0);
        it1++;
      }
      M_CALL_IT_REMOVE(CONT_OPL, c0, it0);
      c1.erase(IT(it1));
    }
#endif
    break;
#endif

#if M_TEST_METHOD_P(REVERSE, CONT_OPL)
  case 20:
    M_CALL_REVERSE(CONT_OPL, c0);
    c1.reverse();
    break;
    // 21 is above
#endif

#ifdef HAVE_PUSH_FRONT
  case 22:
    TYPE_SET_INT(b0, index);
    CLASS_SET_INT(b1, index);
    M_C(CONT_NAME, _push_front)(c0, b0);
    c1.push_front(b1);
    break;
    
  case 23:
    index = index % 1000;
    for(int i = 0; i < index; i++) {
      TYPE_SET_INT(b0, i);
      CLASS_SET_INT(b1, i);
      M_C(CONT_NAME, _push_front)(c0, b0);
      c1.push_front(b1);
    }
    break;   
#endif

#ifdef HAVE_POP_FRONT
  case 24:
    if (!c1.empty()) {
      M_C(CONT_NAME, _pop_front)(&b0, c0);
      b1 = c1.front();
      c1.pop_front();
    }
    break;
#endif

#ifdef HAVE_RESIZE
  case 25:
    index = (1 + (c1.size() < 1000000)) * (index % ( c1.size() + 1) );
    M_C(CONT_NAME, _resize)(c0, index);
    c1.resize(index);
    break;
#endif

#ifdef HAVE_RESERVE
  case 26:
    index = c1.size() + ( index % (c1.size() + 1) );
    M_C(CONT_NAME, _reserve)(c0, index);
    c1.reserve(index);
    break;

  case 27:
    M_C(CONT_NAME, _reserve)(c0, 0);
    c1.shrink_to_fit();
    break;
#endif

    // SPLICE_BACK
    // SPLICE_AT
    // Needs SPLICE ?
    
  default:
    M_STATIC_ASSERT( 28 == MAX_TEST, MLIB_INTERNAL_ERROR, "Constant MAX_TEST is no coherent with the effective number of tests");
    // Nothing to do
    break;
  }
  CMP_BASE(b0, b1);
  CMP_CONT(c0, c1);
  assert(p0 == p1);
  assert(s0 == s1);
}

static void init_rand(void)
{
  const char *seed = getenv("CHECK_RANDOMIZE");
  if (seed == NULL) {
    // No check randomize requested.
    // Initialize with a common value
    srand(0x45856525U);
  } else {
    unsigned int seed_value = strtoul (seed, NULL, 10);
    if (seed_value > 1) {
      // Seed has been set by the user. Use it.
      printf("Using seed = %u\n", seed_value);
    } else {
      time_t  tv;
      time (&tv);
      seed_value = tv;
      printf("Generating seed = %u\n", seed_value);
    }
    srand(seed_value);
  }
}

static int read_int(FILE *f)
{
  int c = 0;
  while (true) {
    int n = fscanf(f, " %d", &c);
    if (n == 1) {
      return c;
    }
    if (feof(f)) {
      return 0;
    }
    // Skip next character
    c = fgetc(f);
  }
}

int main(int argc, const char *argv[])
{
  int loop = argc >= 2 ? atoi(argv[1]) : DEFAULT_NUMBER;

  BASE_TYPE  b0;
  BASE_CLASS b1;
  CONT_TYPE  c0;
  CONT_CLASS c1;

  init_rand();
  M_CALL_INIT(CONT_OPL, c0);
  M_CALL_INIT(M_GET_OPLIST CONT_OPL, b0);
  TYPE_SET_INT(b0, 0);
  CLASS_SET_INT(b1, 0);

  if (loop < 0) {
    /* test input is provided from stdin */
    while (!feof(stdin)) {
      int t = read_int(stdin);
      int s = read_int(stdin);
      test(t, s, b0, b1, c0, c1);
    }
  } else {
    for(int i = 0; i < loop; i++) {
      int t = rand();
      int s = rand();
      test(t, s, b0, b1, c0, c1);
    }
  }

  M_CALL_CLEAR(M_GET_OPLIST CONT_OPL, b0);
  M_CALL_CLEAR(CONT_OPL, c0);
  return 0;
}
