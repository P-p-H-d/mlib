# Copyright (c) 2017-2023, Patrick Pelissier
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# + Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# + Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CC=cc -std=c99
CXX=c++ -std=c++11
WCFLAGS_GCC=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wunused-function -ftrapv -Wsign-conversion -Wdate-time -Wconversion -Wformat=2 -Winit-self -Wjump-misses-init -Wstack-protector -Wtrampolines -Wwrite-strings -Wformat-security -Wno-float-conversion -O
WCFLAGS_CLANG=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wunused-function -ftrapv -Wsign-conversion -O
WCFLAGS_CPP=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wstrict-overflow=5 -Winline -Wundef -Wcast-qual -Wshadow -Wunreachable-code -Wstrict-aliasing=2 -Wredundant-decls -Wunused-function -ftrapv -Wsign-conversion
XCFLAGS=
CFLAGS=$(WCFLAGS_GCC) $(XCFLAGS)
CPPFLAGS=-I..
LDFLAGS=-pthread
RM=rm -f
RMDIR=rm -rf
LOG_COMPILER=
SED=sed -e

all:
	@echo "Nothing to be done."


#####################################################################################################
# Rules
#####################################################################################################

.POSIX:
.PHONY: scan-build sanitize check clean *.log coverage synthesis.synt
.SUFFIXES:
.SUFFIXES: .c .exe .log .depend .gcov .analyzer .cpp .fuzz
.SECONDARY:

.c.exe:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $< -o $@ $(LDFLAGS)

.cpp.exe:
	$(CXX) $(WCFLAGS_CPP) -Wno-sign-conversion -Wno-inline $(XCFLAGS) $(CPPFLAGS) -g $< -o $@ $(LDFLAGS)

.c.depend:
	$(CC) $(CPPFLAGS) -MM -MT `echo $< |$(SED) 's/\.c/\.exe/g'` $< > $@

.cpp.depend:
	$(CXX) $(CPPFLAGS) -MM -MT `echo $< |$(SED) 's/\.cpp/\.exe/g'` $< > $@

.exe.log:
	$(LOG_COMPILER) ./$<

.c.gcov:
	awk '/START_COVERAGE/ { printit=1} (printit == 0) { print }' $< > $<.first.c
	awk '  printit { print } /END_COVERAGE/ { printit=1}' $< > $<.end.c
	$(CC) $(CPPFLAGS) -E -DNDEBUG -DWITHIN_COVERAGE $< |grep -v '^#' |awk ' /END_COVERAGE/ { printit=0} printit { print } /START_COVERAGE/ { printit=1 }' |perl -p -e "s/;(?![\"'])/;\n/g ; s/}(?![\"'])/}\n/g ; s/{(?![\"'])/{\n/g" > $<.middle.c
	cat $<.first.c $<.middle.c $<.end.c > $<.c
	$(CC) $(CPPFLAGS) -DBUILD_COVERAGE -ftest-coverage -fprofile-arcs -O0 -g $<.c -o $<.exe $(LDFLAGS)
	$(LOG_COMPILER) ./$<.exe
	LANG=C gcov -b $<.gcno >  `basename $< .c`.synt
	cat `basename $< .c`.synt
	mv $<.c.gcov `basename $< .c`.gcov
	$(RM) $<.first.c $<.end.c $<.middle.c $<.c $<.exe

.cpp.fuzz:
	afl-g++ $(WCFLAGS_CPP) $(CPPFLAGS) -Wno-sign-conversion -Wno-inline -Wno-conversion -O2 -fsanitize=address,undefined,leak -m32 $< -o $@.exe
	mkdir -p fuzzer-testsuite
	for i in $$(seq 1 1000) ; do printf "%d " $$RANDOM ; done > fuzzer-testsuite/t1
	afl-fuzz -i fuzzer-testsuite/ -o fuzzer-findings -m 800 ./$@.exe -1

-include depend

