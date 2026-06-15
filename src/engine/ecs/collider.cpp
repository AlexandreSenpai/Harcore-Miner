#include "engine/ecs/collider.h"
#include "engine/core/scene.h"
#include "engine/ecs/entity.h"
#include "engine/level/level_generator.h"
#include "engine/managers/scene_manager.h"
#include <imgui.h>
#include <raylib.h>

ColliderComponent::ColliderComponent(IEntity *entity, float width, float height,
                                     float offsetX, float offsetY)
    : IComponent(entity, "ColliderComponent") {
  this->boundingBox = {offsetX, offsetY, width, height};
  this->isSolid = true;
}

Rectangle ColliderComponent::GetWorldBounds() const {
  Vector2 *pos = this->entity->GetPosition();

  float posX = (pos->x + boundingBox.x);
  float posY = (pos->y + boundingBox.y);

  return {posX, posY, boundingBox.width, boundingBox.height};
}

bool ColliderComponent::CheckCollision(Rectangle selfBounds, Rectangle other,
                                       Rectangle &overlap) const {
  if (::CheckCollisionRecs(selfBounds, other)) {
    overlap = ::GetCollisionRec(selfBounds, other);
    return true;
  }
  return false;
}

CollisionResult ColliderComponent::CheckCollisionWithEntities(
    Rectangle *entityColliderBounds) const {
  IScene *currentScene = SceneManager::GetInstance()->GetCurrentScene();

  CollisionResult result;
  result.hasCollided = false;

  if (currentScene == nullptr) {
    return result;
  }

  Rectangle bounds;

  if (entityColliderBounds == nullptr) {
    bounds = this->GetWorldBounds();
  } else {
    bounds = *entityColliderBounds;
  }

  std::vector<IEntity *> entities = currentScene->GetEntities();

  for (IEntity *entity : entities) {
    if (entity == this->entity)
      continue;

    ColliderComponent *collisionComponent =
        entity->GetComponent<ColliderComponent>();

    if (collisionComponent == nullptr || !collisionComponent->IsEnabled() ||
        !collisionComponent->IsSolid())
      continue;

    Rectangle overlap;
    Rectangle entityBounds = collisionComponent->GetWorldBounds();

    if (this->CheckCollision(bounds, entityBounds, overlap)) {
      result.type = CollisionType::ENTITY_COLLISION;
      result.overlap = overlap;
      result.otherEntity = entity;
      result.selfEntity = this->entity;
      result.hasCollided = true;

      return result;
    }
  }

  return result;
}

CollisionResult
ColliderComponent::CheckLevelCollision(Rectangle *entityColliderBounds) const {
  IScene *currentScene = SceneManager::GetInstance()->GetCurrentScene();
  LevelGenerator *levelGen =
      currentScene ? currentScene->GetLevelGenerator() : nullptr;

  CollisionResult result;
  result.hasCollided = false;

  if (levelGen == nullptr) {
    return result;
  }

  Rectangle bounds;

  if (entityColliderBounds == nullptr) {
    bounds = this->GetWorldBounds();
  } else {
    bounds = *entityColliderBounds;
  }

  std::vector<Rectangle> tiles = levelGen->GetAdjacentSolidTiles(bounds);

  for (Rectangle &tile : tiles) {

    Rectangle overlap;

    if (this->CheckCollision(bounds, tile, overlap)) {
      result.type = CollisionType::ENVIRONMENT_COLLISION;
      result.overlap = overlap;
      result.otherTile = tile;
      result.selfEntity = this->entity;
      result.hasCollided = true;

      return result;
    }
  }

  return result;
}

CollisionResult
ColliderComponent::EvaluateCollisions(Rectangle *entityColliderBounds) {
  CollisionResult result;
  result.hasCollided = false;

  if (!this->IsEnabled()) {
    return result;
  }

  result = this->CheckCollisionWithEntities(entityColliderBounds);
  if (result.hasCollided) {
    return result;
  }

  result = this->CheckLevelCollision(entityColliderBounds);
  if (result.hasCollided) {
    return result;
  }

  return result;
}

void ColliderComponent::Update() {
  if (!this->IsEnabled()) {
    return;
  }

  if (IsKeyPressed(KEY_F1))
    this->showDebugCollisions = !this->showDebugCollisions;
}

void ColliderComponent::Draw() {
  if (!this->IsEnabled())
    return;

  Rectangle bounds = this->GetWorldBounds();
  if (this->showDebugCollisions)
    ::DrawRectangleLinesEx(bounds, 1.0f, RED);
}

void ColliderComponent::Debug() {
  IComponent::Debug();
  ImGui::Checkbox("Is Solid", &this->isSolid);
  ImGui::InputFloat2("Offset", &this->boundingBox.x);
  ImGui::InputFloat2("Size", &this->boundingBox.width);
}