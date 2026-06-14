#pragma once
#include "engine/ecs/component.h"
#include "engine/events/event_system.h"
#include <raylib.h>
#include <string>
#include <vector>

class IComponent;

class IEntity {
protected:
  Vector2 position = {0.0f, 0.0f};
  Vector2 transform = {0, 0};
  std::string name;
  std::string id;

  std::vector<IComponent *> components;

public:
  IEntity(const char *name);
  virtual ~IEntity();

  virtual void Update() = 0;
  virtual void Draw() = 0;
  virtual void Debug();

  std::string GetId();
  std::string GetName();
  void SetName(std::string name);

  Vector2 *GetTransform();
  virtual void SetTransform(Vector2 newTransform);

  Vector2 *GetPosition();
  virtual void SetPosition(Vector2 newPosition);

  void AddComponent(IComponent *component);

  template <typename TargetType> TargetType *GetComponent() {
    static_assert(std::is_base_of<IComponent, TargetType>::value,
                  "TargetType must be a derived class of IComponent!");

    for (IComponent *component : this->components) {
      if (component->GetId() == TargetType::ID) {
        return static_cast<TargetType *>(component);
      }
    }
    return nullptr;
  }
};