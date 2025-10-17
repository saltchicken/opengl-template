#include "core/Time.h"
#include <GLFW/glfw3.h> // For glfwGetTime()
#include <chrono>
#include <iomanip> // For std::fixed and std::setprecision
#include <iostream>
#include <thread>

float Time::s_target_fps = 60.0f;
double Time::s_target_frame_time = 1.0 / 60.0;
double Time::s_frame_start_time = 0.0;
double Time::s_last_frame_time = 0.0;
double Time::s_delta_time = 0.0;
double Time::s_last_fps_time = 0.0;
int Time::s_frame_count = 0;
int Time::s_missed_frames_count = 0;

void Time::init(float target_fps) {
  s_target_fps = target_fps;
  s_target_frame_time = 1.0 / target_fps;
  // Initialize time points
  s_last_frame_time = glfwGetTime();
  s_last_fps_time = s_last_frame_time;
}

void Time::begin_frame() {
  s_frame_start_time = glfwGetTime();
  s_delta_time = s_frame_start_time - s_last_frame_time;
  s_last_frame_time = s_frame_start_time;

  update_fps();
}

void Time::end_frame() {
  // NOTE: This can be removed if perfect accuracy detection is not needed
  const double tolerance = 0.0001; // 0.1ms

  // Check if the frame's work took too long, including the tolerance.
  if (glfwGetTime() > s_frame_start_time + s_target_frame_time + tolerance) {
    s_missed_frames_count++;
  }
  // End accuracy detection

  // The target time for when the current frame *should* end.
  const double target_frame_end_time = s_frame_start_time + s_target_frame_time;

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

    // NOTE: This can be removed if perfect accuracy detection is not needed
    double time_before_sleep = glfwGetTime();
    // End accuracy detection

    std::this_thread::sleep_for(sleep_duration);

    // NOTE: This can be removed if perfect accuracy detection is not needed
    double time_after_sleep = glfwGetTime();
    double actual_sleep_duration = time_after_sleep - time_before_sleep;
    double over_sleep_amount = actual_sleep_duration - sleep_duration.count();

    if (over_sleep_amount > 0.001) {
      std::cout << std::fixed << std::setprecision(4)
                << "DIAGNOSTIC: sleep_for over-slept by "
                << over_sleep_amount * 1000.0 << std::endl;
    }
    // End accuracy detection
  }

  // After the coarse sleep (or if we never slept at all),
  // we enter a tight loop to wait for the exact moment our frame should end.
  // This provides high-precision timing without burning the CPU for the entire
  // wait period.
  while (glfwGetTime() < target_frame_end_time) {
    // Busy-wait (spin) until the target time is reached.
    // std::cout << "Spinning..." << std::endl;
  }
}

void Time::update_fps() {
  s_frame_count++;
  // If one second has passed since the last FPS update
  if (s_frame_start_time - s_last_fps_time >= 1.0) {
    std::cout << "FPS: " << s_frame_count << std::endl;

    // NOTE: This can be removed if perfect accuracy detection is not needed
    if (s_missed_frames_count > 0) {
      // Send to std::cerr as it's a warning/error condition.
      std::cerr << "WARN: Failed to meet target FPS. Missed "
                << s_missed_frames_count << " of " << s_frame_count
                << " frames in the last second." << std::endl;
    }
    // End accuracy detection

    // Reset for the next second
    s_frame_count = 0;
    s_missed_frames_count = 0; // NOTE: This can be removed if perfect accuracy
                               // detection is not needed
    s_last_fps_time = s_frame_start_time;
  }
}
