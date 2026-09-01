#pragma once

#include <cmath>

class Vector3
{
public:
  float x;
  float y;
  float z;

  // Constructors
  Vector3()
      : x(0.0f), y(0.0f), z(0.0f)
  {
  }

  Vector3(float x, float y, float z)
      : x(x), y(y), z(z)
  {
  }

  // Addition
  Vector3 operator+(const Vector3 &other) const
  {
    return Vector3(
        x + other.x,
        y + other.y,
        z + other.z);
  }

  Vector3 &operator+=(const Vector3 &other)
  {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Vector3 operator+(float scalar) const
  {
    return Vector3(
        x + scalar,
        y + scalar,
        z + scalar);
  }

  Vector3 &operator+=(float scalar)
  {
    x += scalar;
    y += scalar;
    z += scalar;
    return *this;
  }

  // Subtraction
  Vector3 operator-(const Vector3 &other) const
  {
    return Vector3(
        x - other.x,
        y - other.y,
        z - other.z);
  }

  Vector3 &operator-=(const Vector3 &other)
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  Vector3 operator-(float scalar) const
  {
    return Vector3(
        x - scalar,
        y - scalar,
        z - scalar);
  }

  Vector3 &operator-=(float scalar)
  {
    x -= scalar;
    y -= scalar;
    z -= scalar;
    return *this;
  }

  // Multiplication
  Vector3 operator*(float scalar) const
  {
    return Vector3(
        x * scalar,
        y * scalar,
        z * scalar);
  }

  Vector3 &operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  // Division
  Vector3 operator/(float scalar) const
  {
    return Vector3(
        x / scalar,
        y / scalar,
        z / scalar);
  }

  Vector3 &operator/=(float scalar)
  {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
  }

  // Negation
  Vector3 operator-() const
  {
    return Vector3(-x, -y, -z);
  }

  // Length / Magnitude
  float length() const
  {
    return std::sqrt(
        x * x +
        y * y +
        z * z);
  }

  // Normalize in place
  Vector3 &normalize()
  {
    float magnitude = length();

    if (magnitude == 0.0f)
      return *this;

    x /= magnitude;
    y /= magnitude;
    z /= magnitude;

    return *this;
  }

  // Return normalized copy
  Vector3 normalized() const
  {
    float magnitude = length();

    if (magnitude == 0.0f)
      return Vector3{0.0f, 0.0f, 0.0f};

    return Vector3{
        x / magnitude,
        y / magnitude,
        z / magnitude};
  }

  // Distance
  float distance(const Vector3 &other) const
  {
    float dx = other.x - x;
    float dy = other.y - y;
    float dz = other.z - z;

    return std::sqrt(
        dx * dx +
        dy * dy +
        dz * dz);
  }

  // Dot product
  static float dot(const Vector3 &a, const Vector3 &b)
  {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
  }

  // Cross product
  static Vector3 cross(const Vector3 &a, const Vector3 &b)
  {
    return Vector3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
  };
};