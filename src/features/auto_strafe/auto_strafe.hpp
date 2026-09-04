#pragma once

#include "core/engine/cache/cache.hpp"

struct MovementState
{
  bool W_KEY;
  bool A_KEY;
  bool S_KEY;
  bool D_KEY;
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
  bool shouldRelease(const Cache &cache, float currentVel);
};