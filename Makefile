RM=rm -f

all:
	@echo "Nothing to be done."

check:
	cd tests && make check
	cd example && make examples

doc:	README.md
	markdown < README.md > README.html

clean:
	cd tests && make clean
	cd example && make clean
	$(RM) -f README.html


