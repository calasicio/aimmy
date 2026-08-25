#pragma once
#include <string_view>

namespace Config
{
  inline bool rcsEnabled = true;
  inline bool rcsHumanizerEnabled = true;
  inline int rcsPerfection = 70;
  inline int rcsSmoothness = 100;
  inline int rcsJitter = 100;

  void LoadSettings(std::string_view filePath);
}
