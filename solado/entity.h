#ifndef SOLADO_ENTITY_H
#define SOLADO_ENTITY_H

#include <memory>
#include <string>

#include "components.h"

namespace solado::ecs
{

class Entity
{
  friend class EntityManager;

public:
  void               destroy();
  size_t             id() const;
  bool               isAlive() const;
  const std::string& tag() const;

  std::shared_ptr<solado::ecs::CCollision> cCollision;
  std::shared_ptr<solado::ecs::CCooldown> cCooldown;
  std::shared_ptr<solado::ecs::CEnemyType> cEnemyType;
  std::shared_ptr<solado::ecs::CInput> cInput;
  std::shared_ptr<solado::ecs::CLifespan> cLifespan;
  std::shared_ptr<solado::ecs::CShape> cShape;
  std::shared_ptr<solado::ecs::CScore> cScore;
  std::shared_ptr<solado::ecs::CTransform> cTransform;

private:
  Entity(const size_t id, const std::string& tag);

  const size_t      m_id = 0;
  const std::string m_tag = "Default";
  bool              m_alive = true;

};
}

#endif // SOLADO_ENTITY_H