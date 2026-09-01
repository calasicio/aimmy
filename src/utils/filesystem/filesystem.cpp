#include "filesystem.hpp"

#include <fstream>

namespace filesystem
{
  bool exists(const fs::path &path)
  {
    return fs::exists(path);
  }

  bool isFile(const fs::path &path)
  {
    return fs::is_regular_file(path);
  }

  bool isDirectory(const fs::path &path)
  {
    return fs::is_directory(path);
  }

  bool createDirectory(const fs::path &path)
  {
    return fs::create_directories(path);
  }

  bool remove(const fs::path &path)
  {
    return fs::remove(path);
  }

  bool copy(const fs::path &from, const fs::path &to)
  {
    std::error_code error;

    fs::copy_file(
        from,
        to,
        fs::copy_options::overwrite_existing,
        error);

    return !error;
  }

  json readJson(const fs::path &path)
  {
    std::ifstream file(path);

    if (!file)
      return {};

    try
    {
      return json::parse(file);
    }
    catch (const json::exception &)
    {
      return {};
    }
  }

  bool writeJson(const fs::path &path, const json &data)
  {
    std::ofstream file(path);

    if (!file)
      return false;

    try
    {
      file << data.dump(4);
    }
    catch (const json::exception &)
    {
      return false;
    }

    return true;
  }
}