PREFIX=/usr/local
RM=rm -f
MKDIR=mkdir -p

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
	$(RM) -f README.html

distclean:
	cd tests && $(MAKE) distclean

depend:
	cd tests && $(MAKE) depend

install:
	$(MKDIR) $(DESTDIR)$(PREFIX)/include
	cp m-*.h $(DESTDIR)$(PREFIX)/include
