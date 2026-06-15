#include "engine/ecs/rigidbody_2d.h"
#include "engine/ecs/collider.h"
#include <imgui.h>
#include <raylib.h>

RigidBody2DComponent::RigidBody2DComponent(IEntity *entity)
    : IComponent(entity, "RigidBody2D") {
  this->gravity = 20.0f;
  this->gravityScale = 4.0f;
  this->velocity = {0.0f, 0.0f};
  this->isGrounded = false;
  this->isGroundedReleaseTimer = 0.0f;
  this->hasJumped = false;
}

void RigidBody2DComponent::ApplyGravity() {
  float dt = GetFrameTime();
  if (dt > 0.05f)
    dt = 0.05f;

  this->velocity.y += this->gravity * this->gravityScale * 10.0f * dt;
}

void RigidBody2DComponent::Update() {
  if (!this->IsEnabled())
    return;

  this->ApplyGravity();

  ColliderComponent *collider = this->entity->GetComponent<ColliderComponent>();
  Vector2 *pos = this->entity->GetPosition();

  float dt = GetFrameTime();
  if (dt > 0.05f)
    dt = 0.05f;

  if (collider == nullptr || !collider->IsEnabled()) {
    // If entity has no collider, just move and return
    pos->x += velocity.x * dt;
    pos->y += velocity.y * dt;
    return;
  }

  // --- RESOLUÇÃO DO EIXO X ---
  if (this->velocity.x != 0.0f) {
    float deltaX = this->velocity.x * dt;

    Rectangle nextPosition = collider->GetWorldBounds();
    nextPosition.x += deltaX;
    
    // Inset vertically to prevent colliding with floors/ceilings
    nextPosition.y += 0.05f;
    nextPosition.height -= 0.10f;

    CollisionResult collision = collider->EvaluateCollisions(&nextPosition);

    if (collision.hasCollided && collider->IsSolid()) {
      if (deltaX > 0.0f) {
        pos->x += deltaX - collision.overlap.width;
      } else {
        pos->x += deltaX + collision.overlap.width;
      }
      this->velocity.x = 0.0f;
    } else {
      pos->x += deltaX;
    }
  }

  // --- RESOLUÇÃO DO EIXO Y ---
  float deltaY = this->velocity.y * dt;
  if (deltaY != 0.0f) {
    Rectangle nextPosition = collider->GetWorldBounds();
    nextPosition.y += deltaY;
    
    // Inset horizontally to prevent colliding with walls
    nextPosition.x += 0.05f;
    nextPosition.width -= 0.10f;

    CollisionResult collision = collider->EvaluateCollisions(&nextPosition);

    if (collision.hasCollided && collider->IsSolid()) {
      if (deltaY > 0.0f) {
        pos->y += deltaY - collision.overlap.height;
        this->isGrounded = true;
      } else {
        pos->y += deltaY + collision.overlap.height;
      }
      this->velocity.y = 0.0f;
    } else {
      pos->y += deltaY;
      this->isGrounded = false;
    }
  }
}

void RigidBody2DComponent::Debug() {
  IComponent::Debug();
  ImGui::DragFloat2("Velocity##RigidBody", &this->velocity.x);
  ImGui::DragFloat("Gravity##RigidBody", &this->gravity, 0.1f);
  ImGui::DragFloat("Gravity Scale##RigidBody", &this->gravityScale, 0.1f);
  ImGui::Text("Grounded: %s", isGrounded ? "Yes" : "No");
}