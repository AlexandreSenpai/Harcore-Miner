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
  if ((this->IsGrounded() && this->velocity.y > 0) || this->hasJumped) {
    this->isGroundedReleaseTimer += dt;
    if (this->isGroundedReleaseTimer > 0.25f) {
      this->isGrounded = false;
      this->isGroundedReleaseTimer = 0.0f;
      this->hasJumped = false;
    }
  }

  if (collider == nullptr || !collider->IsEnabled()) {
    // If entity has no collider, just move and return
    pos->x += velocity.x * dt;
    pos->y += velocity.y * dt;
    return;
  }

  // If entity is trying to move horizontally let's check for collisions
  if (this->velocity.x != 0.0f) {
    float deltaX = this->velocity.x * dt;

    Rectangle nextPosition = collider->GetWorldBounds();
    nextPosition.x += deltaX;

    CollisionResult collision = collider->EvaluateCollisions(&nextPosition);
    pos->x += deltaX;

    if (collision.hasCollided && collider->IsSolid()) {
      if (this->velocity.x > 0)
        pos->x -= collision.overlap.width;
      else
        pos->x += collision.overlap.width;

      this->velocity.x = 0;
    }
  }

  // --- RESOLUÇÃO DO EIXO Y ---
  if (this->velocity.y != 0.0f) {
    float deltaY = this->velocity.y * dt;

    Rectangle nextPosition = collider->GetWorldBounds();
    nextPosition.y += deltaY;

    CollisionResult collision = collider->EvaluateCollisions(&nextPosition);
    pos->y += deltaY;

    if (collision.hasCollided && collider->IsSolid()) {
      if (this->velocity.y > 0) {
        pos->y -= collision.overlap.height;
        this->isGrounded = true;
      } else {
        pos->y += collision.overlap.height;
      }

      this->velocity.y = 0;
    } else {
      this->hasJumped = true;
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