set(APP_SOURCES
src/app.cpp
)
add_executable(app ${APP_SOURCES})
target_include_directories(app PRIVATE ../core/include)
target_link_libraries(app PRIVATE core)
