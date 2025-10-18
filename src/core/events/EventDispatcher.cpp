#include "core/events/EventDispatcher.h"

// Instantiate the static map
std::unordered_map<std::type_index, std::vector<std::function<void(Event &)>>>
    EventDispatcher::s_subscribers;

// Instantiate the static event queue
std::vector<std::unique_ptr<Event>> EventDispatcher::s_event_queue;

// Implementation for dispatch_events
void EventDispatcher::dispatch_events() {
  // Iterate over a copy of the queue's pointers to allow
  // event handlers to queue new events without invalidating iterators.
  auto events_to_dispatch = std::move(s_event_queue);
  s_event_queue.clear(); // Clear the main queue immediately

  for (const auto &event_ptr : events_to_dispatch) {
    Event &event = *event_ptr;
    std::type_index type_index(typeid(event));

    if (s_subscribers.count(type_index)) {
      for (auto &callback : s_subscribers.at(type_index)) {
        callback(event);
        if (event.handled) {
          break;
        }
      }
    }
  }
}
