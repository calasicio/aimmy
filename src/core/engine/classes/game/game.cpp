#include "game.hpp"

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"

bool Game::update()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();
  auto engine = Engine::getEngine();

  // Update view matrix
  this->viewMatrix = process->read<ViewMatrix>(client.base + offsets::dwViewMatrix);

  // Update entity list and list entry
  this->entityList = process->read<uintptr_t>(client.base + offsets::dwEntityList);
  this->listEntry = process->read<uintptr_t>(this->entityList + 0x10);

  uintptr_t viewRenderPtr = process->read<uintptr_t>(client.base + offsets::dwViewRender);

  const int width = process->read<int>(viewRenderPtr + 0x440);
  const int height = process->read<int>(viewRenderPtr + 0x448);

  this->windowSize = {static_cast<float>(width), static_cast<float>(height)};

  return true;
}
