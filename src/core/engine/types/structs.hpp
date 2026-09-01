#pragma once

#include "vector3.hpp"

struct ViewMatrix
{
  float m[4][4];

  const float *operator[](int index) const { return m[index]; }
  float *operator[](int index) { return m[index]; }
};

struct HudElementNode_t
{
  int left_child_index;
  int right_child_index;
  int parent_index;
  int color_flags;
  uintptr_t string_address;
  uintptr_t element_address;
};

struct CCSGO_HudRadar_t
{
  bool m_is_round;
  Vector3 m_map_texture_position;
  float m_visibility_size_max;
  float m_visibility_size;
  float m_map_texture_scale;
  float m_max_visibility_squared;
  Vector3 m_origin_texture_position_difference;
};
