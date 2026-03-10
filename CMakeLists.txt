cmake_minimum_required(VERSION 3.31)
project(OU4)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(OU4)
target_compile_options(OU4
        PRIVATE
            -std=c99
            -Wall
            -Wextra
            -g
            )
target_sources(OU4
        PRIVATE
            src/graph.c
            src/is_connected.c
            #src/graph2.c
            src/dlist.c
            src/array_2d.c
            src/array_1d.c
            )
target_include_directories(OU4
        PRIVATE
            include/)

if(APPLE)
    add_custom_command(TARGET OU4 POST_BUILD
        COMMAND dsymutil $<TARGET_FILE:OU4> -o $<TARGET_FILE:OU4>.dSYM
        COMMENT "Generating dSYM bundle for debugging"
    )
endif()
