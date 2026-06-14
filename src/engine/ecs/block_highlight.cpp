#include "engine/ecs/block_highlight.h"
#include "engine/core/scene.h"
#include "engine/ecs/circle_collider.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/component.h"
#include "engine/ecs/health.h"
#include "engine/managers/scene_manager.h"
#include "game/ecs/player.h"
#include <cstring>
#include <imgui.h>
#include <raylib.h>

BlockHighlightComponent::BlockHighlightComponent(IEntity *entity)
    : IComponent(entity, "BlockHighlightComponent"), playerIsNearby(false) {}

void BlockHighlightComponent::Debug() {
  IComponent::Debug();
  ImGui::Text("Player is nearby: %s", playerIsNearby ? "Yes" : "No");
}

void BlockHighlightComponent::DrawHighlight() {
  if (playerIsNearby) {
    Vector2 *pos = this->entity->GetPosition();
    Vector2 *transform = this->entity->GetTransform();
    Rectangle bounds = {pos->x, pos->y, transform->x, transform->y};

    ::DrawRectangleLinesEx(bounds, 1.0f, YELLOW);
  }
}

void BlockHighlightComponent::Draw() { DrawHighlight(); }

void BlockHighlightComponent::Update() {
  IScene *currentScene = SceneManager::GetInstance()->GetCurrentScene();

  if (this->player == nullptr) {
    for (IEntity *entity : currentScene->GetEntities()) {
      if (entity->GetName() == "Player") {
        this->player = dynamic_cast<Player *>(entity);
        break;
      }
    }

    if (this->player == nullptr) {
      return;
    }
  }

  CircleColliderComponent *playerCircleCollider =
      this->player->GetComponent<CircleColliderComponent>();
  ColliderComponent *blockCollider =
      this->entity->GetComponent<ColliderComponent>();
  HealthComponent *blockHealth = this->entity->GetComponent<HealthComponent>();

  if (playerCircleCollider == nullptr || blockCollider == nullptr ||
      blockHealth == nullptr || blockHealth->IsDead()) {
    this->playerIsNearby = false;
    return;
  }

  Vector2 playerBounds = playerCircleCollider->GetWorldCenter();
  Rectangle blockBounds = blockCollider->GetWorldBounds();

  if (playerCircleCollider->CheckCollisionRec(playerBounds, blockBounds)) {
    this->playerIsNearby = true;
  } else {
    this->playerIsNearby = false;
  }
}
