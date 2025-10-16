#pragma once

#include "Mesh.h"
#include <memory>

// A static factory class for creating primitive mesh shapes.
class PrimitiveFactory {
public:
  // This class is not meant to be instantiated.
  PrimitiveFactory() = delete;

  // Creates a standard 2D quad mesh centered at the origin.
  static std::shared_ptr<Mesh> create_quad();

  // TODO: Add other primitives here later
  // static std::shared_ptr<Mesh> CreateCube();
  // static std::shared_ptr<Mesh> CreateTriangle();
};
