#include "rcs.hpp"
#include "core/memory.hpp"
#include "game/offsets.hpp"
#include "game/structures.hpp"
#include "utils/mouse.hpp"
#include "utils/config.hpp"
#include <Windows.h>
#include <algorithm>

RCS g_RCS;

float RCS::RandomFloat(float min, float max)
{
  return std::uniform_real_distribution<float>(min, max)(gen);
}

int RCS::RandomInt(int min, int max)
{
  return std::uniform_int_distribution<int>(min, max)(gen);
}

Vector2 RCS::ComputeBezier(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, float t)
{
  float u = 1.0f - t;
  return (p0 * (u * u)) + (p1 * (2.0f * u * t)) + (p2 * (t * t));
}

float RCS::getSensitivity(uintptr_t clientBase)
{
  float sens = g_pProcess.read<float>(clientBase + cs2_dumper::offsets::client_dll::dwSensitivity +
                                      cs2_dumper::offsets::client_dll::dwSensitivity_sensitivity);
  return (sens <= 0.0f) ? 1.0f : sens;
}

RCS::MappedValues RCS::MapConfig()
{
  MappedValues m;

  // Control Level: 100 = perfect, 0 = chaotic
  // Maps to: error scale (0.0 to 1.0)
  m.errorScale = (100.0f - Config::rcsControl) / 100.0f;

  // Reaction Speed: 100 = instant (8ms), 0 = slow (25ms)
  float speedT = Config::rcsReactionSpeed / 100.0f;
  m.baseReactionDelay = (25.0f - (speedT * 17.0f)) / 1000.0f; // 25ms -> 8ms
  m.reactionVariance = (20.0f - (speedT * 15.0f)) / 1000.0f;  // 20ms -> 5ms variance

  // Smoothness: 0 = straight lines, 100 = very curved
  m.curveAmount = (Config::rcsSmoothness / 100.0f) * 30.0f; // 0-30 pixels

  // Stability: 100 = rock solid, 0 = very shaky
  m.jitterAmount = ((100.0f - Config::rcsStability) / 100.0f) * 6.0f; // 0-6 pixels

  // Aggression: 0 = gentle, 100 = hard yank
  m.overcompensation = 1.0f + ((Config::rcsAggression / 100.0f) * 0.15f); // 1.0 to 1.15

  // Attention Lapse: Lower control = more "forgetting" to correct
  m.lapseChance = 0.02f + (m.errorScale * 0.13f); // 2% to 15%

  return m;
}

void RCS::Update(uintptr_t localPlayerPawn, uintptr_t clientBase)
{
  auto cfg = MapConfig();
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
  float reactionDelay = cfg.baseReactionDelay + (RandomFloat(0.0f, cfg.reactionVariance) * cfg.errorScale);

  if (elapsed < reactionDelay)
    return;

  // Attention lapse: occasionally skip correction (distraction)
  if (RandomFloat(0.0f, 1.0f) < cfg.lapseChance)
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
    // Aggression affects initial overcompensation
    target.y *= cfg.overcompensation + (RandomFloat(0.0f, 0.05f) * cfg.errorScale);
  }
  else if (midSpray)
  {
    // Horizontal gets harder to control
    target.x *= 1.0f + (0.05f * cfg.errorScale);
  }
  else if (lateSpray)
  {
    // Fatigue: more variance
    float fatigue = (std::min)((shotsFired - 10) * 0.012f, 0.12f) * cfg.errorScale;
    target.x *= (1.0f + RandomFloat(-fatigue, fatigue));
    target.y *= (1.0f + RandomFloat(-fatigue * 0.5f, fatigue * 0.5f));
  }

  // === ERROR INJECTION ===
  if (Config::rcsHumanizerEnabled && cfg.errorScale > 0.0f)
  {
    float varX = (shotsFired < 9) ? 0.12f : 0.20f;
    float varY = (shotsFired < 9) ? 0.18f : 0.10f;

    target.x *= (1.0f + RandomFloat(-varX, varX) * cfg.errorScale);
    target.y *= (1.0f + RandomFloat(-varY, varY) * cfg.errorScale);

    // Micro-overshoot (30% chance, only when not perfect)
    if (RandomFloat(0.0f, 1.0f) < 0.3f * cfg.errorScale && shotsFired > 3 && shotsFired < 25)
    {
      float overshoot = RandomFloat(1.03f, 1.10f);
      target.y *= overshoot;
      pendingCorrection.y = target.y * (1.0f - overshoot) * 0.6f;
    }
  }

  // === BEZIER SMOOTHING ===
  if (Config::rcsSmoothness > 0 && cfg.curveAmount > 0.0f)
  {
    Vector2 start{0.0f, 0.0f};
    Vector2 end{target.x, target.y};

    float biasX = (lastShotsFired > 0) ? (target.x * 0.15f) : 0.0f;
    float biasY = (lastShotsFired > 0) ? (target.y * 0.15f) : 0.0f;

    Vector2 control{
        (target.x / 2.0f) + RandomFloat(-cfg.curveAmount, cfg.curveAmount) + biasX,
        (target.y / 2.0f) + RandomFloat(-cfg.curveAmount, cfg.curveAmount) + biasY};

    // Ease-out cubic sampling
    float t = 1.0f - std::pow(1.0f - RandomFloat(0.6f, 1.0f), 3.0f);
    target = ComputeBezier(start, control, end, t);
  }

  // === JITTER (Stability) ===
  if (Config::rcsStability < 100 && cfg.jitterAmount > 0.0f)
  {
    float jitterMult = (shotsFired <= 3) ? 1.5f : 1.0f;
    target.x += RandomFloat(-cfg.jitterAmount, cfg.jitterAmount) * jitterMult;
    target.y += RandomFloat(-cfg.jitterAmount, cfg.jitterAmount) * jitterMult;
  }

  // === CROSSHAIR DRIFT ===
  aimDrift.x += RandomFloat(-0.4f, 0.4f);
  aimDrift.y += RandomFloat(-0.4f, 0.4f);
  aimDrift = aimDrift * 0.92f;

  float driftStrength = (Config::rcsControl > 70) ? 0.4f : 0.8f;
  target += aimDrift * driftStrength;

  // === SETTLING ===
  if (wasSpraying && shotsFired <= lastShotsFired)
  {
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

  // Pixel walk
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

void features::ExecuteRCS(uintptr_t localPlayerPawn, uintptr_t clientBase)
{
  g_RCS.Update(localPlayerPawn, clientBase);
}