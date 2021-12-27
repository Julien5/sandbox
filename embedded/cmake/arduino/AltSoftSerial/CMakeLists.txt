add_library(AltSoftSerial STATIC)
target_sources(AltSoftSerial PRIVATE
  /opt/arduino/libraries/AltSoftSerial/AltSoftSerial.cpp
  )
target_include_directories(AltSoftSerial PUBLIC /opt/arduino/libraries/AltSoftSerial)
