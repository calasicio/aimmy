#pragma once

#include <cmath>

class Vector2
{
public:
  float x;
  float y;

  // Constructors
  Vector2() : x(0.0f), y(0.0f) {}
  Vector2(float x, float y) : x(x), y(y) {}

  // Addition
  Vector2 operator+(const Vector2 &other) const
  {
    return Vector2(x + other.x, y + other.y);
  }

  Vector2 &operator+=(const Vector2 &other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vector2 operator+(float s) const
  {
    return Vector2(x + s, y + s);
  }

  Vector2 &operator+=(float s)
  {
    x += s;
    y += s;
    return *this;
  }

  // Subtraction
  Vector2 operator-(const Vector2 &other) const
  {
    return Vector2(x - other.x, y - other.y);
  }

  Vector2 &operator-=(const Vector2 &other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vector2 operator-(float s) const
  {
    return Vector2(x - s, y - s);
  }

  Vector2 &operator-=(float s)
  {
    x -= s;
    y -= s;
    return *this;
  }

  // Multiplication
  Vector2 operator*(float s) const
  {
    return Vector2(x * s, y * s);
  }

  Vector2 &operator*=(float s)
  {
    x *= s;
    y *= s;
    return *this;
  }

  // Division
  Vector2 operator/(float s) const
  {
    return Vector2(x / s, y / s);
  }

  Vector2 &operator/=(float s)
  {
    x /= s;
    y /= s;
    return *this;
  }

  // Length / Magnitude
  float length() const
  {
    return std::sqrt(x * x + y * y);
  }

  // Normalize
  Vector2 &normalize()
  {
    float magnitude = length();

    if (magnitude == 0.0f)
      return *this;

    x /= magnitude;
    y /= magnitude;

    return *this;
  }

  Vector2 normalized() const
  {
    float magnitude = length();

    if (magnitude == 0.0f)
      return Vector2{0.0f, 0.0f};

    return Vector2{x / magnitude, y / magnitude};
  }

  // Distance between two vectors
  float distance(const Vector2 &other) const
  {
    float dx = other.x - x;
    float dy = other.y - y;

    return std::sqrt(dx * dx + dy * dy);
  }

  // Dot product
  static float dot(const Vector2 &a, const Vector2 &b)
  {
    return (a.x * b.x) + (a.y * b.y);
  }

  // 2D Cross product
  static float cross(const Vector2 &a, const Vector2 &b)
  {
    return (a.x * b.y) - (a.y * b.x);
  }
};