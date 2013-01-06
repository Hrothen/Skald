#
# Quick-and-dirty buildscript for skald
#

cmake_minimum_required(VERSION 2.6)
project(Skald)

# Set relative link path base to source directory
cmake_policy(SET CMP0015 NEW)

# Library Sources
set (${PROJECT_NAME}_SOURCES
    src/Component.hpp
    src/Components.hpp
    src/EntityManager.hpp
    src/System.hpp
    src/SystemManager.hpp
    src/World.hpp
    src/VectorTuple.hpp
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
    test/vectorTupleTests.cpp
)

# Test Build Rules
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall")
include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(testrunner ${${PROJECT_NAME}_TEST_SOURCES})
target_link_libraries(testrunner gtest gtest_main)
add_test(testrunner testrunner)
