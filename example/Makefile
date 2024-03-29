# Copyright (c) 2017-2023, Patrick Pelissier
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# + Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# + Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CC=cc -std=c99
CFLAGS=-Wall -W -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op -Wstrict-aliasing=2 -Wredundant-decls -Wold-style-definition -Wno-unused-function -g
GMP_DIR=
MPFR_DIR=$(GMP_DIR)
CPPFLAGS=-I..
LDFLAGS=-pthread
RM=rm -f

.SUFFIXES: .c .exe

.c.exe:
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ `cat config` `cat config.curl` $(LDFLAGS)

.c.o:
	@$(MAKE) config
	$(CC) $(CFLAGS) $(CPPFLAGS) -O2 `cat config.link` -DNDEBUG -g $< -c -o $@

all: config
	$(MAKE) `ls ex-*.c|sed -e 's/\.c/\.exe/g'`
	if test "$(CC)" = "cc -std=c99" ; then $(MAKE) `ls ex11-*.c|sed -e 's/\.c/\.exe/g'` CC="cc -std=c11" ; fi
	$(MAKE) exm.exe exn.exe

exm.exe: exm-lib.o exm-main.o
	$(CC) `cat config.link` -g $^ -o $@

exn.exe: exn-lib.o exn-main.o
	$(CC) `cat config.link` -g $^ -o $@

config:
	printf "#include <gmp.h>\nint main(void){mpz_t z; mpz_init(z); return 0;}\n" > config-test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I$(GMP_DIR)/include -L$(GMP_DIR)/lib config-test.c -lgmp -o config-test.exe $(LDFLAGS) 2> /dev/null && echo "-DHAVE_GMP=1 -DHAVE_MPFR=0 -I$(GMP_DIR)/include -L$(GMP_DIR)/lib -lgmp" > config || echo "-DHAVE_GMP=0 -DHAVE_MPFR=0 " > config
	printf "#include <mpfr.h>\nint main(void){mpfr_t z; mpfr_init(z); return 0;}\n" > config-test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I$(GMP_DIR)/include -I$(MPFR_DIR)/include -L$(GMP_DIR)/lib -L$(MPFR_DIR)/lib config-test.c -lmpfr -lgmp -o config-test.exe $(LDFLAGS) 2> /dev/null && echo "-DHAVE_GMP=1 -DHAVE_MPFR=1  -I$(GMP_DIR)/include -L$(GMP_DIR)/lib -I$(MPFR_DIR)/include -L$(MPFR_DIR)/lib -lmpfr -lgmp" > config || echo "MPFR not available"
	printf "#include <curl/curl.h>\nint main(void){curl_global_init(CURL_GLOBAL_DEFAULT);}" > config-test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) config-test.c `curl-config --cflags --libs`  -o config-test.exe 2> /dev/null && echo "-DHAVE_CURL=1 `curl-config --cflags --libs`" > config.curl || echo "-DHAVE_CURL=0 " > config.curl
	printf "#include <stdio.h>\nint main(void){printf(\"Ok\"); return 0;}\n" > config-test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) config-test.c -ffunction-sections -fdata-sections -Wl,--gc-sections 2> /dev/null && echo "-ffunction-sections -fdata-sections -Wl,--gc-sections" > config.link || echo "" > config.link
	$(RM) config-test.c config-test.exe

clean:
	$(RM) *.exe *.s *~ *.o ./a.dat config config.link config.curl
