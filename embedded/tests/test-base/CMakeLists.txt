project(test-base)
add_portable_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE application.cpp)

if (ARDUINO)
  target_sources(${PROJECT_NAME} PRIVATE main_arduino.cpp)
elseif(ESP8266)
  target_sources(${PROJECT_NAME} PRIVATE main_esp8266.cpp)
else()
  target_sources(${PROJECT_NAME} PRIVATE main_pc.cpp)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE common)

