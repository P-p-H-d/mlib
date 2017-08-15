/*
 * This source file is part of the bstring string library as cppbench.cpp
 * The bstring string library is written by Paul Hsieh in 2002-2015, 
 * and is covered by the BSD open source license. See :
 * * https://github.com/websnarf/bstrlib
 * * http://bstring.sourceforge.net/
 * * http://prdownloads.sourceforge.net/bstring/examples-09282006.zip?download

 Copyright (c) 2014, Paul Hsieh
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of bstrlib nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Update of integrating M*LIB done by Patrick Pelissier */

#include <stdio.h>
#include <time.h>
#include <limits.h>

#ifdef BENCH_CAN_USE_MSTARLIB
#define NDEBUG
#include "m-string.h"
#endif

#ifdef BENCH_CAN_USE_BSTRLIB
#include "bstrlib.h"
#include "bstrwrap.h"
#include "bstraux.h"
#endif

#ifdef BENCH_CAN_USE_LIBSRT
#include "sstring.h"
#endif

/*
 *  Benchmark based on the features tested by benchmark seen here:
 *  http://www.utilitycode.com/str/performance.aspx
 *
 *  The idea is to measure the performance empty constructors, char * 
 *  constructors, assignment, concatenation and scanning.  But rather than
 *  comparing MFC's CString to Str Library, we are going to compare STL to
 *  CBString.
 *
 *  The results are interesting.  CBString basically beats std::string quite 
 *  severely, except for the empty constructor.
 */
#if defined (BENCH_CAN_USE_STL)
#include <string>
#endif

#define TEST_SECONDS (5)

int timeTest (double &res, int (*testfn) (int count), int count) {
	clock_t t0, t1;
	int p = 0;
	double x, c = 0;

	t0 = clock ();

	do {
		p += testfn (count);
		t1 = clock ();

		c += count;
		if (t1 == t0) {
			if (count < (INT_MAX / 2)) count += count;
			continue;
		}

		if (t1 - t0 >= TEST_SECONDS*CLOCKS_PER_SEC) break;

		x = (TEST_SECONDS*CLOCKS_PER_SEC - (t1 - t0)) * c / (t1 - t0);

		if (x > INT_MAX) x = INT_MAX;
		count = (int) x;
		if (count < 1000) count = 1000;

	} while (1);

	res = c * CLOCKS_PER_SEC / (t1 - t0);
	return p;
}

#define TESTSTRING1 ("<sometag name=\"John Doe\" position=\"Executive VP Marketing\"/>")

#if defined (BENCH_CAN_USE_STL)
int testSTL_emptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    std::string b;
    c += b.length () ^ i;
  }
  return c;
}

int testSTL_nonemptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    std::string b (TESTSTRING1);
    c += b.length () ^ i;
  }
  return c;
}

int testSTL_cstrAssignment (int count) {
  int i, c = 0;
  std::string b;
  
  for (i=0; i < count; i++) {
    b = TESTSTRING1;
    c += b.length () ^ i;
  }
  return c;
}

int testSTL_extraction (int count) {
  int i, c = 0;
  std::string b (TESTSTRING1);

  for (i=0; i < count; i++) {
    c += b[(i & 7)];
    c += b[(i & 7) ^ 8];
    c += b.c_str()[(i & 7) ^ 4] ^ i;
  }
  return c;
}

int testSTL_scan (int count) {
int i, c = 0;
 std::string b ("Dot. 123. Some more data.");
 
 for (i=0; i < count; i++) {
   c += b.find ('.');
   c += b.find ("123");
   c += b.find_first_of ("sm") ^i;
 }
 return c;
}

int testSTL_concat (int count) {
  int i, j, c = 0;
  std::string a (TESTSTRING1);
  std::string accum;
  
  for (j=0; j < count; j++) {
    accum = "";
    for (i=0; i < 250; i++) {
      accum += a;
      accum += "!!";
      c += accum.length() ^i;
    }
  }
  return c;
}

int testSTL_replace (int count) {
  int j, c = 0;
  std::string a (TESTSTRING1);
  
  for (j=0; j < count; j++) {
    a.replace (11, 4, "XXXXXX");
    a.replace (23, 2, "XXXXXX");
    a.replace ( 4, 8, "XX");
    c += a.length () ^ j;
  }
  return c;
}

