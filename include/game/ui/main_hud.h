#pragma once

#include "engine/ecs/entity.h"
#include <RmlUi/Core.h>

struct MoneyChangedEvent {
  int money;
};

struct StageChangedEvent {
  int stageLevel;
};

class MainHud : public IEntity {
private:
  Rml::ElementDocument *document = nullptr;

  void OnMoneyChanged(void *data);
  void OnStageChanged(void *data);

public:
  MainHud(Rml::ElementDocument *doc);
  ~MainHud();

  void Update() override;
  void Draw() override;
};
