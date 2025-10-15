#pragma once

class Time {
public:
    // Constructor takes the target FPS for the limiter.
    Time(float targetFPS);

    // Call at the beginning of the frame.
    // Calculates delta time and updates the FPS counter.
    void beginFrame();

    // Call at the end of the frame.
    // Sleeps if necessary to cap the frame rate.
    void endFrame();

    // Returns the time in seconds it took to complete the last frame.
    float getDeltaTime() const { return m_deltaTime; }

private:
    void updateFPS();

    const float m_targetFPS;
    const double m_targetFrameTime;

    double m_frameStartTime = 0.0;
    double m_lastFrameTime = 0.0;
    double m_deltaTime = 0.0;

    // For FPS calculation
    double m_lastFPSTime = 0.0;
    int m_frameCount = 0;
};
