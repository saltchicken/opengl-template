#pragma once
#include "graphics/PointCloud.h" // For the Point struct
#include <vector>

class AffineFractal {
public:
  AffineFractal();
  std::vector<Point> generate(int num_points);

private:
  struct AffineTransform {
    glm::mat3 matrix;
    glm::vec3 translation;
    glm::vec4 color;
  };

  std::vector<AffineTransform> m_transforms;
  void setup_sierpinski_tetrahedron();
};
