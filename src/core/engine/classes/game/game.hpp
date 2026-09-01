#pragma once

#include <cstdint>

#include "core/engine/types/structs.hpp"

class Game
{
public:
  Game() {}

  bool update();

public:
  float sensitivity = 1.0;
  ViewMatrix viewMatrix;
};