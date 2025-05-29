# Copyright (c) 2017-2025, Patrick Pelissier
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

# Environnement variable. To customize if needed.
RM=rm -f
MKDIR=mkdir -p
RMDIR=rmdir
INSTALL=install
INSTALL_PROGRAM=${INSTALL}
INSTALL_DATA=${INSTALL} -m 644

# Installation directory
PREFIX=/usr/local
DESTDIR=

# Package name & version.
PACKAGE=m_lib-$(VERSION)
VERSION=0.8.1

# Define the contain of the distribution tarball
HEADER=m-algo.h m-array.h m-atomic.h m-bitset.h m-bptree.h m-buffer.h m-core.h m-deque.h m-dict.h m-funcobj.h m-generic.h m-genint.h m-i-list.h m-list.h m-thread.h m-prioqueue.h m-rbtree.h m-serial-bin.h m-serial-json.h m-snapshot.h m-string.h m-tree.h m-try.h m-tuple.h m-variant.h m-worker.h m-bstring.h m-shared-ptr.h m-queue.h
DOC1=LICENSE README.md
DOC2=doc/API.txt doc/Container.html doc/Container.ods doc/depend.png doc/DEV.md doc/ISSUES.org doc/oplist.odp doc/oplist.png doc/bench-array-log.png doc/bench-array.png doc/bench-list-log.png doc/bench-list.png doc/bench-oset-log.png doc/bench-oset.png doc/bench-umap-log.png doc/bench-umap.png doc/cc.sh
EXAMPLE=example/ex11-algo01.c example/ex11-algo02.c example/ex11-json01.c example/ex11-section.c example/ex-algo02.c example/ex-algo03.c example/ex-algo04.c example/ex-array00.c example/ex-array01.c example/ex-array02.c example/ex-array03.c example/ex-array04.c example/ex-array05.c example/ex-bptree01.c example/ex-buffer01.c example/ex-dict01.c example/ex-dict02.c example/ex-dict03.c example/ex-dict04.c example/ex-grep01.c example/ex-list01.c example/ex-mph.c example/ex-multi01.c example/ex11-multi02.c example/ex-multi03.c example/ex-multi04.c example/ex-multi05.c example/ex-rbtree01.c example/ex11-algo02.json example/ex11-json01.json example/Makefile example/ex-defer01.c example/ex-string01.c example/ex-string02.c example/ex-astar.c example/ex-string03.c example/ex11-tstc.c example/ex-no-stdio.c example/ex11-count-lines.c example/ex11-json-net.c example/ex11-rbtree02.c example/ex-curl.c example/ex-dict05.c example/ex-dict06.c  example/exm-lib.c example/exm-main.c example/exn-lib.c example/exn-main.c example/exm-header.h example/exn-header.h example/ex-alloc1.c example/ex-alloc2.c example/ex-try01.c example/ex11-generic01.c example/ex-bptree02.c example/ex-i-list.c example/ex11-generic02.c example/ex11-small-name.c example/ex11-variant01.c example/ex11-use-pool.c
TEST=tests/test-malgo.c tests/test-marray.c tests/test-mbitset.c tests/test-mbptree.c tests/test-mbuffer.c tests/test-mcore.c tests/test-mdeque.c tests/test-mdict.c tests/test-mfuncobj.c tests/test-mgenint.c tests/test-milist.c tests/test-mlist.c tests/test-mmutex.c tests/test-mprioqueue.c tests/test-mrbtree.c tests/test-mserial-bin.c tests/test-mserial-json.c tests/test-msnapshot.c tests/test-mstring.c tests/test-mtuple.c tests/test-mvariant.c tests/test-mworker.c tests/tgen-bitset.c tests/tgen-marray.c tests/tgen-mdict.c tests/tgen-mlist.c tests/tgen-mstring.c tests/tgen-openmp.c tests/tgen-queue.c tests/tgen-mserial.c tests/Makefile tests/coverage.h tests/test-obj.h tests/dict.txt tests/fail-chain-oplist.c  tests/fail-incompatible.c  tests/fail-no-oplist.c tests/check-array.cpp tests/check-deque.cpp tests/check-dplist.cpp tests/check-list.cpp tests/check-rbtree.cpp tests/check-uset.cpp tests/check-generic.hpp tests/test-mtree.c tests/check-bptree-map.cpp tests/check-bptree-set.cpp tests/check-prioqueue.cpp tests/check-umap.cpp tests/test-mtry.c tests/tgen-mmap.c tests/tgen-try.c tests/tgen-tuple.c tests/test-mgeneric.c tests/test-mbstring.c tests/except-array.c tests/except-bitset.c tests/except-deque.c tests/except-list.c tests/except-string.c tests/test-mshared-ptr.c tests/test-mshared-ptr.h tests/test-obj-except.h tests/test-mqueue.c

