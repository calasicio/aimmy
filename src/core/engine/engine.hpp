#pragma once

#include <memory>

#include "core/memory/memory.hpp"

class Engine
{
public:
  static bool init();
  static pProcess &getProcess();

private:
  Engine() = default;

  static Engine &getInstance()
  {
    static Engine instance;
    return instance;
  }

  bool initImpl();

  pProcess process_;
};