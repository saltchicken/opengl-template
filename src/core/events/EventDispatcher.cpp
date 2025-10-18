#include "core/events/EventDispatcher.h"
#include <utility>

// --- ScopedSubscription Implementation ---
ScopedSubscription::ScopedSubscription(SubscriptionHandle handle,
                                       std::type_index type_index)
    : m_handle(handle), m_type_index(type_index) {}

ScopedSubscription::~ScopedSubscription() {
  if (m_handle != static_cast<SubscriptionHandle>(-1)) {
    EventDispatcher::unsubscribe(m_handle, m_type_index);
  }
}

ScopedSubscription::ScopedSubscription(ScopedSubscription &&other) noexcept
    : m_handle(other.m_handle), m_type_index(other.m_type_index) {
  other.reset(); // The moved-from object no longer owns the subscription
}

ScopedSubscription &
ScopedSubscription::operator=(ScopedSubscription &&other) noexcept {
  if (this != &other) {
    // Unsubscribe from the current subscription if it's valid
    if (m_handle != static_cast<SubscriptionHandle>(-1)) {
      EventDispatcher::unsubscribe(m_handle, m_type_index);
    }
    // Move resources from the other object
    m_handle = other.m_handle;
    m_type_index = other.m_type_index;
    // The moved-from object no longer owns the subscription
    other.reset();
  }
  return *this;
}

void ScopedSubscription::reset() {
  m_handle = -1;
  m_type_index = typeid(void);
}

// --- EventDispatcher Implementation ---
std::unordered_map<
    std::type_index,
    std::map<EventDispatcher::SubscriptionHandle, std::function<void(Event &)>>>
    EventDispatcher::s_subscribers;

std::vector<std::unique_ptr<Event>> EventDispatcher::s_event_queue;

// Private unsubscribe implementation
void EventDispatcher::unsubscribe(SubscriptionHandle handle,
                                  std::type_index type_index) {
  if (s_subscribers.count(type_index)) {
    s_subscribers.at(type_index).erase(handle);
  }
}

void EventDispatcher::queue_event(std::unique_ptr<Event> event) {
  s_event_queue.push_back(std::move(event));
}

void EventDispatcher::dispatch_events() {
  auto events_to_dispatch = std::move(s_event_queue);
  s_event_queue.clear();
  for (const auto &event_ptr : events_to_dispatch) {
    Event &event = *event_ptr;
    std::type_index type_index(typeid(event));
    if (s_subscribers.count(type_index)) {
      for (auto const &[handle, callback] : s_subscribers.at(type_index)) {
        callback(event);
        if (event.handled) {
          break;
        }
      }
    }
  }
}