depend:
	$(RM) depend
	echo "" > depend
	$(MAKE) `ls test-*.c|$(SED) 's/\.c/\.depend/g'` `ls except-*.c|$(SED) 's/\.c/\.depend/g'`
	$(CXX) --version 2> /dev/null && $(MAKE) `ls check-*.cpp|$(SED) 's/\.cpp/\.depend/g'` || echo "Skip C++ tests"
	cat *.depend > depend
	$(RM) *.depend


#####################################################################################################
# Main target
#####################################################################################################

# This target generates all executable to be tested by CodeQL
exe: depend
	$(MAKE) `ls test-*.c|$(SED) 's/\.c/\.exe/g'` CC="$(CC)" CFLAGS="$(CFLAGS)"
	@echo All executable generated

check: depend
	$(MAKE) `ls test-*.c|$(SED) 's/\.c/\.log/g'` `ls fail-*.c|$(SED) 's/\.c/\.log.err/g'` `ls except-*.c|$(SED) 's/\.c/\.log/g'` CC="$(CC)" CFLAGS="$(CFLAGS)"
	@echo All tests passed

check-stl: depend
	$(MAKE) `ls check-*.cpp|$(SED) 's/\.cpp/\.log/g'` CXX="$(CXX)" XCFLAGS="-O2 -fsanitize=address,undefined,leak"
	@echo All comparison against C++ STL passed

clean:
	$(RM) *.exe *.s *~ *.o *.log *.log.err a*.dat
	$(RM) *.gcda *.gcno *.gcov all.info test.tar *.synt clang_output_* *.c.c *.end.c *.first.c *.middle.c *.analyzer
	$(RMDIR) coverage coverage-dir mlib-report
	$(RMDIR) fuzzer-testsuite fuzzer-findings

distclean: clean
	$(RM) *.depend depend


#####################################################################################################
# Proper failure generation (test if the code properly fails to compile)
#####################################################################################################

# TODO: should grep failure message?
FAIL_BASE=
FAIL_SEQ=
fail-generic.log.err:
	for i in `seq 1 $(FAIL_SEQ)` ; do \
	if $(CC) $(CFLAGS) $(CPPFLAGS) -g -c $(FAIL_BASE).c -o tmp.o -DTEST=$$i 2> $(FAIL_BASE)-$$i.log ; then echo "$(FAIL_BASE) -- TEST $$i : ERROR (compilation succeeded)" ; false ; elif grep -q "M_LIB_" $(FAIL_BASE)-$$i.log ; then echo "$(FAIL_BASE) -- TEST $$i : OK" ; else echo "$(FAIL_BASE) -- TEST $$i : WARNING (Not a M*LIB detected error)" ; fi \
	done

fail-no-oplist.log.err:
	@$(MAKE) fail-generic.log.err FAIL_BASE=fail-no-oplist FAIL_SEQ=44
fail-chain-oplist.log.err:
	@$(MAKE) fail-generic.log.err FAIL_BASE=fail-chain-oplist FAIL_SEQ=24
fail-incompatible.log.err:
	@$(MAKE) fail-generic.log.err FAIL_BASE=fail-incompatible FAIL_SEQ=24


#####################################################################################################
# Code generation analysis
#####################################################################################################

gene-list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-mlist.c && less tgen-mlist.s

gene-array:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-marray.c && less tgen-marray.s

gene-shared:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-shared.c && less tgen-shared.s

gene-bitset:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-bitset.c && less tgen-bitset.s

gene-dict:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-mdict.c && less tgen-mdict.s

gene-map:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-mmap.c && less tgen-mmap.s

gene-queue:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-queue.c && less tgen-queue.s

gene-string:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S tgen-mstring.c && less tgen-mstring.s

gene-serial:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S -std=c11 tgen-mserial.c && less tgen-mserial.s

gene-try:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S -std=c11 tgen-try.c && less tgen-try.s

gene-tuple:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -march=native -DNDEBUG -S -std=c11 tgen-tuple.c && less tgen-tuple.s

#####################################################################################################
# Coverage
#####################################################################################################

