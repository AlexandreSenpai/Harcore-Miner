#pragma once
#include "engine/ecs/entity.h"

class Player : public IEntity {
public:
  Player();
  ~Player();

  void Update() override;
  void Draw() override;
};