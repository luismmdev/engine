message(STATUS "solado")

set(EXECUTABLE_NAME solado)

add_executable(${EXECUTABLE_NAME})

# add_subdirectory(ecs)
# add_subdirectory(math)

set(
  HEADERS
  components.h
  entity-manager.h
  entity.h
  game.h
  vectory2.h
)
set(
  SOURCES
  entity-manager.cpp
  entity.cpp
  game.cpp
  vector2.cpp
)

target_sources(${EXECUTABLE_NAME} PUBLIC main.cpp ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ImGui-SFML::ImGui-SFML)
