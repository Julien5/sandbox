cmake_minimum_required(VERSION 3.10)
add_executable(main main.cpp)
include_directories(~/projects/pnm/)
set_target_properties(main
    PROPERTIES
    COMPILE_FLAGS "-std=c++11 -O2 -Wall -Wpedantic -Wextra"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/main
)
