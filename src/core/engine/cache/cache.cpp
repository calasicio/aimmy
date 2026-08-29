#include "cache.hpp"

#include <mutex>

#include "core/engine/engine.hpp"

bool Cache::update()
{
  return getInstance().updateImpl();
}

Snapshot Cache::copySnapshot()
{
  std::lock_guard<std::mutex> lock(getInstance().mtx);
  return {
      getInstance().localPlayer,
  };
}

bool Cache::updateImpl()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  if (!process)
  {
    return false;
  }

  localPlayer.update();

  std::lock_guard<std::mutex>
      lock(mtx);
}