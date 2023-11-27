cmake_minimum_required(VERSION 3.16.0)
project(rkvec)
enable_language(C)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_definitions(-Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(${PROJECT_NAME} src/main.c)
