add_portable_executable(compteur)

target_sources(compteur PRIVATE
  adcfile.cpp
  application.cpp
  capacity.cpp
  compteur.cpp
  detection.cpp
  intermittentread.cpp
  httpsender.cpp
  sleep_authorization.cpp
  softdebug.cpp
  )

if (ARDUINO)
  target_sources(compteur PRIVATE main_arduino.cpp simulation_arduino.cpp)
  #target_compile_definitions(compteur PRIVATE -DSIMULATION=1)
else()
  #target_compile_definitions(compteur PRIVATE -DSIMULATION=1)
  target_sources(compteur PRIVATE
	main_pc.cpp
	simulation_pc.cpp
	parameters_pc.cpp
	read_file.cpp
	)
  target_link_libraries(compteur PRIVATE wifi-transmitter)
endif()

target_link_libraries(compteur PRIVATE common)
