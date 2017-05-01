CC=cc -std=c99
CFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function
CPPFLAGS=-I..
LDFLAGS=-pthread -lgmp
RM=rm -f
EXE=./a.exe

all:
	@echo "Nothing to be done."

check: examples

examples:
	for f in ex-*.c ; do $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -g $$f -o $(EXE)  ; done

clean:
	$(RM) $(EXE) *.s *~ *.o ./a.dat
	$(RM) -r coverage *.gcda *.gcno all.info test.tar README.html


