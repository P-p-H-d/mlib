cmake_minimum_required(VERSION 3.10)

project(GTEngine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

include(CTest)

include_directories(".")

file(GLOB_RECURSE includes CONFIGURE_DEPENDS "./*.h")

include(CheckCSourceCompiles)

set(CMAKE_REQUIRED_LINK_OPTIONS "-pthread")
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

check_c_source_compiles(
"
#include <malloc.h>
int main () {
    void* data = reallocarray(NULL, 8, sizeof(int));
    free(data);
    return 0;
}
" HAS_REALLOCARRAY)

# TODO: Implement
# add_subdirectory("bench")

# if(BUILD_TESTING)

# endif()

add_subdirectory("tests")
add_subdirectory("example")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
