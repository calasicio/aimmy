#include "globals.hpp"

#include "core/engine/engine.hpp"
#include "core/offsets/offsets.hpp"

bool Globals::update()
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

  return true;
}
