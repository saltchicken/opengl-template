#include "audio/AudioEngine.h"
#include "utils/Log.h"
#include <glad/glad.h> // For texture creation
#include <mutex>
#include <vector>

// --- KissFFT Includes ---
#include "kiss_fftr.h" // Real FFT

// --- Miniaudio Implementation ---
// Define this in one C/C++ file before including miniaudio.h
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace {
// --- Constants ---
constexpr unsigned int FFT_SIZE = 1024; // Number of audio samples for FFT
constexpr unsigned int CHANNEL_COUNT = 1;
constexpr unsigned int SAMPLE_RATE = 48000;
// Number of frequency bins we get from the FFT
constexpr unsigned int FREQUENCY_BINS = FFT_SIZE / 2 + 1;

// --- Static variables for audio processing ---
ma_device_config g_device_config;
ma_device g_device;

// --- FFT related variables ---
kiss_fftr_cfg g_fft_cfg;
std::vector<float> g_window; // Hanning window
std::vector<float> g_in_samples;
std::vector<kiss_fft_cpx> g_out_fft;

// --- Thread-safe data transfer ---
std::mutex g_fft_mutex;
std::vector<float> g_fft_magnitudes;
bool g_new_data_available = false;

// --- OpenGL Texture for shader access ---
GLuint g_fft_texture = 0;

// --- Helper Functions ---
void create_hanning_window(std::vector<float> &window, size_t size) {
  window.resize(size);
  for (size_t i = 0; i < size; ++i) {
    window[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (size - 1)));
  }
}
} // namespace

void AudioEngine::data_callback(ma_device *pDevice, void *pOutput,
                                const void *pInput, ma_uint32 frameCount) {
  // This function is our real-time audio processor.
  // It's called by miniaudio on a dedicated audio thread.

  const float *input_samples = static_cast<const float *>(pInput);

  // Ensure our buffer has the correct size
  if (g_in_samples.size() != FFT_SIZE) {
    return;
  }

  // 1. Shift old samples and add new ones
  std::copy(g_in_samples.begin() + frameCount, g_in_samples.end(),
            g_in_samples.begin());
  std::copy(input_samples, input_samples + frameCount,
            g_in_samples.end() - frameCount);

  // 2. Apply the Hanning window to reduce spectral leakage
  std::vector<float> windowed_samples(FFT_SIZE);
  for (size_t i = 0; i < FFT_SIZE; ++i) {
    windowed_samples[i] = g_in_samples[i] * g_window[i];
  }

  // 3. Perform the FFT
  kiss_fftr(g_fft_cfg, windowed_samples.data(), g_out_fft.data());

  // 4. Calculate magnitudes and transfer to the shared buffer
  {
    std::lock_guard<std::mutex> lock(g_fft_mutex);
    for (size_t i = 0; i < FREQUENCY_BINS; ++i) {
      float real = g_out_fft[i].r;
      float imag = g_out_fft[i].i;
      // We scale the magnitude to make it more visually useful
      g_fft_magnitudes[i] =
          log10(1.0f + sqrtf(real * real + imag * imag)) * 0.2f;
    }
    g_new_data_available = true;
  }
}

bool AudioEngine::init() {
  g_fft_cfg = kiss_fftr_alloc(FFT_SIZE, 0, nullptr, nullptr);
  if (!g_fft_cfg) {
    Log::error("Failed to initialize KissFFT");
    return false;
  }

  // Prepare buffers
  create_hanning_window(g_window, FFT_SIZE);
  g_in_samples.assign(FFT_SIZE, 0.0f);
  g_out_fft.resize(FREQUENCY_BINS);
  g_fft_magnitudes.resize(FREQUENCY_BINS);

  // Initialize miniaudio device config for capture
  g_device_config = ma_device_config_init(ma_device_type_loopback);
  g_device_config.capture.format = ma_format_f32;
  g_device_config.capture.channels = CHANNEL_COUNT;
  g_device_config.sampleRate = SAMPLE_RATE;
  g_device_config.dataCallback = data_callback;
  g_device_config.pUserData = nullptr;
  g_device_config.periodSizeInFrames = 256; // Smaller buffer for low latency

  if (ma_device_init(NULL, &g_device_config, &g_device) != MA_SUCCESS) {
    Log::error("Failed to open loopback audio device.");
    // Fallback to microphone if loopback fails
    Log::warn("Falling back to microphone.");
    g_device_config = ma_device_config_init(ma_device_type_capture);
    g_device_config.capture.format = ma_format_f32;
    g_device_config.capture.channels = CHANNEL_COUNT;
    g_device_config.sampleRate = SAMPLE_RATE;
    g_device_config.dataCallback = data_callback;
    if (ma_device_init(NULL, &g_device_config, &g_device) != MA_SUCCESS) {
      Log::error("Failed to open microphone.");
      kiss_fftr_free(g_fft_cfg);
      return false;
    }
  }

  if (ma_device_start(&g_device) != MA_SUCCESS) {
    Log::error("Failed to start audio device.");
    ma_device_uninit(&g_device);
    kiss_fftr_free(g_fft_cfg);
    return false;
  }

  // --- Create OpenGL Texture for FFT data ---
  glGenTextures(1, &g_fft_texture);
  glBindTexture(GL_TEXTURE_1D, g_fft_texture);
  // Use GL_R32F for single-channel float data
  glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, FREQUENCY_BINS, 0, GL_RED, GL_FLOAT,
               nullptr);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_1D, 0);

  Log::info("AudioEngine initialized successfully.");
  return true;
}

void AudioEngine::shutdown() {
  ma_device_uninit(&g_device);
  kiss_fftr_free(g_fft_cfg);
  if (g_fft_texture != 0) {
    glDeleteTextures(1, &g_fft_texture);
  }
  Log::info("AudioEngine shut down.");
}

bool AudioEngine::get_frequency_data(std::vector<float> &out_data) {
  std::lock_guard<std::mutex> lock(g_fft_mutex);
  if (g_new_data_available) {
    out_data = g_fft_magnitudes; // Copy the data

    // Also update the GPU texture
    if (g_fft_texture != 0) {
      glBindTexture(GL_TEXTURE_1D, g_fft_texture);
      glTexSubImage1D(GL_TEXTURE_1D, 0, 0, FREQUENCY_BINS, GL_RED, GL_FLOAT,
                      g_fft_magnitudes.data());
      glBindTexture(GL_TEXTURE_1D, 0);
    }

    g_new_data_available = false; // Mark data as "read"
    return true;
  }
  return false;
}

unsigned int AudioEngine::get_fft_texture() { return g_fft_texture; }
