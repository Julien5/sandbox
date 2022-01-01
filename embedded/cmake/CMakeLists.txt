if(ARDUINO)
  add_subdirectory(arduino)
elseif(ESP8266)
  add_subdirectory(esp8266)
else()
  add_subdirectory(pc)
endif()

function(add_avr_executable targetname)
  add_executable(${targetname} ${ARGN})
  
  if (ARDUINO)
	add_custom_command(
	  TARGET ${targetname}
	  POST_BUILD
	  COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex "$<TARGET_FILE:${targetname}>" "$<TARGET_FILE:${targetname}>.hex"
	  )
	add_custom_target(flash-${targetname}
	  avrdude -q -V -p atmega328p -C /etc/avrdude.conf 
	  -D -c arduino -b ${ARDUINO_BAUD} -P ${ARDUINO_PORT}
	  -U flash:w:"$<TARGET_FILE:${targetname}>.hex":i
	  )
  elseif (ESP8266)
	string(CONCAT cmd ${CMAKE_SOURCE_DIR} "/cmake/esp8266/flash.sh")
	add_custom_target(flash-${targetname}
	  ${cmd} "$<TARGET_FILE:${targetname}>"
	  )
  endif()
endfunction()

if(TARGET monitor)
else()
  if (ARDUINO)
	add_custom_target(monitor
	  screen -L ${ARDUINO_PORT} 9600
	  )
  elseif (ESP8266)
    add_custom_target(monitor
	  ./cmake/esp8266/monitor.sh
	  )
  endif()
endif()
