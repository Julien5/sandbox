if (DEFINED ARDUINO)
    add_subdirectory(test-avr)
endif()

add_subdirectory(test-base)
add_subdirectory(test-sdfat)
