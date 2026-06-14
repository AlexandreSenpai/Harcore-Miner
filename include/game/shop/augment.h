#pragma once

#include <string>
#include <vector>

struct Augment {
  std::string id;
  std::string name;
  std::string description;
  int cost;
};

// Returns the full pool of available augments.
// Add new augments here to make them available in the shop.
inline std::vector<Augment> GetAllAugments() {
  return {
      {"dmg_up", "Damage Up", "+20% mining damage", 50},
      {"spd_up", "Speed Up", "+15% mining speed", 40},
      {"range_up", "Range Up", "+1 block attack range", 60},
      {"double_rwd", "Double Rewards", "2x block rewards for next stage", 80},
      {"armor", "Tough Skin", "-25% damage taken", 70},
      {"magnet", "Ore Magnet", "Auto-collect nearby drops", 55},
  };
}
