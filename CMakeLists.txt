cmake_minimum_required(VERSION 3.31)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(OU4)
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
            src/dlist.c
            src/array_2d.c
            src/array_1d.c
            src/queue.c
            src/list.c
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

project(OU4_2)
add_executable(OU4_2)

target_compile_options(OU4_2
        PRIVATE
            -std=c99
            -Wall
            -Wextra
            -g
            )
target_sources(OU4_2
        PRIVATE
            src/is_connected.c
            src/graph2.c
            src/dlist.c
            src/array_2d.c
            src/array_1d.c
            src/queue.c
            src/list.c
            )
target_include_directories(OU4_2
        PRIVATE
            include/)

if(APPLE)
    add_custom_command(TARGET OU4_2 POST_BUILD
        COMMAND dsymutil $<TARGET_FILE:OU4> -o $<TARGET_FILE:OU4>.dSYM
        COMMENT "Generating dSYM bundle for debugging"
    )
endif()
