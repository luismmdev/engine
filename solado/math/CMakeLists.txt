message(STATUS "solado/math")

SET(
  HEADERS
  include/vector2.h
)
SET(
  SOURCES
  src/vector2.cpp
)

target_sources(
  ${EXECUTABLE_NAME}
  PUBLIC ${HEADERS}
  PRIVATE ${SOURCES}
)
