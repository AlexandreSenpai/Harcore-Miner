#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/entity.h"
#include <raylib.h>

class AttackComponent : public IComponent {
private:
  float damage;

public:
  static const int ID = ComponentType::ATTACK;

  AttackComponent(IEntity *entity, float damage);
  ~AttackComponent() override = default;

  void Update() override;
  void Draw() override {}
  void Debug() override;

  int GetId() const override { return ID; }

  void SetDamage(float damage) { this->damage = damage; }
  float GetDamage() const { return damage; }
};
