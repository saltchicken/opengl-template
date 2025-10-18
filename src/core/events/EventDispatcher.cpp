#include "core/events/EventDispatcher.h"

// CHANGE: Instantiate the new static map type
std::unordered_map<
    std::type_index,
    std::map<EventDispatcher::SubscriptionHandle, std::function<void(Event &)>>>
    EventDispatcher::s_subscribers;

std::vector<std::unique_ptr<Event>> EventDispatcher::s_event_queue;

void EventDispatcher::dispatch_events() {
  auto events_to_dispatch = std::move(s_event_queue);
  s_event_queue.clear();

  for (const auto &event_ptr : events_to_dispatch) {
    Event &event = *event_ptr;
    std::type_index type_index(typeid(event));

    if (s_subscribers.count(type_index)) {
      // CHANGE: Iterate over the map of subscribers
      for (auto const &[handle, callback] : s_subscribers.at(type_index)) {
        callback(event);
        if (event.handled) {
          break;
        }
      }
    }
  }
}
