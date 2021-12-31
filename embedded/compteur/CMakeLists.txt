add_avr_executable(compteur)

target_sources(compteur PRIVATE
  adcfile.cpp
  application.cpp
  compteur.cpp
  detection.cpp
  histogram.cpp
  intermittentread.cpp
  status.cpp
  transient.cpp
  )

if (ARDUINO)
  target_sources(compteur PRIVATE main_arduino.cpp)
else()
  target_sources(compteur PRIVATE main_pc.cpp)
  target_link_libraries(compteur PRIVATE wifi-transmitter)
endif()

target_link_libraries(compteur PRIVATE common)
