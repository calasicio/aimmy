#pragma once

#include "core/engine/cache/cache.hpp"

struct KeyState
{
  bool state = false;
  float time = 0.0;
};

struct MovementState
{
  KeyState W_KEY;
  KeyState A_KEY;
  KeyState S_KEY;
  KeyState D_KEY;
};

class AutoStrafe
{
public:
  AutoStrafe() {}

  void update(float dt);

private:
  MovementState state;

private:
  void resetState();

  float getTimeToStop(const Cache &cache, float speed);
};