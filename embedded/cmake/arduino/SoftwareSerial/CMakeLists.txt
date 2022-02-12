add_library(SoftwareSerial INTERFACE)
target_sources(SoftwareSerial INTERFACE
  /opt/arduino/ArduinoCore-avr-1.8.2/libraries/SoftwareSerial/src/SoftwareSerial.h
  /opt/arduino/ArduinoCore-avr-1.8.2/libraries/SoftwareSerial/src/SoftwareSerial.cpp
  )
target_include_directories(SoftwareSerial INTERFACE /opt/arduino/ArduinoCore-avr-1.8.2/libraries/SoftwareSerial/src)
