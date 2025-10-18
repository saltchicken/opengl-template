#pragma once

#include "scene/Component.h"
#include <glm/glm.hpp> // <-- ADD THIS LINE
#include <vector>

class AudioReactiveComponent : public Component {
public:
  // Reacts to a range of frequency bins (e.g., bins 1-5 for bass)
  // and applies the effect with a given sensitivity.
  AudioReactiveComponent(int start_bin, int end_bin, float sensitivity);

  void update(float delta_time) override;
  void awake() override;

private:
  int m_start_bin;
  int m_end_bin;
  float m_sensitivity;
  glm::vec3 m_original_scale;

  // For smoothing
  float m_current_amplitude = 0.0f;
  float m_target_amplitude = 0.0f;
};
