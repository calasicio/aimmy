#pragma once
#include <cmath>

struct Vector2
{
    float x, y;

    Vector2 operator-(const Vector2 &other) const { return {x - other.x, y - other.y}; }
    Vector2 operator+(const Vector2 &other) const { return {x + other.x, y + other.y}; }
    Vector2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vector2 &operator+=(const Vector2 &o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }
};

struct Vector3
{
    float x, y, z;
};
