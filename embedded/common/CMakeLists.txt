add_library(common STATIC)
target_sources(common PRIVATE
  src/clock.cpp
  src/crc.cpp
  src/debug.cpp
  src/eeprom.cpp
  src/lcd.cpp
  src/platform.cpp
  src/rtcmemory.cpp
  src/serial.cpp
  src/sleep.cpp
  src/stringawaiter.cpp
  src/tickscounter.cpp
  src/time.cpp
  src/utils.cpp
  src/wifi_arduino.cpp
  )

if (ARDUINO)
  target_sources(common PRIVATE
	src/analog_arduino.cpp
	src/serial_arduino.cpp
	src/wifi_arduino.cpp
	)
else()
  target_sources(common PRIVATE
	src/analog_pc.cpp
	src/digital_pc.cpp
	src/serial_pc.cpp
	src/wifi_pc.cpp
	)
endif()

if (ARDUINO)
  target_link_libraries(common PRIVATE LowPower)
  target_link_libraries(common PRIVATE AltSoftSerial)
  target_link_libraries(common PRIVATE LiquidCrystal)
  target_link_libraries(common PRIVATE EEPROM)
  target_link_libraries(common PRIVATE core)
else()
  target_include_directories(common PUBLIC include/)
endif()
