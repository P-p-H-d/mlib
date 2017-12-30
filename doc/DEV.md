How to make a release?
======================

1) Check that all test pass on all primary systems.
   Run 'make check' on all primary systems.
   No error shall be reported.
   
2) Check that no memory leak is reported by valgrind:
   Run 'make valgrind' in tests folder on a system.

3) Check that no undefined behavior, nor data race, ... is reported by sanitizer:
   Run 'make sanitize'  in tests folder on a system.
   This needs at least GCC 7

4) Check that no new warning is reported by static analyser:
   Run 'make scan-build' in tests folder on a system.
   This needs at least clang 4.
   Currently there is a false positive detected in m-shared:
   "Argument to free() is offset by 8 bytes from the start of memory allocated by malloc()"
   scan-build is not able to properly track that this case cannot open.
   
5) Check that coverage is reasonnable.
   Run 'make coverage' and analyze result.

In case of problem, open a problem error.

List of primary systems
=======================

* gcc 4.9 on linux/x86-64
* gcc 7 on linux/x86-64
* clang 3.5 on linux/x86-64
* clang 4 on linux/x86-64
* g++ 7 on linux/x86-64
* tcc on linux/x86-64
