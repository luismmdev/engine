#include "entity.h"

namespace solado::ecs
{

Entity::Entity(const size_t id, const std::string& tag)
  : m_id(id), m_tag(tag)
{ }

void Entity::destroy()
{
  m_alive = false;
}

size_t Entity::id() const
{
  return m_id;
}

bool Entity::isAlive() const
{
  return m_alive;
}

const std::string& Entity::tag() const
{
  return m_tag;
}

}
