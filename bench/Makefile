CC=cc -std=c11
CXX=c++ -std=c++11
CFLAGS=-O2 -march=native -Wall
CPPFLAGS=-I..
N=4

all: benchmark bench-plain

benchmark: bench-mlib bench-mlib-mempool bench-stl bench-klib bench-qt bench-glib bench-libdynamic bench-sparsepp

bench-mlib:
	$(CC) $(CFLAGS) $(CPPFLAGS) -pthread bench-mlib.c -o bench-mlib.exe
	@./bench-mlib.exe 10
	@./bench-mlib.exe 20
	@./bench-mlib.exe 30
	@./bench-mlib.exe 40
	@./bench-mlib.exe 41
	@./bench-mlib.exe 42
	@./bench-mlib.exe 43
	@./bench-mlib.exe 50
	@./bench-mlib.exe 60

bench-mlib-mempool:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c -DUSE_MEMPOOL -pthread -o bench-mlib-mempool.exe
	@./bench-mlib-mempool.exe 10
	@./bench-mlib-mempool.exe 20
	@./bench-mlib-mempool.exe 30
	@./bench-mlib-mempool.exe 40
	@./bench-mlib-mempool.exe 41
	@./bench-mlib-mempool.exe 42
	@./bench-mlib-mempool.exe 43
	@./bench-mlib-mempool.exe 50
	@./bench-mlib-mempool.exe 60

bench-stl:
	$(CXX) $(CFLAGS) $(CPPFLAGS) bench-stl.cpp -o bench-stl.exe
	@./bench-stl.exe 10
	@./bench-stl.exe 20
	@./bench-stl.exe 30
	@./bench-stl.exe 40
	@./bench-stl.exe 41
	@./bench-stl.exe 42
	@./bench-stl.exe 43
	@./bench-stl.exe 50

bench-qt:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -fPIC $$(pkg-config --libs --cflags Qt5Core) bench-qt.cpp -o bench-qt.exe
	@./bench-qt.exe 10
	@./bench-qt.exe 20
	@./bench-qt.exe 30
	@./bench-qt.exe 40
	@./bench-qt.exe 41
	@./bench-qt.exe 42
	@./bench-qt.exe 50

# KLIB shall point to the directory where the headers are
bench-klib:
	if test -n "$${KLIB}" ; then make bench-klib0 ; else echo "Nothing to be done." ; fi

bench-klib0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I $${KLIB} bench-klib.c -o bench-klib.exe
	@./bench-klib.exe 10
	@./bench-klib.exe 20
	@./bench-klib.exe 30
	@./bench-klib.exe 40
	@./bench-klib.exe 41
	@./bench-klib.exe 50

bench-glib:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-glib.c -o bench-glib.exe `pkg-config --libs --cflags glib-2.0`
	@./bench-glib.exe 10
	@./bench-glib.exe 20
	@./bench-glib.exe 30
	@./bench-glib.exe 40
	@./bench-glib.exe 41
	@./bench-glib.exe 50

# LIBDYNAMIC shall point to the the directory containing the file libdynamic.pc
bench-libdynamic:
	if test -n "$${LIBDYNAMIC}" ; then make bench-libdynamic0 ; else echo "Nothing to be done." ; fi

bench-libdynamic0:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-libdynamic.c -o bench-libdynamic.exe `PKG_CONFIG_PATH="$${LIBDYNAMIC}" pkg-config --libs --cflags libdynamic` 
	@./bench-libdynamic.exe 10
	@./bench-libdynamic.exe 20
	@./bench-libdynamic.exe 30
	@./bench-libdynamic.exe 40
	@./bench-libdynamic.exe 41
	@./bench-libdynamic.exe 50

# SPARSEPP shall point to the directory where the headers are
bench-sparsepp:
	if test -n "$${SPARSEPP}" ; then make bench-sparsepp0 ; else echo "Nothing to be done." ; fi

bench-sparsepp0:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -I "$${SPARSEPP}"  -I "$${SPARSEPP}/.." bench-sparsepp.cpp -o bench-sparsepp.exe
	@./bench-sparsepp.exe 41
	@./bench-sparsepp.exe 42
	@./bench-sparsepp.exe 43

# LIBCOLLECTION shall point to the directory where the 'src' directory is.
bench-libcollection:
	if test -n "$${LIBCOLLECTION}" ; then make bench-libcollection0 ; else echo "Nothing to be done." ; fi

bench-libcollection0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${LIBCOLLECTION}/include" -L "$${LIBCOLLECTION}" -static bench-libcollection.c -lcollectc -o bench-libcollection.exe
	@./bench-libcollection.exe 20

bench-plain:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-plain-c.c -o bench-plain-c.exe
	@./bench-plain-c.exe

bench-mempool:
	$(CXX) $(CFLAGS) $(CPPFLAGS) pool.cc  `pkg-config --libs --cflags glib-2.0` -lboost_system -o bench-mempool.exe
	@./bench-mempool.exe

bench-maxdict:
	$(CC) $(CFLAGS) $(CPPFLAGS) max-dict.c -o bench-max-dict.exe
	@./bench-max-dict.exe

############################################################################

clean:
	rm -f *.o bench-*.exe bench-*.s *~ massif.out.*

############################################################################

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