#endif

#ifdef BENCH_CAN_USE_BSTRLIB
int testCBS_emptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    CBString b;
    c += b.length ()^i;
  }
  return c;
}

int testCBS_nonemptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    CBString b (TESTSTRING1);
    c += b.length () ^i;
  }
  return c;
}

int testCBS_cstrAssignment (int count) {
  int i, c = 0;
  CBString b;
  
  for (i=0; i < count; i++) {
    b = TESTSTRING1;
    c += b.length () ^i;
  }
  return c;
}

int testCBS_extraction (int count) {
  int i, c = 0;
  CBString b (TESTSTRING1);
  
  for (i=0; i < count; i++) {
    c += b[(i & 7)];
    c += b[(i & 7) ^ 8];
    c += ((const char *)b)[(i & 7) ^ 4] ^i;
  }
  return c;
}

int testCBS_scan (int count) {
  int i, c = 0;
  CBString b ("Dot. 123. Some more data.");
  
  for (c=i=0; i < count; i++) {
    c += b.find ('.');
    c += b.find ("123");
    c += b.findchr ("sm") ^i;
  }
  return c;
}

int testCBS_concat (int count) {
  int i, j, c = 0;
  CBString a (TESTSTRING1);
  CBString accum;
  
  for (j=0; j < count; j++) {
    accum = "";
    for (i=0; i < 250; i++) {
      accum += a;
      accum += "!!";
      c+= accum.length() ^i;
    }
  }
  return c;
}

int testCBS_replace (int count) {
  int j, c = 0;
  CBString a (TESTSTRING1);

  for (j=0; j < count; j++) {
    a.replace (11, 4, "XXXXXX");
    a.replace (23, 2, "XXXXXX");
    a.replace ( 4, 8, "XX");
    c += a.length () ^j;
  }
  return c;
}
#endif

#ifdef BENCH_CAN_USE_LIBSRT
int testSRT_emptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    ss_t *b = ss_alloc(0);
    c += ss_size(b) ^i;
    ss_free(&b);
  }
  return c;
}

int testSRT_nonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    ss_t *b = ss_dup_c(TESTSTRING1);
    c += ss_size(b) ^i;
    ss_free(&b);
  }
  return c;
}

int testSRT_cstrAssignment (int count) {
  int i, c = 0;
  ss_t *b = ss_alloc(0);
  for (c=i=0; i < count; i++) {
    ss_cpy_cn(&b, TESTSTRING1, strlen(TESTSTRING1));
    c += ss_size(b) ^i;
  }
  ss_free(&b);
  return c;
}

int testSRT_extraction (int count) {
  int i, c = 0;
  ss_t *b = ss_dup_c(TESTSTRING1);
  for (c=i=0; i < count; i++) {
    c += ss_at(b,(i & 7));
    c += ss_at(b,(i & 7) ^ 8);
    c += ss_at(b,(i & 7) ^ 4) ^i;
  }
  ss_free(&b);
  return c;
}

int testSRT_scan (int count) {
  int i, c = 0;
  ss_t *b = ss_dup_c("Dot. 123. Some more data.");
  for (c=i=0; i < count; i++) {
    c += ss_findc (b, 0, '.');
    c += ss_find_cn (b, 0, "123", strlen("123"));
    //c += ss_pbrk (b, 0, "sm") ^i;
    c += ss_findb (b, 0) ^i;
  }
  ss_free(&b);
  return c;
}


int testSRT_concat (int count) {
  int i, j, c = 0;
  ss_t *a = ss_dup_c(TESTSTRING1);
  ss_t *accum = ss_alloc(0);

  for (j=0; j < count; j++) {
    ss_cpy_c(&accum, "");
    for (i=0; i < 250; i++) {
      ss_cat(&accum, a, NULL);
      ss_cat_c(&accum, "!!", NULL);
      c += ss_size(accum) ^i;
    }
  }
  ss_free(&a);
  ss_free(&accum);
  return c;
}

#endif

