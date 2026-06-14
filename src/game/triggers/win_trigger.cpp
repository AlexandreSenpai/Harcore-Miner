#include "game/triggers/win_trigger.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/entity.h"
#include "engine/events/event_system.h"

WinTrigger::WinTrigger() : IEntity("WinTrigger") {
  ColliderComponent *collider = new ColliderComponent(this, 160.f, 66.0f);
  collider->SetSolid(false);
  this->AddComponent(collider);

  this->SetTransform({150.0f, 66.0f});
  this->SetPosition({272.0f, 814.0f});
}

WinTrigger::~WinTrigger() {}

void WinTrigger::Update() {
  IEntity::Update();

  ColliderComponent *collider = this->GetComponent<ColliderComponent>();

  CollisionResult collisionResult = collider->EvaluateCollisions();

  if (collisionResult.hasCollided &&
      collisionResult.type == CollisionType::ENTITY_COLLISION &&
      collisionResult.otherEntity->GetName() == "Player") {
    EventSystem::GetInstance()->Dispatch(EventType::ON_WIN_TRIGGER, nullptr);
  }
}

void WinTrigger::Draw() { IEntity::Draw(); }
void WinTrigger::Debug() { IEntity::Debug(); }