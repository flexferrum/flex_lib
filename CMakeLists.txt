cmake_minimum_required(VERSION 3.0)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(collect_sources)

if(NOT "${GTest_DIR}" STREQUAL "")
    list (APPEND CMAKE_PREFIX_PATH $ENV{GTest_DIR})
    message(STATUS "GTest library search path: $ENV{GTest_DIR}")
endif()


if(NOT "${Boost_DIR}" STREQUAL "")
    list (APPEND CMAKE_PREFIX_PATH $ENV{Boost_DIR})
    message(STATUS "BOOST library search path: $ENV{Boost_DIR}")
endif()

find_package(GTest)
find_package(Boost)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif ()

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

if (GTEST_INCLUDE_DIRS)
    add_subdirectory(tests)
endif ()
