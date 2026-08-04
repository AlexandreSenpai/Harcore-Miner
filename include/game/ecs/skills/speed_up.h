#pragma once

#include "game/ecs/skills/skill.h"

class SpeedUpSkill : public Skill {
public:
  SpeedUpSkill(std::string name, std::string description, float speedMultiplier,
               int cost, int maxLevel);
  ~SpeedUpSkill();

  void Update() override;
  void Draw() override;
  bool LevelUp() override;
  std::unique_ptr<Skill> Clone() const override;

private:
  float speed_multiplier;
};