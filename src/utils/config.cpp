#include "config.hpp"
#include <fstream>
#include <string>
#include <sstream>

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

  std::string rcsEn = GetJsonValue(jsonStr, "\"rcs_enabled\"");
  if (!rcsEn.empty())
    rcsEnabled = (rcsEn == "true" || rcsEn == "1");

  std::string rcsHumanizerEn = GetJsonValue(jsonStr, "\"rcs_humanizer_enabled\"");
  if (!rcsHumanizerEn.empty())
    rcsHumanizerEnabled = (rcsHumanizerEn == "true" || rcsHumanizerEn == "1");

  std::string rcsPerf = GetJsonValue(jsonStr, "\"rcs_perfection\"");
  if (!rcsPerf.empty())
    rcsPerfection = std::stoi(rcsPerf);
  
  std::string rcsSmooth = GetJsonValue(jsonStr, "\"rcs_smoothness\"");
  if (!rcsSmooth.empty())
    rcsSmoothness = std::stoi(rcsSmooth);

  std::string rcsJit = GetJsonValue(jsonStr, "\"rcs_jitter\"");
  if (!rcsJit.empty())
    rcsJitter = std::stoi(rcsJit);
}
