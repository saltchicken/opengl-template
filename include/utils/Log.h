#pragma once

#include <string>

// A simple static class for logging to the console.
class Log {
public:
  // This class is not meant to be instantiated.
  Log() = delete;

  // Logs a standard informational message (white).
  static void info(const std::string &message);

  // Logs a warning message (yellow).
  static void warn(const std::string &message);

  // Logs an error message (red).
  static void error(const std::string &message);

#ifndef NDEBUG
  static void debug(const std::string &message);
#else
  static inline void debug(const std::string &message) {}
#endif
};
