add_library(core STATIC)

FILE(GLOB csources
  /opt/arduino/ArduinoCore-avr-1.8.2/cores/arduino/*.c
  /opt/arduino/ArduinoCore-avr-1.8.2/cores/arduino/avr-libc/*.c
  )

FILE(GLOB cxxsources
  /opt/arduino/ArduinoCore-avr-1.8.2/cores/arduino/*.cpp
  )

target_sources(core PRIVATE
  ${csources}
  ${cxxsources}
  )

target_include_directories(core PUBLIC /opt/arduino/ArduinoCore-avr-1.8.2/cores/arduino)
