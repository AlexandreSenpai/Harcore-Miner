#include "game/ecs/skills/reach_skill.h"
#include "engine/ecs/circle_collider.h"
#include "game/ecs/player.h"
#include "game/ecs/skills/skill.h"
#include <iostream>

ReachSkill::ReachSkill(std::string name, std::string description,
                       float reachMultiplier, int cost, int maxLevel)
    : Skill(name, description, cost, maxLevel),
      reach_multiplier(reachMultiplier) {}

ReachSkill::~ReachSkill() {}

bool ReachSkill::LevelUp() {
  if (Skill::LevelUp()) {
    this->Reset();

    this->reach_multiplier += 0.20f;

    return true;
  }
  return false;
}

void ReachSkill::Reset() {
  if (!this->applied)
    return;

  if (this->player == nullptr) {
    std::cout << "Player reference not set for skill " << this->name
              << std::endl;
    return;
  }

  CircleColliderComponent *colliderComponent =
      this->player->GetComponent<CircleColliderComponent>();
  if (colliderComponent == nullptr) {
    std::cout << "CircleColliderComponent not found for skill " << this->name
              << std::endl;
    return;
  }

  float currentRadius = colliderComponent->GetRadius();
  float newRadius = currentRadius / this->reach_multiplier;

  colliderComponent->SetRadius(newRadius);
  this->applied = false;

  std::cout << "Reach skill reset: " << newRadius << std::endl;
}

std::unique_ptr<Skill> ReachSkill::Clone() const {
  return std::make_unique<ReachSkill>(*this);
}

void ReachSkill::Update() {
  if (!this->is_active || this->applied)
    return;

  if (this->player == nullptr) {
    std::cout << "Player reference not set for skill " << this->name
              << std::endl;
    return;
  }

  CircleColliderComponent *colliderComponent =
      this->player->GetComponent<CircleColliderComponent>();
  if (colliderComponent == nullptr) {
    std::cout << "CircleColliderComponent not found for skill " << this->name
              << std::endl;
    return;
  }

  float currentRadius = colliderComponent->GetRadius();
  float newRadius = currentRadius * this->reach_multiplier;

  colliderComponent->SetRadius(newRadius);
  this->applied = true;
  std::cout << "Reach skill applied: " << newRadius << std::endl;
}

void ReachSkill::Draw() {}
