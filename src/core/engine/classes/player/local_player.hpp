#pragma once

#include "player.hpp"

class LocalPlayer : public Player
{
public:
  LocalPlayer() : Player() {}

  bool update();

public:
  Vector2 relVelocity = {0, 0};
  float maxMovementSpeed = 250;

  bool isOnGround = true;
  uint8_t moveType;

  Vector3 viewAngle = {0, 0, 0};
  Vector3 cameraPos = {0, 0, 0};
  Vector3 aimPunch = {0, 0, 0};

private:
  bool getPawn();
  bool getController();

  bool updatePawn();
  bool updateAimPunch();

  void updateVelocity();
  void updateMovement();
  void updateWeapon();
};
