#include "game/ecs/skills/damage_skill.h"
#include "engine/ecs/attack.h"
#include "game/ecs/player.h"
#include "game/ecs/skills/skill.h"
#include <iostream>

DamageSkill::DamageSkill(std::string name, std::string description,
                         float damageMultiplier, int cost, int maxLevel)
    : Skill(name, description, cost, maxLevel),
      damage_multiplier(damageMultiplier) {}

DamageSkill::~DamageSkill() {}

bool DamageSkill::LevelUp() {
  if (Skill::LevelUp()) {
    this->Reset();

    this->damage_multiplier += 0.30f;

    return true;
  }
  return false;
}

void DamageSkill::Reset() {
  if (!this->applied)
    return;

  if (this->player == nullptr) {
    std::cout << "Player reference not set for skill " << this->name
              << std::endl;
    return;
  }

  AttackComponent *attackComponent =
      this->player->GetComponent<AttackComponent>();
  if (attackComponent == nullptr) {
    std::cout << "AttackComponent not found for skill " << this->name
              << std::endl;
    return;
  }

  float currentDamage = attackComponent->GetDamage();
  float newDamage = currentDamage / this->damage_multiplier;

  attackComponent->SetDamage(newDamage);
  this->applied = false;

  std::cout << "Damage skill reset: " << newDamage << std::endl;
}

std::unique_ptr<Skill> DamageSkill::Clone() const {
  return std::make_unique<DamageSkill>(*this);
}

void DamageSkill::Update() {
  if (!this->is_active || this->applied)
    return;

  if (this->player == nullptr) {
    std::cout << "Player reference not set for skill " << this->name
              << std::endl;
    return;
  }

  AttackComponent *attackComponent =
      this->player->GetComponent<AttackComponent>();
  if (attackComponent == nullptr) {
    std::cout << "AttackComponent not found for skill " << this->name
              << std::endl;
    return;
  }

  float currentDamage = attackComponent->GetDamage();
  float newDamage = currentDamage * this->damage_multiplier;

  attackComponent->SetDamage(newDamage);
  this->applied = true;
  std::cout << "Damage skill applied: " << newDamage << std::endl;
}

void DamageSkill::Draw() {}
