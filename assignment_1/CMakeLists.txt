cmake_minimum_required(VERSION 3.28)
project(assignment_1 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(assignment_1
        io.c
        io.h
        io_demo.c
        main.c)
