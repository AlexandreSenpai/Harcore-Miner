#include "engine/ecs/attack.h"
#include "engine/ecs/circle_collider.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/health.h"
#include "engine/managers/scene_manager.h"
#include <imgui.h>

AttackComponent::AttackComponent(IEntity *entity, float damage)
    : IComponent(entity, "AttackComponent"), damage(damage) {}

void AttackComponent::Update() {
  if (!this->IsEnabled() || !::IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    return;
  }

  IScene *currentScene = SceneManager::GetInstance()->GetCurrentScene();
  if (currentScene == nullptr)
    return;

  Vector2 mouseWorldPos = ::GetScreenToWorld2D(
      ::GetMousePosition(), currentScene->GetCamera().GetRaylibCamera());

  CircleColliderComponent *circleCollider =
      this->entity->GetComponent<CircleColliderComponent>();
  std::vector<IEntity *> entitiesInRadius =
      circleCollider->GetEntitiesInRadius();

  for (IEntity *otherEntity : entitiesInRadius) {
    if (otherEntity == this->entity)
      continue;

    ColliderComponent *collider =
        otherEntity->GetComponent<ColliderComponent>();
    HealthComponent *health = otherEntity->GetComponent<HealthComponent>();

    if (collider != nullptr && collider->IsEnabled() && health != nullptr &&
        !health->IsDead()) {
      if (::CheckCollisionPointRec(mouseWorldPos, collider->GetWorldBounds())) {
        health->TakeDamage(this->damage);
        break;
      }
    }
  }
}

void AttackComponent::Debug() {
  IComponent::Debug();
  ImGui::InputFloat("Damage", &this->damage);
}
