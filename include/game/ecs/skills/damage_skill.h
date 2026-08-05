#pragma once

#include "game/ecs/skills/skill.h"

class DamageSkill : public Skill {
public:
  DamageSkill(std::string name, std::string description, float damageMultiplier,
              int cost, int maxLevel);
  ~DamageSkill() override;

  void Update() override;
  void Draw() override;
  bool LevelUp() override;
  void Reset() override;
  std::unique_ptr<Skill> Clone() const override;

private:
  float damage_multiplier;
};