.PHONY: all test check doc clean distclean depend install uninstall dist

all:
	@echo "Nothing to be done."

test:	check

check:
	cd tests && $(MAKE) check
	cd example && $(MAKE) all

checkall:
	cd tests && $(MAKE) checkall
	cd example && $(MAKE) all

html:	README.md
	markdown < README.md > README.html

doc:	html doc/depend.png

clean:
	cd tests && $(MAKE) clean
	cd example && $(MAKE) clean
	$(RM) README.html depend.dot
	$(RM) -r 'm_lib-$(VERSION)'
	$(RM) 'm_lib-$(VERSION).tar.bz2'

distclean: clean
	cd tests && $(MAKE) distclean

depend:
	cd tests && $(MAKE) depend

doc/depend.png: $(HEADER)
	(echo "digraph g { " ; for i in *.h ; do list=$$(grep "include \"" $$i |cut -f2 -d\") ; for j in $$list ; do echo "\"$$i\" -> \"$$j\" ;" ; done ; done ; echo "}" )> depend.dot
	dot -Tpng depend.dot -o doc/depend.png
	optipng -o7 doc/depend.png

dist: $(HEADER) $(DOC1) $(DOC2) $(EXAMPLE) $(TEST) Makefile clean versioncheck
	$(MKDIR) '$(PACKAGE)'
	$(MKDIR) '$(PACKAGE)/doc'
	$(MKDIR) '$(PACKAGE)/tests'
	$(MKDIR) '$(PACKAGE)/example'
	cp $(HEADER) $(DOC1) Makefile '$(PACKAGE)'
	cp $(EXAMPLE) '$(PACKAGE)/example'
	cp $(TEST) '$(PACKAGE)/tests'
	cp $(DOC2) '$(PACKAGE)/doc'
	cd '$(PACKAGE)/tests' && $(MAKE) depend
	tar jcf '$(PACKAGE).tar.bz2' '$(PACKAGE)'

distcheck: dist
	cd '$(PACKAGE)' && make check

versioncheck:
	@if test "$(VERSION)." != `grep M_CORE_VERSION m-core.h |awk '{printf "%d.",$$3 } END {printf "\n"}'` ; then echo "ERROR: Version mismatch between Makefile & HEADERS" ; exit 2 ; fi
	@for i in *.h ; do echo $(HEADER) | grep -q $$i ; if test $$? -ne 0 ; then echo "ERROR: Missing header $$i in Makefile" ; exit 3 ; fi ; done
	@for i in doc/* ; do echo $(DOC2) | grep -q $$i ; if test $$? -ne 0 ; then echo "ERROR: Missing document $$i in Makefile" ; exit 4 ; fi ; done
	@for i in tests/*.c tests/*.txt tests/*.h ; do echo $(TEST) | grep -q $$i ; if test $$? -ne 0 ; then echo "ERROR: Missing test $$i in Makefile" ; exit 5 ; fi ; done
	@for i in example/*.c example/*.json ; do echo $(EXAMPLE) | grep -q $$i ; if test $$? -ne 0 ; then echo "ERROR: Missing example $$i in Makefile" ; exit 6 ; fi ; done
	@for i in tests/*.cpp tests/*.hpp ; do echo $(TEST) | grep -q $$i ; if test $$? -ne 0 ; then echo "ERROR: Missing test $$i in Makefile" ; exit 7 ; fi ; done

install:
	$(MKDIR) $(DESTDIR)$(PREFIX)/include/m-lib
	$(INSTALL_DATA) $(HEADER) $(DESTDIR)$(PREFIX)/include/m-lib

uninstall:
	for i in $(HEADER) ; do $(RM) $(DESTDIR)$(PREFIX)/include/m-lib/$$i ; done
	$(RMDIR) $(DESTDIR)$(PREFIX)/include/m-lib/

format:
	cd example && $(MAKE) ex-string01.exe
	for i in $(HEADER) ; do ./example/ex-string01.exe $$i > tmp.h && mv tmp.h $$i ; done
