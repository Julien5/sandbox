SET(TARGET LiquidCrystal)
add_library(${TARGET} STATIC)
target_sources(${TARGET} PRIVATE
  /opt/arduino/libraries/${TARGET}/src/LiquidCrystal.cpp
  )
target_include_directories(${TARGET} PUBLIC /opt/arduino/libraries/LiquidCrystal/src)
target_link_libraries(${TARGET} PRIVATE core)
