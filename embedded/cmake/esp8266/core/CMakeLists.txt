add_library(core INTERFACE)

string(CONCAT cmd ${CMAKE_CURRENT_LIST_DIR} "/build.sh")
execute_process(COMMAND ${cmd} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
include(/tmp/esp8266/core/core.cmake)

target_compile_definitions(core INTERFACE -DESP8266=1)
target_compile_options(core INTERFACE ${CORE_CFLAGS})
target_link_options(core INTERFACE ${CORE_LFLAGS})
message(${CORE_LFLAGS})



