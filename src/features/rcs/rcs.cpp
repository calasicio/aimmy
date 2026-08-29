#include "rcs.hpp"

#include <string>

#include "core/engine/cache/cache.hpp"
#include "utils/mouse/mouse.hpp"
#include "utils/logger/logger.hpp"

const float CORRECTION_FACTOR = 3.37;
const float YAW_PITCH_FACTOR = 0.022;

const float EMA_ALPHA = 0.1f;

void RCS::update(float dt)
{
  auto snapshot = Cache::copySnapshot();

  if (!snapshot.localPlayer.isAlive)
  {
    oldAimPunch = {0.0, 0.0};
    accumulatedError = {0.0, 0.0};
    filteredDeltaPunch = {0.0, 0.0};
    return;
  }

  Vector3 aimPunch = snapshot.localPlayer.aimPunch;
  Vector2 currentAimPunch = {aimPunch.x, aimPunch.y};

  if (snapshot.localPlayer.shotsFired <= 1)
  {
    oldAimPunch = currentAimPunch;
    accumulatedError = {0.0, 0.0};
    filteredDeltaPunch = {0.0, 0.0};
    return;
  }

  Vector2 rawDeltaPunch = (currentAimPunch - oldAimPunch);

  rawDeltaPunch *= -1.0 * CORRECTION_FACTOR;

  Vector2 deltaPunch = rawDeltaPunch;

  // Low pass filter, Exponential Moving Average (EMA)
  {
    Vector2 emaDeltaPunch = {
        filteredDeltaPunch.x + EMA_ALPHA * (rawDeltaPunch.x - filteredDeltaPunch.x),
        filteredDeltaPunch.y + EMA_ALPHA * (rawDeltaPunch.y - filteredDeltaPunch.y)};
    deltaPunch = emaDeltaPunch;
    filteredDeltaPunch = emaDeltaPunch;
  }

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