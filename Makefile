RM=rm -f

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
	$(RM) -f README.html


