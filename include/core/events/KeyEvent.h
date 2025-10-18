#pragma once

#include "core/events/Event.h"

// Base class for all keyboard events.
class KeyEvent : public Event {
public:
  int get_key_code() const { return m_key_code; }
  EventCategory get_category() const override {
    return EventCategory::Keyboard;
  }

protected:
  KeyEvent(int key_code) : m_key_code(key_code) {}
  int m_key_code;
};

// Event for when a key is pressed down.
class KeyPressedEvent : public KeyEvent {
public:
  KeyPressedEvent(int key_code) : KeyEvent(key_code) {}

  EVENT_CLASS_TYPE(KeyPressed)

  std::string to_string() const override {
    return "KeyPressedEvent: " + std::to_string(m_key_code);
  }
};

// (You can add KeyReleasedEvent here later in the same pattern)