coverage:
	@$(MAKE) `ls test-*.c|$(SED) 's/\.c/\.gcov/g'` CC="cc -std=c11"
	@$(MAKE) synthesis.synt

SYNTHESIS_DATA=	M-ALGO test-malgo.c.c test-malgo.synt			\
		M-ARRAY test-marray.c.c test-marray.synt				\
		M-BITSET ../m-bitset.h test-mbitset.synt				\
		M-BBPTREE test-mbptree.c test-mbptree.synt				\
		M-BSTRING ../m-bstring.h test-mbstring.synt 		    \
		M-BUFFER test-mbuffer.c.c test-mbuffer.synt				\
		M-CONCURRENT test-mconcurrent.c.c test-mconcurrent.synt	\
		M-CORE ../m-core.h test-mcore.synt					    \
		M-DEQUE test-mdeque.c.c test-mdeque.synt				\
		M-DICT test-mdict.c.c test-mdict.synt					\
		M-FUNCOBJ test-mfuncobj.c.c test-mfuncobj.synt			\
		M-GENINT ../m-genint.h test-mgenint.synt				\
		M-I-LIST test-milist.c.c test-milist.synt				\
		M-I-SHARED test-mishared.c.c test-mishared.synt			\
		M-QUEUE test-mqueue.c.c test-mqueue.synt			    \
		M-LIST test-mlist.c.c test-mlist.synt					\
		M-MEMPOOL test-mmempool.c.c test-mmempool.synt			\
		M-PRIOQUEUE test-mprioqueue.c.c test-mprioqueue.synt	\
		M-RBTREE test-mrbtree.c.c test-mrbtree.synt				\
		M-SERIAL-BIN ../m-serial-bin.h test-mserial-bin.synt	\
		M-SERIAL-JSON ../m-serial-json.h test-mserial-json.synt	\
		M-SHARED-PTR test-mshared-ptr.c.c test-mshared-ptr.synt	\
		M-SHARED test-mshared.c.c test-mshared.synt				\
		M-SNAPSHOT test-msnapshot.c.c test-msnapshot.synt		\
		M-STRING ../m-string.h test-mstring.synt 				\
		M-TREE test-mtree.c.c test-mtree.synt 				    \
		M-THREAD ../m-thread.h test-mmutex.synt					\
		M-TRY ../m-try.h test-mtry.synt							\
		M-TUPLE test-mtuple.c.c test-mtuple.synt 				\
		M-VARIANT test-mvariant.c.c test-mvariant.synt 			\
		M-WORKER ../m-worker.h test-mworker.synt		

synthesis.synt:
	@printf "+------------------+------------------+------------------+------------------+\n"
	@printf "|    Functions     | Instruction Cov. |   Decision Cov.  |     MCDC Cov.    |\n"
	@printf "+------------------+------------------+------------------+------------------+\n"
	@for i in $(SYNTHESIS_DATA) ; do 							\
	if test "$$j" = "" ; then j=$$i ; continue ; fi;			\
	if test "$$k" = "" ; then k=$$i ; continue ; fi;			\
	printf "| %16.16s | %16.16s | %16.16s | %16.16s |\n"	"$$j"	\
	"`grep -v Creating $$i|grep $$k -F -A1 |tail -1|cut -f2 -d':'`" 	\
	"`grep -v Creating $$i|grep $$k -F -A2 |tail -1|cut -f2 -d':'`" 	\
	"`grep -v Creating $$i|grep $$k -F -A3 |tail -1|cut -f2 -d':'`"; 	\
	j="";														\
	k="";														\
	done                                                        > synthesis.synt
	@cat synthesis.synt
	@printf "+------------------+------------------+------------------+------------------+\n"


#####################################################################################################
# Code analyzer tools
#####################################################################################################

# Static analysis of the code using clang scan-build
scan-build: clean
	$(RM) -rf ./mlib-report
	scan-build -o ./mlib-report $(MAKE) exe XCFLAGS="-std=c99"

