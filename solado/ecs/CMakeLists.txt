message(STATUS "solado/ecs")

SET(
  HEADERS
  include/components.h
  include/entity-manager.h
  include/entity.h
)
SET(
  SOURCES
  src/entity-manager.cpp
  src/entity.cpp
)

target_sources(
  ${EXECUTABLE_NAME}
  PUBLIC ${HEADERS}
  PRIVATE ${SOURCES}
)
