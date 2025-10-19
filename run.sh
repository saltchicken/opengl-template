#!/bin/bash
set -e

BUILD_DIR="build"
EXECUTABLE_NAME="OpenGLTemplate" # <-- Change to your target name if needed
NUM_CORES=$(nproc)               # Detects the number of CPU cores

# Parse command-line arguments
if [[ "$1" == "--build" ]]; then
  echo "--- Rebuilding from scratch ---"
  rm -rf "$BUILD_DIR"
fi

# If no build directory, configure and build
if [[ ! -d "$BUILD_DIR" ]]; then
  echo "--- No build folder found. Building project ---"
  cmake -S . -B "$BUILD_DIR" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
  echo "--- Building with $NUM_CORES cores ---"
  cmake --build "$BUILD_DIR" -- -j"$NUM_CORES"
  echo "--- Build complete! ---"
fi

# Locate executable if it exists
EXEC_PATH="$BUILD_DIR/$EXECUTABLE_NAME"

if [[ -f "$EXEC_PATH" ]]; then
  echo "--- Running executable ---"
  "$EXEC_PATH"
else
  echo "Error: Executable not found at $EXEC_PATH"
  echo "Make sure the target name matches your CMake project or set EXECUTABLE_NAME correctly."
  exit 1
fi
