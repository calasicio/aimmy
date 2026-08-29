#include "cheat.hpp"

#include "core/engine/engine.hpp"
#include "utils/logger/logger.hpp"

bool Cheat::init()
{
  return getInstance().initImpl();
}

void Cheat::thread()
{
  return getInstance().threadImpl();
}

void Cheat::destroy()
{
  return getInstance().destroyImpl();
}

bool Cheat::initImpl()
{
  SetForegroundWindow(Engine::getProcess()->hwnd_);

  logger::info("Succesfully initialized cheat...");
  return true;
}

void Cheat::threadImpl()
{
  while (isRunning)
  {
  }
}

void Cheat::destroyImpl()
{
  isRunning = false;
}
