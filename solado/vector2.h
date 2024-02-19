#ifndef SOLADO_VECTOR2_H
#define SOLADO_VECTOR2_H

#include <iostream>

namespace solado::math
{

class Vector2
{
public:
  float x = 0.f;
  float y = 0.f;

  Vector2() { }
  Vector2(float xin, float yin);

  float    dist(const Vector2& rhs) const;
  float    length() const;
  Vector2& normalize();

  bool    operator == (const Vector2& rhs)  const;
  bool    operator != (const Vector2& rhs)  const;

  Vector2 operator +  (const Vector2& rhs)  const;
  Vector2 operator -  (const Vector2& rhs)  const;
  Vector2 operator *  (const float    val)  const;
  Vector2 operator /  (const float    val)  const;

  void operator +=  (const Vector2& rhs);
  void operator -=  (const Vector2& rhs);
  void operator *=  (const float    val);
  void operator /=  (const float    val);

  friend std::ostream& operator<<(std::ostream& out, const Vector2& v)
  {
    out << "(" << v.x << ", " << v.y << ")";
    return out;
  }
};
}

#endif
