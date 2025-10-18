#include "scene/AudioReactiveComponent.h"
#include "audio/AudioEngine.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include <numeric>

// A simple lerp function for smoothing
float lerp(float a, float b, float f) { return a + f * (b - a); }

AudioReactiveComponent::AudioReactiveComponent(int start_bin, int end_bin,
                                               float sensitivity)
    : m_start_bin(start_bin), m_end_bin(end_bin), m_sensitivity(sensitivity) {}

void AudioReactiveComponent::awake() {
  if (m_owner && m_owner->transform) {
    m_original_scale = m_owner->transform->scale;
  } else {
    Log::error("AudioReactiveComponent needs an owner with a transform!");
  }
}

void AudioReactiveComponent::update(float delta_time) {
  if (!m_owner || !m_owner->transform) {
    return;
  }

  // We keep a static copy so we don't reallocate every frame
  static std::vector<float> freq_data;

  // This just copies the latest data if it's available.
  // The actual FFT happens on the audio thread.
  AudioEngine::get_frequency_data(freq_data);

  if (freq_data.empty()) {
    return;
  }

  // 1. Calculate the average amplitude in our designated frequency band
  float avg_amplitude = 0.0f;
  if (m_end_bin < freq_data.size() && m_start_bin < m_end_bin) {
    float sum = 0.0f;
    for (int i = m_start_bin; i <= m_end_bin; ++i) {
      sum += freq_data[i];
    }
    avg_amplitude = sum / (m_end_bin - m_start_bin + 1);
  }
  m_target_amplitude = avg_amplitude;

  // 2. Smooth the transition to the new amplitude
  // A higher value (e.g., 10.0f) means faster reaction, lower is smoother
  m_current_amplitude =
      lerp(m_current_amplitude, m_target_amplitude, delta_time * 10.0f);

  // 3. Apply the effect to the object's scale
  float scale_effect = 1.0f + m_current_amplitude * m_sensitivity;
  m_owner->transform->scale.y = m_original_scale.y * scale_effect;
  // Let's make it pulse uniformly
  m_owner->transform->scale.x = m_original_scale.x * scale_effect;
  m_owner->transform->scale.z = m_original_scale.z * scale_effect;
}
