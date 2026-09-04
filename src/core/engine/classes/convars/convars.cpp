#include "convars.hpp"

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"
#include "utils/logger/logger.hpp"

bool ConVars::init()
{
  auto process = Engine::getProcess();
  auto tier0 = Engine::getTier0();

  if (!process || tier0.base == 0)
    return false;

  ccvarBase = tier0.base + static_cast<uintptr_t>(offsets::CCVars);
  if (!ccvarBase)
    return false;

  entriesPtr = process->read<uintptr_t>(ccvarBase + 0x50);
  entryCount = process->read<uint16_t>(ccvarBase + 0x5E);

  if (!entriesPtr || entryCount == 0)
    return false;

  initialized = true;

  // debugFindValueOffset("cl_radar_scale", "0.55");
  // debugDumpConVar("cl_radar_scale");

  return true;
}

bool ConVars::update()
{
  if (!initialized)
    return false;

  svAccelerate = getFloat("sv_accelerate", 5.5f);
  svFriction = getFloat("sv_friction", 5.2f);
  svStopSpeed = getFloat("sv_stopspeed", 80.0f);
  svMaxSpeed = getFloat("sv_maxspeed", 320.0f);

  sensitivity = getFloat("sensitivity", 1.0f);

  hudScaling = getFloat("hud_scaling", 0.9f);
  safeZoneX = getFloat("safezonex", 1.0f);
  safeZoneY = getFloat("safezoney", 1.0f);
  hudRadarScale = getFloat("cl_hud_radar_scale", 1.0f);
  radarIconScaleMin = getFloat("cl_radar_icon_scale_min", 0.6f);
  radarRotate = getBool("cl_radar_rotate", true);
  radarScale = getFloat("cl_radar_scale", 1.0f);
  teammatesAreEnemies = getBool("mp_teammates_are_enemies", false);

  return true;
}

float ConVars::getFloat(const char *name, float defaultValue)
{
  uintptr_t addr = findConVar(name);
  if (!addr)
    return defaultValue;

  auto process = Engine::getProcess();
  if (!process)
    return defaultValue;

  return process->read<float>(addr + CONVAR_VALUE_OFFSET);
}

int32_t ConVars::getInt(const char *name, int32_t defaultValue)
{
  uintptr_t addr = findConVar(name);
  if (!addr)
    return defaultValue;

  auto process = Engine::getProcess();
  if (!process)
    return defaultValue;

  return process->read<int32_t>(addr + CONVAR_VALUE_OFFSET);
}

bool ConVars::getBool(const char *name, bool defaultValue)
{
  return getInt(name, defaultValue ? 1 : 0) != 0;
}

uintptr_t ConVars::findConVar(const char *name)
{
  if (!initialized)
    return 0;

  auto it = convarCache.find(name);
  if (it != convarCache.end())
    return it->second;

  auto process = Engine::getProcess();
  if (!process)
    return 0;

  for (uint16_t i = 0; i < entryCount; i++)
  {
    uintptr_t entryAddr = entriesPtr + (i * 0x10);
    uintptr_t convarAddr = process->read<uintptr_t>(entryAddr);
    if (!convarAddr)
      continue;

    uintptr_t namePtr = process->read<uintptr_t>(convarAddr);
    if (!namePtr)
      continue;

    std::string convarName = process->read_string(namePtr);
    if (convarName == name)
    {
      convarCache[name] = convarAddr;
      return convarAddr;
    }
  }

  convarCache[name] = 0;
  return 0;
}

void ConVars::debugDumpConVar(const char *name)
{
  auto process = Engine::getProcess();
  if (!process)
    return;

  uintptr_t addr = findConVar(name);
  if (!addr)
  {
    logger::info(std::string("ConVar ") + name + " NOT FOUND");
    return;
  }

  logger::info(std::string("ConVar ") + name + " 0x" + std::to_string(addr));

  uint8_t buf[0x60];
  if (process->read_raw(addr, buf, sizeof(buf)))
  {
    std::string hex;
    for (int i = 0; i < sizeof(buf); i++)
    {
      char b[4];
      sprintf_s(b, "%02X ", buf[i]);
      hex += b;
      if ((i + 1) % 16 == 0)
        hex += "\n";
    }
    logger::info(hex);
  }

  for (int off = 0x20; off <= 0x58; off += 4)
  {
    float f = process->read<float>(addr + off);
    int32_t n = process->read<int32_t>(addr + off);
    uintptr_t p = process->read<uintptr_t>(addr + off);
    char msg[128];
    sprintf_s(msg, "  [+%02X] float=%.4f int=%d ptr=0x%llX", off, f, n, p);
    logger::info(msg);
  }
}

void ConVars::debugFindValueOffset(const char *knownConvar, float knownValue)
{
  auto process = Engine::getProcess();

  uintptr_t addr = findConVar(knownConvar);
  if (!addr)
  {
    logger::info("Target convar NOT FOUND in list");
    return;
  }

  logger::info(std::string("sensitivity convar @ 0x" + std::to_string(addr)));

  for (int off = 0x20; off <= 0x58; off += 4)
  {
    float f = process->read<float>(addr + off);
    if (fabsf(f - knownValue) < 0.001f)
    {
      char msg[128];
      sprintf_s(msg, ">>> MATCH at +0x%02X (read %.4f)", off, f);
      logger::info(msg);
    }
  }
}