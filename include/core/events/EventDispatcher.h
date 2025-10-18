#pragma once

#include "core/events/Event.h"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventDispatcher {
public:
  using SubscriptionHandle = uint64_t;

  EventDispatcher() = delete;

  // Subscribes a listener function to a specific event type T.
  template <typename T>
  static SubscriptionHandle subscribe(std::function<void(T &)> callback) {
    static SubscriptionHandle s_next_subscription_id = 0;

    SubscriptionHandle handle = s_next_subscription_id++;
    // Get the type index for event type T
    std::type_index type_index(typeid(T));

    // The callback is for a specific event T, but we store it as a
    // function that takes the base Event&. We wrap it in a lambda
    // that performs the safe downcast.
    auto wrapper = [callback](Event &event) {
      callback(static_cast<T &>(event));
    };

    // Add the wrapped callback to the list for this event type.
    s_subscribers[type_index][handle] = wrapper;
    return handle;
  }

  template <typename T> static void unsubscribe(SubscriptionHandle handle) {
    std::type_index type_index(typeid(T));
    if (s_subscribers.count(type_index)) {
      s_subscribers.at(type_index).erase(handle);
    }
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
  static std::unordered_map<
      std::type_index,
      std::map<SubscriptionHandle, std::function<void(Event &)>>>
      s_subscribers;

  // A queue of events to be dispatched.
  static std::vector<std::unique_ptr<Event>> s_event_queue;
};
