#include "engine/events/event_system.h"

EventSystem *EventSystem::instance = nullptr;

void EventSystem::AddListener(EventType type, ISubscriber subscriber) {
  if (this->listeners.find(type) == this->listeners.end()) {
    this->listeners[type] = std::vector<ISubscriber>();
  }
  this->listeners[type].push_back(subscriber);
}

void EventSystem::RemoveListener(EventType type, ISubscriber subscriber) {
  if (this->listeners.find(type) == this->listeners.end()) {
    return;
  }
}

void EventSystem::Dispatch(EventType type, void *data) {
  if (this->listeners.find(type) == this->listeners.end())
    return;
  for (ISubscriber subscriber : this->listeners[type]) {
    subscriber(data);
  }
}
