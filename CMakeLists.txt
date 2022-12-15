cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(PhotoShep)



FILE(GLOB_RECURSE MyCSources src/*.c)
FILE(GLOB_RECURSE MyCSources src/*.cpp)
add_executable(PhotoShep ${MyCSources})
target_include_directories(PhotoShep PUBLIC stb)
