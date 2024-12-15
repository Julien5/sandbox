SET(TARGET Low-Power)
add_library(${TARGET} STATIC)
target_sources(${TARGET} PRIVATE
  ${ARDUINOCOREDIR}/libraries/${TARGET}/LowPower.cpp
  )
target_include_directories(${TARGET} PUBLIC ${ARDUINOCOREDIR}/libraries/${TARGET})
target_link_libraries(${TARGET} PRIVATE core)
