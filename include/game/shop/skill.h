#pragma once

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
  return skills;
}
