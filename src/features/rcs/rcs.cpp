#include "rcs.hpp"

#include <string>

#include "core/engine/cache/cache.hpp"
#include "utils/mouse/mouse.hpp"
#include "utils/logger/logger.hpp"

const float CORRECTION_FACTOR = 3.37;
const float YAW_PITCH_FACTOR = 0.022;

void RCS::update(float dt)
{
  auto snapshot = Cache::copySnapshot();

  if (!snapshot.localPlayer.isAlive)
  {
    oldAimPunch = {0.0, 0.0};
    accumulatedError = {0.0, 0.0};
    return;
  }

  Vector3 aimPunch = snapshot.localPlayer.aimPunch;
  Vector2 currentAimPunch = {aimPunch.x, aimPunch.y};

  if (snapshot.localPlayer.shotsFired <= 1)
  {
    oldAimPunch = currentAimPunch;
    accumulatedError = {0.0, 0.0};
    return;
  }

  Vector2 deltaPunch = (currentAimPunch - oldAimPunch);

  deltaPunch *= -1.0 * CORRECTION_FACTOR;

  Vector2 moveAmount = {
      (deltaPunch.y / snapshot.globals.sensitivity) / -YAW_PITCH_FACTOR + accumulatedError.x,
      (deltaPunch.x / snapshot.globals.sensitivity) / YAW_PITCH_FACTOR + accumulatedError.y};

  int moveX = static_cast<int>(moveAmount.x);
  int moveY = static_cast<int>(moveAmount.y);

  accumulatedError.x = moveAmount.x - moveX;
  accumulatedError.y = moveAmount.y - moveY;

  if (moveX != 0 || moveY != 0)
  {
    mouse::moveMouseRelative(moveX, moveY);
  }

  oldAimPunch = currentAimPunch;
};