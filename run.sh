#!/bin/bash
set -e

BUILD_DIR="build"
NUM_CORES=$(nproc)

# Detect executable name(s) from CMakeLists.txt
EXECUTABLES=($(grep -Po '(?<=add_executable\()\s*\K[^\s\)]+' CMakeLists.txt))

if [[ ${#EXECUTABLES[@]} -eq 0 ]]; then
  echo "Error: No add_executable() found in CMakeLists.txt."
  exit 1
elif [[ ${#EXECUTABLES[@]} -eq 1 ]]; then
  EXECUTABLE_NAME="${EXECUTABLES[0]}"
else
  echo "Multiple executables found:"
  select EXECUTABLE_NAME in "${EXECUTABLES[@]}"; do
    [[ -n "$EXECUTABLE_NAME" ]] && break
  done
fi

# Handle command line arguments
if [[ "$1" == "--build" ]]; then
  echo "--- Rebuilding from scratch ---"
  rm -rf "$BUILD_DIR"
fi

# Configure and build if needed
if [[ ! -d "$BUILD_DIR" ]]; then
  echo "--- No build folder found. Building project ---"
  cmake -S . -B "$BUILD_DIR" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
  echo "--- Building with $NUM_CORES cores ---"
  cmake --build "$BUILD_DIR" -- -j"$NUM_CORES"
  echo "--- Build complete! ---"
fi

# Locate executable (supports nested build targets)
EXEC_PATH=$(find "$BUILD_DIR" -type f -name "$EXECUTABLE_NAME" -perm -u+x | head -n 1)

if [[ -f "$EXEC_PATH" ]]; then
  echo "--- Running $EXECUTABLE_NAME ---"
  "$EXEC_PATH"
else
  echo "Error: Executable '$EXECUTABLE_NAME' not found in $BUILD_DIR"
  exit 1
fi
