#include "logger.hpp"

#include <iostream>

namespace logger
{
  static const char *levelToString(Level level)
  {
    switch (level)
    {
    case Level::info:
      return "INFO";

    case Level::warning:
      return "WARNING";

    case Level::error:
      return "ERROR";

    case Level::fatal:
      return "FATAL";

    case Level::debug:
      return "DEBUG";
    }

    return "UNKNOWN";
  }

  void log(Level level, std::string_view message)
  {
    std::cout
        << "[Aimmy] ["
        << levelToString(level)
        << "] : "
        << message
        << '\n';
  }

  void info(std::string_view message)
  {
    log(Level::info, message);
  }

  void warning(std::string_view message)
  {
    log(Level::warning, message);
  }

  void error(std::string_view message)
  {
    log(Level::error, message);
  }

  void fatal(std::string_view message)
  {
    log(Level::fatal, message);
  }

  void debug(std::string_view message)
  {
    log(Level::debug, message);
  }
}