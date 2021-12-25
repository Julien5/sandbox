set(CORE_SOURCES
  src/core.cpp
  )

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PRIVATE include/)
