#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

constexpr int CONVAR_VALUE_OFFSET = 0x58;

class ConVars
{
public:
  ConVars() {}

  bool init();
  bool update();

  float getFloat(const char *name, float defaultValue = 0.0f);
  int32_t getInt(const char *name, int32_t defaultValue = 0);
  bool getBool(const char *name, bool defaultValue = false);

  float hudScaling = 0.0f;
  float safeZoneX = 0.0f;
  float safeZoneY = 0.0f;
  float hudRadarScale = 0.0f;
  float radarIconScaleMin = 0.0f;
  bool radarRotate = true;
  float radarScale = 0.0f;
  bool teammatesAreEnemies = false;

private:
  bool initialized = false;

  uintptr_t ccvarBase = 0;
  uintptr_t entriesPtr = 0;
  uint16_t entryCount = 0;

  std::unordered_map<std::string, uintptr_t> convarCache;

  uintptr_t findConVar(const char *name);

private:
  void debugDumpConVar(const char *name);
  void debugFindValueOffset(const char *knownConvar, float knownValue);
};