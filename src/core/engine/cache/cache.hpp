#pragma once

#include "core/engine/classes/player/local_player.hpp"

struct Snapshot
{
  LocalPlayer localPlayer;
};

class Cache
{
public:
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
