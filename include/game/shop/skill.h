#pragma once

#include <game/ecs/skills/damage_skill.h>
#include <game/ecs/skills/reach_skill.h>
#include <game/ecs/skills/skill.h>
#include <game/ecs/skills/speed_up.h>
#include <memory>
#include <string>
#include <unordered_map>

// Returns the full pool of available augments.
// Add new augments here to make them available in the shop.
inline std::unordered_map<std::string, std::unique_ptr<Skill>> GetAllSkills() {
  std::unordered_map<std::string, std::unique_ptr<Skill>> skills;
  skills.emplace("speed_up",
                 std::make_unique<SpeedUpSkill>(
                     "Speed Up", "Increases movement speed", 1.2f, 50, 5));
  skills.emplace("pickaxe_power",
                 std::make_unique<DamageSkill>(
                     "Pickaxe Power", "Increases pickaxe damage per hit", 1.3f, 60, 5));
  skills.emplace("pickaxe_reach",
                 std::make_unique<ReachSkill>(
                     "Pickaxe Reach", "Expands mining interaction circle radius", 1.2f, 45, 5));
  return skills;
}
