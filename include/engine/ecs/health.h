#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/entity.h"

class HealthComponent : public IComponent {
private:
  float maxHealth;
  float currentHealth;
  bool isDead;

public:
  static const int ID = ComponentType::HEALTH;

  HealthComponent(IEntity *entity, float maxHealth);
  ~HealthComponent() override = default;

  void Update() override {}
  void Draw() override {}
  void Debug() override;

  int GetId() const override { return ID; }

  void TakeDamage(float amount);
  void Heal(float amount);

  float GetMaxHealth() const { return maxHealth; }
  float GetCurrentHealth() const { return currentHealth; }
  float GetLifePercentage() const;
  bool IsDead() const { return isDead; }
};
