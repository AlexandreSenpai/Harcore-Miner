#pragma once

#include "engine/ecs/entity.h"
#include "game/shop/augment.h"
#include <RmlUi/Core.h>
#include <vector>

class Timer;

struct AugmentPurchasedEvent {
  Augment augment;
};

struct PurchaseEvent {
  int cost;
};

class Shop : public IEntity {
private:
  Rml::ElementDocument *document = nullptr;
  Timer *timer = nullptr;

  std::vector<Augment> allAugments;
  std::vector<Augment> currentOffers; // 3 augments shown this round

  bool isOpen = false;
  int buyTimeCost = 30;
  int buyRerollCost = 20;

  void Open();
  void Close();
  void RollAugments();
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
