#include "game/shop/shop.h"
#include "game/level/timer.h"
#include "game/ui/main_hud.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>

Shop::Shop(Rml::ElementDocument *doc, Timer *timer) : IEntity("Shop") {
  this->document = doc;
  this->timer = timer;
  this->allAugments = GetAllAugments();

  // Start hidden
  if (this->document) {
    this->document->Hide();
  }

  EventSystem::GetInstance()->AddListener(
      EventType::ON_WIN_TRIGGER,
      [this](void *data) { this->OnWinTrigger(data); });
  EventSystem::GetInstance()->AddListener(
      EventType::ON_MONEY_CHANGED,
      [this](void *data) { this->OnMoneyChanged(data); });
}

Shop::~Shop() { IEntity::~IEntity(); }

void Shop::Update() {
  if (!this->isOpen || !this->document)
    return;

  // Poll for card clicks
  for (int i = 0; i < 3; i++) {
    std::string cardId = "augment-" + std::to_string(i);
    Rml::Element *card = this->document->GetElementById(cardId);

    if (card && i < (int)this->currentOffers.size()) {
      // Check if this card was clicked via a pseudo-class or mouse check
      // RmlUi doesn't have a simple "was clicked" API, so we check mouse
      // position + click against element bounds
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Rml::Vector2f absPos = card->GetAbsoluteOffset(Rml::BoxArea::Border);
        Rml::Vector2f size = card->GetBox().GetSize(Rml::BoxArea::Border);
        Vector2 mouse = GetMousePosition();

        if (mouse.x >= absPos.x && mouse.x <= absPos.x + size.x &&
            mouse.y >= absPos.y && mouse.y <= absPos.y + size.y) {

          const Augment &aug = this->currentOffers[i];
          if (this->currentMoney >= aug.cost) {
            // Deduct money
            PurchaseEvent purchaseEvt;
            purchaseEvt.cost = aug.cost;
            EventSystem::GetInstance()->Dispatch(EventType::ON_PURCHASE,
                                                 &purchaseEvt);

            // Dispatch augment info (for future use)
            AugmentPurchasedEvent augEvt;
            augEvt.augment = aug;

            std::cout << "Purchased augment: " << aug.name << std::endl;
            this->Close();
            return;
          }
        }
      }
    }
  }

  // Check buy time button
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    Rml::Element *buyTimeBtn =
        this->document->GetElementById("buy-time-btn");
    if (buyTimeBtn) {
      Rml::Vector2f absPos =
          buyTimeBtn->GetAbsoluteOffset(Rml::BoxArea::Border);
      Rml::Vector2f size =
          buyTimeBtn->GetBox().GetSize(Rml::BoxArea::Border);
      Vector2 mouse = GetMousePosition();

      if (mouse.x >= absPos.x && mouse.x <= absPos.x + size.x &&
          mouse.y >= absPos.y && mouse.y <= absPos.y + size.y) {
        if (this->currentMoney >= this->buyTimeCost) {
          PurchaseEvent purchaseEvt;
          purchaseEvt.cost = this->buyTimeCost;
          EventSystem::GetInstance()->Dispatch(EventType::ON_PURCHASE,
                                               &purchaseEvt);
          this->timer->AddTime(10.0f);
          std::cout << "Bought +10s time" << std::endl;
          // Don't close — let them keep shopping
        }
      }
    }

    // Check reroll button
    Rml::Element *rerollBtn = this->document->GetElementById("reroll-btn");
    if (rerollBtn) {
      Rml::Vector2f absPos = rerollBtn->GetAbsoluteOffset(Rml::BoxArea::Border);
      Rml::Vector2f size = rerollBtn->GetBox().GetSize(Rml::BoxArea::Border);
      Vector2 mouse = GetMousePosition();

      if (mouse.x >= absPos.x && mouse.x <= absPos.x + size.x &&
          mouse.y >= absPos.y && mouse.y <= absPos.y + size.y) {
        if (this->currentMoney >= this->buyRerollCost) {
          PurchaseEvent purchaseEvt;
          purchaseEvt.cost = this->buyRerollCost;
          EventSystem::GetInstance()->Dispatch(EventType::ON_PURCHASE,
                                               &purchaseEvt);
          std::cout << "Rerolled augments" << std::endl;
          this->RollAugments();
          this->UpdateDOM();
        }
      }
    }

    // Check skip button
    Rml::Element *skipBtn = this->document->GetElementById("skip-btn");
    if (skipBtn) {
      Rml::Vector2f absPos =
          skipBtn->GetAbsoluteOffset(Rml::BoxArea::Border);
      Rml::Vector2f size = skipBtn->GetBox().GetSize(Rml::BoxArea::Border);
      Vector2 mouse = GetMousePosition();

      if (mouse.x >= absPos.x && mouse.x <= absPos.x + size.x &&
          mouse.y >= absPos.y && mouse.y <= absPos.y + size.y) {
        std::cout << "Skipped shop" << std::endl;
        this->Close();
        return;
      }
    }
  }
}

