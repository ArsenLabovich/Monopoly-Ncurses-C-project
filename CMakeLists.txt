cmake_minimum_required(VERSION 3.28)
project(MONOPOLY C)

set(CMAKE_C_STANDARD 11)

add_executable(MONOPOLY src/z6.c)
target_include_directories(MONOPOLY PRIVATE include)
target_link_libraries(MONOPOLY -lncurses)
