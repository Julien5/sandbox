SET(TARGET LowPower)
add_library(${TARGET} STATIC)
target_sources(${TARGET} PRIVATE
  /opt/arduino/libraries/${TARGET}/LowPower.cpp
  )
target_include_directories(${TARGET} PUBLIC /opt/arduino/libraries/${TARGET})
target_link_libraries(${TARGET} PRIVATE core)
