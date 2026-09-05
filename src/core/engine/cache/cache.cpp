#include "cache.hpp"

#include <iostream>
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
  std::lock_guard<std::mutex>
      lock(mtx);

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
  tempPlayerList.reserve(globals.maxClients);

  if (game.entityList != 0)
  {
    for (int i = 0; i < globals.maxClients; i++)
    {
      uintptr_t controllerAddr = process->read<DWORD64>(game.listEntry + (i + 1) * 0x70);

      if (!controllerAddr)
        continue;

      auto it = std::find_if(players.begin(), players.end(), [i](const Player &p)
                             { return p.index == i; });

      Player player = (it != players.end()) ? *it : Player(i, game.entityList, game.listEntry);

      if (!player.update())
        continue;

      if (player.pawn == localPlayer.pawn)
      {
        localPlayer.index = i;
        continue;
      }

      tempPlayerList.push_back(player);
    }
  }

  for (auto &enemy : tempPlayerList)
  {
    if (enemy.teamNum == localPlayer.teamNum && !this->convars.teammatesAreEnemies)
      continue;

    if (!enemy.isAlive)
      continue;

    bool isSpotted = false;
    if (localPlayer.isAlive && localPlayer.index != -1)
    {
      isSpotted = (enemy.spottedMask & (1 << localPlayer.index)) != 0;
    }

    auto now = std::chrono::steady_clock::now();
    bool isHeard = false;

    if (now - enemy.lastSoundMade.timestamp <= std::chrono::seconds(1))
    {
      if (localPlayer.isAlive && enemy.origin.distance(localPlayer.origin) <= enemy.lastSoundMade.radius)
      {
        isHeard = true;
      }
    }

    if ((!isHeard || !isSpotted) && !this->convars.teammatesAreEnemies)
    {
      for (const auto &teammate : tempPlayerList)
      {
        if (teammate.teamNum != localPlayer.teamNum || !teammate.isAlive)
          continue;

        if (!isSpotted && (enemy.spottedMask & (1 << teammate.index)) != 0)
        {
          isSpotted = true;
        }

        if (!isHeard && (now - enemy.lastSoundMade.timestamp <= std::chrono::seconds(1)))
        {
          if (enemy.origin.distance(teammate.origin) <= enemy.lastSoundMade.radius)
          {
            isHeard = true;
          }
        }

        if (isSpotted && isHeard)
        {
          break;
        }
      }
    }

    if (isHeard)
    {
      enemy.lastHeard = now;
    }

    enemy.isShownInRadar = (now - enemy.lastHeard <= std::chrono::seconds(5)) || isSpotted;
  }

  players = std::move(tempPlayerList);

  debugData();

  return true;
}

void Cache::debugData()
{
  if (this->hasAlreadyLoggedData)
    return;

  this->hasAlreadyLoggedData = true;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "CACHE DEBUG:" << std::endl;
  std::cout << "cache.game    |" << std::endl;
  std::cout << "              |- entityList = " << this->game.entityList << std::endl;
  std::cout << "              |- listEntry = " << this->game.listEntry << std::endl;
  std::cout << "              |- windowSize = " << this->game.windowSize << std::endl;
  std::cout << std::endl;
  std::cout << "cache.globals |" << std::endl;
  std::cout << "              |- maxClients = " << this->globals.maxClients << std::endl;
  std::cout << "              |- mapName = " << this->globals.mapName << std::endl;
  std::cout << "              |- inMatch = " << this->globals.inMatch << std::endl;
  std::cout << std::endl;
  std::cout << "cache.convars |" << std::endl;
  std::cout << "              |- svAccelerate = " << this->convars.svAccelerate << std::endl;
  std::cout << "              |- svFriction = " << this->convars.svFriction << std::endl;
  std::cout << "              |- svStopSpeed = " << this->convars.svStopSpeed << std::endl;
  std::cout << "              |- svMaxSpeed = " << this->convars.svMaxSpeed << std::endl;
  std::cout << "              |- sensitivity = " << this->convars.sensitivity << std::endl;
  std::cout << "              |- teammatesAreEnemies = " << this->convars.teammatesAreEnemies << std::endl;
}