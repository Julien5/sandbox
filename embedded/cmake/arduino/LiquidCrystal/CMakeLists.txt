SET(TARGET LiquidCrystal)
add_library(${TARGET} STATIC)
target_sources(${TARGET} PRIVATE
  /opt/avr/ArduinoCore-avr-1.8.6/libraries/${TARGET}/src/LiquidCrystal.cpp
  )
target_include_directories(${TARGET} PUBLIC /opt/avr/ArduinoCore-avr-1.8.6/libraries/LiquidCrystal/src)
target_link_libraries(${TARGET} PRIVATE core)
