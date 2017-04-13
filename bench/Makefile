CC=cc -std=c99
CXX=c++ -std=c++11
CFLAGS=-O2 -march=native -Wall
CPPFLAGS=-I..
N=4

all: bench-mlib bench-mlib-mempool bench-stl bench-klib bench-qt bench-glib bench-libdynamic

bench-mlib:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c -o bench-mlib.exe
	@./bench-mlib.exe 1
	@./bench-mlib.exe 2
	@./bench-mlib.exe 3
	@./bench-mlib.exe 4
	@./bench-mlib.exe 6
	@./bench-mlib.exe 7

bench-mlib-mempool:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c -DUSE_MEMPOOL -o bench-mlib-mempool.exe
	@./bench-mlib-mempool.exe 1
	@./bench-mlib-mempool.exe 2
	@./bench-mlib-mempool.exe 3
	@./bench-mlib-mempool.exe 4
	@./bench-mlib-mempool.exe 6
	@./bench-mlib-mempool.exe 7

bench-stl:
	$(CXX) $(CFLAGS) $(CPPFLAGS) bench-stl.cpp -o bench-stl.exe
	@./bench-stl.exe 1
	@./bench-stl.exe 2
	@./bench-stl.exe 3
	@./bench-stl.exe 4
	@./bench-stl.exe 5
	@./bench-stl.exe 6
	@./bench-stl.exe 7

bench-qt:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -fPIC $$(pkg-config --libs --cflags Qt5Core) bench-qt.cpp -o bench-qt.exe
	@./bench-qt.exe 1
	@./bench-qt.exe 2
	@./bench-qt.exe 3
	@./bench-qt.exe 4
	@./bench-qt.exe 5
	@./bench-qt.exe 6
	@./bench-qt.exe 7

# KLIB shall point to the directory where the headers are
bench-klib:
	if test -n "$${KLIB}" ; then make bench-klib0 ; else echo "Nothing to be done." ; fi

bench-klib0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I $${KLIB} bench-klib.c -o bench-klib.exe
	@./bench-klib.exe 1
	@./bench-klib.exe 2
	@./bench-klib.exe 3
	@./bench-klib.exe 4
	@./bench-klib.exe 6
	@./bench-klib.exe 7

bench-glib:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-glib.c -o bench-glib.exe `pkg-config --libs --cflags glib-2.0`
	@./bench-glib.exe 1
	@./bench-glib.exe 2
	@./bench-glib.exe 3
	@./bench-glib.exe 4
	@./bench-glib.exe 6
	@./bench-glib.exe 7

# LIBDYNAMIC shall point to the the directory containing the file libdynamic.pc
bench-libdynamic:
	if test -n "$${LIBDYNAMIC}" ; then make bench-libdynamic0 ; else echo "Nothing to be done." ; fi

bench-libdynamic0:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-libdynamic.c -o bench-libdynamic.exe `PKG_CONFIG_PATH="$${LIBDYNAMIC}" pkg-config --libs --cflags libdynamic` 
	@./bench-libdynamic.exe 1
	@./bench-libdynamic.exe 2
	@./bench-libdynamic.exe 3
	@./bench-libdynamic.exe 4
	@./bench-libdynamic.exe 6
	@./bench-libdynamic.exe 7

clean:
	rm -f *.o bench-*.exe *~ massif.out.*

massif-do: all
	@valgrind --tool=massif ./bench-mlib.exe $(N)
	@valgrind --tool=massif ./bench-mlib-mempool.exe $(N)
	@valgrind --tool=massif ./bench-stl.exe $(N)
	@valgrind --tool=massif ./bench-qt.exe $(N)
	@valgrind --tool=massif ./bench-glib.exe $(N)
	@test -f ./bench-klib.exe && valgrind --tool=massif ./bench-klib.exe $(N)
	@test -f ./bench-libdynamic.exe && valgrind --tool=massif ./bench-libdynamic.exe $(N)

massif-visu:
	test -f massif.out.* || make massif-do
	for i in massif.out.* ; do ms_print $$i|head -30 ; done