#ifdef BENCH_CAN_USE_MSTARLIB
int testMLIB_emptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    string_t b;
    string_init(b);
    c += string_size(b) ^i;
    string_clear(b);
  }
  return c;
}

int testMLIB_nonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    string_t b;
    string_init_set_str(b, TESTSTRING1);
    c += string_size(b) ^i;
    string_clear(b);
  }
  return c;
}

int testMLIB_cstrAssignment (int count) {
  int i, c = 0;
  string_t b;
  string_init(b);
  for (c=i=0; i < count; i++) {
    string_set_str(b, TESTSTRING1);
    c += string_size(b) ^i;
  }
  string_clear(b);
  return c;
}

int testMLIB_extraction (int count) {
  int i, c = 0;

  string_t b;
  string_init_set_str(b, TESTSTRING1);
  
  for (c=i=0; i < count; i++) {
    c += string_get_char(b, (i & 7));
    c += string_get_char(b, (i & 7) ^ 8);
    c += string_get_char(b, (i & 7) ^ 4) ^i;
  }
  string_clear(b);
  return c;
}

int testMLIB_scan (int count) {
  int i, c = 0;
  string_t b;
  string_init_set_str(b, "Dot. 123. Some more data.");

  for (c=i=0; i < count; i++) {
    c += string_search_char (b, '.');
    c += string_search_str (b, "123");
    c += string_search_pbrk (b, "sm") ^i;
  }
  string_clear(b);
  return c;
}


int testMLIB_concat (int count) {
  int i, j, c = 0;
  string_t a, accum;
  string_init_set_str(a, TESTSTRING1);
  string_init (accum);

  for (j=0; j < count; j++) {
    string_set_str(accum, "");
    for (i=0; i < 250; i++) {
      string_cat(accum, a);
      string_cat_str(accum, "!!");
      c += string_size(accum) ^i;
    }
  }
  string_clear(a);
  string_clear(accum);
  return c;
}

int testMLIB_replace (int count) {
  int j, c = 0;
  string_t a;
  string_init_set_str(a, TESTSTRING1);
  
  for (j=0; j < count; j++) {
    string_replace_at(a, 11, 4, "XXXXXX");
    string_replace_at(a, 23, 2, "XXXXXX");
    string_replace_at(a, 4, 8, "XX");
    c += string_size(a) ^j;
  }
  
  string_clear(a);
  return c;
}
#endif

#define NTESTS 7
struct flags {
	int runtest[NTESTS];
};

