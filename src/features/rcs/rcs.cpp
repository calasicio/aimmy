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

  Vector2 deltaPunch = (currentAimPunch - oldAimPunch) * -1.0 * CORRECTION_FACTOR;

  Vector2 moveAmount = {
      (deltaPunch.y / snapshot.globals.sensitivity) / -0.022f,
      (deltaPunch.x / snapshot.globals.sensitivity) / 0.022f};

  int moveXAmountInPixel = static_cast<int>(moveAmount.x);
  int moveYAmountInPixel = static_cast<int>(moveAmount.y);

  if (moveXAmountInPixel != 0 || moveYAmountInPixel != 0)
  {
    mouse::moveMouseRelative(moveXAmountInPixel, moveYAmountInPixel);
  }

  oldAimPunch = currentAimPunch;
};