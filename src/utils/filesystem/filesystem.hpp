#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace filesystem
{
  bool exists(const fs::path &path);
  bool isFile(const fs::path &path);
  bool isDirectory(const fs::path &path);

  bool createDirectory(const fs::path &path);

  bool remove(const fs::path &path);
  bool copy(const fs::path &from, const fs::path &to);

  json readJson(const fs::path &path);
  bool writeJson(const fs::path &path, const json &data);
}