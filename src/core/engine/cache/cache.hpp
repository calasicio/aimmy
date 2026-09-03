#pragma once

#include <mutex>
#include <vector>

#include "core/engine/classes/game/game.hpp"
#include "core/engine/classes/globals/globals.hpp"
#include "core/engine/classes/hud/hud.hpp"
#include "core/engine/classes/convars/convars.hpp"
#include "core/engine/classes/player/local_player.hpp"
#include "core/engine/classes/player/player.hpp"

struct Snapshot
{
  Game game;
  Globals globals;
  Hud hud;
  LocalPlayer localPlayer;
  std::vector<Player> players;
};

class Cache
{
public:
  Game game;
  Globals globals;
  Hud hud;
  ConVars convars;
  LocalPlayer localPlayer;
  std::vector<Player> players;

public:
  static Cache &getInstance()
  {
    static Cache instance{};
    return instance;
  }

  static Snapshot copySnapshot();

  template <typename Fn>
  static void withLock(Fn &&fn)
  {
    std::lock_guard<std::mutex> lock(getInstance().mtx);
    fn(getInstance());
  }

  static bool init();
  static bool update();

private:
  std::mutex mtx;

  bool initImpl();
  bool updateImpl();
};
