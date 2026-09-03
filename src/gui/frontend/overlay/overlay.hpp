#pragma once

#include "core/engine/cache/cache.hpp"

class Overlay
{
public:
  ~Overlay() = default;
  Overlay(const Overlay &) = delete;
  Overlay(Overlay &&) = delete;
  Overlay &operator=(const Overlay &) = delete;
  Overlay &operator=(Overlay &&) = delete;

  static bool init();
  static void render();

private:
  Overlay() {};

  static Overlay &getInstance()
  {
    static Overlay i{};
    return i;
  }

  bool initImpl();
  void renderImpl();

  void renderFollowRecoil(Snapshot snapshot);
  void renderRadar(Snapshot snapshot);
};