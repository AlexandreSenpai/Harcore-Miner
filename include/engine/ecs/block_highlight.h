#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/entity.h"
#include "game/ecs/player.h"

class BlockHighlightComponent : public IComponent {
private:
  bool playerIsNearby = false;
  Player *player = nullptr;

public:
  static const int ID = ComponentType::BLOCK_HIGHLIGHT;

  BlockHighlightComponent(IEntity *entity);
  ~BlockHighlightComponent() override = default;

  void Update() override;
  void Draw() override;
  void Debug() override;

  int GetId() const override { return ID; }

  void SetPlayerIsNearby(bool playerIsNearby) {
    this->playerIsNearby = playerIsNearby;
  }
  bool GetPlayerIsNearby() const { return this->playerIsNearby; }

  void DrawHighlight();
};
