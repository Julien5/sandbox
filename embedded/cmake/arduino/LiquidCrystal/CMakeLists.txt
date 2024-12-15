SET(TARGET LiquidCrystal)
add_library(${TARGET} STATIC)
target_sources(${TARGET} PRIVATE
  ${ARDUINOCOREDIR}/libraries/${TARGET}/src/LiquidCrystal.cpp
  )
target_include_directories(${TARGET} PUBLIC ${ARDUINOCOREDIR}/libraries/LiquidCrystal/src)
target_link_libraries(${TARGET} PRIVATE core)
