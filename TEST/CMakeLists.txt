# CMakeLists
cmake_minimum_required(VERSION 3.0)
project(HELLO VERSION 1.0 LANGUAGES C CXX)
set(SOURCES test.cpp)
add_executable(hello ${SOURCES})