# Relaunch the test suite with the sanitizer.
sanitize:
	$(MAKE) clean
	$(MAKE) check XCFLAGS="-fsanitize=address,undefined,leak"
	$(MAKE) clean
	$(MAKE) check XCFLAGS="-fsanitize=thread -pie -fPIE"


# Static analysis of the code using GCC "-fanalyzer" option
# Need GCC >= 10
# Preprocess the file before given it to the analyser (just like coverage) to have better report.
.c.analyzer:
	awk '/START_COVERAGE/ { printit=1} (printit == 0) { print }' $< > $<.first.c
	awk '  printit { print } /END_COVERAGE/ { printit=1}' $< > $<.end.c
	$(CC) $(CPPFLAGS) -E -DWITHIN_ANALYZER $< |grep -v '^#' |awk ' /END_COVERAGE/ { printit=0} printit { print } /START_COVERAGE/ { printit=1 }' |perl -p -e "s/;(?![\"'])/;\n/g ; s/}(?![\"'])/}\n/g ; s/{(?![\"'])/{\n/g" > $<.middle.c
	cat $<.first.c $<.middle.c $<.end.c > $<.c
	$(CC) $(CPPFLAGS) -fanalyzer -Wall -W -Wno-analyzer-malloc-leak -Wno-stringop-truncation -O2 -g $<.c -o $<.exe 2> `basename $< .c`.analyzer $(LDFLAGS)
	$(RM) $<.first.c $<.end.c $<.middle.c $<.c $<.exe

analyze:
	$(MAKE) `ls test-*.c|$(SED) 's/\.c/\.analyzer/g'`


#####################################################################################################
# Test with the maximum representative number of compiler and flags.
#####################################################################################################

checkall:
	@$(MAKE) clean ; $(MAKE) check CC="gcc -std=c99 -Werror"
	@$(MAKE) clean ; $(MAKE) check CC="gcc -std=c99 -m32 -Werror"
	@$(MAKE) clean ; $(MAKE) check CC="gcc -std=c11 -Werror"
	@$(MAKE) clean ; $(MAKE) check CC="gcc -std=c11 -m32 -Werror"
	@if which clang ; then $(MAKE) clean ; $(MAKE) check CC="clang -std=c99 -Werror" CFLAGS="$(WCFLAGS_CLANG)" ; else echo "SYSTEM CLANG not found: SKIP TEST" ; fi
	@if which g++ ; then $(MAKE) clean ; $(MAKE) check CC="g++ -std=c++11 -Werror" CFLAGS="$(WCFLAGS_CPP)" ; else echo "SYSTEM G++ not found: SKIP TEST" ; fi
	@if which clang++ ; then $(MAKE) clean ; $(MAKE) check CC="clang++ -std=c++11" CFLAGS="$(WCFLAGS_CLANG)" ; else echo "SYSTEM CLANG++ not found: SKIP TEST" ; fi
	@if which tcc ; then $(MAKE) clean ; $(MAKE) check CC="tcc -std=c99 -Werror" ; else echo "SYSTEM TCC not found: SKIP TEST" ; fi
	@if which musl-gcc ; then $(MAKE) clean ; $(MAKE) check CC="musl-gcc -std=c11 -Werror" ; else echo "SYSTEM MUSL-GCC not found: SKIP TEST" ; fi
	@for f in /opt/gcc* ; do if test -x "$$f/bin/gcc" ; then $(MAKE) clean ; $(MAKE) check CC="$$f/bin/gcc -std=c99 -Werror" ; fi ; done
	@for f in /opt/clang* ; do if test -x "$$f/bin/clang" ; then $(MAKE) clean ; $(MAKE) check CC="$$f/bin/clang -std=c99 -Werror" CFLAGS="$(WCFLAGS_CLANG)" ; fi ; done
	@for f in /opt/tcc* ; do if test -x "$$f/bin/tcc" ; then $(MAKE) clean ; $(MAKE) check CC="$$f/bin/tcc -std=c99 -Werror" ; fi ; done
