#pragma once

#include "engine/ecs/entity.h"
#include <RmlUi/Core.h>
#include <game/ecs/skills/skill.h>
#include <unordered_map>
#include <vector>

class Timer;

struct SkillPurchasedEvent {
  std::unique_ptr<Skill> skill;
};

struct PurchaseEvent {
  int cost;
};

class Shop : public IEntity {
private:
  Rml::ElementDocument *document = nullptr;
  Timer *timer = nullptr;

  std::unordered_map<std::string, std::unique_ptr<Skill>> allSkills;
  std::unordered_map<std::string, int> purchasedLevels;
  std::vector<std::string> currentOffers; // 3 skills shown this round

  bool isOpen = false;
  int buyTimeCost = 30;
  int buyRerollCost = 20;

  void Open();
  void Close();
  void RollSkills();
  void UpdateDOM();

  void OnWinTrigger(void *data);
  void OnMoneyChanged(void *data);

  int currentMoney = 0;

public:
  Shop(Rml::ElementDocument *doc, Timer *timer);
  ~Shop();

  void Update() override;
  void Draw() override;

  bool IsOpen() const { return isOpen; }
};
