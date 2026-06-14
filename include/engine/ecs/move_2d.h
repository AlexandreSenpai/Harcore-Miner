#pragma once

#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/texture.h"

class Move2DComponent : public IComponent {
private:
  float speed = 1.0f;

public:
  Move2DComponent(IEntity *entity, float speed = 1.0f);
  ~Move2DComponent();

  static const int ID = ComponentType::MOVE_2D;

  void Update() override;
  void Draw() override;
  void Debug() override;

  int GetId() const override { return this->ID; }

  void FlipTexture(TextureComponent *textureComponent, bool flip);
};