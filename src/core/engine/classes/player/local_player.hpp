#pragma once

#include "player.hpp"

class LocalPlayer : public Player
{
public:
  LocalPlayer() : Player(), shotsFired(0) {}

  bool update();

public:
  int shotsFired;
  Vector3 viewAngle = {0, 0, 0};
  Vector3 cameraPos = {0, 0, 0};
  Vector3 aimPunch = {0, 0, 0};

private:
  bool getPawn();
  bool getController();

  bool updatePawn();
  bool updateAimPunch();
};
