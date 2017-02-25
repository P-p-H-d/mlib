#NOTE: GMP is only used for the tests
CC=gcc -std=c99
CFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function
CPPFLAGS=-I..
RM=rm -f
VALGRIND=
EXE=./a.exe

all:
	@echo "Nothing to be done."

check: core array list i-list string buffer shared tuple snapshot algo dict rbtree variant

valgrind:
	make check VALGRIND=valgrind

core:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mcore.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

array:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-marray.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mlist.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

i-list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-milist.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

string:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mstring.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

buffer:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mbuffer.c -lpthread -o $(EXE) && $(VALGRIND) $(EXE)

shared:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mshared.c -lpthread -o $(EXE) && $(VALGRIND) $(EXE)

algo:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-malgo.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

tuple:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mtuple.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

snapshot:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-msnapshot.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

dict:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mdict.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

rbtree:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mrbtree.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

bitset:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mbitset.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

variant:
	$(CC) $(CFLAGS) $(CPPFLAGS) -g $(COVERAGE_CFLAGS) test-mvariant.c -lgmp -o $(EXE) && $(VALGRIND) $(EXE)

gene-list:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-mlist.c && less tgen-mlist.s

gene-array:
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -DNDEBUG -S tgen-marray.c && less tgen-marray.s

clean:
	$(RM) $(EXE) *.s *~ *.o ./a.dat
	$(RM) -r coverage *.gcda *.gcno all.info test.tar README.html
