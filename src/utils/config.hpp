#pragma once
#include <string_view>

namespace Config
{
  inline bool rcsEnabled = true;
  inline bool rcsHumanizerEnabled = true;
  inline int rcsPerfection = 50;
  inline int rcsSmoothness = 50;
  inline int rcsJitter = 50;

  void LoadSettings(std::string_view filePath);
}
