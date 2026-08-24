#pragma once
#include <string_view>

namespace Config
{
  inline bool rcsEnabled = true;
  inline float rcsStrengthX = 1.0f;
  inline float rcsStrengthY = 1.0f;

  void LoadSettings(std::string_view filePath);
}
