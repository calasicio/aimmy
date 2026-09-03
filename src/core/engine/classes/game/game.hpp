#pragma once

#include <cstdint>

#include "core/engine/types/vector2.hpp"
#include "core/engine/types/structs.hpp"

class Game
{
public:
  Game() {}

  bool update();

public:
  ViewMatrix viewMatrix;

  uintptr_t entityList;
  uintptr_t listEntry;

  Vector2 windowSize = {0, 0};
};