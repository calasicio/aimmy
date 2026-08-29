#include "rcs.hpp"

#include <string>

#include "core/engine/cache/cache.hpp"
#include "utils/random/random.hpp"
#include "utils/mouse/mouse.hpp"
#include "utils/logger/logger.hpp"

const float CORRECTION_FACTOR = 3.37;
const float YAW_PITCH_FACTOR = 0.022;

/* EMA_ALPHA: How fast the target moves
 *
 * Value: 1.0 = No smoothing
 *        0.0 = Heavy smoothing, drags behind raw input, ignoring
 *              spikes and only tracking sustained trends
 *
 */
const float EMA_ALPHA = 0.25f;

/* MAX_ACCEL: How fast the output can chase
 *
 * Value: Infinite = No limit, snaps to whatever EMA target is immediately
 *        0.8      = Output can only change by 0.8 units per frame to the target,
 *                   no matter how far away the target is
 *
 * Controls the maximum frame-to-frame delta of the final correction. Adds inertia.
 * Makes output ramps up gradually
 *
 */
const float MAX_ACCEL = 0.5f;

const float NOISE_SCALE = 0.12f;
const float NOISE_CORRELATION = 0.9f;

void RCS::update(float dt)
{
  auto snapshot = Cache::copySnapshot();

  if (!snapshot.localPlayer.isAlive)
  {
    resetState(std::nullopt);
    return;
  }

  Vector3 aimPunch = snapshot.localPlayer.aimPunch;
  Vector2 currentAimPunch = {aimPunch.x, aimPunch.y};

  if (snapshot.localPlayer.shotsFired <= 1)
  {
    resetState(currentAimPunch);
    return;
  }

  Vector2 rawDeltaPunch = (currentAimPunch - oldAimPunch);

  rawDeltaPunch *= -1.0 * CORRECTION_FACTOR;

  Vector2 deltaPunch = rawDeltaPunch;

  // Low pass filter, Exponential Moving Average (EMA)
  {
    deltaPunch = {
        filteredDeltaPunch.x + EMA_ALPHA * (rawDeltaPunch.x - filteredDeltaPunch.x),
        filteredDeltaPunch.y + EMA_ALPHA * (rawDeltaPunch.y - filteredDeltaPunch.y)};
    filteredDeltaPunch = deltaPunch;
  }

  // Acceleration rate limiting
  {
    Vector2 desired = deltaPunch;
    Vector2 diff = desired - limitedDeltaPunch;

    float diffLen = diff.length();
    if (diffLen > MAX_ACCEL && diffLen > 0.0f)
    {
      diff = (diff / diffLen) * MAX_ACCEL;
    }

    limitedDeltaPunch += diff;
    deltaPunch = limitedDeltaPunch;
  }

  // Colored Noise
  {
    Vector2 whiteNoise = {
        random::rangeFloat(-1.0f, 1.0f),
        random::rangeFloat(-1.0f, 1.0f)};

    noiseOffset = noiseOffset * NOISE_CORRELATION + whiteNoise * NOISE_SCALE * (1.0f - NOISE_CORRELATION) - noiseOffset * 0.02f;

    deltaPunch += noiseOffset;
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

void RCS::resetState(std::optional<Vector2> aimPunch)
{
  if (aimPunch.has_value())
  {
    oldAimPunch = aimPunch.value();
  }
  else
  {
    oldAimPunch = {0.0, 0.0};
  }

  accumulatedError = {0.0, 0.0};
  filteredDeltaPunch = {0.0, 0.0};
  limitedDeltaPunch = {0.0, 0.0};
  noiseOffset = {0.0, 0.0};
}