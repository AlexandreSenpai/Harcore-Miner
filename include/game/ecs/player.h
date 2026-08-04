#pragma once

#include "engine/ecs/entity.h"
#include "game/shop/shop.h"
#include <game/ecs/skills/skill.h>
#include <memory>
#include <unordered_map>

class SkillPurchasedEvent;

class Player : public IEntity {
public:
  Player();
  ~Player();

  void AddSkill(std::unique_ptr<Skill> skill);
  void RemoveSkill(const std::string &name);
  Skill *GetSkill(const std::string &name);

  void Update() override;
  void Draw() override;

  void OnSkillPurchaseEvent(SkillPurchasedEvent *event);

private:
  std::unordered_map<std::string, std::unique_ptr<Skill>> skills;
};