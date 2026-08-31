#pragma once

struct ViewMatrix
{
  float m[4][4];

  const float *operator[](int index) const { return m[index]; }
  float *operator[](int index) { return m[index]; }
};