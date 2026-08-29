#include "cheat.hpp"

#include <chrono>
#include <thread>

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
    auto start = std::chrono::steady_clock::now();

    if (GetAsyncKeyState(VK_END) & 0x8000)
    {
      isRunning = false;
      break;
    }

    std::this_thread::sleep_until(start + std::chrono::milliseconds(1));
  }
}

void Cheat::destroyImpl()
{
  isRunning = false;
}
