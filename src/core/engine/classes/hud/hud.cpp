#include "hud.hpp"

#include <sstream>

#include "core/offsets/offsets.hpp"
#include "utils/logger/logger.hpp"

bool Hud::init()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  this->address = process->read<uintptr_t>(client.base + offsets::hud::cHud);

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

// static bool hasLoggedHudSnapshot = false;
bool Hud::update()
{
  if (!this->isInitialized)
    return false;

  auto process = Engine::getProcess();
  auto client = Engine::getClient();
  uintptr_t base = this->radarHudElementAddress;

  this->snapshot.isRound = process->read<bool>(base + offsets::hud::CCSGO_HudRadar::IsRound);
  this->snapshot.mapTexturePosition = process->read<Vector3>(base + offsets::hud::CCSGO_HudRadar::MapTexturePosition);
  this->snapshot.visibilitySizeMax = process->read<float>(base + offsets::hud::CCSGO_HudRadar::VisibilitySizeMax);
  this->snapshot.visibilitySize = process->read<float>(base + offsets::hud::CCSGO_HudRadar::VisibilitySize);
  this->snapshot.mapTextureScale = process->read<float>(base + offsets::hud::CCSGO_HudRadar::MapTextureScale);
  this->snapshot.maxVisibilitySquared = process->read<float>(base + offsets::hud::CCSGO_HudRadar::MaxVisibilitySquared);
  this->snapshot.originTexturePositionDifference = process->read<Vector3>(base + offsets::hud::CCSGO_HudRadar::OriginTextureDiff);
  this->snapshot.radarScale = process->read<float>(base + offsets::hud::CCSGO_HudRadar::RadarScale);

  this->snapshot.isValid = isSnapshotValid(this->snapshot);

  // if (!hasLoggedHudSnapshot)
  // {
  //   std::ostringstream oss;
  //   oss << "Hud snapshot: "
  //       << "address=0x" << std::hex << this->address << std::dec
  //       << ", radarHudElementAddress=0x" << std::hex << this->radarHudElementAddress << std::dec
  //       << ", base=0x" << std::hex << base << std::dec
  //       << ", isRound=" << (this->snapshot.isRound ? "true" : "false")
  //       << ", mapTexturePosition=(" << this->snapshot.mapTexturePosition.x << ", "
  //       << this->snapshot.mapTexturePosition.y << ", " << this->snapshot.mapTexturePosition.z << ")"
  //       << ", visibilitySizeMax=" << this->snapshot.visibilitySizeMax
  //       << ", visibilitySize=" << this->snapshot.visibilitySize
  //       << ", mapTextureScale=" << this->snapshot.mapTextureScale
  //       << ", maxVisibilitySquared=" << this->snapshot.maxVisibilitySquared
  //       << ", originTexturePositionDifference=(" << this->snapshot.originTexturePositionDifference.x << ", "
  //       << this->snapshot.originTexturePositionDifference.y << ", " << this->snapshot.originTexturePositionDifference.z << ")"
  //       << ", radarScale=" << this->snapshot.radarScale
  //       << ", isValid=" << (this->snapshot.isValid ? "true" : "false");
  //   logger::info(oss.str());
  //   hasLoggedHudSnapshot = true;
  // }

  return true;
}

bool Hud::isSnapshotValid(const HudSnapshot &snapshot) const
{
  if (snapshot.visibilitySizeMax <= 0.0f || snapshot.visibilitySize <= 0.0f)
  {
    return false;
  }

  if (!std::isfinite(snapshot.mapTextureScale) || snapshot.mapTextureScale <= 0.0f)
  {
    return false;
  }

  if (!std::isfinite(snapshot.radarScale) || snapshot.radarScale <= 0.0f)
  {
    return false;
  }

  return snapshot.mapTexturePosition.length() > 1.0f;
}

std::uintptr_t Hud::findHudElement(const char *targetName)
{
  auto process = Engine::getProcess();

  int rootIndex = process->read<int>(this->address + offsets::hud::CCSGO_HudTree::Base + 0x18);
  uint32_t treeFlags = process->read<uint32_t>(this->address + offsets::hud::CCSGO_HudTree::Base + 0x0C);
  uintptr_t dataPool = process->read<uintptr_t>(this->address + offsets::hud::CCSGO_HudTree::Pool);

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
