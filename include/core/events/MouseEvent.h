#pragma once
#include "core/events/Event.h"

class MouseMovedEvent : public Event {
public:
  MouseMovedEvent(float x, float y) : m_mouse_x(x), m_mouse_y(y) {}

  float get_x() const { return m_mouse_x; }
  float get_y() const { return m_mouse_y; }

  EventCategory get_category() const override { return EventCategory::Mouse; }
  EVENT_CLASS_TYPE(MouseMoved)

private:
  float m_mouse_x, m_mouse_y;
};

// Base class for mouse button events.
class MouseButtonEvent : public Event {
public:
  int get_mouse_button() const { return m_button; }

  EventCategory get_category() const override { return EventCategory::Mouse; }

protected:
  MouseButtonEvent(int button) : m_button(button) {}
  int m_button;
};

// Event for when a mouse button is pressed.
class MouseButtonPressedEvent : public MouseButtonEvent {
public:
  MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

  EVENT_CLASS_TYPE(MouseButtonPressed)

  std::string to_string() const override {
    return "MouseButtonPressedEvent: " + std::to_string(m_button);
  }
};

// Event for when a mouse button is released.
class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
  MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

  EVENT_CLASS_TYPE(MouseButtonReleased)

  std::string to_string() const override {
    return "MouseButtonReleasedEvent: " + std::to_string(m_button);
  }
};
