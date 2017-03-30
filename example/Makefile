CC=gcc -std=c99
CFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function
CPPFLAGS=-I..
RM=rm -f
EXE=./a.exe
EXAMPLES=ex01b.c ex01.c ex02.c ex03.c ex04.c ex05.c ex06.c ex07.c ex08.c ex09.c

all:
	@echo "Nothing to be done."

check: examples

examples:
	for f in $(EXAMPLES) ; do $(CC) $(CFLAGS) $(CPPFLAGS) -g $$f -o $(EXE) -pthread -lgmp ; done

clean:
	$(RM) $(EXE) *.s *~ *.o ./a.dat
	$(RM) -r coverage *.gcda *.gcno all.info test.tar README.html


