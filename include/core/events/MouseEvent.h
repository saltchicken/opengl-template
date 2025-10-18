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

// TODO: Add MouseButtonPressedEvent, MouseButtonReleasedEvent, etc.
