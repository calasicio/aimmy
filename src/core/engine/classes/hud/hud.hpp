#pragma once

#include "core/engine/engine.hpp"
#include "core/engine/types/structs.hpp"

struct HudSnapshot
{
  bool isValid;

  bool isRound;
  float textureScale;
  Vector3 mapTexturePosition;
  float visibilitySizeMax;
  float visibilitySize;
  float mapTextureScale;
  float maxVisibilitySquared;
  Vector3 originTexturePositionDifference;
  float radarScale;
};

class Hud
{
public:
  Hud() {}

  bool init();
  bool update();

public:
  bool isInitialized = false;

  HudSnapshot snapshot;

private:
  std::uintptr_t address;
  std::uintptr_t radarHudElementAddress;

private:
  bool isSnapshotValid(const HudSnapshot &snapshot) const;

private:
  std::uintptr_t findHudElement(const char *targetName);
};