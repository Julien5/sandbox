add_library(EEPROM STATIC)
target_sources(EEPROM PRIVATE
  /opt/arduino/ArduinoCore-avr-1.8.2/libraries/EEPROM/src/EEPROM.cpp
  )
target_include_directories(EEPROM PUBLIC /opt/arduino/ArduinoCore-avr-1.8.2/libraries/EEPROM/src)
