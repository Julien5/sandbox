if (ESP8266)
  add_portable_executable(wifi-transmitter)
else()
  add_library(wifi-transmitter STATIC)
endif()

target_sources(wifi-transmitter PRIVATE
  application.cpp
  message.cpp
  )
#main_esp8266.cpp
if (ARDUINO)
  target_sources(wifi-transmitter PRIVATE main_arduino.cpp)
elseif(ESP8266)
  target_sources(wifi-transmitter PRIVATE main_esp8266.cpp)
else()
  target_sources(wifi-transmitter PRIVATE main_pc.cpp wifi_curl_pc.cpp)
  target_compile_definitions(wifi-transmitter PRIVATE NOMAIN)
  target_link_libraries(wifi-transmitter PRIVATE "-lpthread")
endif()

target_link_libraries(wifi-transmitter PRIVATE common)

