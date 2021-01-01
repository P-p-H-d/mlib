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

add_executable(algo02 ${includes} ex-algo02.c)
add_executable(algo03 ${includes} ex-algo03.c)
add_executable(algo04 ${includes} ex-algo04.c)
add_executable(array00 ${includes} ex-array00.c)
add_executable(array01 ${includes} ex-array01.c)
add_executable(array02 ${includes} ex-array02.c)
add_executable(array03 ${includes} ex-array03.c)
add_executable(array04 ${includes} ex-array04.c)
add_executable(bptree01 ${includes} ex-bptree01.c)
add_executable(buffer01 ${includes} ex-buffer01.c)
add_executable(defer01 ${includes} ex-defer01.c)
add_executable(dict01 ${includes} ex-dict01.c)
add_executable(dict02 ${includes} ex-dict02.c)
add_executable(dict03 ${includes} ex-dict03.c)
add_executable(dict04 ${includes} ex-dict04.c)
add_executable(grep01 ${includes} ex-grep01.c)
add_executable(list01 ${includes} ex-list01.c)
add_executable(mph ${includes} ex-mph.c)
add_executable(multi01 ${includes} ex-multi01.c)
add_executable(multi02 ${includes} ex-multi02.c)
add_executable(multi03 ${includes} ex-multi03.c)
add_executable(multi04 ${includes} ex-multi04.c)
add_executable(multi05 ${includes} ex-multi05.c)
add_executable(rbtree01 ${includes} ex-rbtree01.c)
add_executable(string01 ${includes} ex-string01.c)
add_executable(string02 ${includes} ex-string02.c)

add_executable(c11-algo01 ${includes} ex11-algo01.c)
add_executable(c11-algo02 ${includes} ex11-algo02.c)
add_executable(c11-json01 ${includes} ex11-json01.c)
add_executable(c11-section ${includes} ex11-section.c)
