#include "game/shop/shop.h"
#include "engine/events/event_system.h"
#include "game/level/timer.h"
#include "game/shop/skill.h"
#include "game/ui/main_hud.h"
#include <iostream>
#include <memory>
#include <raylib.h>

Shop::Shop(Rml::ElementDocument *doc, Timer *timer) : IEntity("Shop") {
  this->document = doc;
  this->timer = timer;
  this->allSkills = GetAllSkills();

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

Shop::~Shop() {}

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

          Skill *skill = this->allSkills[this->currentOffers[i]].get();
          if (this->currentMoney >= skill->cost) {
            // Deduct money
            PurchaseEvent purchaseEvt;
            purchaseEvt.cost = skill->cost;
            EventSystem::GetInstance()->Dispatch(EventType::ON_PURCHASE,
                                                 &purchaseEvt);

            // Dispatch augment info (for future use)
            SkillPurchasedEvent skillEvt;
            skillEvt.skill = skill->Clone();

            EventSystem::GetInstance()->Dispatch(EventType::ON_SKILL_PURCHASED,
                                                 &skillEvt);

            std::cout << "Purchased skill: " << skill->name << std::endl;
            this->Close();
            return;
          }
        }
      }
    }
  }

  // Check buy time button
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    Rml::Element *buyTimeBtn = this->document->GetElementById("buy-time-btn");
    if (buyTimeBtn) {
      Rml::Vector2f absPos =
          buyTimeBtn->GetAbsoluteOffset(Rml::BoxArea::Border);
      Rml::Vector2f size = buyTimeBtn->GetBox().GetSize(Rml::BoxArea::Border);
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
          std::cout << "Rerolled skills" << std::endl;
          this->RollSkills();
          this->UpdateDOM();
        }
      }
    }

    // Check skip button
    Rml::Element *skipBtn = this->document->GetElementById("skip-btn");
    if (skipBtn) {
      Rml::Vector2f absPos = skipBtn->GetAbsoluteOffset(Rml::BoxArea::Border);
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
    Rml::Element *moneyEl = this->document->GetElementById("shop-money-value");
    if (moneyEl) {
      moneyEl->SetInnerRML(std::to_string(this->currentMoney));
    }
  }
}

void Shop::Open() {
  if (this->isOpen)
    return;

  this->isOpen = true;
  this->RollSkills();
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

void Shop::RollSkills() {
  this->currentOffers.clear();
  if (this->allSkills.empty())
    return;

  std::vector<std::string> keys;
  for (const auto &[key, _] : this->allSkills) {
    keys.push_back(key);
  }

  for (int i = 0; i < 3 && !keys.empty(); i++) {
    int idx = GetRandomValue(0, (int)keys.size() - 1);
    this->currentOffers.push_back(keys[idx]);
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
      const std::string skillName = this->currentOffers[i];
      Skill *skill = this->allSkills[skillName].get();

      // Update card with augment data
      card->SetInnerRML("<p class=\"card-name\">" + skill->name +
                        "</p><p class=\"card-desc\">" + skill->description +
                        "</p><div class=\"card-cost\"><p "
                        "class=\"card-cost-value\">" +
                        std::to_string(skill->cost) + "</p></div>");
    }
  }

  // Update money display
  Rml::Element *moneyEl = this->document->GetElementById("shop-money-value");
  if (moneyEl) {
    moneyEl->SetInnerRML(std::to_string(this->currentMoney));
  }

  // Update buy time cost
  Rml::Element *buyTimeBtn = this->document->GetElementById("buy-time-btn");
  if (buyTimeBtn) {
    buyTimeBtn->SetInnerRML("<p>Buy Time (+10s)</p><p class=\"btn-cost\">" +
                            std::to_string(this->buyTimeCost) + "</p>");
  }
}
