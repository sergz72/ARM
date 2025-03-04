cmake_minimum_required(VERSION 3.30)
project(serial C)

set(CMAKE_C_STANDARD 11)

add_library(serial SHARED library.c)
