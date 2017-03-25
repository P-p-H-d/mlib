CC=cc -std=c99
CXX=c++ -std=c++11
CFLAGS=-O2 -march=native -Wall
CPPFLAGS=-I..

all: bench-mlib bench-stl bench-klib

bench-mlib:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c -o bench-mlib.exe
	@./bench-mlib.exe 1
	@./bench-mlib.exe 2
	@./bench-mlib.exe 3
	@./bench-mlib.exe 4
	@./bench-mlib.exe 6

bench-stl:
	$(CXX) $(CFLAGS) $(CPPFLAGS) bench-stl.cpp -o bench-stl.exe
	@./bench-stl.exe 1
	@./bench-stl.exe 2
	@./bench-stl.exe 3
	@./bench-stl.exe 4
	@./bench-stl.exe 5
	@./bench-stl.exe 6

bench-klib:
	$(CC) $(CFLAGS) $(CPPFLAGS) $${KLIB_CPPFLAGS} bench-klib.c -o bench-klib.exe
	@./bench-klib.exe 1
	@./bench-klib.exe 2
	@./bench-klib.exe 3
	@./bench-klib.exe 4
	@./bench-klib.exe 6

clean:
	rm -f *.o bench-*.exe *~ 
