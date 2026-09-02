#include "engine.hpp"

#include <chrono>
#include <string>
#include <thread>

using namespace std::chrono_literals;

#include "core/engine/cache/cache.hpp"
#include "core/offsets/dumper.hpp"
#include "utils/logger/logger.hpp"

bool Engine::init()
{
  return getInstance().initImpl();
}

ProcessModule Engine::getClient()
{
  return getInstance().client;
}

ProcessModule Engine::getEngine()
{
  return getInstance().engine;
}

ProcessModule Engine::getTier0()
{
  return getInstance().tier0;
}

std::shared_ptr<pProcess> Engine::getProcess()
{
  return getInstance().process;
}

bool Engine::initImpl()
{
  process = std::make_shared<pProcess>();

  if (!this->awaitProcess())
  {
    logger::fatal("Could not find process, please make sure the game is open");
    return false;
  }

  if (!this->awaitModules())
  {
    logger::fatal("Game took too long to load, please open me again once its fully loaded");
    return false;
  }

  if (Dumper::init())
  {
    logger::info("Successfully dumped offsets");
  }
  else
  {
    logger::warning("Failed to dump offsets");
    logger::info("Using default offsets");
  }

  if (Cache::init())
  {
    logger::info("Successfully initialized cache");
  }
  else
  {
    logger::fatal("Failed to initialize cache");
    return false;
  }

  std::thread(&Engine::thread, this).detach();

  logger::info("Successfully initialized engine...");

  return true;
}

void Engine::thread()
{
  while (true)
  {
    Cache::update();
  }
}

bool Engine::awaitProcess()
{
  if (!process || process->handle_)
  {
    return false;
  }

  do
  {
    if (process->AttachProcess("cs2.exe"))
      break;

    if (process->pid_ && !process->handle_)
    {
      logger::fatal("Insufficient permissions to open a handle to the process. Try running as Administrator.");
      return false;
    }

    static int attempts = 0;

    if (!attempts)
    {
      logger::info("Waiting 50s for the game to open...");
    }

    if (attempts > 10)
      return false;
    attempts++;

    std::this_thread::sleep_for(5s);
  } while (true);

  return true;
}

bool Engine::awaitModules()
{
  if (!process || !process->handle_)
    return false;

  logger::info("Waiting for the game to load...");

  do
  {
    this->client = process->GetModule("client.dll");
    this->engine = process->GetModule("engine2.dll");
    this->tier0 = process->GetModule("tier0.dll");

    if (this->client.base && this->engine.base && this->tier0.base)
      break;

    static int attempts = 0;
    if (attempts > 10)
      return false;
    attempts++;

    std::this_thread::sleep_for(5s);
  } while (true);

  return true;
}