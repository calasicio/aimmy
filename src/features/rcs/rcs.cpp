#include "rcs.hpp"

#include <string>

#include "core/engine/cache/cache.hpp"
#include "utils/mouse/mouse.hpp"
#include "utils/logger/logger.hpp"

const float CORRECTION_FACTOR = 3.37;

void RCS::update(float dt)
{
  auto snapshot = Cache::copySnapshot();

  if (!snapshot.localPlayer.isAlive)
  {
    oldAimPunch = {0.0, 0.0};
    return;
  }

  if (snapshot.localPlayer.shotsFired <= 1)
  {
    oldAimPunch = {0.0f, 0.0f};
    return;
  }

  Vector3 aimPunch = snapshot.localPlayer.aimPunch;
  Vector2 currentAimPunch = {aimPunch.x, aimPunch.y};

  Vector2 deltaPunch = (currentAimPunch - oldAimPunch);

  deltaPunch *= -1.0 * CORRECTION_FACTOR;

  Vector2 moveAmount = {
      (deltaPunch.y / snapshot.globals.sensitivity) / -0.022f + accumulatedError.x,
      (deltaPunch.x / snapshot.globals.sensitivity) / 0.022f + accumulatedError.y};

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