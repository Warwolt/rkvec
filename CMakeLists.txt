cmake_minimum_required(VERSION 3.16.0)
project(rkvec)
enable_language(C)

set(TESTS "${PROJECT_NAME}_test")

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_definitions(-Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(${TESTS} tests/tests.c)
target_include_directories(${TESTS} PUBLIC include)
