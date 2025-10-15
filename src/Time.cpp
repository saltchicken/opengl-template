#include "Time.h"
#include <GLFW/glfw3.h> // For glfwGetTime()
#include <thread>
#include <chrono>
#include <iostream>

Time::Time(float targetFPS)
    : m_targetFPS(targetFPS), m_targetFrameTime(1.0 / targetFPS) {
    // Initialize time points
    m_lastFrameTime = glfwGetTime();
    m_lastFPSTime = m_lastFrameTime;
}

void Time::beginFrame() {
    m_frameStartTime = glfwGetTime();
    m_deltaTime = m_frameStartTime - m_lastFrameTime;
    m_lastFrameTime = m_frameStartTime;

    updateFPS();
}

void Time::endFrame() {
    // The target time for when the current frame *should* end.
    const double targetFrameEndTime = m_frameStartTime + m_targetFrameTime;

    // A threshold for when to switch from sleeping to busy-waiting.
    // 2ms is a common value. Sleeping for less than this can be inaccurate.
    const double busyWaitThreshold = 0.002; // 2 milliseconds

    // Calculate how much time we have left in the current frame.
    double timeToWait = targetFrameEndTime - glfwGetTime();

    // If we have more time left than our threshold, we can afford to sleep.
    if (timeToWait > busyWaitThreshold) {
        // We sleep for a duration that is slightly less than what we need,
        // to account for sleep inaccuracies and leave the rest for the busy-wait.
        auto sleepDuration = std::chrono::duration<double>(timeToWait - busyWaitThreshold);
        std::this_thread::sleep_for(sleepDuration);
    }

    // After the coarse sleep (or if we never slept at all),
    // we enter a tight loop to wait for the exact moment our frame should end.
    // This provides high-precision timing without burning the CPU for the entire wait period.
    while (glfwGetTime() < targetFrameEndTime) {
        // Busy-wait (spin) until the target time is reached.
    }

    // NOTE: This can be removed if perfect accuracy detection is not needed
    const double tolerance = 0.0001; // 0.1ms
    if (glfwGetTime() > targetFrameEndTime + tolerance) {
        m_missedFramesCount++;
    }
    // End accuracy detection
}

void Time::updateFPS() {
    m_frameCount++;
    // If one second has passed since the last FPS update
    if (m_frameStartTime - m_lastFPSTime >= 1.0) {
        std::cout << "FPS: " << m_frameCount << std::endl;
        
        // NOTE: This can be removed if perfect accuracy detection is not needed
        if (m_missedFramesCount > 0) {
            // Send to std::cerr as it's a warning/error condition.
            std::cerr << "WARN: Failed to meet target FPS. Missed "
                      << m_missedFramesCount << " of " << m_frameCount 
                      << " frames in the last second." << std::endl;
        }
        // End accuracy detection
      
        // Reset for the next second
        m_frameCount = 0;
        m_missedFramesCount = 0; // NOTE: This can be removed if perfect accuracy detection is not needed
        m_lastFPSTime = m_frameStartTime;
    }
}
