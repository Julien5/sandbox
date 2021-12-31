cmake_minimum_required(VERSION 3.12)
set(CMAKE_TOOLCHAIN_FILE /home/julien/projects/sandbox/embedded/cmake/arduino/toolchain.cmake)
project(project)
add_executable(test)
target_sources(test PRIVATE application.cpp  main_arduino.cpp)
target_link_libraries(test PRIVATE common)

#add_custom_target(flash
#    COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/do_flash.pl ${
#    DEPENDS ${MAIN_BIN_FILE}
#)
message(${CMAKE_CURRENT_BINARY_DIR}/test)

add_custom_command(
  TARGET test 
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex "$<TARGET_FILE:test>" "$<TARGET_FILE:test>.hex"
  )

add_custom_target(flash avrdude -q -V -p atmega328p -C /etc/avrdude.conf 
  -D -c arduino -b ${ARDUINO_BAUD} -P ${ARDUINO_PORT}
  -U flash:w:"$<TARGET_FILE:test>.hex":i
  )

# Upload the eeprom with avrdude
#add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)

# Burn fuses
#add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )

# Clean extra files
#set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")
