CC=cc -std=c99
CXX=c++ -std=c++11
CFLAGS=-O2 -march=native -Wall
CPPFLAGS=-I..

all: bench-mlib bench-stl

bench-mlib:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c -o bench-mlib.exe
	./bench-mlib.exe

bench-stl:
	$(CXX) $(CFLAGS) $(CPPFLAGS) bench-stl.cpp -o bench-stl.exe
	./bench-stl.exe

clean:
	rm -f *.o bench-mlib.exe bench-stl.exe *~ 
