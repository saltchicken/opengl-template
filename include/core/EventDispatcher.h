#pragma once

#include "core/Event.h"
#include <functional>
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

  // Publishes an event to all subscribed listeners.
  static void publish(Event &event) {
    // Get the type index for the event's actual runtime type.
    std::type_index type_index(typeid(event));

    // If there are any subscribers for this event type...
    if (s_subscribers.count(type_index)) {
      // ...call each of them.
      for (auto &callback : s_subscribers.at(type_index)) {
        callback(event);
        // Optional: If an event is "handled", we can stop propagating it.
        if (event.handled) {
          break;
        }
      }
    }
  }

private:
  // A map from an event's type_index to a vector of listener functions.
  // The functions take the base Event& for generic storage.
  static std::unordered_map<std::type_index,
                            std::vector<std::function<void(Event &)>>>
      s_subscribers;
};
