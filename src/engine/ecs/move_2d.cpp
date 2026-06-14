#include "engine/ecs/move_2d.h"
#include "engine/ecs/component.h"
#include "engine/ecs/entity.h"
#include "engine/ecs/rigidbody_2d.h"
#include "engine/ecs/texture.h"
#include <imgui.h>
#include <raylib.h>

Move2DComponent::Move2DComponent(IEntity *entity, float speed)
    : IComponent(entity, "Move2D") {
  this->speed = speed;
}

Move2DComponent::~Move2DComponent() {}

void Move2DComponent::Update() {
  if (!this->IsEnabled())
    return;

  TextureComponent *texture = this->entity->GetComponent<TextureComponent>();
  RigidBody2DComponent *rb = this->entity->GetComponent<RigidBody2DComponent>();

  if (rb == nullptr)
    return; // Se não tem física, não consegue mover

  float targetVelocityX = 0.0f;

  // 1. Apenas lê o Input Horizontal
  if (IsKeyDown(KEY_D))
    targetVelocityX += this->speed;
  if (IsKeyDown(KEY_A))
    targetVelocityX -= this->speed;

  rb->SetVelocityX(targetVelocityX);

  // 2. Cálculo de Pulo (Vertical)
  if (IsKeyPressed(KEY_W) && rb->IsGrounded()) {
    rb->SetVelocityY(-this->speed * 3.0f); // Impulso para cima
  }

  // 3. Atualiza as animações visuais
  if (texture) {
    if (targetVelocityX > 0) {
      this->FlipTexture(texture, false);
      texture->SetCurrentAnimation("running");
      texture->PlayAnimation(true);
    } else if (targetVelocityX < 0) {
      this->FlipTexture(texture, true);
      texture->SetCurrentAnimation("running");
      texture->PlayAnimation(true);
    } else if (!rb->IsGrounded()) {
      texture->SetCurrentAnimation("running");
      texture->PlayAnimation(true);
    } else {
      texture->SetCurrentAnimation("idle");
      texture->PlayAnimation(true);
    }
  }
}

void Move2DComponent::Draw() {}

void Move2DComponent::Debug() {
  IComponent::Debug();
  ImGui::SliderFloat("Speed", &this->speed, 0.0f, 10.0f);
}

void Move2DComponent::FlipTexture(TextureComponent *textureComponent,
                                  bool flip) {
  if (textureComponent == nullptr) {
    return;
  }
  textureComponent->FlipHorizontally(flip);
}
