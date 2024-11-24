add_library(EEPROM INTERFACE)
target_sources(EEPROM INTERFACE
  /opt/avr/ArduinoCore-avr-1.8.6/libraries/EEPROM/src/EEPROM.h
  )
target_include_directories(EEPROM INTERFACE /opt/avr/ArduinoCore-avr-1.8.6/libraries/EEPROM/src)
