add_library(wifi-transmitter STATIC)

target_sources(wifi-transmitter PRIVATE
  application.cpp
  message.cpp
  )
#main_esp8266.cpp
if (ARDUINO)
  target_sources(wifi-transmitter PRIVATE main_arduino.cpp)
else()
  target_sources(wifi-transmitter PRIVATE main_pc.cpp wifi_curl_pc.cpp)
  target_compile_definitions(wifi-transmitter PRIVATE NOMAIN)
  target_link_libraries(wifi-transmitter PRIVATE "-lpthread")
endif()

target_link_libraries(wifi-transmitter PRIVATE common)

