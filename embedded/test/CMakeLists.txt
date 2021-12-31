cmake_minimum_required(VERSION 3.12)
set(CMAKE_TOOLCHAIN_FILE /home/julien/projects/sandbox/embedded/cmake/arduino/toolchain.cmake)
project(project)
add_executable(test)
target_sources(test PRIVATE application.cpp)

if (ARDUINO)
  target_sources(test PRIVATE main_arduino.cpp)
else()
  target_sources(test PRIVATE main_pc.cpp)
endif()

target_link_libraries(test PRIVATE common)

add_custom_command(
  TARGET test 
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex "$<TARGET_FILE:test>" "$<TARGET_FILE:test>.hex"
  )

add_custom_target(flash
  avrdude -q -V -p atmega328p -C /etc/avrdude.conf 
  -D -c arduino -b ${ARDUINO_BAUD} -P ${ARDUINO_PORT}
  -U flash:w:"$<TARGET_FILE:test>.hex":i
  )

add_custom_target(monitor
  screen -L ${ARDUINO_PORT} 9600
  )

