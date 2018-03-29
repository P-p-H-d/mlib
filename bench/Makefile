CC=cc -std=c11
CXX=c++ -std=c++11
CFLAGS=-O2 -march=native -Wall
CPPFLAGS=-I..
N=4
BENCH_DEF=

.PHONY: all container queue mempool string plain bench-mlib bench-mlib-mempool bench-stl bench-qt bench-glib bench-klib bench-libdynamic bench-sparsepp bench-collectionc bench-tommyds bench-flathashmap bench-hopscotchmap bench-mlib-thread bench-liblfds bench-concurrentqueue bench-boost bench-mempool bench-string bench-plain

all: container queue mempool string plain

#########################################################################

container: bench-mlib bench-mlib-mempool bench-stl bench-qt bench-glib bench-klib bench-libdynamic bench-sparsepp bench-collectionc bench-tommyds bench-flathashmap bench-hopscotchmap
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

queue: bench-mlib-thread bench-liblfds bench-concurrentqueue bench-boost
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

mempool: bench-mempool
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

string: bench-string
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

plain: bench-plain
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"



#########################################################################


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
	@./bench-mlib.exe 51

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
	@./bench-mlib-mempool.exe 51

bench-mlib-thread:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c -DMULTI_THREAD_MEASURE -pthread -o bench-mlib-thread.exe
	@./bench-mlib-thread.exe 60
	@./bench-mlib-thread.exe 61

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
	@./bench-stl.exe 51

# QT shall point to the the directory containing the file .pc are stored
bench-qt:
	@if test -n "$${QT}" ; then $(MAKE) bench-qt0 ; else echo "Nothing to be done for QT." ; fi
bench-qt0:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -fPIC `PKG_CONFIG_PATH="$${QT}" pkg-config --libs --cflags Qt5Core` bench-qt.cpp -o bench-qt.exe
	@./bench-qt.exe 10
	@./bench-qt.exe 20
	@./bench-qt.exe 30
	@./bench-qt.exe 40
	@./bench-qt.exe 41
	@./bench-qt.exe 42
	@./bench-qt.exe 50

# GLIB shall point to the the directory containing the file .pc are stored
bench-glib:
	@if test -n "$${GLIB}" ; then $(MAKE) bench-glib0 ; else echo "Nothing to be done for GLIB." ; fi
bench-glib0:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-glib.c -o bench-glib.exe `PKG_CONFIG_PATH="$${GLIB}" pkg-config --libs --cflags glib-2.0`
	@./bench-glib.exe 10
	@./bench-glib.exe 20
	@./bench-glib.exe 30
	@./bench-glib.exe 40
	@./bench-glib.exe 41
	@./bench-glib.exe 50

# KLIB shall point to the directory where the headers are
bench-klib:
	@if test -n "$${KLIB}" ; then $(MAKE) bench-klib0 ; else echo "Nothing to be done for KLIB." ; fi

bench-klib0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I $${KLIB} bench-klib.c -o bench-klib.exe
	@./bench-klib.exe 10
	@./bench-klib.exe 20
	@./bench-klib.exe 30
	@./bench-klib.exe 40
	@./bench-klib.exe 41
	@./bench-klib.exe 50

# LIBDYNAMIC shall point to the the directory containing the file libdynamic.pc
bench-libdynamic:
	@if test -n "$${LIBDYNAMIC}" ; then $(MAKE) bench-libdynamic0 ; else echo "Nothing to be done for LIBDYNAMIC." ; fi

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
	@if test -n "$${SPARSEPP}" ; then $(MAKE) bench-sparsepp0 ; else echo "Nothing to be done for SPARSEPP." ; fi

bench-sparsepp0:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -I "$${SPARSEPP}"  -I "$${SPARSEPP}/.." bench-sparsepp.cpp -o bench-sparsepp.exe
	@./bench-sparsepp.exe 41
	@./bench-sparsepp.exe 42
	@./bench-sparsepp.exe 43

# DENSEHASHMAP shall point to the directory where the src are
bench-densehashmap:
	@if test -n "$${DENSEHASHMAP}" ; then $(MAKE) bench-densehashmap0 ; else echo "Nothing to be done Google DENSEHASHMAP." ; fi

