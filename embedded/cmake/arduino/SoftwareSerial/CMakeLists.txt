add_library(SoftwareSerial INTERFACE)
target_sources(SoftwareSerial INTERFACE
  ${ARDUINOCOREDIR}/libraries/SoftwareSerial/src/SoftwareSerial.h
  ${ARDUINOCOREDIR}/libraries/SoftwareSerial/src/SoftwareSerial.cpp
  )
target_include_directories(SoftwareSerial INTERFACE ${ARDUINOCOREDIR}/libraries/SoftwareSerial/src)
