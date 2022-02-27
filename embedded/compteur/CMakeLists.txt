add_avr_executable(compteur)

target_sources(compteur PRIVATE
  adcfile.cpp
  application.cpp
  compteur.cpp
  detection.cpp
  intermittentread.cpp
  httpsender.cpp
  )

if (ARDUINO)
  target_sources(compteur PRIVATE main_arduino.cpp simulation_arduino.cpp)
  target_compile_definitions(compteur INTERFACE -DSIMULATION=1)
else()
  target_compile_definitions(compteur PRIVATE -DSIMULATION=1)
  target_sources(compteur PRIVATE
	main_pc.cpp
	simulation_pc.cpp
	parameters_pc.cpp
	read_file.cpp
	)
  target_link_libraries(compteur PRIVATE wifi-transmitter)
endif()

target_link_libraries(compteur PRIVATE common)
