#include "game/ui/main_hud.h"

MainHud::MainHud(Rml::ElementDocument *doc) : IEntity("MainHud") {
  this->document = doc;

  EventSystem::GetInstance()->AddListener(
      EventType::ON_MONEY_CHANGED,
      [this](void *data) { this->OnMoneyChanged(data); });
  EventSystem::GetInstance()->AddListener(
      EventType::ON_STAGE_CHANGED,
      [this](void *data) { this->OnStageChanged(data); });
}

MainHud::~MainHud() { IEntity::~IEntity(); }

void MainHud::Update() { IEntity::Update(); }

void MainHud::Draw() { IEntity::Draw(); }

void MainHud::OnMoneyChanged(void *data) {
  MoneyChangedEvent *event = (MoneyChangedEvent *)data;

  if (this->document == nullptr)
    return;

  Rml::Element *el = this->document->GetElementById("money-counter-value");
  if (el) {
    el->SetInnerRML("<p>" + std::to_string(event->money) + "</p>");
  }
}

void MainHud::OnStageChanged(void *data) {
  StageChangedEvent *event = (StageChangedEvent *)data;

  if (this->document == nullptr)
    return;

  Rml::Element *el = this->document->GetElementById("stage-number");
  if (el) {
    el->SetInnerRML("<p>" + std::to_string(event->stageLevel) + "</p>");
  }
}
