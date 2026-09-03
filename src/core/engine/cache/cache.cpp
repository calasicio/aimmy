#include "cache.hpp"

#include <mutex>

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"

bool Cache::init()
{
  return getInstance().initImpl();
}

bool Cache::update()
{
  return getInstance().updateImpl();
}

Snapshot Cache::copySnapshot()
{
  std::lock_guard<std::mutex> lock(getInstance().mtx);
  return {
      getInstance().game,
      getInstance().globals,
      getInstance().hud,
      getInstance().convars,
      getInstance().localPlayer,
      getInstance().players,
  };
}

bool Cache::initImpl()
{
  hud.init();
  convars.init();

  return true;
}

bool Cache::updateImpl()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  if (!process)
  {
    return false;
  }

  game.update();
  globals.update();
  hud.update();
  convars.update();
  localPlayer.update();

  std::vector<Player> tempPlayerList;
  tempPlayerList.reserve(12);

  if (game.entityList != 0)
  {
    for (int i = 0; i < globals.maxClients; i++)
    {
      uintptr_t controllerAddr = process->read<DWORD64>(game.listEntry + (i + 1) * 0x70);

      if (!controllerAddr)
        continue;

      auto player = Player(i, game.entityList, game.listEntry);

      if (!player.update())
        continue;

      if (player.pawn == localPlayer.pawn)
        continue;

      tempPlayerList.push_back(player);
    }
  }

  std::lock_guard<std::mutex>
      lock(mtx);

  players = std::move(tempPlayerList);

  return true;
}