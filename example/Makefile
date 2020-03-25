CC=cc -std=c99
CFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function
GMP_DIR=
CPPFLAGS=-I..
LDFLAGS=-pthread
RM=rm -f

.SUFFIXES: .c .exe

.c.exe:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) `cat config` $< -o $@

all: config
	$(MAKE) `ls ex-*.c|sed -e 's/\.c/\.exe/g'`
	if test "$(CC)" = "cc -std=c99" ; then $(MAKE) `ls ex11-*.c|sed -e 's/\.c/\.exe/g'` CC="cc -std=c11" ; fi

config:
	printf "#include <gmp.h>\nint main(void){mpz_t z; mpz_init(z); return 0;}\n" > config-test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)  -I$(GMP_DIR)/include -L$(GMP_DIR)/lib -lgmp config-test.c -o config-test.exe 2> /dev/null && echo "-DHAVE_GMP=1  -I$(GMP_DIR)/include -L$(GMP_DIR)/lib -lgmp" > config || echo "-DHAVE_GMP=0" > config
	$(RM) config-test.*

clean:
	$(RM) *.exe *.s *~ *.o ./a.dat config


