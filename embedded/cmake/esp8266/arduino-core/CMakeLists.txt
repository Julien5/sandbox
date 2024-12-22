project(arduino-core)

add_library(${PROJECT_NAME} STATIC)

set(COREDIR /opt/esp8266-toolchain/Arduino-3.1.2)

target_sources(${PROJECT_NAME} PRIVATE
    ${COREDIRE}/opt/esp8266-toolchain/Arduino-3.1.2/libraries/SDFS/src/SDFS.cpp
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    -DMMU_IRAM_SIZE=0xC000
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${COREDIR}/variants/nodemcu
    ${COREDIR}/cores/esp8266
    ${COREDIR}/libraries/SDFS/src
    ${COREDIR}/libraries/SPI
    ${COREDIR}/tools/sdk/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC core)

