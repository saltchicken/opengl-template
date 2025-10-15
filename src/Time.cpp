#include "Time.h"
#include <GLFW/glfw3.h> // For glfwGetTime()
#include <chrono>
#include <iostream>
#include <thread>

Time::Time(float targetFPS)
    : m_target_fps(targetFPS), m_target_frame_time(1.0 / targetFPS) {
  // Initialize time points
  m_last_frame_time = glfwGetTime();
  m_last_fps_time = m_last_frame_time;
}

void Time::begin_frame() {
  m_frame_start_time = glfwGetTime();
  m_delta_time = m_frame_start_time - m_last_frame_time;
  m_last_frame_time = m_frame_start_time;

  update_fps();
}

void Time::end_frame() {
  // The target time for when the current frame *should* end.
  const double target_frame_end_time = m_frame_start_time + m_target_frame_time;

  // A threshold for when to switch from sleeping to busy-waiting.
  // 2ms is a common value. Sleeping for less than this can be inaccurate.
  const double busy_wait_threshold = 0.002; // 2 milliseconds

  // Calculate how much time we have left in the current frame.
  double time_to_wait = target_frame_end_time - glfwGetTime();

  // If we have more time left than our threshold, we can afford to sleep.
  if (time_to_wait > busy_wait_threshold) {
    // We sleep for a duration that is slightly less than what we need,
    // to account for sleep inaccuracies and leave the rest for the busy-wait.
    auto sleep_duration =
        std::chrono::duration<double>(time_to_wait - busy_wait_threshold);
    std::this_thread::sleep_for(sleep_duration);
  }

  // After the coarse sleep (or if we never slept at all),
  // we enter a tight loop to wait for the exact moment our frame should end.
  // This provides high-precision timing without burning the CPU for the entire
  // wait period.
  while (glfwGetTime() < target_frame_end_time) {
    // Busy-wait (spin) until the target time is reached.
  }

  // NOTE: This can be removed if perfect accuracy detection is not needed
  const double tolerance = 0.0001; // 0.1ms
  if (glfwGetTime() > target_frame_end_time + tolerance) {
    m_missed_frames_count++;
  }
  // End accuracy detection
}

void Time::update_fps() {
  m_frame_count++;
  // If one second has passed since the last FPS update
  if (m_frame_start_time - m_last_fps_time >= 1.0) {
    std::cout << "FPS: " << m_frame_count << std::endl;

    // NOTE: This can be removed if perfect accuracy detection is not needed
    if (m_missed_frames_count > 0) {
      // Send to std::cerr as it's a warning/error condition.
      std::cerr << "WARN: Failed to meet target FPS. Missed "
                << m_missed_frames_count << " of " << m_frame_count
                << " frames in the last second." << std::endl;
    }
    // End accuracy detection

    // Reset for the next second
    m_frame_count = 0;
    m_missed_frames_count = 0; // NOTE: This can be removed if perfect accuracy
                               // detection is not needed
    m_last_fps_time = m_frame_start_time;
  }
}
