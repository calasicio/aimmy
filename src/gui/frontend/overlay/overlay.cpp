#include "overlay.hpp"

#include <imgui.h>

#include "utils/math/math.hpp"

bool Overlay::init()
{
  return getInstance().initImpl();
}

void Overlay::render()
{
  return getInstance().renderImpl();
}

bool Overlay::initImpl()
{
  return true;
}

void Overlay::renderImpl()
{
  renderFollowRecoil();
}

const float YAW_PITCH_FACTOR = 0.022f;

void Overlay::renderFollowRecoil()
{
  auto &io = ImGui::GetIO();
  auto *d = ImGui::GetBackgroundDrawList();

  auto snapshot = Cache::copySnapshot();
  auto &shotsFired = snapshot.localPlayer.shotsFired;
  
  if (shotsFired <= 0)
  {
    return;
  }

  auto &aimPunch = snapshot.localPlayer.aimPunch;
  auto &viewAngle = snapshot.localPlayer.viewAngle;
  auto &viewMatrix = snapshot.game.viewMatrix;
  auto &cameraPosition = snapshot.localPlayer.cameraPos;

  Vector3 firingAngles;
  firingAngles.x = viewAngle.x + (aimPunch.x * 2.0f);
  firingAngles.y = viewAngle.y + (aimPunch.y * 2.0f);
  firingAngles.z = 0.0f;

  Vector3 forward = anglesToForward(firingAngles);

  Vector3 targetWorldPos = {
      cameraPosition.x + (forward.x * 150.0f),
      cameraPosition.y + (forward.y * 150.0f),
      cameraPosition.z + (forward.z * 150.0f)};

  ImVec2 bulletPoint;
  if (worldToScreen(targetWorldPos, bulletPoint, viewMatrix, io.DisplaySize))
  {
    d->AddCircleFilled(bulletPoint, 3.0f, IM_COL32(0, 0, 255, 255), 12);
  }
}