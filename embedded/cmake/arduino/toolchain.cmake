# executables in use
find_program(AVR_CC avr-gcc REQUIRED)
find_program(AVR_AR avr-gcc-ar REQUIRED)
find_program(AVR_RANLIB avr-gcc-ranlib REQUIRED)
find_program(AVR_CXX avr-g++ REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_SIZE_TOOL avr-size REQUIRED)
find_program(AVR_OBJDUMP avr-objdump REQUIRED)

set(ARDUINO 1)
set(ARDUINOCOREDIR /opt/arduino/ArduinoCore-avr-1.8.2/)

# toolchain starts with defining mandatory variables
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})
set(CMAKE_AR ${AVR_AR})
set(CMAKE_RANLIB ${AVR_RANLIB})

add_definitions(-DF_CPU=16000000L -DARDUINO=182 -D__PROG_TYPES_COMPAT__ -DCMAKE)

set(CMAKE_C_FLAGS "-mmcu=atmega328p -Wall -ffunction-sections -fdata-sections -Os -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fpermissive -fno-exceptions -std=gnu++11 -fno-threadsafe-statics -include Arduino.h")


include_directories(/opt/arduino/ArduinoCore-avr-1.8.2/cores/arduino/)
include_directories(/opt/arduino/ArduinoCore-avr-1.8.2/variants/standard/)
