#include "game.hpp"

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"

bool Game::update()
{
  auto process = Engine::getProcess();
  auto client = Engine::getClient();

  float sensitivity_ = process->read<float>(client.base + offsets::dwSensitivity + offsets::dwSensitivity_sensitivity);

  if (sensitivity_ <= 0.0 || !sensitivity_)
  {
    sensitivity = 1.0;
  }
  else
  {
    sensitivity = sensitivity_;
  }

  // Update view matrix
  this->viewMatrix = process->read<ViewMatrix>(client.base + offsets::dwViewMatrix);

  // Update entity list and list entry
  this->entityList = process->read<uintptr_t>(client.base + offsets::dwEntityList);
  this->listEntry = process->read<uintptr_t>(this->entityList + 0x10);

  const int width = process->read<int>(client.base + offsets::dwWindowWidth);
  const int height = process->read<int>(client.base + offsets::dwWindowHeight);

  this->windowSize = {static_cast<float>(width), static_cast<float>(height)};

  return true;
}
