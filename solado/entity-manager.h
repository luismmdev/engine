#ifndef SOLADO_ENTITY_MANAGER_H
#define SOLADO_ENTITY_MANAGER_H

#include <map>
#include <memory>
#include <vector>

#include "entity.h"

namespace solado::ecs
{

typedef std::vector<std::shared_ptr<Entity>> EntityVector;
typedef std::map<std::string, EntityVector>  EntityMap;

class EntityManager
{
public:
  EntityManager();

  void update();

  std::shared_ptr<Entity> addEntity(const std::string& tag);
  const EntityVector&     getEntities();
  const EntityVector&     getEntitiesByTag(const std::string& tag);
  const EntityMap&        getEntitiesMap();

private:
  EntityVector  m_entities;
  EntityVector  m_entitiesToAdd;
  EntityMap     m_entitiesMap;
  size_t        m_totalEntities = 0;

  void removeDeadEntities(EntityVector& entities);
};

} // namespace solado::ecs

#endif // SOLADO_ENTITY_MANAGER_H
