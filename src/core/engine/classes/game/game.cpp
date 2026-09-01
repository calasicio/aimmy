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

  this->viewMatrix = process->read<ViewMatrix>(client.base + offsets::dwViewMatrix);

  return true;
}
