#include "config.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

std::string GetJsonValue(const std::string &json, std::string_view key)
{
  size_t keyPos = json.find(key);
  if (keyPos == std::string::npos)
    return "";

  size_t colonPos = json.find(":", keyPos);
  if (colonPos == std::string::npos)
    return "";

  size_t valStart = json.find_first_not_of(" \t\"", colonPos + 1);
  if (valStart == std::string::npos)
    return "";

  size_t valEnd = json.find_first_of(",}\n\"", valStart);
  if (valEnd == std::string::npos)
    return json.substr(valStart);

  return json.substr(valStart, valEnd - valStart);
}

void Config::LoadSettings(std::string_view filePath)
{
  std::ifstream file(filePath.data());
  if (!file.is_open())
    return;

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string jsonStr = buffer.str();
  file.close();

  auto loadBool = [&](std::string_view key, bool &target)
  {
    std::string val = GetJsonValue(jsonStr, key);
    if (!val.empty())
      target = (val == "true" || val == "1");
  };

  auto loadInt = [&](std::string_view key, int &target)
  {
    std::string val = GetJsonValue(jsonStr, key);
    if (!val.empty())
    {
      try
      {
        target = std::stoi(val);
      }
      catch (...)
      {
      }
    }
  };

  loadBool("\"rcs_enabled\"", rcsEnabled);
  loadBool("\"rcs_humanizer_enabled\"", rcsHumanizerEnabled);

  loadInt("\"rcs_control\"", rcsControl);
  loadInt("\"rcs_reaction_speed\"", rcsReactionSpeed);
  loadInt("\"rcs_smoothness\"", rcsSmoothness);
  loadInt("\"rcs_stability\"", rcsStability);
  loadInt("\"rcs_aggression\"", rcsAggression);

  // Clamp values
  rcsControl = std::clamp(rcsControl, 0, 100);
  rcsReactionSpeed = std::clamp(rcsReactionSpeed, 0, 100);
  rcsSmoothness = std::clamp(rcsSmoothness, 0, 100);
  rcsStability = std::clamp(rcsStability, 0, 100);
  rcsAggression = std::clamp(rcsAggression, 0, 100);
}