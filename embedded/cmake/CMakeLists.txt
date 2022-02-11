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
	string(CONCAT cmd ${CMAKE_SOURCE_DIR} "/cmake/arduino/flash.sh")
	add_custom_target(flash-${targetname} ${cmd} "$<TARGET_FILE:${targetname}>.hex")
  elseif (ESP8266)
	string(CONCAT cmd ${CMAKE_SOURCE_DIR} "/cmake/esp8266/flash.sh")
	add_custom_target(flash-${targetname} ${cmd} "$<TARGET_FILE:${targetname}>")
	string(CONCAT cmd ${CMAKE_SOURCE_DIR} "/cmake/esp8266/monitor.sh")
	add_custom_target(monitor-${targetname} ${cmd} "$<TARGET_FILE:${targetname}>")
  endif()
endfunction()

if(TARGET monitor)
else()
  if (ARDUINO)
	string(CONCAT cmd ${CMAKE_SOURCE_DIR} "/cmake/arduino/monitor.sh")
	add_custom_target(monitor ${cmd})
  endif()
endif()

