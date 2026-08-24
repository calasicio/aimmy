#pragma once
#include <string_view>

namespace Config
{
  inline bool rcsEnabled = true;
  inline bool rcsHumanizerEnabled = true;
  inline int rcsSmoothness = 15;
  inline int rcsJitter = 20;

  void LoadSettings(std::string_view filePath);
}
