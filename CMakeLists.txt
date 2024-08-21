cmake_minimum_required(VERSION 3.28)
project(monopoly_demo2 C)

set(CMAKE_C_STANDARD 11)

add_executable(monopoly_demo2 src/z6.c)
target_include_directories(monopoly_demo2 PRIVATE include)
target_link_libraries(monopoly_demo2 -lncurses)