int benchTest (const struct flags * runflags) {
	int c = 0;
	double cps;

#if defined (BENCH_CAN_USE_STL)
	if (runflags->runtest[0]) {
		c += timeTest (cps, testSTL_emptyCtor, 100000);
		printf ("std::string empty constructor:     %20.1f per second\n", cps);
	}
	if (runflags->runtest[1]) {
		c += timeTest (cps, testSTL_nonemptyCtor, 100000);
		printf ("std::string non-empty constructor: %20.1f per second\n", cps);
	}
	if (runflags->runtest[2]) {
		c += timeTest (cps, testSTL_cstrAssignment, 100000);
		printf ("std::string char * assignment:     %20.1f per second\n", cps);
	}
	if (runflags->runtest[3]) {
		c += timeTest (cps, testSTL_extraction, 100000);
		printf ("std::string char extraction:       %20.1f per second\n", cps);
	}
	if (runflags->runtest[4]) {
		c += timeTest (cps, testSTL_scan, 100000);
		printf ("std::string scan:                  %20.1f per second\n", cps);
	}
	if (runflags->runtest[5]) {
		c += timeTest (cps, testSTL_concat, 10);
		printf ("std::string concatenation:         %20.1f per second\n", cps * 250);
	}
	if (runflags->runtest[6]) {
		c += timeTest (cps, testSTL_replace, 10000);
		printf ("std::string replace:               %20.1f per second\n", cps);
	}
#endif

#ifdef BENCH_CAN_USE_BSTRLIB
	if (runflags->runtest[0]) {
		c += timeTest (cps, testCBS_emptyCtor, 100000);
		printf ("CBString empty constructor:        %20.1f per second\n", cps);
	}
	if (runflags->runtest[1]) {
		c += timeTest (cps, testCBS_nonemptyCtor, 100000);
		printf ("CBString non-empty constructor:    %20.1f per second\n", cps);
	}
	if (runflags->runtest[2]) {
		c += timeTest (cps, testCBS_cstrAssignment, 100000);
		printf ("CBString char * assignment:        %20.1f per second\n", cps);
	}
	if (runflags->runtest[3]) {
		c += timeTest (cps, testCBS_extraction, 100000);
		printf ("CBString char extraction:          %20.1f per second\n", cps);
	}
	if (runflags->runtest[4]) {
		c += timeTest (cps, testCBS_scan, 100000);
		printf ("CBString scan:                     %20.1f per second\n", cps);
	}
	if (runflags->runtest[5]) {
		c += timeTest (cps, testCBS_concat, 10);
		printf ("CBString concatenation:            %20.1f per second\n", cps * 250);
	}
	if (runflags->runtest[6]) {
		c += timeTest (cps, testCBS_replace, 10000);
		printf ("CBString replace:                  %20.1f per second\n", cps);
	}
#endif

#ifdef BENCH_CAN_USE_LIBSRT
	if (runflags->runtest[0]) {
		c += timeTest (cps, testSRT_emptyCtor, 100000);
		printf ("LIBSRT string empty constructor:   %20.1f per second\n", cps);
	}
	if (runflags->runtest[1]) {
		c += timeTest (cps, testSRT_nonemptyCtor, 100000);
		printf ("LIBSRT string non-empty constructor:%20.1f per second\n", cps);
	}
	if (runflags->runtest[2]) {
		c += timeTest (cps, testSRT_cstrAssignment, 100000);
		printf ("LIBSRT string char * assignment:   %20.1f per second\n", cps);
	}
	if (runflags->runtest[3]) {
		c += timeTest (cps, testSRT_extraction, 100000);
		printf ("LIBSRT string char extraction:     %20.1f per second\n", cps);
	}
	if (runflags->runtest[4]) {
		c += timeTest (cps, testSRT_scan, 100000);
		printf ("LIBSRT string scan:                %20.1f per second\n", cps);
	}
	if (runflags->runtest[5]) {
		c += timeTest (cps, testSRT_concat, 10);
		printf ("LIBSRT string concatenation:       %20.1f per second\n", cps * 250);
	}
#endif

#ifdef BENCH_CAN_USE_MSTARLIB
	if (runflags->runtest[0]) {
		c += timeTest (cps, testMLIB_emptyCtor, 100000);
		printf ("M*LIB string empty constructor:    %20.1f per second\n", cps);
	}
	if (runflags->runtest[1]) {
		c += timeTest (cps, testMLIB_nonemptyCtor, 100000);
		printf ("M*LIB string non-empty constructor:%20.1f per second\n", cps);
	}
	if (runflags->runtest[2]) {
		c += timeTest (cps, testMLIB_cstrAssignment, 100000);
		printf ("M*LIB string char * assignment:    %20.1f per second\n", cps);
	}
	if (runflags->runtest[3]) {
		c += timeTest (cps, testMLIB_extraction, 100000);
		printf ("M*LIB string char extraction:      %20.1f per second\n", cps);
	}
	if (runflags->runtest[4]) {
		c += timeTest (cps, testMLIB_scan, 100000);
		printf ("M*LIB string scan:                 %20.1f per second\n", cps);
	}
	if (runflags->runtest[5]) {
		c += timeTest (cps, testMLIB_concat, 10);
		printf ("M*LIB string concatenation:        %20.1f per second\n", cps * 250);
	}
	if (runflags->runtest[6]) {
		c += timeTest (cps, testMLIB_replace, 10000);
		printf ("M*LIB string replace:              %20.1f per second\n", cps);
	}
#endif
	return c;
}

int main (int argc, char * argv[]) {
  struct flags runflags;
  int i, j;
  
  for (i=0; i < NTESTS; i++) runflags.runtest[i] = argc < 2;
  for (i=1; i < argc; i++) {
    if (1 == sscanf (argv[i], "%d", &j)) {
      if (j >= 1 && j <= NTESTS) runflags.runtest[j-1] = 1;
    }
  }
  
  benchTest (&runflags);
  return 0;
}
