cmake_minimum_required(VERSION 4.0)
project(ccforth)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(ccforth
        forth.cc)
