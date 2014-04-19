cmake_minimum_required(VERSION 2.6)

if("${GTEST_ROOT}" STREQUAL "")
    set(GTEST_INCLUDE_DIRS "$ENV{GTEST_ROOT}/fused-src")
else()
    set(GTEST_INCLUDE_DIRS "${GTEST_ROOT}/fused-src")
endif()

find_package(Boost)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

set(FLEX_LIB_INCLUDE_DIR "flex_lib")

include_directories("./")

add_subdirectory(sources)
if (GTEST_INCLUDE_DIRS)
add_subdirectory(tests)
endif ()
