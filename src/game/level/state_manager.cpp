#include "game/level/state_manager.h"
#include "game/ecs/block.h"
#include "game/shop/shop.h"
#include "game/ui/main_hud.h"
#include <iostream>

StateManager::StateManager() : IEntity("StateManager") {
  EventSystem::GetInstance()->AddListener(
      EventType::ON_BLOCK_DESTROYED,
      [this](void *data) { this->OnReceiveMoney(data); });
  EventSystem::GetInstance()->AddListener(
      EventType::ON_PURCHASE,
      [this](void *data) { this->OnPurchase(data); });
}

StateManager::~StateManager() { IEntity::~IEntity(); };

void StateManager::Update() { IEntity::Update(); }

void StateManager::Draw() { IEntity::Draw(); }

void StateManager::AddMoney(int amount) {
  this->money += amount;

  MoneyChangedEvent event;
  event.money = this->money;
  EventSystem::GetInstance()->Dispatch(EventType::ON_MONEY_CHANGED, &event);
}

void StateManager::RemoveMoney(int amount) {
  this->money -= amount;

  if (this->money < 0) {
    this->money = 0;
  }

  MoneyChangedEvent event;
  event.money = this->money;
  EventSystem::GetInstance()->Dispatch(EventType::ON_MONEY_CHANGED, &event);
}

int StateManager::GetMoney() const { return this->money; }

void StateManager::NextStage() { this->currentStageLevel++; }

int StateManager::GetCurrentStageLevel() const {
  return this->currentStageLevel;
}

void StateManager::OnReceiveMoney(void *data) {
  DestroyedBlockEvent *event = (DestroyedBlockEvent *)data;

  std::cout << "Received " << event->reward << " money!" << std::endl;
  std::cout << "Money: " << this->money << std::endl;

  this->AddMoney(event->reward);
}

void StateManager::OnPurchase(void *data) {
  PurchaseEvent *event = (PurchaseEvent *)data;
  std::cout << "Purchase: -" << event->cost << " money" << std::endl;
  this->RemoveMoney(event->cost);
}