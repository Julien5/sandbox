add_avr_executable(test-avr)

target_sources(test-avr PRIVATE application.cpp)
target_sources(test-avr PRIVATE main_arduino.cpp)

target_link_libraries(test-avr PUBLIC core)

