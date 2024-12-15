add_library(EEPROM INTERFACE)
target_sources(EEPROM INTERFACE
  ${ARDUINOCOREDIR}/libraries/EEPROM/src/EEPROM.h
  )
target_include_directories(EEPROM INTERFACE ${ARDUINOCOREDIR}/libraries/EEPROM/src)
