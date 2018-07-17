PREFIX=/usr/local
RM=rm -f
MKDIR=mkdir -p

.PHONY: all test check doc clean distclean depend depend.png install

all:
	@echo "Nothing to be done."

test:	check

check:
	cd tests && $(MAKE) check
	cd example && $(MAKE) all

doc:	README.md
	markdown < README.md > README.html

clean:
	cd tests && $(MAKE) clean
	cd example && $(MAKE) clean
	cd bench && $(MAKE) clean
	$(RM) -f README.html depend.dot

distclean: clean

depend:
	cd tests && $(MAKE) depend

depend.png:
	(echo "digraph g { " ; for i in *.h ; do list=$$(grep "include \"" $$i |cut -f2 -d\") ; for j in $$list ; do echo "\"$$i\" -> \"$$j\" ;" ; done ; done ; echo "}" )> depend.dot
	dot -Tpng depend.dot -o depend.png

install:
	$(MKDIR) $(DESTDIR)$(PREFIX)/include
	cp m-*.h $(DESTDIR)$(PREFIX)/include
