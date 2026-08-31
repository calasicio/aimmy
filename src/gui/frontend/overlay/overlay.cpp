#include "overlay.hpp"
#include "imgui.h"

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
  auto &aimPunch = snapshot.localPlayer.aimPunch;
  // auto &sensitivity = snapshot.globals.sensitivity;

  ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

  float offsetX = (aimPunch.y * 0.2) / YAW_PITCH_FACTOR;
  float offsetY = (aimPunch.x * 0.2) / YAW_PITCH_FACTOR;

  ImVec2 bulletPoint(center.x - offsetX, center.y + offsetY);

  d->AddCircle(bulletPoint, 3.0f, IM_COL32(255, 0, 0, 180), 12, 1.0f);
}