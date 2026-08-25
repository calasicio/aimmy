#pragma once
#include "game/structures.hpp"
#include <cstdint>
#include <chrono>
#include <random>

class RCS
{
public:
  void Update(uintptr_t localPlayerPawn, uintptr_t clientBase);

private:
  std::mt19937 gen{std::random_device{}()};

  // States
  Vector2 oldPunch{0.0f, 0.0f};
  Vector2 accumulatedSub{0.0f, 0.0f};
  Vector2 pendingCorrection{0.0f, 0.0f};
  Vector2 aimDrift{0.0f, 0.0f};

  std::chrono::steady_clock::time_point lastCorrectionTime;
  std::chrono::steady_clock::time_point sprayEndTime;
  int lastShotsFired = 0;
  bool wasSpraying = false;
  int pixelError = 0;

  // Constants
  static constexpr float YAW_PITCH_SCALE = 0.022f;
  static constexpr float CORRECTION_FACTOR = 3.37f;

  struct MappedValues
  {
    float errorScale;        // 0.0-1.0 from rcsControl
    float baseReactionDelay; // seconds from rcsReactionSpeed
    float reactionVariance;  // seconds from rcsReactionSpeed
    float curveAmount;       // pixels from rcsSmoothness
    float jitterAmount;      // pixels from rcsStability
    float overcompensation;  // multiplier from rcsAggression
    float lapseChance;       // 0.0-1.0 from rcsControl
  };

  MappedValues MapConfig();
  float RandomFloat(float min, float max);
  int RandomInt(int min, int max);
  Vector2 ComputeBezier(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, float t);
  float getSensitivity(uintptr_t clientBase);
};

extern RCS g_RCS;

namespace features
{
  void ExecuteRCS(std::uintptr_t localPlayerPawn, std::uintptr_t clientBase);
}