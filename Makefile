# Copyright (c) 2017-2020, Patrick Pelissier
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
INSTALL=install
INSTALL_PROGRAM=${INSTALL}
INSTALL_DATA=${INSTALL} -m 644

# Installation directory
PREFIX=/usr/local
DESTDIR=

# Package name & version.
PACKAGE=m_lib-$(VERSION)
VERSION=0.4.1

# Define the contain of the distribution tarball
HEADER=m-algo.h m-array.h m-atomic.h m-bitset.h m-bptree.h m-buffer.h m-c-mempool.h m-concurrent.h m-core.h m-deque.h m-dict.h m-funcobj.h m-genint.h m-i-list.h m-i-shared.h m-list.h m-mempool.h m-mutex.h m-prioqueue.h m-rbtree.h m-serial-bin.h m-serial-json.h m-shared.h m-snapshot.h m-string.h m-tuple.h m-variant.h m-worker.h
DOC1=LICENSE README.md
DOC2=doc/API.txt doc/Container.html doc/Container.ods doc/depend.png doc/DEV.md doc/ISSUES.org doc/oplist.odp doc/oplist.png
EXAMPLE=example/ex-array00.c example/ex-array01.c example/ex-array02.c example/ex-array03.c example/ex-array04.c example/ex-bptree01.c example/ex-buffer01.c example/ex-dict01.c example/ex-dict02.c example/ex-dict03.c example/ex-dict04.c example/ex-grep01.c example/ex-json01.c example/ex-list01.c example/ex-mph.c example/ex-multi01.c example/ex-multi02.c example/ex-multi03.c example/ex-multi04.c example/ex-rbtree01.c example/ex-section.c example/Makefile
TEST=tests/test-malgo.c tests/test-marray.c tests/test-mbitset.c tests/test-mbptree.c tests/test-mbuffer.c tests/test-mcmempool.c tests/test-mconcurrent.c tests/test-mcore.c tests/test-mdeque.c tests/test-mdict.c tests/test-mfuncobj.c tests/test-mgenint.c tests/test-milist.c tests/test-mlist.c tests/test-mmempool.c tests/test-mmutex.c tests/test-mprioqueue.c tests/test-mrbtree.c tests/test-mserial-bin.c tests/test-mserial-json.c tests/test-mshared.c tests/test-msnapshot.c tests/test-mstring.c tests/test-mtuple.c tests/test-mvariant.c tests/test-mworker.c tests/tgen-bitset.c tests/tgen-marray.c tests/tgen-mdict.c tests/tgen-mlist.c tests/tgen-mstring.c tests/tgen-openmp.c tests/tgen-queue.c tests/tgen-shared.c tests/wip-mregister.c tests/Makefile tests/coverage.h tests/test-obj.h tests/dict.txt

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

html:	doc
doc:	README.md doc/depend.png
	markdown < README.md > README.html

clean:
	cd tests && $(MAKE) clean
	cd example && $(MAKE) clean
	cd bench && $(MAKE) clean
	$(RM) README.html depend.dot
	$(RM) -r 'm*lib-$(VERSION)'
	$(RM) 'm*lib-$(VERSION).tar.bz2'

distclean: clean
	cd tests && $(MAKE) distclean

depend:
	cd tests && $(MAKE) depend

doc/depend.png: $(HEADER)
	(echo "digraph g { " ; for i in *.h ; do list=$$(grep "include \"" $$i |cut -f2 -d\") ; for j in $$list ; do echo "\"$$i\" -> \"$$j\" ;" ; done ; done ; echo "}" )> depend.dot
	dot -Tpng depend.dot -o doc/depend.png
	optipng -o7 doc/depend.png

dist: $(HEADER) $(DOC1) $(DOC2) $(EXAMPLE) $(TEST) Makefile clean
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

install:
	$(MKDIR) $(DESTDIR)$(PREFIX)/include
	$(INSTALL_DATA) $(HEADER) $(DESTDIR)$(PREFIX)/include

uninstall:
	for i in $(HEADER) ; do $(RM) $(DESTDIR)$(PREFIX)/include/$$i ; done
