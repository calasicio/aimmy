#pragma once

#include "core/engine/engine.hpp"
#include "core/engine/types/structs.hpp"

class Hud
{
public:
  Hud() {}

  bool init();
  bool update();

public:
  bool isInitialized = false;

  float radarScale;
  float textureScale;
  float radarScaleFactor;
  float finalRenderScale;

private:
  std::uintptr_t address;
  std::uintptr_t radarHudElementAddress;

private:
  std::uintptr_t findHudElement(const char *targetName);
};