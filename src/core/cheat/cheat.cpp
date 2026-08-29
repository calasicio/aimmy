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
  auto lastFrame = std::chrono::steady_clock::now();

  while (isRunning)
  {
    const auto now = std::chrono::steady_clock::now();

    const float dt = std::chrono::duration<float>(now - lastFrame).count();
    lastFrame = now;

    if (GetAsyncKeyState(VK_END) & 0x8000)
    {
      isRunning = false;
      break;
    }

    std::this_thread::sleep_until(now + std::chrono::milliseconds(1));
  }
}

void Cheat::destroyImpl()
{
  isRunning = false;
}
