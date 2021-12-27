add_library(common STATIC)
target_sources(common PRIVATE
src/analog_arduino.cpp
src/clock.cpp
src/crc.cpp
src/debug.cpp
src/eeprom.cpp
src/lcd.cpp
src/platform.cpp
src/rtcmemory.cpp
src/serial_arduino.cpp
src/serial.cpp
src/sleep.cpp
src/stringawaiter.cpp
src/tickscounter.cpp
src/time.cpp
src/utils.cpp
src/wifi_arduino.cpp
)
target_link_libraries(common PRIVATE LowPower)
target_link_libraries(common PRIVATE AltSoftSerial)
target_link_libraries(common PRIVATE LiquidCrystal)
target_link_libraries(common PRIVATE EEPROM)
target_link_libraries(common PRIVATE core)
target_include_directories(common PUBLIC include/)

