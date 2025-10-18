#pragma once
#include <miniaudio.h>
#include <vector>

class AudioEngine {
public:
  AudioEngine() = delete;

  // Initializes the audio device and starts capturing.
  static bool init();

  // Stops capturing and cleans up resources.
  static void shutdown();

  // Fills the provided vector with the latest frequency bin magnitudes.
  // Returns false if no new data is available.
  static bool get_frequency_data(std::vector<float> &out_data);

  // Gets the OpenGL texture ID for the FFT data
  static unsigned int get_fft_texture();

private:
  // This callback is executed by miniaudio on a separate thread whenever audio
  // data is available.
  static void data_callback(ma_device *pDevice, void *pOutput,
                            const void *pInput, ma_uint32 frameCount);
};
