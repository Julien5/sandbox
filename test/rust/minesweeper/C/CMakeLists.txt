cmake_minimum_required(VERSION 3.16)

project(minesweeper-cpp)

add_executable(minesweeper-cpp 
    baseline.cpp
    baseline.h
    main.cpp
    margin.cpp
    margin.h
    minesweeper.cpp
    minesweeper.h
    printable.cpp
    printable.h
)

set(CMAKE_CPP_FLAGS_RELEASE "-O3")
