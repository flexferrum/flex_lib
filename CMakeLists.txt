cmake_minimum_required(VERSION 3.0)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(collect_sources)

set(GTEST_ROOT $ENV{GTEST_DIR} CACHE PATH "Path to GTest/GMock library root")
set(BOOST_ROOT $ENV{BOOST_DIR} CACHE PATH "Path to boost library root")
set(FLEXLIB_BUILD_CODEGEN OFF CACHE BOOL "Build codegenerator tools")
set(FLEXLIB_CXX_STANDARD "14" CACHE STRING "C++ standard conformance for library build")

if(NOT "${GTEST_ROOT}" STREQUAL "")
    list (APPEND CMAKE_PREFIX_PATH ${GTEST_ROOT})
    set (Gtest_DIR ${GTEST_ROOT})
    message(STATUS "GTest library search path: ${Gtest_DIR}")
endif()


if(NOT "${BOOST_ROOT}" STREQUAL "")
    list (APPEND CMAKE_PREFIX_PATH ${BOOST_ROOT})
    set (Boost_DIR ${GTEST_ROOT})
    message(STATUS "BOOST library search path: ${Boost_DIR}")
endif()

find_package(GTest)
find_package(Boost)

message (STATUS "##### COMPILER = ${CMAKE_CXX_COMPILER_ID}")
message (STATUS "##### COMPILER = ${CMAKE_CXX_COMPILER_VERSION}")

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    if (${FLEXLIB_CXX_STANDARD} STREQUAL "17" OR ${FLEXLIB_CXX_STANDARD} STREQUAL "1z")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
        message(STATUS "FlexLib is building with C++17 enabled")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
        message(STATUS "FlexLib is building with C++14 enabled")
    endif()
endif()

CollectSources(FlexLibSources FlexLibHeaders ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(FlexLibHeadersFakeLib EXCLUDE_FROM_ALL ${FlexLibHeaders})
set_target_properties(FlexLibHeadersFakeLib PROPERTIES LINKER_LANGUAGE CXX)

set(FLEX_LIB_INCLUDE_DIR "flex_lib")

include_directories(
    "./include"
    ${GTEST_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    )

add_subdirectory(sources)
if (FLEXLIB_BUILD_CODEGEN)
    add_subdirectory(tools)
endif()

if (GTEST_INCLUDE_DIRS)
    add_subdirectory(tests)
endif ()

install (DIRECTORY include/ DESTINATION include)
install (DIRECTORY cmake/ DESTINATION cmake)
