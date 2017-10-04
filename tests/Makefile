#NOTE: Neither GMP not pthread is not a real dependency. It is only used for the test suite.
CC=cc -std=c99
WCFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function -ftrapv
XCFLAGS=
CFLAGS=$(WCFLAGS) $(XCFLAGS)
CPPFLAGS=-I..
LDFLAGS=-pthread -lgmp
RM=rm -f

all:
	@echo "Nothing to be done."

.POSIX:
.PHONY: valgrind scan-build sanitize check clean *.log
.SUFFIXES:
.SUFFIXES: .c .exe .log .depend .valgrind .gcov
.SECONDARY:

.c.exe:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(LDFLAGS) $< -o $@

.c.depend:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM -MT `echo $< |sed -e 's/\.c/\.exe/g'` $< > $@

.exe.log:
	./$<

.exe.valgrind:
	valgrind ./$<

.c.gcov:
	awk '/START_COVERAGE/ { printit=1} (printit == 0) { print }' $< > $<.first.c
	awk '  printit { print } /END_COVERAGE/ { printit=1}' $< > $<.end.c
	$(CC) $(CPPFLAGS) -E -DNDEBUG -DWITHIN_COVERAGE $< |grep -v '^#' |awk ' /END_COVERAGE/ { printit=0} printit { print } /START_COVERAGE/ { printit=1 }' |perl -pi -e "s/;(?![\"'])/;\n/g ; s/}(?![\"'])/}\n/g ; s/{(?![\"'])/{\n/g" > $<.middle.c
	cat $<.first.c $<.middle.c $<.end.c > $<.c
	$(CC) $(CPPFLAGS) $(LDFLAGS) -ftest-coverage -fprofile-arcs -g $<.c -o $<.exe
	./$<.exe
	gcov $<.gcno
	mv $<.c.gcov `basename $< .c`.gcov
	$(RM) $<.first.c $<.end.c $<.middle.c $<.c $<.exe

-include depend


#####################################################################################################
# Test suite
#####################################################################################################

depend:
	$(RM) depend
	echo "" > depend
	$(MAKE) `ls test-*.c|sed -e 's/\.c/\.depend/g'`
	cat *.depend > depend
	$(RM) *.depend

check: depend
	$(MAKE) `ls test-*.c|sed -e 's/\.c/\.log/g'`

clean:
	$(RM) *.exe *.s *~ *.o *.log *.valgrind ./a.dat clang_output_* *.c.c *.end.c *.first.c *.middle.c
	$(RM) -rf coverage *.gcda *.gcno *.gcov all.info test.tar coverage-dir mlib-report

distclean: clean
	$(RM) *.depend depend

#####################################################################################################
# Code generation analysis
#####################################################################################################

gene-list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-mlist.c && less tgen-mlist.s

gene-array:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-marray.c && less tgen-marray.s

gene-shared:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-shared.c && less tgen-shared.s

gene-bitset:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-bitset.c && less tgen-bitset.s


#####################################################################################################
# Coverage
#####################################################################################################

coverage:
	$(MAKE) `ls test-*.c|sed -e 's/\.c/\.gcov/g'`


#####################################################################################################
# Code analyzer tools
#####################################################################################################

valgrind:
	$(MAKE) `ls test-*.c|sed -e 's/\.c/\.valgrind/g'`

scan-build:
	$(RM) -rf ./mlib-report
	scan-build -o ./mlib-report make check XCFLAGS="-std=c99"

sanitize:
	$(MAKE) clean check XCFLAGS="-fsanitize=address,undefined"
	$(MAKE) clean check XCFLAGS="-fsanitize=thread -pie -fPIE"
