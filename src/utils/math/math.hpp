#pragma once

#include <cmath>

#include <imgui.h>
#include "core/engine/types/vector2.hpp"
#include "core/engine/types/vector3.hpp"
#include "core/engine/types/structs.hpp"

inline Vector3 anglesToForward(const Vector3 &angles)
{
  float pitchRad = angles.x * (3.14159265358979323846f / 180.0f);
  float yawRad = angles.y * (3.14159265358979323846f / 180.0f);

  float cp = std::cos(pitchRad);
  float sp = std::sin(pitchRad);
  float cy = std::cos(yawRad);
  float sy = std::sin(yawRad);

  Vector3 forward;
  forward.x = cp * cy;
  forward.y = cp * sy;
  forward.z = -sp;

  return forward;
}

inline bool worldToScreen(const Vector3 &worldPos, ImVec2 &screenPos, const ViewMatrix &matrix, const ImVec2 &displaySize)
{
  float w = worldPos.x * matrix.m[3][0] + worldPos.y * matrix.m[3][1] + worldPos.z * matrix.m[3][2] + matrix.m[3][3];

  if (w < 0.001f)
    return false;

  float x = worldPos.x * matrix.m[0][0] + worldPos.y * matrix.m[0][1] + worldPos.z * matrix.m[0][2] + matrix.m[0][3];
  float y = worldPos.x * matrix.m[1][0] + worldPos.y * matrix.m[1][1] + worldPos.z * matrix.m[1][2] + matrix.m[1][3];

  float ndcX = x / w;
  float ndcY = y / w;

  screenPos.x = (displaySize.x * 0.5f) + (ndcX * displaySize.x * 0.5f);
  screenPos.y = (displaySize.y * 0.5f) - (ndcY * displaySize.y * 0.5f);

  return true;
}