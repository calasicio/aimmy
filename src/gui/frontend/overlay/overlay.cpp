#include "overlay.hpp"

#include <imgui.h>
#include <algorithm>

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
  auto snapshot = Cache::copySnapshot();
  renderFollowRecoil(snapshot);
  renderRadar(snapshot);
}

const float YAW_PITCH_FACTOR = 0.022f;

void Overlay::renderFollowRecoil(Snapshot snapshot)
{
  auto &io = ImGui::GetIO();
  auto *d = ImGui::GetBackgroundDrawList();

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

void Overlay::renderRadar(Snapshot snapshot)
{
  auto &io = ImGui::GetIO();
  auto *d = ImGui::GetBackgroundDrawList();

  const auto &hudSnap = snapshot.hud.snapshot;
  const auto &convars = snapshot.convars;

  if (!snapshot.hud.isInitialized || !hudSnap.isValid)
    return;

  const float hudScaling = std::max<float>(convars.hudScaling, 0.01f);
  const float safeZoneX = convars.safeZoneX;
  const float safeZoneY = convars.safeZoneY;
  const float clHudRadarScale = std::max<float>(convars.hudRadarScale, 0.01f);

  const float dispW = io.DisplaySize.x;
  const float dispH = io.DisplaySize.y;

  float logicalW = dispW, logicalH = dispH;
  float scaleX = 1.0f, scaleY = 1.0f;

  const float gameW = snapshot.game.windowSize.x;
  const float gameH = snapshot.game.windowSize.y;
  if (gameW >= 640.0f && gameH >= 480.0f && gameW <= 7680.0f && gameH <= 4320.0f)
  {
    float dispAspect = dispW / dispH;
    float gameAspect = gameW / gameH;
    if (std::fabs(dispAspect - gameAspect) >= 0.01f)
    {
      logicalW = gameW;
      logicalH = gameH;
      scaleX = dispW / logicalW;
      scaleY = dispH / logicalH;
    }
  }

  const float resScale = logicalH / 1080.0f;
  const float hudPadding = hudScaling * 5.0f * resScale;
  const float radarPaddingX = (logicalW * 0.5f) * (1.0f - safeZoneX) + hudPadding;
  const float radarPaddingY = (logicalH * 0.5f) * (1.0f - safeZoneY) + hudPadding;

  const float radarHudScaling = clHudRadarScale * hudScaling * resScale;
  const float radarSize = 290.0f * radarHudScaling;

  const float radarMinX = radarPaddingX;
  const float radarMinY = radarPaddingY;
  const float radarMaxX = radarPaddingX + radarSize;
  const float radarMaxY = radarPaddingY + radarSize;
  const float radarCenterX = radarMinX + radarSize * 0.5f;
  const float radarCenterY = radarMinY + radarSize * 0.5f;

  constexpr float PI = 3.14159265358979f;
  const float yaw = convars.radarRotate
                        ? (snapshot.localPlayer.viewAngle.y * (PI / 180.0f) + PI * 0.5f)
                        : PI;

  const float radarScale = hudSnap.isRound
                               ? hudSnap.radarScale
                               : (hudSnap.visibilitySize / hudSnap.visibilitySizeMax);

  const float radarToTextureScale = radarScale * hudSnap.mapTextureScale;

  const float iconScaleMin = std::max<float>(convars.radarIconScaleMin, 0.1f);
  float dotRadius = std::clamp(radarScale, 0.0f, 1.0f) * (1.25f - iconScaleMin) + iconScaleMin;
  dotRadius *= 7.5f * radarHudScaling * resScale;

  for (const auto &player : snapshot.players)
  {
    if (player.health <= 0)
      continue;

    if (player.teamNum == snapshot.localPlayer.teamNum && !convars.teammatesAreEnemies)
      continue;

    const float dx = player.origin.x - hudSnap.mapTexturePosition.x;
    const float dy = hudSnap.mapTexturePosition.y - player.origin.y;

    const float px = dx * radarToTextureScale - hudSnap.originTexturePositionDifference.x;
    const float py = dy * radarToTextureScale - hudSnap.originTexturePositionDifference.y;
    const float distSq = px * px + py * py;

    float rx = px, ry = py;
    bool outOfBounds = false;

    if (hudSnap.isRound)
    {
      const float cosYaw = std::cos(yaw);
      const float sinYaw = std::sin(yaw);

      if (distSq >= hudSnap.maxVisibilitySquared)
      {
        const float clampScale = std::sqrt(hudSnap.maxVisibilitySquared / distSq);
        rx = -px * clampScale * cosYaw + py * clampScale * sinYaw;
        ry = -px * clampScale * sinYaw - py * clampScale * cosYaw;
        outOfBounds = true;
      }
      else
      {
        rx = -px * cosYaw + py * sinYaw;
        ry = -px * sinYaw - py * cosYaw;
      }
    }

    const float sx = radarCenterX + rx * radarHudScaling;
    const float sy = radarCenterY + ry * radarHudScaling;

    if (sx < radarMinX || sx > radarMaxX || sy < radarMinY || sy > radarMaxY)
      continue;

    const ImU32 color = convars.teammatesAreEnemies
                            ? (player.teamNum == 3 ? IM_COL32(100, 150, 255, 255) : IM_COL32(255, 50, 50, 255))
                            : IM_COL32(255, 50, 50, 255);

    const float drawX = sx * scaleX;
    const float drawY = sy * scaleY;

    if (outOfBounds)
    {
      float len = std::sqrt(rx * rx + ry * ry);
      if (len > 0.0f)
      {
        float dirX = rx / len;
        float dirY = ry / len;
        float tipX = sx - dirX * dotRadius;
        float tipY = sy - dirY * dotRadius;

        ImVec2 p1((tipX + dirY * dotRadius) * scaleX, (tipY - dirX * dotRadius) * scaleY);
        ImVec2 p2((tipX + 2.0f * dirX * dotRadius) * scaleX, (tipY + 2.0f * dirY * dotRadius) * scaleY);
        ImVec2 p3((tipX - dirY * dotRadius) * scaleX, (tipY + dirX * dotRadius) * scaleY);
        d->AddTriangleFilled(p1, p2, p3, color);
      }
    }
    else
    {
      d->AddCircleFilled(ImVec2(drawX, drawY), dotRadius * scaleY, color, 12);
    }
  }
}