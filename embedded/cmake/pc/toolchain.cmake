set(PC 1)

# toolchain starts with defining mandatory variables
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_AR ar)
set(CMAKE_RANLIB ranlib)

add_definitions(-DPC=1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works
set(CMAKE_C_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-g")
