cmake_minimum_required(VERSION 3.5)

project(MyVector_Svynchuk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MyVector_Svynchuk main.cpp my_vector.h)

################
# Define a test
add_executable(MyVector_TEST my_vector_test.cpp)

######################################
# Configure the test to use GoogleTest
#
#add_dependencies(MyVector_TEST gtest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/googletest/include)
target_link_libraries(MyVector_TEST ${CMAKE_CURRENT_SOURCE_DIR}/gtest/lib/libgtest.a)
target_link_libraries(MyVector_TEST ${CMAKE_CURRENT_SOURCE_DIR}/gtest/lib/libgtest_main.a)


##################################
# Just make the test runnable with
#   $ make test

enable_testing()
add_test(NAME    MyVector_TEST
         COMMAND MyVector_TEST)
