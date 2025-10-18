#include "core/events/EventDispatcher.h"

// Instantiate the static map
std::unordered_map<std::type_index, std::vector<std::function<void(Event &)>>>
    EventDispatcher::s_subscribers;
