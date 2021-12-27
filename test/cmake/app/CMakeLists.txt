add_executable(app)
target_sources(app PRIVATE src/app.cpp)
target_link_libraries(app PUBLIC lib)

add_executable(test)
target_sources(test PRIVATE test/test.cpp)
target_link_libraries(test PUBLIC lib)
