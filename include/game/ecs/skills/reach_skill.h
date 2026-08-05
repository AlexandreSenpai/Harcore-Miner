#pragma once

#include "game/ecs/skills/skill.h"

class ReachSkill : public Skill {
public:
  ReachSkill(std::string name, std::string description, float reachMultiplier,
             int cost, int maxLevel);
  ~ReachSkill() override;

  void Update() override;
  void Draw() override;
  bool LevelUp() override;
  void Reset() override;
  std::unique_ptr<Skill> Clone() const override;

private:
  float reach_multiplier;
};
