#pragma once

#include <mutex>

#include "core/engine/classes/game/game.hpp"
#include "core/engine/classes/globals/globals.hpp"
#include "core/engine/classes/player/local_player.hpp"

struct Snapshot
{
  Game game;
  Globals globals;
  LocalPlayer localPlayer;
};

class Cache
{
public:
  Game game;
  Globals globals;
  LocalPlayer localPlayer;

public:
  static Cache &getInstance()
  {
    static Cache instance{};
    return instance;
  }

  static Snapshot copySnapshot();

  static bool update();

private:
  std::mutex mtx;

  bool updateImpl();
};
