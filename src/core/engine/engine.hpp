#pragma once

#include <memory>

#include "core/memory/memory.hpp"

class Engine
{
public:
  ~Engine() = default;
  Engine(const Engine &) = delete;
  Engine(Engine &&) = delete;
  Engine &operator=(const Engine &) = delete;
  Engine &operator=(Engine &&) = delete;

  static bool init();
  static ProcessModule getClient();
  static ProcessModule getEngine();
  static ProcessModule getTier0();
  static std::shared_ptr<pProcess> getProcess(); // Refactor this so its easier to access
private:
  Engine() {};

  static Engine &getInstance()
  {
    static Engine i{};
    return i;
  }

  bool initImpl();

  bool awaitProcess();
  bool awaitModules();

  void thread();

private:
  std::shared_ptr<pProcess> process;
  ProcessModule client;
  ProcessModule engine;
  ProcessModule tier0;
};