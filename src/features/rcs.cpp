#include "rcs.hpp"
#include "core/memory.hpp"
#include "game/offsets.hpp"
#include "game/structures.hpp"
#include "utils/mouse.hpp"
#include "utils/config.hpp"
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <random>

/**** Vars ****/

struct CUtlVector_t
{
  uintptr_t count = 0;
  uintptr_t data = 0;
};

Vector2 oldPunch{0.0f, 0.0f};
Vector2 accumulatedSub{0.0f, 0.0f};

float CORRECTION_FACTOR = 3.37f;

std::random_device rd;
std::mt19937 gen(rd());

/**** Utils ****/

float getSensitivity(uintptr_t clientBase)
{
  float sensitivity = g_pProcess.read<float>(clientBase + cs2_dumper::offsets::client_dll::dwSensitivity + cs2_dumper::offsets::client_dll::dwSensitivity_sensitivity);

  if (sensitivity <= 0.0f || !sensitivity)
  {
    return 1.0f;
  }
}

Vector2 computeBezierPoint(Vector2 start, Vector2 control, Vector2 end, float t)
{
  float u = 1.0f - t;

  return (start * (u * u)) + (control * (2.0f * u * t)) + (end * (t * t));
}

/**** Core Function ****/

void features::ExecuteRCS(uintptr_t localPlayerPawn, uintptr_t clientBase)
{
  // ======== Sanity Checks ========

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

  int localHealth = g_pProcess.read<int>(localPlayerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
  if (localHealth <= 0 || localHealth > 100)
  {
    oldPunch = {0.0f, 0.0f};
    return;
  }

  int shotsFired = g_pProcess.read<int>(localPlayerPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iShotsFired);
  if (shotsFired <= 1)
  {
    oldPunch = {0.0f, 0.0f};
    accumulatedSub = {0.0f, 0.0f};
    return;
  }

  uintptr_t aimPunchServices = g_pProcess.read<uintptr_t>(localPlayerPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_pAimPunchServices);
  if (!aimPunchServices)
  {
    oldPunch = {0.0f, 0.0f};
    return;
  }

  CUtlVector_t aimPunchCache = g_pProcess.read<CUtlVector_t>(aimPunchServices + 0x88);

  if (aimPunchCache.count <= 0 || aimPunchCache.count >= 0xFFFF)
  {
    oldPunch = {0.0f, 0.0f};
    return;
  }

  // ======== Core Logic ========

  Vector3 aimPunch = g_pProcess.read<Vector3>(aimPunchCache.data + (aimPunchCache.count - 1) * sizeof(Vector3));
  Vector2 currentPunch{aimPunch.x, aimPunch.y};

  Vector2 deltaPunch = (currentPunch - oldPunch) * -1.0 * CORRECTION_FACTOR;

  float sensitivity = getSensitivity(clientBase);

  Vector2 rawPixel = {
      (deltaPunch.y / sensitivity) / -0.022f,
      (deltaPunch.x / sensitivity) / 0.022f};

  if (rawPixel.x != 0.0f && rawPixel.y != 0.0f)
  {
    // --- Humanizer ---
    Vector2 target = rawPixel;
    if (Config::rcsHumanizerEnabled)
    {
      //>> Error things

      if (Config::rcsPerfection < 100)
      {
        float perfection = static_cast<float>(Config::rcsPerfection);
        if (perfection < 0.0f)
          perfection = 0.0f;
        if (perfection > 100.0f)
          perfection = 100.0f;

        float maxErrorScale = (100.0f - perfection) / 100.0f;

        float minMultX = 0.0f, maxMultX = 0.0f;
        float minMultY = 0.0f, maxMultY = 0.0f;

        if (shotsFired < 9)
        {
          minMultX = 1.8f;
          maxMultX = 0.15f;
          minMultY = 1.95f;
          maxMultY = 0.15f;
        }
        else
        {
          minMultX = 2.0f;
          maxMultX = 0.1f;
          minMultY = 2.0f;
          maxMultY = 0.01f;
        }

        std::uniform_real_distribution<float> errorDistX(1.0f - (maxErrorScale * minMultX), 1.0f + (maxErrorScale * maxMultX));
        std::uniform_real_distribution<float> errorDistY(1.0f - (maxErrorScale * minMultY), 1.0f + (maxErrorScale * maxMultY));

        target.x *= errorDistX(gen);
        target.y *= errorDistY(gen);

        if (abs(target.x) > 4.0f)
        {
          std::uniform_real_distribution<float> catchUpDist(0.70f, 1.0f);
          target.x *= (catchUpDist(gen) + (perfection / 100.0f * 0.30f));
        }
      }

      //>> Bezier Anchor Randomization

      if (Config::rcsSmoothness > 0)
      {
        float maxCurveDist = (static_cast<float>(Config::rcsSmoothness) / 100.0f) * 20.0f;
        std::uniform_real_distribution<float> controlDist(-maxCurveDist, maxCurveDist);

        Vector2 startPt{0.0f, 0.0f};
        Vector2 endPt{target.x, target.y};
        Vector2 controlPt{(target.x / 2.0f) + controlDist(gen), (target.y / 2.0f) + controlDist(gen)};

        target = computeBezierPoint(startPt, controlPt, endPt, 1.0f);
      }

      //>> Perlin biomid tremor jitter
      if (Config::rcsJitter > 0 && shotsFired > 3)
      {
        float maxJitter = (static_cast<float>(Config::rcsJitter) / 100.0f) * 5.0f;
        std::uniform_real_distribution<float> jitterDist(-maxJitter, maxJitter);

        target.x += jitterDist(gen);
        target.y += jitterDist(gen);
      }
    }

    target.x += accumulatedSub.x;
    target.y += accumulatedSub.y;

    // --- Mouse Movement ---
    int dispatchX = static_cast<int>(target.x);
    int dispatchY = static_cast<int>(target.y);

    accumulatedSub.x = target.x - static_cast<float>(dispatchX);
    accumulatedSub.y = target.y - static_cast<float>(dispatchY);

    if (dispatchX != 0 || dispatchY != 0)
    {
      utils::MoveMouseRelative(dispatchX, dispatchY);
    }
  }

  oldPunch = currentPunch;
}
