cmake_minimum_required(VERSION 2.6)

if("${GTEST_ROOT}" STREQUAL "")
    set(GTEST_INCLUDE_DIRS "$ENV{GTEST_ROOT}/fused-src")
else()
    set(GTEST_INCLUDE_DIRS "${GTEST_ROOT}/fused-src")
endif()

message(${GTEST_INCLUDE_DIRS})

find_package(Boost)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(FLEX_LIB_INCLUDE_DIR "flex_lib")


include_directories("./" ${GTEST_INCLUDE_DIRS})

add_subdirectory(sources)
add_subdirectory(tests)
