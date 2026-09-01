#include "cache.hpp"

#include <mutex>

#include "core/engine/engine.hpp"

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
  };
}

bool Cache::initImpl()
{
  hud.init();

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
  localPlayer.update();

  std::lock_guard<std::mutex>
      lock(mtx);

  return true;
}