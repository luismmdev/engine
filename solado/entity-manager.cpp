#include "entity-manager.h"

#include <iostream>

namespace solado::ecs
{

EntityManager::EntityManager()
{ }

void EntityManager::update()
{
  for (auto& e : m_entitiesToAdd)
  {
    auto& tag = e->tag();

    m_entities.push_back(e);
    m_entitiesMap[tag].push_back(e);
  }
  m_entitiesToAdd.clear();


  removeDeadEntities(m_entities);
  for (auto& [_, entities] : m_entitiesMap)
  {
    removeDeadEntities(entities);
  }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
  auto entity = std::shared_ptr<Entity>(new Entity(++m_totalEntities, tag));

  m_entitiesToAdd.push_back(entity);

  return entity;
}

const EntityVector& EntityManager::getEntities()
{
  return m_entities;
}

const EntityVector& EntityManager::getEntitiesByTag(const std::string& tag)
{
  return m_entitiesMap[tag];
}

const EntityMap& EntityManager::getEntitiesMap()
{
  return m_entitiesMap;
}

void EntityManager::removeDeadEntities(EntityVector& entities)
{
  entities.erase
  (
    std::remove_if
    (
      entities.begin(),
      entities.end(),
      [](const auto& e) { return !e->isAlive(); }
    ),
    entities.end()
  );
}

} // namespace solado::ecs
