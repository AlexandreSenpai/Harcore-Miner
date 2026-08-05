#include "game/ecs/shopper.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/texture.h"

Shopper::Shopper() : IEntity("Shopper") {
  this->SetTransform({16, 16});
  this->AddComponent(new ColliderComponent(this, 11, 13, 1.6f, 3.0f));

  TextureComponent *texture = new TextureComponent(this);
  texture->AddAnimation(
      "idle", {
                  .texturePath = "src/game/assets/shopper/shopper.png",
                  .frameRow = 0,
                  .totalFrames = 6,
                  .frameWidth = 16,
                  .frameHeight = 16,
                  .framesPerSecond = 2,
              });

  texture->SetCurrentAnimation("idle");
  texture->PlayAnimation(true);

  this->AddComponent(texture);
};

Shopper::~Shopper() {};

void Shopper::Update() { IEntity::Update(); };

void Shopper::Draw() { IEntity::Draw(); };