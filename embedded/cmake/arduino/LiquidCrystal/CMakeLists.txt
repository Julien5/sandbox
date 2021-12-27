add_library(LiquidCrystal STATIC)
target_sources(LiquidCrystal PRIVATE
  /opt/arduino/libraries/LiquidCrystal/src/LiquidCrystal.cpp
  )
target_include_directories(LiquidCrystal PUBLIC /opt/arduino/libraries/LiquidCrystal/src)
