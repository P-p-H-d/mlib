How to make a release?
======================

1) Check that all test pass on all primary systems.
   Run 'make check' or 'make checkall' on all primary systems.
   No error shall be reported.
   
2) Check that no undefined behavior, nor data race, ... is reported by sanitizer:
   Run 'make sanitize'  in tests folder on a system.
   This needs at least GCC 7

3) Check that coverage is reasonable.
   Run 'make coverage' and analyze result.

4) Publish the release

In case of problem, open a problem report.


List of tested systems
=======================

* gcc 4.9, 7 & 8 on linux/x86-64
* clang 3.5, 4 & 6 on linux/x86-64
* g++ 7 & 8 on linux/x86-64
* tcc on linux/x86-64
* gcc 7 on windows/x86-64
* gcc 4.9 on linux/powerpc
* gcc 6 on linux/armv7l
* clang 3.8 on linux/armv7l


Atomic library
==============

For target, armel m68k mips mipsel powerpc powerpcspe sh4, you may need to run the test suite:

make check LDFLAGS="-pthread -latomic"

since 'atomic' library is not automatically included for 32 bits systems.


Cross compilation
=================

There is no need to cross compile the library as it is a header only library.

However if you want to run the test with a cross-compiler, do like the following
command. It performs a cross-compilation of the test suite from linux to windows
and run the test suite with wine:

        make CC="i586-mingw32msvc-gcc -std=c99 -DWINVER=0x600 -D_WIN32_WINNT=0x600" LOG_COMPILER=wine check


Handling issues
===============

Issues are opened in the bug tracker of the project (typically https://github.com/P-p-H-d/mlib/issues )

If the issue cannot be closed and taken into account in less than (typically) one month,
or before a release,
it shall be moved into the long term issues included in the repository:
https://github.com/P-p-H-d/mlib/blob/master/doc/ISSUES.org
This is a file in ORG mode: the issues are created with the proper tags.

The original issue in the tracker can then be closed with the issuer agreement.

This will have the following gains:

- no hard dependencies on any bug tracking infrastructure (can be migrated easily). 
- a release will automatically contain all its open problem reports.

Once one issue of ISSUES.org is taken into account,
its state shall be changed to DONE (the issue shall not be removed).


Debugging
===============

Debugging the library can be quite difficult
as it performs heavy preprocessing expansion
and expands all code in one line.
Different strategy for debugging the library itself are still possible:

1) Good old printf.
Add printf of interresting values in the debugged function.


2) Preprocess the library and then compile the preprocessed file:

          cc -std=c99 -E test-file.c > test-file.i
          perl -pi -e 's/;/;\n/g' test-file.i
          cc -std=c99 -c -Wall test-file.i

 The resulting executable can be debugged much more easily.


3) To debug the preprocessing itself, the Boot Wave library can be used
and in particular the [wave driver](https://www.boost.org/doc/libs/1_71_0/libs/wave/doc/wave_driver.html)

 Add in the code the following pragmas around the code to debug:

     	  #pragma wave trace(enable)  
     	  ARRAY_DEF (array_charp, char *)
     	  #pragma wave trace(disable)

Run the wave driver:

    	 ./wave ex-array01.c -I .. --c99 -t test.trace

The different expansion performed by the library are described in test.trace
