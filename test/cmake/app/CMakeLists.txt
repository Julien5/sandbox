set(APP_SOURCES
src/app.cpp
)
add_executable(app ${APP_SOURCES})
target_link_libraries(app PUBLIC lib)
