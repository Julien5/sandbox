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
  )

if (ARDUINO)
  target_sources(common PRIVATE
	src/analog_arduino.cpp
	src/serial_arduino.cpp
	src/wifi_arduino.cpp
	src/digital_arduino.cpp
	)
elseif(ESP8266)
  target_sources(common PRIVATE
	src/digital_esp8266.cpp
	src/serial_esp8266.cpp
	src/wifi_esp8266.cpp
	)
else()
  target_sources(common PRIVATE
	src/analog_pc.cpp
	src/digital_pc.cpp
	src/serial_pc.cpp
	src/wifi_pc.cpp
	)
endif()

target_include_directories(common PUBLIC include/)

if (ARDUINO)
  target_link_libraries(common PRIVATE LowPower)
  target_link_libraries(common PRIVATE AltSoftSerial)
  target_link_libraries(common PRIVATE LiquidCrystal)
  target_link_libraries(common PRIVATE EEPROM)
  target_link_libraries(common PUBLIC core)
endif()

if (ESP8266)
  target_link_libraries(common PUBLIC core)
endif()

