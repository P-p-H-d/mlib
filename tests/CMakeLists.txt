set(CMAKE_C_STANDARD 11)

include(CheckCSourceCompiles)

check_c_source_compiles(
"
#include <threads.h>
mtx_t lock;
int main () {
    int err = mtx_init(&lock, mtx_plain);
    mtx_destroy(&lock);
    return 0;
}
" HAS_C11_THREADS)

if(WIN32)
add_compile_definitions(__STDC_WANT_LIB_EXT1__=1 __STDC_WANT_LIB_EXT2__=1)
endif()

if(NOT(HAS_C11_THREADS))
add_compile_definitions(M_USE_C11_THREADS=0)
endif()

if(MSVC AND NOT(CMAKE_C_COMPILER_ID MATCHES "Clang"))
add_compile_options(/W4 /WX)
else()
add_compile_options(-fmacro-backtrace-limit=0
                    -Wall -Wextra -Werror
                    -Wno-unused-function
                    -Wno-unused-macros
                    -Wno-gnu-zero-variadic-macro-arguments
                    -Wno-c++98-compat-pedantic
                    -Wno-old-style-cast
                    -Wno-zero-as-null-pointer-constant)
endif()

add_executable(test-mcore ${includes} test-mcore.c)
add_executable(test-mstring ${includes} test-mstring.c)
add_executable(test-mtuple ${includes} test-mtuple.c)
add_executable(test-mvariant ${includes} test-mvariant.c)
add_executable(test-marray ${includes} test-marray.c)
add_executable(test-mbitset ${includes} test-mbitset.c)
add_executable(test-mlist ${includes} test-mlist.c)
add_executable(test-mdict ${includes} test-mdict.c)
add_executable(test-malgo ${includes} test-malgo.c)
add_executable(test-mgenint ${includes} test-mgenint.c)
add_executable(test-mmutex ${includes} test-mmutex.c)
add_executable(test-mshared ${includes} test-mshared.c)
add_executable(test-mishared ${includes} test-mishared.c)
add_executable(test-msnapshot ${includes} test-msnapshot.c)

add_test(NAME core COMMAND test-mcore
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME string COMMAND test-mstring
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME tuple COMMAND test-mtuple
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME variant COMMAND test-mvariant
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME array COMMAND test-marray
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME bitset COMMAND test-mbitset
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME list COMMAND test-mlist
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME dict COMMAND test-mdict
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME algo COMMAND test-malgo
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME genint COMMAND test-mgenint
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME mutex COMMAND test-mmutex
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME shared COMMAND test-mshared
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME ishared COMMAND test-mishared
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME snapshot COMMAND test-msnapshot
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


