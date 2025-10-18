#pragma once

#include <string>

// A simple enum to classify events into broad categories.
// This is useful for future filtering.
enum class EventCategory { None = 0, Application, Input, Keyboard, Mouse };

// This macro is a handy way to implement the necessary virtual functions
// for identifying an event's type at runtime.
#define EVENT_CLASS_TYPE(type)                                                 \
  static const char *get_static_type() { return #type; }                       \
  virtual const char *get_type_name() const override {                         \
    return get_static_type();                                                  \
  }

class Event {
public:
  virtual ~Event() = default;

  bool handled = false;

  virtual const char *get_type_name() const = 0;
  virtual EventCategory get_category() const = 0;
  virtual std::string to_string() const { return get_type_name(); }
};
