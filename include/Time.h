#pragma once

class Time {
public:
  // Constructor takes the target FPS for the limiter.
  Time(float target_fps);

  // Call at the beginning of the frame.
  // Calculates delta time and updates the FPS counter.
  void begin_frame();

  // Call at the end of the frame.
  // Sleeps if necessary to cap the frame rate.
  void end_frame();

  // Returns the time in seconds it took to complete the last frame.
  float get_delta_time() const { return m_delta_time; }

private:
  void update_fps();

  const float m_target_fps;
  const double m_target_frame_time;

  double m_frame_start_time = 0.0;
  double m_last_frame_time = 0.0;
  double m_delta_time = 0.0;

  // For FPS calculation
  double m_last_fps_time = 0.0;
  int m_frame_count = 0;

  int m_missed_frames_count = 0; // NOTE: Potentially to be removed if perfect
                                 // accuracy detection is not needed
};
