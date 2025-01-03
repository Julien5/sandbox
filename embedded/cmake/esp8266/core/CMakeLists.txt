# must be before the project call(), otherwise the core
# target is not known to make. I dont know why it is like
# this. The template is used
# https://github.com/espressif/ESP8266_RTOS_SDK/issues/1083
# has project() before idf_build_process.
include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
idf_build_process(esp8266
    COMPONENTS esp8266 freertos esptool_py pthread spi_flash
    SDKCONFIG sdkconfig
)

project(core)

# It seems idf_build_process does not support INTERFACE
# => empty lib with empty dummy.cpp

add_library(${PROJECT_NAME} dummy.cpp)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    ESP8266=1
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    .
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    idf::esp8266
    idf::spi_flash
)


