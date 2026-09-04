#include "globals.hpp"

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"

bool Globals::update()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  this->address = process->read<std::uintptr_t>(client.base + offsets::globals::dwGlobalVars);

  if (!this->address)
    return false;

  this->maxClients = process->read<int>(this->address + offsets::globals::m_iMaxClients);

  auto mapNameAddress = process->read<DWORD64>(this->address + offsets::globals::m_strMapName);

  if (!process->read_raw(mapNameAddress, this->mapName, sizeof(this->mapName)))
    return false;

  return true;
}
