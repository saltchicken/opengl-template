#pragma once

class Time {
public:
  static void init(float target_fps);

  // Call at the beginning of the frame.
  // Calculates delta time and updates the FPS counter.
  static void begin_frame();

  // Call at the end of the frame.
  // Sleeps if necessary to cap the frame rate.
  static void end_frame();

  // Returns the time in seconds it took to complete the last frame.
  static float get_delta_time() { return s_delta_time; }

private:
  Time() = default;
  static void update_fps();

  static float s_target_fps;
  static double s_target_frame_time;
  static double s_frame_start_time;
  static double s_last_frame_time;
  static double s_delta_time;

  // For FPS calculation
  static double s_last_fps_time;
  static int s_frame_count;

  static int s_missed_frames_count; // NOTE: Potentially to be removed if
                                    // perfect accuracy detection is not needed
};
