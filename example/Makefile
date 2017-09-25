CC=cc -std=c99
CFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function
CPPFLAGS=-I..
LDFLAGS=-pthread -lgmp
RM=rm -f

.SUFFIXES: .c .exe

.c.exe:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $< -o $@

all:
	$(MAKE) `ls ex-*.c|sed -e 's/\.c/\.exe/g'`

clean:
	$(RM) *.exe *.s *~ *.o ./a.dat


