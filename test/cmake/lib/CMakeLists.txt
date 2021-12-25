
add_library(lib STATIC src/lib.cpp)

target_include_directories(lib PUBLIC include/)
target_link_libraries(lib PUBLIC core)
