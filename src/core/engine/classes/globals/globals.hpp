#pragma once

#include <cstdint>

class Globals
{
public:
  Globals() {}

  bool update();

public:
  float sensitivity = 1.0;
};