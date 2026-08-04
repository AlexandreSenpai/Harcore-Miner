#include "game/ecs/skills/speed_up.h"
#include "engine/ecs/move_2d.h"
#include "game/ecs/player.h"
#include "game/ecs/skills/skill.h"
#include <iostream>

SpeedUpSkill::SpeedUpSkill(std::string name, std::string description,
                           float speedMultiplier, int cost, int maxLevel)
    : Skill(name, description, cost, maxLevel),
      speed_multiplier(speedMultiplier) {}

SpeedUpSkill::~SpeedUpSkill() {}

bool SpeedUpSkill::LevelUp() {
  if (Skill::LevelUp()) {
    return true;
  }
  return false;
}

std::unique_ptr<Skill> SpeedUpSkill::Clone() const {
  return std::make_unique<SpeedUpSkill>(*this);
}

void SpeedUpSkill::Update() {
  if (!this->is_active || this->applied)
    return;

  if (this->player == nullptr) {
    std::cout << "Player reference not set for skill " << this->name
              << std::endl;
    return;
  }

  Move2DComponent *moveComponent =
      this->player->GetComponent<Move2DComponent>();
  if (moveComponent == nullptr) {
    std::cout << "Move2DComponent not found for skill " << this->name
              << std::endl;
    return;
  }
  float currentSpeed = moveComponent->GetMoveSpeed();
  float newSpeed = currentSpeed * this->speed_multiplier;

  moveComponent->SetMoveSpeed(newSpeed);
  this->applied = true;
  std::cout << "Speed Up skill applied: " << newSpeed << std::endl;
}

void SpeedUpSkill::Draw() {}