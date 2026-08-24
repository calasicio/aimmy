#include "rcs.hpp"
#include "core/memory.hpp"
#include "game/offsets.hpp"
#include "game/structures.hpp"
#include "utils/mouse.hpp"
#include "utils/config.hpp"
#include <Windows.h>
#include <iostream>

struct CUtlVector_t
{
  uintptr_t count = 0;
  uintptr_t data = 0;
};

Vector2 oldPunch{0.0f, 0.0f};

float CORRECTION_FACTOR = 3.37f;

float getSensitivity(uintptr_t clientBase)
{
  float sensitivity = g_pProcess.read<float>(clientBase + cs2_dumper::offsets::client_dll::dwSensitivity + cs2_dumper::offsets::client_dll::dwSensitivity_sensitivity);

  if (sensitivity <= 0.0f || !sensitivity)
  {
    return 1.0f;
  }
}

void features::ExecuteRCS(uintptr_t localPlayerPawn, uintptr_t clientBase)
{
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

  Vector3 aimPunch = g_pProcess.read<Vector3>(aimPunchCache.data + (aimPunchCache.count - 1) * sizeof(Vector3));
  Vector2 currentPunch{aimPunch.x, aimPunch.y};

  Vector2 deltaPunch = (currentPunch - oldPunch) * -1.0 * CORRECTION_FACTOR;

  float sensitivity = getSensitivity(clientBase);

  int pixelMoveX = static_cast<int>((deltaPunch.y / sensitivity) / -0.022f);
  int pixelMoveY = static_cast<int>((deltaPunch.x / sensitivity) / 0.022f);

  if (pixelMoveX != 0 || pixelMoveY != 0)
  {
    utils::MoveMouseRelative(pixelMoveX, pixelMoveY);
  }

  oldPunch = currentPunch;
}
