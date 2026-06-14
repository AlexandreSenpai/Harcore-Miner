#pragma once

class IEntity;

class IComponent {
protected:
  IEntity *entity;
  const char *name;
  bool enabled;

public:
  IComponent(IEntity *entity, const char *name) {
    this->entity = entity;
    this->name = name;
    this->enabled = true;
  };
  virtual ~IComponent() = default;

  virtual int GetId() const = 0;

  virtual void Update() = 0;
  virtual void Draw() = 0;
  virtual void Debug() {};

  const char *GetName() { return this->name; };
  bool IsEnabled() { return this->enabled; };
  void SetEnabled(bool enabled) { this->enabled = enabled; };
};