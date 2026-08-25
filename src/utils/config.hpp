#pragma once
#include <string_view>

namespace Config
{
  inline bool rcsEnabled = true;
  inline bool rcsHumanizerEnabled = true;
  
  inline int rcsControl = 75;        // "Control Level" - overall accuracy/consistency
  inline int rcsReactionSpeed = 50;  // "Reaction Speed" - how fast to respond to kick
  inline int rcsSmoothness = 30;      // "Smoothness" - curve of mouse path
  inline int rcsStability = 60;       // "Stability" - hand steadiness/shake
  inline int rcsAggression = 40;      // "Aggression" - initial pull strength

  void LoadSettings(std::string_view filePath);
}