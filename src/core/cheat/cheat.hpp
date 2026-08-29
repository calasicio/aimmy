#pragma once

#include "features/rcs/rcs.hpp"

class Cheat
{
public:
  ~Cheat() = default;
  Cheat(const Cheat &) = delete;
  Cheat(Cheat &&) = delete;
  Cheat &operator=(const Cheat &) = delete;
  Cheat &operator=(Cheat &&) = delete;

  static bool init();
  static void thread();
  static void destroy();

private:
  Cheat() {};

  static Cheat &getInstance()
  {
    static Cheat i{};
    return i;
  }

  bool initImpl();
  void threadImpl();
  void destroyImpl();

private:
  RCS rcs;

private:
  bool isRunning = true;
};