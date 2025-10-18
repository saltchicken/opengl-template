#pragma once

#include "core/Event.h"

class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(unsigned int width, unsigned int height)
      : m_width(width), m_height(height) {}

  unsigned int get_width() const { return m_width; }
  unsigned int get_height() const { return m_height; }

  EventCategory get_category() const override {
    return EventCategory::Application;
  }

  EVENT_CLASS_TYPE(WindowResize)

  std::string to_string() const override {
    return "WindowResizeEvent: " + std::to_string(m_width) + ", " +
           std::to_string(m_height);
  }

private:
  unsigned int m_width, m_height;
};
