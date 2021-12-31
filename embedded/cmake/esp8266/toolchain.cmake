set(ESP8266 1)
add_definitions(-DESP8266=1)
string(CONCAT cmd ${CMAKE_CURRENT_LIST_DIR} "/core/build.sh")
execute_process(COMMAND ${cmd} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
file(STRINGS /tmp/esp82266/core/CFLAGS.txt FLAGS)
set(CMAKE_C_FLAGS   "${FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${FLAGS} -std=c++11")
file(STRINGS /tmp/esp82266/core/LFLAGS.txt CMAKE_EXE_LINKER_FLAGS)
include(/opt/esp8266/esp8266-toolchain-espressif/ESP8266_RTOS_SDK/tools/cmake/toolchain-esp8266.cmake)


