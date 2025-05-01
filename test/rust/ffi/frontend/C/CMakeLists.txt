cmake_minimum_required(VERSION 3.15)
project(frontend-c)

set(PROJECTSDIR "../..")
set(RUST_TARGET_DIR "/home/julien/delme/rust-targets")

add_executable(frontend-c src/main.c)

target_include_directories(frontend-c PUBLIC ${PROJECTSDIR}/backend/export)
target_link_libraries(frontend-c ${RUST_TARGET_DIR}/debug/libbackend.so)
# todo: export compilation database.
