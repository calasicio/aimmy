#include "rcs.hpp"
#include "core/memory.hpp"
#include "game/offsets.hpp"
#include "game/structures.hpp"
#include "utils/mouse.hpp"
#include "utils/config.hpp"
#include <Windows.h>
#include <cmath>

float HumanizedRCS::RandomFloat(float min, float max)
{
  return std::uniform_real_distribution<float>(min, max)(gen);
}

int HumanizedRCS::RandomInt(int min, int max)
{
  return std::uniform_int_distribution<int>(min, max)(gen);
}

Vector2 HumanizedRCS::ComputeBezier(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, float t)
{
  float u = 1.0f - t;
  return (p0 * (u * u)) + (p1 * (2.0f * u * t)) + (p2 * (t * t));
}

float HumanizedRCS::getSensitivity(uintptr_t clientBase)
{
  float sens = g_pProcess.read<float>(clientBase + cs2_dumper::offsets::client_dll::dwSensitivity +
                                      cs2_dumper::offsets::client_dll::dwSensitivity_sensitivity);
  return (sens <= 0.0f) ? 1.0f : sens;
}

void HumanizedRCS::Update(uintptr_t localPlayerPawn, uintptr_t clientBase)
{
  auto now = std::chrono::steady_clock::now();

  // Sanity checks
  if (!Config::rcsEnabled)
  {
    oldPunch = {0.0f, 0.0f};
    return;
  }

  HWND gameWindow = FindWindowA("SDL_app", "Counter-Strike 2");
  if (GetForegroundWindow() != gameWindow)
  {
    oldPunch = {0.0f, 0.0f};
    return;
  }

  int health = g_pProcess.read<int>(localPlayerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
  if (health <= 0 || health > 100)
  {
    oldPunch = {0.0f, 0.0f};
    return;
  }

  int shotsFired = g_pProcess.read<int>(localPlayerPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iShotsFired);
  if (shotsFired <= 1)
  {
    oldPunch = {0.0f, 0.0f};
    accumulatedSub = {0.0f, 0.0f};
    wasSpraying = false;
    return;
  }

  // === TIMING HUMANIZATION ===
  float elapsed = std::chrono::duration<float>(now - lastCorrectionTime).count();
  float perfection = std::clamp(static_cast<float>(Config::rcsPerfection), 0.0f, 100.0f);

  // Reaction delay: 8ms base + up to 17ms variance based on perfection
  float reactionDelay = BASE_REACTION_DELAY + (RandomFloat(0.0f, 0.017f) * (100.0f - perfection) / 100.0f);
  if (elapsed < reactionDelay)
    return;

  // Attention lapse: 5-15% chance to skip frame
  float lapseChance = 0.05f + (0.10f * (100.0f - perfection) / 100.0f);
  if (RandomFloat(0.0f, 1.0f) < lapseChance)
  {
    lastCorrectionTime = now;
    return;
  }

  // === READ AIM PUNCH ===
  uintptr_t aimPunchServices = g_pProcess.read<uintptr_t>(localPlayerPawn +
                                                          cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_pAimPunchServices);
  if (!aimPunchServices)
    return;

  struct CUtlVector_t
  {
    uintptr_t count;
    uintptr_t data;
  };
  CUtlVector_t cache = g_pProcess.read<CUtlVector_t>(aimPunchServices + 0x88);
  if (cache.count <= 0 || cache.count >= 0xFFFF)
    return;

  struct Vector3
  {
    float x, y, z;
  };
  Vector3 aimPunch = g_pProcess.read<Vector3>(cache.data + (cache.count - 1) * sizeof(Vector3));
  Vector2 currentPunch{aimPunch.x, aimPunch.y};

  // === CORE CALCULATION ===
  Vector2 deltaPunch = (currentPunch - oldPunch) * -1.0f * CORRECTION_FACTOR;

  // Apply pending correction from previous overshoot
  deltaPunch += pendingCorrection;
  pendingCorrection = {0.0f, 0.0f};

  float sensitivity = getSensitivity(clientBase);

  Vector2 rawPixel = {
      (deltaPunch.y / sensitivity) / -YAW_PITCH_SCALE,
      (deltaPunch.x / sensitivity) / YAW_PITCH_SCALE};

  if (rawPixel.x == 0.0f && rawPixel.y == 0.0f)
  {
    oldPunch = currentPunch;
    return;
  }

  Vector2 target = rawPixel;

  // === SPRAY PHASE MODELING ===
  bool earlySpray = shotsFired <= 3;
  bool midSpray = shotsFired > 3 && shotsFired <= 10;
  bool lateSpray = shotsFired > 10;

  if (earlySpray)
  {
    target.y *= 1.08f + (RandomFloat(0.0f, 0.06f) * (100.0f - perfection) / 100.0f);
  }
  else if (midSpray)
  {
    target.x *= 1.05f;
  }
  else if (lateSpray)
  {
    float fatigue = (std::min)((shotsFired - 10) * 0.012f, 0.12f);
    target.x *= (1.0f + RandomFloat(-fatigue, fatigue));
    target.y *= (1.0f + RandomFloat(-fatigue * 0.5f, fatigue * 0.5f));
  }

  // === ERROR INJECTION ===
  if (Config::rcsHumanizerEnabled && perfection < 100.0f)
  {
    float errorScale = (100.0f - perfection) / 100.0f;

    // Phase-dependent error multipliers (FIXED: proper min/max)
    float minErrX, maxErrX, minErrY, maxErrY;

    if (shotsFired < 9)
    {
      minErrX = 0.88f;
      maxErrX = 1.12f; // 12% variance
      minErrY = 0.82f;
      maxErrY = 1.18f; // 18% variance (vertical harder)
    }
    else
    {
      minErrX = 0.80f;
      maxErrX = 1.20f; // 20% variance (late spray)
      minErrY = 0.90f;
      maxErrY = 1.10f; // 10% variance
    }

    // Scale by perfection
    float rangeX = (maxErrX - minErrX) * errorScale;
    float rangeY = (maxErrY - minErrY) * errorScale;

    target.x *= (1.0f - (rangeX * 0.5f) + RandomFloat(0.0f, rangeX));
    target.y *= (1.0f - (rangeY * 0.5f) + RandomFloat(0.0f, rangeY));

    // Micro-overshoot (30% chance)
    if (RandomFloat(0.0f, 1.0f) < 0.3f * errorScale && shotsFired > 3 && shotsFired < 25)
    {
      float overshoot = RandomFloat(1.03f, 1.10f);
      target.y *= overshoot;
      // Queue correction for next frame
      pendingCorrection.y = target.y * (1.0f - overshoot) * 0.6f;
    }
  }

  // === BEZIER SMOOTHING (FIXED: variable t with easing) ===
  if (Config::rcsSmoothness > 0)
  {
    float maxCurve = (static_cast<float>(Config::rcsSmoothness) / 100.0f) * 25.0f;

    Vector2 start{0.0f, 0.0f};
    Vector2 end{target.x, target.y};

    // Control point with bias from previous movement
    float biasX = (lastShotsFired > 0) ? (target.x * 0.15f) : 0.0f;
    float biasY = (lastShotsFired > 0) ? (target.y * 0.15f) : 0.0f;

    Vector2 control{
        (target.x / 2.0f) + RandomFloat(-maxCurve, maxCurve) + biasX,
        (target.y / 2.0f) + RandomFloat(-maxCurve, maxCurve) + biasY};

    // Ease-out cubic: fast start, slow end
    float t = 1.0f - std::pow(1.0f - RandomFloat(0.6f, 1.0f), 3.0f);
    target = ComputeBezier(start, control, end, t);
  }

  // === PERLIN JITTER ===
  if (Config::rcsJitter > 0)
  {
    float jitterScale = static_cast<float>(Config::rcsJitter) / 100.0f;
    float jitterAmt = (shotsFired <= 3) ? 4.0f : 2.0f;

    float maxJitter = jitterScale * jitterAmt;
    target.x += RandomFloat(-maxJitter, maxJitter);
    target.y += RandomFloat(-maxJitter, maxJitter);
  }

  // === CROSSHAIR DRIFT ===
  aimDrift.x += RandomFloat(-0.4f, 0.4f);
  aimDrift.y += RandomFloat(-0.4f, 0.4f);
  aimDrift = aimDrift * 0.92f; // Decay

  float driftStrength = (perfection > 70.0f) ? 0.4f : 0.8f;
  target += aimDrift * driftStrength;

  // === SETTLING (Post-spray tremor) ===
  if (wasSpraying && shotsFired <= lastShotsFired)
  {
    // Spray just ended
    sprayEndTime = now;
  }
  wasSpraying = true;

  float timeSinceSpray = std::chrono::duration<float>(now - sprayEndTime).count();
  if (shotsFired <= lastShotsFired && timeSinceSpray < 0.25f)
  {
    float settleAmt = (0.25f - timeSinceSpray) * 12.0f;
    target.x += RandomFloat(-settleAmt, settleAmt);
    target.y += RandomFloat(-settleAmt, settleAmt);
  }

  // === MOUSE DISPATCH ===
  target.x += accumulatedSub.x;
  target.y += accumulatedSub.y;

  int dispatchX = static_cast<int>(target.x);
  int dispatchY = static_cast<int>(target.y);

  // Pixel walk error
  if (std::abs(dispatchX) > 2 && RandomFloat(0.0f, 1.0f) < 0.08f)
  {
    pixelError = RandomInt(-1, 1);
    dispatchX += pixelError;
  }
  else if (pixelError != 0 && RandomFloat(0.0f, 1.0f) < 0.7f)
  {
    dispatchX -= pixelError;
    pixelError = 0;
  }

  accumulatedSub.x = target.x - static_cast<float>(dispatchX);
  accumulatedSub.y = target.y - static_cast<float>(dispatchY);

  if (dispatchX != 0 || dispatchY != 0)
  {
    utils::MoveMouseRelative(dispatchX, dispatchY);
  }

  oldPunch = currentPunch;
  lastCorrectionTime = now;
  lastShotsFired = shotsFired;
}

HumanizedRCS g_RCS;

void features::ExecuteRCS(uintptr_t localPlayerPawn, uintptr_t clientBase)
{
  g_RCS.Update(localPlayerPawn, clientBase);
}