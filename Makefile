PREFIX=/usr/local
RM=rm -f
MKDIR=mkdir -p

all:
	@echo "Nothing to be done."

test:	check

check:
	cd tests && make check
	cd example && make examples

doc:	README.md
	markdown < README.md > README.html

clean:
	cd tests && make clean
	cd example && make clean
	cd bench && make clean
	$(RM) -f README.html

install:
	$(MKDIR) $(DESTDIR)$(PREFIX)/include
	cp m-*.h $(DESTDIR)$(PREFIX)/include
