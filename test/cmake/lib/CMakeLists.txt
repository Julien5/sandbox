add_library(lib STATIC src/lib.cpp)
target_include_directories(lib PUBLIC include/)
target_link_libraries(lib PUBLIC core)

add_executable(lib.test)
target_sources(lib.test PRIVATE test/test.cpp)
target_link_libraries(lib.test PUBLIC lib)
