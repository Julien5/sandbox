add_library(core STATIC)

FILE(GLOB csources
  ${ARDUINOCOREDIR}/cores/arduino/*.c
  ${ARDUINOCOREDIR}//cores/arduino/avr-libc/*.c
  )

FILE(GLOB cxxsources
  ${ARDUINOCOREDIR}/cores/arduino/*.cpp
  )

target_sources(core PRIVATE
  ${csources}
  ${cxxsources}
  )

target_include_directories(core PUBLIC
  ${ARDUINOCOREDIR}/cores/arduino
  )
