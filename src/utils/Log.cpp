#include "utils/Log.h"
#include <iostream>

// ANSI escape codes for colors
namespace {
const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_CYAN = "\033[36m";
} // namespace

void Log::info(const std::string &message) {
  // Info messages are printed to the standard output stream.
  std::cout << "[INFO] " << message << std::endl;
}

void Log::warn(const std::string &message) {
  // Warnings are printed to the standard error stream.
  std::cerr << COLOR_YELLOW << "[WARN] " << message << COLOR_RESET << std::endl;
}

void Log::error(const std::string &message) {
  // Errors are printed to the standard error stream.
  std::cerr << COLOR_RED << "[ERROR] " << message << COLOR_RESET << std::endl;
}

#ifndef NDEBUG
void Log::debug(const std::string &message) {
  // Debug messages are printed to the standard output stream.
  std::cout << COLOR_CYAN << "[DEBUG] " << message << COLOR_RESET << std::endl;
}
#endif
