#include "game/ecs/block.h"
#include "engine/ecs/block_highlight.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/entity.h"
#include "engine/ecs/health.h"
#include "engine/ecs/rigidbody_2d.h"
#include "engine/ecs/texture.h"
#include "engine/managers/scene_manager.h"

Block::Block(float maxHealth, int reward) : IEntity("Block") {
  this->SetTransform({16, 16});
  this->AddComponent(new ColliderComponent(this, 16, 16));

  this->health = new HealthComponent(this, maxHealth);
  this->AddComponent(this->health);

  this->texture = new TextureComponent(this);
  this->AddComponent(this->texture);
  this->AddComponent(new BlockHighlightComponent(this));

  RigidBody2DComponent *rb = new RigidBody2DComponent(this);
  rb->SetEnabled(false);
  rb->SetGravityScale(0.5f);
  this->AddComponent(rb);
  this->reward = reward;
};

Block::~Block() {};

void Block::Update() {
  IEntity::Update();

  if (this->health->IsDead() &&
      this->destroyAnimationResult.state != AnimationPlaybackState::FINISHED) {
    this->texture->SetCurrentAnimation("destroying");
    this->texture->PlayAnimation(true, &this->destroyAnimationResult,
                                 AnimationPlaybackType::ONCE);
    this->GetComponent<ColliderComponent>()->SetEnabled(false);
    this->GetComponent<RigidBody2DComponent>()->SetEnabled(true);
    if (!this->rewardDispatched) {
      this->rewardDispatched = true;
      DestroyedBlockEvent event;
      event.reward = this->reward;
      EventSystem::GetInstance()->Dispatch(EventType::ON_BLOCK_DESTROYED, &event);
    }
    return;
  }

  if (this->destroyAnimationResult.state == AnimationPlaybackState::FINISHED) {
    SceneManager::GetInstance()->GetCurrentScene()->RequestDestroyEntity(
        this->GetId());
    return;
  }

  float lifePct = this->health->GetLifePercentage();

  int frameIndex = 0;
  if (lifePct <= 1.0f / 5.0f)
    frameIndex = 4;
  else if (lifePct <= 2.0f / 5.0f)
    frameIndex = 3;
  else if (lifePct <= 3.0f / 5.0f)
    frameIndex = 2;
  else if (lifePct <= 4.0f / 5.0f)
    frameIndex = 1;
  else if (lifePct <= 5.0f / 5.0f)
    frameIndex = 0;

  this->texture->SetAnimationFrame(frameIndex);
};

void Block::Draw() { IEntity::Draw(); };