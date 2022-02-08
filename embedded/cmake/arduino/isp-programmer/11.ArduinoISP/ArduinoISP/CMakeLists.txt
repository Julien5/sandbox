if (ARDUINO)
  add_avr_executable(isp-programmer)
  target_sources(isp-programmer PRIVATE ArduinoISP.cpp)
  target_link_libraries(isp-programmer PRIVATE EEPROM)
  target_link_libraries(isp-programmer PUBLIC core)
endif()


