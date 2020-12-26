# set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
add_compile_definitions(__STDC_WANT_LIB_EXT1__=1 __STDC_WANT_LIB_EXT2__=1)
endif()

add_executable(test-marray ${includes} test-marray.c)

add_test(main test-marray)
