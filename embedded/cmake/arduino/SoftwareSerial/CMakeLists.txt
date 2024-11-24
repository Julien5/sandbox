add_library(SoftwareSerial INTERFACE)
target_sources(SoftwareSerial INTERFACE
  /opt/avr/ArduinoCore-avr-1.8.6/libraries/SoftwareSerial/src/SoftwareSerial.h
  /opt/avr/ArduinoCore-avr-1.8.6/libraries/SoftwareSerial/src/SoftwareSerial.cpp
  )
target_include_directories(SoftwareSerial INTERFACE /opt/avr/ArduinoCore-avr-1.8.6/libraries/SoftwareSerial/src)
