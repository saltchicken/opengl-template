#pragma once

#include "core/events/Event.h"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

// FORWARD DECLARE EventDispatcher to be used by ScopedSubscription
class EventDispatcher;

// NEW: An RAII wrapper for subscriptions.
class ScopedSubscription {
public:
  // Default constructor for an empty/invalid subscription
  ScopedSubscription() = default;

  // Destructor automatically unsubscribes
  ~ScopedSubscription();

  // Disable copying
  ScopedSubscription(const ScopedSubscription &) = delete;
  ScopedSubscription &operator=(const ScopedSubscription &) = delete;

  // Enable moving
  ScopedSubscription(ScopedSubscription &&other) noexcept;
  ScopedSubscription &operator=(ScopedSubscription &&other) noexcept;

private:
  // The dispatcher is the only class that should be creating these.
  friend class EventDispatcher;
  using SubscriptionHandle = uint64_t;

  ScopedSubscription(SubscriptionHandle handle, std::type_index type_index);

  void reset(); // Helper to release ownership

  SubscriptionHandle m_handle = -1; // -1 represents an invalid handle
  std::type_index m_type_index = typeid(void);
};

class EventDispatcher {
public:
  using SubscriptionHandle = uint64_t;

  EventDispatcher() = delete;

  // CHANGE: subscribe now returns our RAII wrapper
  template <typename T>
  static ScopedSubscription subscribe(std::function<void(T &)> callback) {
    static SubscriptionHandle s_next_subscription_id = 0;
    SubscriptionHandle handle = s_next_subscription_id++;
    std::type_index type_index(typeid(T));

    auto wrapper = [callback](Event &event) {
      callback(static_cast<T &>(event));
    };

    s_subscribers[type_index][handle] = wrapper;
    // Return the RAII object which now owns the subscription
    return ScopedSubscription(handle, type_index);
  }

  // Queues an event for later processing.
  static void queue_event(std::unique_ptr<Event> event);
  // Dispatches all queued events to subscribers.
  static void dispatch_events();

private:
  friend class ScopedSubscription; // Allow access to unsubscribe

  // CHANGE: unsubscribe is now private and non-templated
  static void unsubscribe(SubscriptionHandle handle,
                          std::type_index type_index);

  static std::unordered_map<
      std::type_index,
      std::map<SubscriptionHandle, std::function<void(Event &)>>>
      s_subscribers;

  static std::vector<std::unique_ptr<Event>> s_event_queue;
};
