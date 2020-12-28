# set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
add_compile_definitions(__STDC_WANT_LIB_EXT1__=1 __STDC_WANT_LIB_EXT2__=1)
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

add_executable(test-mstring ${includes} test-mstring.c)
add_executable(test-marray ${includes} test-marray.c)
add_executable(test-malgo ${includes} test-malgo.c)

add_test(main test-mstring)
add_test(main test-marray)
add_test(main test-malgo)
