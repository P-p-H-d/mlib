# Environnement variable. To customize if needed.
RM=rm -f
MKDIR=mkdir -p
INSTALL=install
INSTALL_PROGRAM=${INSTALL}
INSTALL_DATA=${INSTALL} -m 644

# Installation directory
PREFIX=/usr/local

# Package name & version.
PACKAGE=m*lib-$(VERSION)
VERSION=0.2.3

# Define the contain of the distribution tarball
# TODO: Get theses lists from GIT itself.
HEADER=mm-algo.h m-array.h m-atomic.h m-bitset.h m-bptree.h m-buffer.h m-c-mempool.h m-concurrent.h m-core.h m-deque.h m-dict.h m-genint.h m-i-list.h m-i-shared.h m-list.h m-mempool.h m-mutex.h m-prioqueue.h m-rbtree.h m-serial-json.h m-shared.h m-snapshot.h m-string.h m-tuple.h m-variant.h m-worker.h
DOC1=LICENSE README.md
DOC2=doc/API.txt doc/Container.html  doc/Container.ods doc/DEV.md doc/ISSUES.org doc/depend.png doc/oplist.png
EXAMPLE=example/ex-array01.c  example/ex-array04.c   example/ex-dict02.c  example/ex-grep01.c  example/ex-multi01.c  example/ex-rbtree01.c example/ex-array02.c  example/ex-buffer01.c  example/ex-dict03.c  example/ex-list01.c  example/ex-multi02.c  example/Makefile example/ex-array03.c  example/ex-dict01.c    example/ex-dict04.c  example/ex-mph.c     example/ex-multi03.c
TEST=tests/coverage.h tests/test-malgo.c tests/test-mbptree.c tests/test-mdeque.c tests/test-milist.c    tests/test-mmutex.c      tests/test-mshared.c    tests/test-mtuple.c    tests/test-obj.h     tests/tgen-mdict.c    tests/tgen-openmp.c  tests/wip-mregister.c tests/dict.txt    tests/test-marray.c   tests/test-mbuffer.c  tests/test-mdict.c    tests/test-mlist.c     tests/test-mprioqueue.c  tests/test-msnapshot.c  tests/test-mvariant.c  tests/tgen-bitset.c  tests/tgen-mlist.c    tests/tgen-queue.c tests/Makefile    tests/test-mbitset.c  tests/test-mcore.c    tests/test-mgenint.c  tests/test-mmempool.c  tests/test-mrbtree.c     tests/test-mstring.c    tests/test-mworker.c   tests/tgen-marray.c  tests/tgen-mstring.c  tests/tgen-shared.c

.PHONY: all test check doc clean distclean depend install uninstall dist

all:
	@echo "Nothing to be done."

test:	check

check:
	cd tests && $(MAKE) check
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
