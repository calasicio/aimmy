#pragma once

#include <string_view>

namespace logger
{
  enum class Level
  {
    info,
    warning,
    error,
    fatal,
    debug
  };

  void log(Level level, std::string_view message);

  void info(std::string_view message);
  void warning(std::string_view message);
  void error(std::string_view message);
  void fatal(std::string_view message);
  void debug(std::string_view message);
}