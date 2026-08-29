#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>

#include "core/engine/engine.hpp"
#include "core/cheat/cheat.hpp"
#include "utils/logger/logger.hpp"

int main()
{
  if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    logger::warning("Could not set application process priority to HIGH");

  if (!Engine::init())
  {
    logger::fatal("Engine failed to initialize, cannot continue execution");
    goto exit;
  }

  if (!Cheat::init())
  {
    logger::fatal("Cheat failed to initialize, cannot continue execution");
    goto exit;
  }

  logger::info("Running main thread");

  Cheat::thread(); // lock

  return 0;
exit:
  std::cout << "Press any key to exit..." << std::endl;
  std::cin.get();
}