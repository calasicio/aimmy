#pragma once

#include <cstdint>

#include "core/engine/types/vector3.hpp"

class LocalPlayer
{
public:
  LocalPlayer() {}

  bool update();

public:
  int health = 0;
  int shotsFired = 0;
  Vector3 aimPunch = {0, 0, 0};

  bool isAlive = false;

public:
  std::uintptr_t pawn;
  std::uintptr_t controller;

private:
  bool getPawn();
  bool getController();

  bool updatePawn();
  bool updateAimPunch();
};