bench-densehashmap0:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -I "$${DENSEHASHMAP}"  -I "$${DENSEHASHMAP}/.." bench-densehashmap.cpp -o bench-densehashmap.exe
	@./bench-densehashmap.exe 41
	@./bench-densehashmap.exe 42
	@./bench-densehashmap.exe 43

# COLLECTIONC shall point to the directory where the 'src' directory is.
bench-collectionc:
	@if test -n "$${COLLECTIONC}" ; then $(MAKE) bench-collectionc0 ; else echo "Nothing to be done for COLLECTIONC." ; fi

bench-collectionc0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${COLLECTIONC}/include" -L "$${COLLECTIONC}" -static bench-collectionc.c -lcollectc -o bench-collectionc.exe
	@./bench-collectionc.exe 10
	@./bench-collectionc.exe 20
	@./bench-collectionc.exe 30
	@./bench-collectionc.exe 40
	@./bench-collectionc.exe 41
	@./bench-collectionc.exe 50

# TOMMYDS shall point to the source directory.
bench-tommyds:
	@if test -n "$${TOMMYDS}" ; then $(MAKE) bench-tommyds0 ; else echo "Nothing to be done for TOMMYDS." ; fi

bench-tommyds0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${TOMMYDS}" bench-tommyds.c $${TOMMYDS}/tommyds/tommyarray.c $${TOMMYDS}/tommyds/tommylist.c $${TOMMYDS}/tommyds/tommytree.c $${TOMMYDS}/tommyds/tommyhashlin.c -o bench-tommyds.exe
	@./bench-tommyds.exe 10
	@./bench-tommyds.exe 20
	@./bench-tommyds.exe 30
	@./bench-tommyds.exe 40
	@./bench-tommyds.exe 41

# FLATHASHMAP shall point to the directory where the headers are
bench-flathashmap:
	@if test -n "$${FLATHASHMAP}" ; then $(MAKE) bench-flathashmap0 ; else echo "Nothing to be done for FLATHASHMAP." ; fi

bench-flathashmap0:
	$(CXX) -std=c++14 $(CFLAGS) $(CPPFLAGS) -I "$${FLATHASHMAP}" bench-flathashmap.cpp -o bench-flathashmap.exe
	@./bench-flathashmap.exe 41
	@./bench-flathashmap.exe 42
	@./bench-flathashmap.exe 43

# HOPSCOTCHMAP shall point to the directory where the source are
bench-hopscotchmap:
	@if test -n "$${HOPSCOTCHMAP}" ; then $(MAKE) bench-hopscotchmap0 ; else echo "Nothing to be done for HOPSCOTCHMAP." ; fi

bench-hopscotchmap0:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -I "$${HOPSCOTCHMAP}" bench-hopscotchmap.cpp -o bench-hopscotchmap.exe
	@./bench-hopscotchmap.exe 41
	@./bench-hopscotchmap.exe 42
	@./bench-hopscotchmap.exe 43

# UTHASH shall point to the directory where the source are
bench-uthash:
	@if test -n "$${UTHASH}" ; then $(MAKE) bench-uthash0 ; else echo "Nothing to be done for UTHASH." ; fi

bench-uthash0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${UTHASH}" bench-uthash.c -o bench-uthash.exe
	@./bench-uthash.exe 10
	@./bench-uthash.exe 20
	@./bench-uthash.exe 41
	@./bench-uthash.exe 42
	@./bench-uthash.exe 43
	@./bench-uthash.exe 50

# QLIBC shall point to the directory where the library is installed.
bench-qlibc:
	@if test -n "$${QLIBC}" ; then $(MAKE) bench-qlibc0 ; else echo "Nothing to be done for QLIBC." ; fi

bench-qlibc0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${QLIBC}/include" -L "$${QLIBC}/lib" -lqlibc -pthread bench-qlibc.c -o bench-qlibc.exe
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 10
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 20
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 30
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 42
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 41

# LIBSRT shall point to the directory where the source are
bench-libsrt:
	@if test -n "$${LIBSRT}" ; then $(MAKE) bench-libsrt0 ; else echo "Nothing to be done for LIBSRT." ; fi

