#pragma once

#include "core/events/Event.h"
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventDispatcher {
public:
  EventDispatcher() = delete;

  // Subscribes a listener function to a specific event type T.
  template <typename T>
  static void subscribe(std::function<void(T &)> callback) {
    // Get the type index for event type T
    std::type_index type_index(typeid(T));

    // The callback is for a specific event T, but we store it as a
    // function that takes the base Event&. We wrap it in a lambda
    // that performs the safe downcast.
    auto wrapper = [callback](Event &event) {
      callback(static_cast<T &>(event));
    };

    // Add the wrapped callback to the list for this event type.
    s_subscribers[type_index].push_back(wrapper);
  }

  // Queues an event for later processing.
  static void queue_event(std::unique_ptr<Event> event) {
    s_event_queue.push_back(std::move(event));
  }

  // Dispatches all queued events to subscribers.
  static void dispatch_events();

private:
  // A map from an event's type_index to a vector of listener functions.
  // The functions take the base Event& for generic storage.
  static std::unordered_map<std::type_index,
                            std::vector<std::function<void(Event &)>>>
      s_subscribers;

  // A queue of events to be dispatched.
  static std::vector<std::unique_ptr<Event>> s_event_queue;
};
