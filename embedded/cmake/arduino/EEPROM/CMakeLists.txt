add_library(EEPROM INTERFACE)
target_sources(EEPROM INTERFACE
  /opt/arduino/ArduinoCore-avr-1.8.2/libraries/EEPROM/src/EEPROM.h
  )
target_include_directories(EEPROM INTERFACE /opt/arduino/ArduinoCore-avr-1.8.2/libraries/EEPROM/src)
