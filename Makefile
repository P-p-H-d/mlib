PREFIX=/usr/local
RM=rm -f
MKDIR=mkdir -p

all:
	@echo "Nothing to be done."

check:
	cd tests && make check		# NOTE: May need to run the test with other compilers? clang ? musl-gcc?
	cd example && make examples

doc:	README.md
	markdown < README.md > README.html

clean:
	cd tests && make clean
	cd example && make clean
	cd bench && make clean
	$(RM) -f README.html

install:
	$(MKDIR) $(PREFIX)/include
	cp m-*.h $(PREFIX)/include
