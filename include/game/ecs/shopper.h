#pragma once
#include "engine/ecs/entity.h"

class Shopper : public IEntity {
public:
  Shopper();
  ~Shopper();

  void Update() override;
  void Draw() override;
};