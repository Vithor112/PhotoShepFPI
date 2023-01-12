cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(PhotoShep)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


find_package(Qt5 COMPONENTS Widgets REQUIRED)


FILE(GLOB_RECURSE MyCPPSources src/*.cpp)
FILE(GLOB_RECURSE MyUISources src/*.ui)
add_executable(PhotoShep
    ${MyCPPSources}
    ${MyUISources})
target_include_directories(PhotoShep PUBLIC stb)


target_link_libraries(PhotoShep Qt5::Widgets)