void Shop::Draw() {}

void Shop::OnWinTrigger(void *data) { this->Open(); }

void Shop::OnMoneyChanged(void *data) {
  MoneyChangedEvent *event = (MoneyChangedEvent *)data;
  this->currentMoney = event->money;

  // Update money display in shop if open
  if (this->isOpen && this->document) {
    Rml::Element *moneyEl =
        this->document->GetElementById("shop-money-value");
    if (moneyEl) {
      moneyEl->SetInnerRML(std::to_string(this->currentMoney));
    }
  }
}

void Shop::Open() {
  if (this->isOpen)
    return;

  this->isOpen = true;
  this->RollAugments();
  this->UpdateDOM();

  if (this->document) {
    this->document->Show();
  }

  EventSystem::GetInstance()->Dispatch(EventType::ON_SHOP_OPENED, nullptr);
  std::cout << "Shop opened" << std::endl;
}

void Shop::Close() {
  if (!this->isOpen)
    return;

  this->isOpen = false;

  if (this->document) {
    this->document->Hide();
  }

  EventSystem::GetInstance()->Dispatch(EventType::ON_SHOP_CLOSED, nullptr);
  std::cout << "Shop closed" << std::endl;
}

void Shop::RollAugments() {
  this->currentOffers.clear();

  // Shuffle and pick 3
  std::vector<Augment> pool = this->allAugments;
  int count = std::min(3, (int)pool.size());

  for (int i = 0; i < count; i++) {
    int idx = GetRandomValue(i, (int)pool.size() - 1);
    std::swap(pool[i], pool[idx]);
    this->currentOffers.push_back(pool[i]);
  }
}

void Shop::UpdateDOM() {
  if (!this->document)
    return;

  // Update augment cards
  for (int i = 0; i < 3; i++) {
    std::string cardId = "augment-" + std::to_string(i);
    Rml::Element *card = this->document->GetElementById(cardId);
    if (!card)
      continue;

    if (i < (int)this->currentOffers.size()) {
      const Augment &aug = this->currentOffers[i];

      // Update card with augment data
      card->SetInnerRML("<p class=\"card-name\">" + aug.name +
                        "</p><p class=\"card-desc\">" + aug.description +
                        "</p><div class=\"card-cost\"><p "
                        "class=\"card-cost-value\">" +
                        std::to_string(aug.cost) + "</p></div>");
    }
  }

  // Update money display
  Rml::Element *moneyEl =
      this->document->GetElementById("shop-money-value");
  if (moneyEl) {
    moneyEl->SetInnerRML(std::to_string(this->currentMoney));
  }

  // Update buy time cost
  Rml::Element *buyTimeBtn =
      this->document->GetElementById("buy-time-btn");
  if (buyTimeBtn) {
    buyTimeBtn->SetInnerRML("<p>Buy Time (+10s)</p><p class=\"btn-cost\">" +
                            std::to_string(this->buyTimeCost) + "</p>");
  }
}
