#ifndef SOLADO_COMPONENTS_H
#define SOLADO_COMPONENTS_H

#include <bitset>

#include <SFML/Graphics.hpp>

#include "vector2.h"

namespace solado::ecs
{

struct CCollision
{
  float radius = 0.f;

  CCollision(float radius) : radius(radius) { }
};

struct CCooldown
{
  float remaining = 0.f;
  float total = 0.f;

  CCooldown(float totalin) : total(totalin) { }
};

enum EnemyType { small, big };
struct CEnemyType
{
  EnemyType type;
  CEnemyType(EnemyType typein) : type(typein) { }
};

struct CInput
{
  bool up = false;
  bool right = false;
  bool down = false;
  bool left = false;

  // LEFT|DOWN|RIGHT|UP
  std::bitset<4> options = 0000;

  CInput() { }
};

struct CLifespan
{
  int remaining = 0;
  int total = 0;

  CLifespan(int totalin) : remaining(totalin), total(totalin) { }
};

struct CScore
{
  int score = 0;

  CScore(float score) : score(score) { }
};

struct CShape
{
  sf::CircleShape circle;

  CShape(
    float radius,
    int points,
    const sf::Color& fill,
    const sf::Color& outline,
    float thickness
  )
    : circle(radius, points)
  {
    circle.setFillColor(fill);
    circle.setOutlineColor(outline);
    circle.setOutlineThickness(thickness);
    circle.setOrigin(radius, radius);
  }
};


struct CTransform
{
  solado::math::Vector2 position = { };
  solado::math::Vector2 velocity = { };
  float                 angle = 0.f;
  float                 speed = 0.f;

  CTransform(
    const solado::math::Vector2& positionin,
    const solado::math::Vector2& velocityin,
    float                         anglein,
    float                         speedin
  )
    :
    position(positionin),
    velocity(velocityin),
    angle(anglein),
    speed(speedin)
  { }
};

} // namespace name

#endif
