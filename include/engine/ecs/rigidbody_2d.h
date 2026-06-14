#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/entity.h"
#include <raylib.h>

class RigidBody2DComponent : public IComponent {
private:
  Vector2 velocity;
  float gravity = 20.0f;
  float gravityScale = 1.0f;
  bool isGrounded = false;
  float isGroundedReleaseTimer = 0.0f;
  bool hasJumped = false;

public:
  static const int ID =
      ComponentType::RIGIDBODY_2D; // ID único para o Rigidbody

  RigidBody2DComponent(IEntity *entity);
  ~RigidBody2DComponent() override = default;

  void SetVelocity(Vector2 newVelocity) { this->velocity = newVelocity; }
  void SetVelocityY(float y) { this->velocity.y = y; }
  void SetVelocityX(float x) { this->velocity.x = x; }
  Vector2 GetVelocity() const { return this->velocity; }

  bool IsGrounded() const { return isGrounded; }

  void ApplyGravity();
  void SetGravityScale(float scale) { this->gravityScale = scale; }

  void Update() override;
  void Draw() override {}
  void Debug() override;

  int GetId() const override { return ID; }
};