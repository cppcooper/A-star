cmake_minimum_required(VERSION 3.19)
project(a_star)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(include/maze)
include_directories(include/pathing)
include_directories(usr/lib/musl/include)

add_executable(a_star
        include/maze/cell_manager.h
        include/maze/debug_funcs.h
        include/maze/directions.h
        include/maze/thebuilder.h
        include/pathing/THEpathfinder.h
        include/global.h
        include/posh.h
        include/keyboard.h
        src/maze/cell_manager.cpp
        src/maze/debug_funcs.cpp
        src/maze/directions.cpp
        src/maze/thebuilder.cpp
        src/pathing/THEpathfinder.cpp
        src/Source.cpp)