bench-libsrt0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${LIBSRT}/src" bench-libsrt.c -o bench-libsrt.exe $${LIBSRT}/libsrt.a 
	@./bench-libsrt.exe 10
	@./bench-libsrt.exe 20
	@./bench-libsrt.exe 41
	@./bench-libsrt.exe 42
	@./bench-libsrt.exe 43
	@./bench-libsrt.exe 50


# LIBLFDS shall point to the directory where the bin/build/inc/obj/src directories are
# The library shall be built.
bench-liblfds:
	@if test -n "$${LIBLFDS}" ; then $(MAKE) bench-liblfds0 ; else echo "Nothing to be done for LIBLFDS." ; fi

bench-liblfds0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${LIBLFDS}/inc" -Wno-unknown-pragmas bench-liblfds.c -o bench-liblfds.exe $${LIBLFDS}/bin/liblfds711.so -lpthread
	@LD_LIBRARY_PATH="$${LIBLFDS}/bin:$${LD_LIBRARY_PATH}" ./bench-liblfds.exe 60


# CONCURRENTQUEUE shall point to the directory where the bin/build/inc/obj/src directories are
# The library shall be built.
bench-concurrentqueue:
	@if test -n "$${CONCURRENTQUEUE}" ; then $(MAKE) bench-concurrentqueue0 ; else echo "Nothing to be done for CONCURRENTQUEUE." ; fi

bench-concurrentqueue0:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -I "$${CONCURRENTQUEUE}/" bench-concurrentqueue.cpp -o bench-concurrentqueue.exe -lpthread
	@./bench-concurrentqueue.exe 60


# XXHASH shall point to the source directory of xxhash
bench-xxhash:
	@if test -n "$${XXHASH}" ; then $(MAKE) bench-xxhash0 ; else echo "Nothing to be done for XXHASH." ; fi

bench-xxhash0:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I "$${XXHASH}/" bench-xxhash.c "$${XXHASH}/libxxhash.a" -o bench-xxhash.exe
	@./bench-xxhash.exe 70

# BOOST is assumed to be installed in the system
bench-boost:
	$(CXX) $(CFLAGS) $(CPPFLAGS) bench-boost.cpp -o bench-boost.exe -lpthread
	@./bench-boost.exe 60

############################################################################

bench-plain:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-plain-c.c -o bench-plain-c.exe
	@./bench-plain-c.exe

############################################################################

bench-mempool:
	$(CXX) $(CFLAGS) $(CPPFLAGS) pool.cc  `pkg-config --libs --cflags glib-2.0` -lboost_system -o bench-mempool.exe
	@./bench-mempool.exe

############################################################################

bench-maxdict:
	$(CC) $(CFLAGS) $(CPPFLAGS) max-dict.c -o bench-max-dict.exe
	@./bench-max-dict.exe

############################################################################

# BSTRLIB shall point to the BSTRLIB sources
# LIBSRT shall point to the LIBSRT sources
bench-string:
	@if test -n "$${BSTRLIB}" ; then $(MAKE) bench-string-with BENCH_DEF="-DBENCH_CAN_USE_BSTRLIB -I ${BSTRLIB} $${BSTRLIB}/obj/bstrlib.o $${BSTRLIB}/obj/bstraux.o $${BSTRLIB}/obj/bstrwrap.o" ; else $(MAKE) bench-string-with ; fi

bench-string-with:
	@if test -n "$${LIBSRT}" ; then $(MAKE) bench-string-exe BENCH_DEF="$(BENCH_DEF) -DBENCH_CAN_USE_LIBSRT -I$${LIBSRT}/src $${LIBSRT}/libsrt.a" ; else $(MAKE) bench-string-exe BENCH_DEF="$(BENCH_DEF)"; fi

bench-string-exe:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -I "$${BSTRLIB}" -DBENCH_CAN_USE_STL -DBENCH_CAN_USE_MSTARLIB bench-string.cpp -o bench-string.exe $(BENCH_DEF)
	@./bench-string.exe


############################################################################

clean:
	rm -f *.o bench-*.exe bench-*.s *~ massif.out.*
	cd nucleotide && $(MAKE) clean

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
	test -f massif.out.* || $(MAKE) massif-do
	for i in massif.out.* ; do ms_print $$i|head -30 ; done
