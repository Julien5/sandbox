SET(TARGET Low-Power)
add_library(${TARGET} STATIC)
target_sources(${TARGET} PRIVATE
  /opt/avr/ArduinoCore-avr-1.8.6/libraries/${TARGET}/LowPower.cpp
  )
target_include_directories(${TARGET} PUBLIC /opt/avr/ArduinoCore-avr-1.8.6/libraries/${TARGET})
target_link_libraries(${TARGET} PRIVATE core)
