#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

class IEntity;

enum EventType {
  ON_COLLISION_ENTER,
  ON_COLLISION_EXIT,
  ON_TIMER_FINISH,
  ON_WIN_TRIGGER,
  ON_BLOCK_DESTROYED,
  ON_MONEY_CHANGED,
  ON_STAGE_CHANGED,
  ON_SHOP_OPENED,
  ON_SHOP_CLOSED,
  ON_PURCHASE
};

using ISubscriber = std::function<void(void *data)>;

class EventSystem {
private:
  std::unordered_map<EventType, std::vector<ISubscriber>> listeners;

  static EventSystem *instance;

  EventSystem() = default;
  ~EventSystem() = default;

public:
  static EventSystem *GetInstance() {
    if (EventSystem::instance == nullptr) {
      EventSystem::instance = new EventSystem();
    }
    return EventSystem::instance;
  }

  void AddListener(EventType type, ISubscriber subscriber);
  void RemoveListener(EventType type, ISubscriber subscriber);
  void Dispatch(EventType type, void *data);
};