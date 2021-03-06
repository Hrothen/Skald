#
# Quick-and-dirty buildscript for skald
#

cmake_minimum_required(VERSION 2.6)
project(Skald)

# Set relative link path base to source directory
cmake_policy(SET CMP0015 NEW)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (CPP11_FLAGS "-std=c++11 -stdlib=libc++")
else ()
    set (CPP11_FLAGS "-std=c++11")
endif()

set (CMAKE_CXX_FLAGS "${CPP11_FLAGS} -Wall")
include_directories(${CMAKE_SOURCE_DIR}/src)

# Library Sources
set (${PROJECT_NAME}_SOURCES
    src/EntityManager.hpp
    src/System.hpp
    src/SystemManager.hpp
    src/World.hpp
)

# Google Test Integration
# NOTE: This could be accomplished with ExternalProject_Add
#       if you wanted to avoid keeping a copy of gtest in the
#       source tree.
add_subdirectory (vendor/gtest)
enable_testing()
include_directories(
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
)

# Test Sources
set (${PROJECT_NAME}_TEST_SOURCES
    test/runAllTests.cpp
    test/entityTests.cpp
    test/systemTests.cpp
    test/worldTests.cpp
)

# Test Build Rules
add_executable(testrunner ${${PROJECT_NAME}_TEST_SOURCES})
target_link_libraries(testrunner gtest_main gtest )
add_test(testrunner testrunner)
