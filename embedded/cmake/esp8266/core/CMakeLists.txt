project(core)

include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
idf_build_process(esp8266
    COMPONENTS esp8266 freertos esptool_py pthread spi_flash
)

add_library(${PROJECT_NAME} dummy.cpp)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC -DESP8266=1)

target_link_libraries(${PROJECT_NAME} PRIVATE
    idf::esp8266
)


