add_avr_executable(test)
target_sources(test PRIVATE application.cpp)

if (ARDUINO)
  target_sources(test PRIVATE main_arduino.cpp)
elseif(ESP8266)
  target_sources(test PRIVATE main_esp8266.cpp)
else()
  target_sources(test PRIVATE main_pc.cpp)
endif()

target_link_libraries(test PRIVATE common)

