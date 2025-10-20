#pragma once
#include "graphics/PointCloud.h" // For the Point struct
#include <vector>

class AffineFractal {
public:
  // ‼️ Add an enum to specify which fractal to generate
  enum class FractalType { SierpinskiTetrahedron, TwistedCube };

  AffineFractal();
  // ‼️ Update the generate function signature
  std::vector<Point> generate(FractalType type, int num_points);

private:
  struct AffineTransform {
    glm::mat3 matrix;
    glm::vec3 translation;
    glm::vec4 color;
  };

  std::vector<AffineTransform> m_transforms;

  // ‼️ Add setup functions for each fractal type
  void setup_sierpinski_tetrahedron();
  void setup_twisted_cube();
};
