#include "engine.hpp"

#include <chrono>
#include <thread>

#include "core/offsets/dumper.hpp"
#include "utils/logger/logger.hpp"

bool Engine::init()
{
  return getInstance().initImpl();
}

pProcess &Engine::getProcess()
{
  return getInstance().process_;
}

bool Engine::initImpl()
{
  if (!process_.AttachProcess("cs2.exe"))
  {
    logger::fatal("Could not find process, please make sure the game is open");
    return false;
  }

  if (Dumper::init())
  {
    logger::info("Successfully dumped offsets");
  }
  else
  {
    logger::fatal("Failed to dump offsets, cannot continue execution");
    return false;
  }

  logger::info("Successfully initialized engine...");

  return true;
}