#pragma once

#include "core/engine/types/vector2.hpp"

class RCS
{
public:
  RCS() {}

  void update(float dt);

private:
  Vector2 oldAimPunch = {0.0, 0.0};
  Vector2 accumulatedError = {0.0, 0.0};
};