#include "rcs.hpp"
#include "rcs_constants.hpp"

#include <cmath>
#include <algorithm>
#include <string>

#include "core/engine/cache/cache.hpp"
#include "utils/random/random.hpp"
#include "utils/mouse/mouse.hpp"
#include "utils/logger/logger.hpp"

inline float smoothstep(float t)
{
  t = std::clamp(t, 0.0f, 1.0f);
  return t * t * (SMOOTHSTEP_A - SMOOTHSTEP_B * t);
}

void RCS::update(float dt)
{
  Cache::withLock([this](const Cache &cache)
                  {
    if (!cache.localPlayer.isAlive)
    {
      resetState(std::nullopt);
      return;
    }

    Vector3 aimPunch = cache.localPlayer.aimPunch;
    Vector2 currentAimPunch = {aimPunch.x, aimPunch.y};

    if (cache.localPlayer.shotsFired <= 1)
    {
      decayState();

      Vector2 deltaPunch = limitedDeltaPunch;

      Vector2 moveAmount = {
          (deltaPunch.y / (cache.convars.sensitivity * -YAW_PITCH_FACTOR)) + accumulatedError.x,
        (deltaPunch.x / (cache.convars.sensitivity * YAW_PITCH_FACTOR)) + accumulatedError.y};

      int moveX = static_cast<int>(moveAmount.x);
      int moveY = static_cast<int>(moveAmount.y);

      accumulatedError.x = moveAmount.x - moveX;
      accumulatedError.y = moveAmount.y - moveY;

      if (moveX != 0 || moveY != 0)
        mouse::moveMouseRelative(moveX, moveY);

      oldAimPunch = currentAimPunch;

      if (isSettled())
        resetState(currentAimPunch);

      return;
    }

    Vector2 rawDeltaPunch = (currentAimPunch - oldAimPunch);
    rawDeltaPunch = -rawDeltaPunch * 2;

    // Pattern recall imperfection (slight rhythmic mis-calibration)
    {
      float recallError = std::sin(cache.localPlayer.shotsFired * RECALL_FREQ) * RECALL_AMP;
      rawDeltaPunch.y += recallError;
    }

    Vector2 deltaPunch = rawDeltaPunch;

    // EMA low-pass filter
    {
      deltaPunch = {
          filteredDeltaPunch.x + EMA_ALPHA * (rawDeltaPunch.x - filteredDeltaPunch.x),
          filteredDeltaPunch.y + EMA_ALPHA * (rawDeltaPunch.y - filteredDeltaPunch.y)};
      filteredDeltaPunch = deltaPunch;
    }

    // Acceleration rate limiting with smoothstep (per-axis)
    {
      Vector2 desired = deltaPunch;
      Vector2 diff = desired - limitedDeltaPunch;

      float diffLenX = std::abs(diff.x);
      if (diffLenX > 0.0f)
      {
        float t = (std::min)(diffLenX / MAX_ACCEL_X, 1.0f);
        float easedT = smoothstep(t);
        diff.x = (diff.x / diffLenX) * (MAX_ACCEL_X * easedT);
      }

      float diffLenY = std::abs(diff.y);
      if (diffLenY > 0.0f)
      {
        float t = (std::min)(diffLenY / MAX_ACCEL_Y, 1.0f);
        float easedT = smoothstep(t);
        diff.y = (diff.y / diffLenY) * (MAX_ACCEL_Y * easedT);
      }

      limitedDeltaPunch += diff;
      deltaPunch = limitedDeltaPunch;
    }

    // Colored noise
    {
      Vector2 whiteNoise = {
          random::rangeFloat(-1.0f, 1.0f),
          random::rangeFloat(-1.0f, 1.0f)};

      noiseOffset = noiseOffset * NOISE_CORRELATION + whiteNoise * NOISE_SCALE * (1.0f - NOISE_CORRELATION) - noiseOffset * NOISE_MEAN_REVERSION;

      deltaPunch += noiseOffset;
    }

    Vector2 moveAmount = {
        (deltaPunch.y / (cache.convars.sensitivity * -YAW_PITCH_FACTOR)) + accumulatedError.x,
        (deltaPunch.x / (cache.convars.sensitivity * YAW_PITCH_FACTOR)) + accumulatedError.y};

    int moveX = static_cast<int>(moveAmount.x);
    int moveY = static_cast<int>(moveAmount.y);

    accumulatedError.x = moveAmount.x - moveX;
    accumulatedError.y = moveAmount.y - moveY;

    if (moveX != 0 || moveY != 0)
      mouse::moveMouseRelative(moveX, moveY);

    oldAimPunch = currentAimPunch; });
};

void RCS::decayState()
{
  filteredDeltaPunch *= RECOVERY_DECAY;
  limitedDeltaPunch *= RECOVERY_DECAY;
  noiseOffset *= RECOVERY_DECAY;
}

bool RCS::isSettled() const
{
  float residual = std::sqrt(
      limitedDeltaPunch.x * limitedDeltaPunch.x +
      limitedDeltaPunch.y * limitedDeltaPunch.y);

  return residual < SETTLED_THRESHOLD;
}

void RCS::resetState(std::optional<Vector2> aimPunch)
{
  if (aimPunch.has_value())
    oldAimPunch = aimPunch.value();
  else
    oldAimPunch = {0.0f, 0.0f};

  accumulatedError = {0.0f, 0.0f};
  filteredDeltaPunch = {0.0f, 0.0f};
  limitedDeltaPunch = {0.0f, 0.0f};
  noiseOffset = {0.0f, 0.0f};
}