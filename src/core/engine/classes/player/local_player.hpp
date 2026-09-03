#pragma once

#include "player.hpp"

class LocalPlayer : public Player
{
public:
  LocalPlayer() : Player(), shotsFired(0) {}

  bool update();

public:
  int shotsFired;

  Vector3 velocity = {0, 0, 0};
  Vector2 relVelocity = {0, 0};
  float maxMovementSpeed = 250;

  Vector3 viewAngle = {0, 0, 0};
  Vector3 cameraPos = {0, 0, 0};
  Vector3 aimPunch = {0, 0, 0};


private:
  bool getPawn();
  bool getController();

  bool updatePawn();
  bool updateAimPunch();

  void updateVelocity();
};
