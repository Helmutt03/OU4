cmake_minimum_required(VERSION 4.1)

project(OU4)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(OU4)

target_compile_options(OU4
        PRIVATE
            -std=c99
            -Wall
            -Wextra
            )

target_sources(OU4
        PRIVATE
            src/graph.c
            src/is_connected.c
            src/graph2.c
            src/is_connected_help_functions_1.c
            src/dlist.c
            src/array_2d.c
            )

target_include_directories(OU4
        PRIVATE
            include/)


