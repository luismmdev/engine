#include "vector2.h"

#include <math.h>

namespace solado::math
{

Vector2::Vector2(float xin, float yin)
{
  x = xin;
  y = yin;
}

float Vector2::dist(const Vector2& rhs) const
{
  return sqrtf((x * rhs.x) + (y * rhs.y));
}

float Vector2::length() const
{
  return sqrtf((x * x) + (y * y));
}

Vector2& Vector2::normalize()
{
  float length = this->length();

  x /= length;
  y /= length;

  return *this;
}

bool Vector2::operator==(const Vector2& rhs) const
{
  return x == rhs.x && y == rhs.y;
}

bool Vector2::operator!=(const Vector2& rhs) const
{
  return x != rhs.x || y != rhs.y;
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
  return Vector2(x + rhs.x, y + rhs.y);
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
  return Vector2(x - rhs.x, y - rhs.y);
}

Vector2 Vector2::operator*(const float scale) const
{
  return Vector2(x * scale, y * scale);
}

Vector2 Vector2::operator/(const float scale) const
{
  return Vector2(x / scale, y / scale);
}

void Vector2::operator+=(const Vector2& rhs)
{
  x += rhs.x;
  y += rhs.y;
}

void Vector2::operator-=(const Vector2& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
}

void Vector2::operator*=(const float val)
{
  x *= val;
  y *= val;
}

void Vector2::operator/=(const float val)
{
  x /= val;
  y /= val;
}

}
