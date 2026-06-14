#include "game/ecs/player.h"
#include "engine/ecs/circle_collider.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/move_2d.h"
#include "engine/ecs/rigidbody_2d.h"
#include "engine/ecs/texture.h"

#include "engine/ecs/attack.h"

Player::Player() : IEntity("Player") {
  this->SetTransform({16, 16});
  this->AddComponent(new Move2DComponent(this, 100.0f));
  this->AddComponent(new ColliderComponent(this, 11, 13, 1.6f, 3.0f));
  this->AddComponent(new CircleColliderComponent(this, 30.0f));
  this->AddComponent(new RigidBody2DComponent(this));
  this->AddComponent(new AttackComponent(this, 25.0f));

  TextureComponent *texture = new TextureComponent(this);
  texture->AddAnimation(
      "idle",
      {
          .texturePath = "C:\\Users\\alexa\\scripts\\games\\Cave "
                         "Miner\\src\\game\\assets\\Player\\player_anim.png",
          .frameRow = 0,
          .totalFrames = 2,
          .frameSize = 16,
          .framesPerSecond = 4,
      });
  texture->AddAnimation(
      "running",
      {
          .texturePath = "C:\\Users\\alexa\\scripts\\games\\Cave "
                         "Miner\\src\\game\\assets\\Player\\player_anim.png",
          .frameRow = 1,
          .totalFrames = 2,
          .frameSize = 16,
          .framesPerSecond = 4,
      });

  texture->SetCurrentAnimation("idle");
  texture->PlayAnimation(true);

  this->AddComponent(texture);
};

Player::~Player() { IEntity::~IEntity(); };

void Player::Update() { IEntity::Update(); };

void Player::Draw() { IEntity::Draw(); };