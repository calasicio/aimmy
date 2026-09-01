#include "hud.hpp"

#include "core/offsets/offsets.hpp"
#include "utils/logger/logger.hpp"

bool Hud::init()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  this->address = process->read<uintptr_t>(client.base + offsets::c_hud);

  if (this->address == 0)
    return false;

  uintptr_t radarHudElement = findHudElement("CCSGO_HudRadar");

  if (radarHudElement == 0)
    return false;

  this->radarHudElementAddress = radarHudElement;

  char hexStr[32];
  sprintf_s(hexStr, "0x%llX", radarHudElement);
  logger::info(std::string("Found `CCSGO_HudRadar` at: ") + hexStr);

  this->isInitialized = true;

  return true;
}

bool Hud::update()
{
  if (!this->isInitialized)
    return false;

  auto process = Engine::getProcess();
  uintptr_t base = this->radarHudElementAddress;

  bool isRound = process->read<bool>(base + offsets::HudRadar::IsRound);
  float mapTextureScale = process->read<float>(base + offsets::HudRadar::MapTextureScale);
  float visibilitySize = process->read<float>(base + offsets::HudRadar::VisibilitySize);
  float visibilitySizeMax = process->read<float>(base + offsets::HudRadar::VisibilitySizeMax);

  this->radarScale = process->read<float>(base + offsets::HudRadar::RadarScaleTail);

  this->textureScale = (mapTextureScale > 0.0f) ? (1.0f / mapTextureScale) : 1.0f;
  this->radarScaleFactor = isRound ? this->radarScale : (visibilitySize / visibilitySizeMax);
  this->finalRenderScale = this->radarScaleFactor / this->textureScale;

  return true;
}

std::uintptr_t Hud::findHudElement(const char *targetName)
{
  auto process = Engine::getProcess();

  int rootIndex = process->read<int>(this->address + offsets::HudTree::Base + 0x18);
  uint32_t treeFlags = process->read<uint32_t>(this->address + offsets::HudTree::Base + 0x0C);
  uintptr_t dataPool = process->read<uintptr_t>(this->address + offsets::HudTree::Pool);

  if (rootIndex == -1 || dataPool == 0)
    return 0;

  int currentNodeIndex = rootIndex;

  while (currentNodeIndex != -1)
  {
    uintptr_t nodeAddress = dataPool + (currentNodeIndex * 0x20);

    uintptr_t stringAddress = process->read<uintptr_t>(nodeAddress + 0x10);
    if (!stringAddress)
      return 0;

    char currentName[128] = {0};
    for (int j = 0; j < 127; j++)
    {
      char character = process->read<char>(stringAddress + j);
      currentName[j] = character;
      if (character == '\0')
        break;
    }

    int cmp = _stricmp(targetName, currentName);

    if (cmp == 0)
    {
      uintptr_t rawElementPointer = process->read<uintptr_t>(nodeAddress + 0x18);
      if (!rawElementPointer)
        return 0;

      return rawElementPointer - 0x20;
    }

    if (cmp < 0)
    {
      currentNodeIndex = process->read<int>(nodeAddress + 0x00);
    }
    else
    {
      currentNodeIndex = process->read<int>(nodeAddress + 0x04);
    }
  }

  return 0;
}
