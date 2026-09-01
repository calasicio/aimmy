#pragma once

#include <cstdint>

#include "core/engine/types/structs.hpp"

class Globals
{
public:
  Globals() {}

  bool update();

public:
  int maxClients;
  char mapName[32];
  bool inMatch;

private:
  std::uintptr_t address;
};