#include "game/ecs/skills/skill.h"
#include <memory>

Skill::Skill(std::string name, std::string description, int cost, int maxLevel)
    : name(name), description(description), cost(cost), max_level(maxLevel),
      level(1), is_active(true), applied(false), player(nullptr) {}

Skill::~Skill() {}

void Skill::Update() {}

void Skill::Draw() {}

void Skill::Reset() {}

bool Skill::LevelUp() {
  if (this->level < this->max_level) {
    this->level++;
    return true;
  }
  return false;
}

std::unique_ptr<Skill> Skill::Clone() const {
  return std::make_unique<Skill>(*this);
}
