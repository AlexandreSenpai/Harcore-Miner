#include "engine/ecs/circle_collider.h"
#include "engine/core/scene.h"
#include "engine/ecs/collider.h"
#include "engine/level/level_generator.h"
#include "engine/managers/scene_manager.h"
#include <imgui.h>
#include <raylib.h>

CircleColliderComponent::CircleColliderComponent(IEntity *entity, float radius,
                                                 float offsetX, float offsetY)
    : IComponent(entity, "CircleColliderComponent") {
  this->radius = radius;
  this->offset = {offsetX, offsetY};
  this->isSolid = false;
}

Vector2 CircleColliderComponent::GetWorldCenter() {
  Vector2 *pos = this->entity->GetPosition();
  Vector2 *transform = this->entity->GetTransform();

  float middleX = pos->x + (transform->x / 2);
  float middleY = pos->y + (transform->y / 2);

  return {middleX + this->offset.x, middleY + this->offset.y};
}

bool CircleColliderComponent::CheckCollisionRec(Vector2 center,
                                                Rectangle other) {
  return ::CheckCollisionCircleRec(center, this->radius, other);
}

bool CircleColliderComponent::CheckCollisionCircle(Vector2 center,
                                                   Vector2 otherCenter,
                                                   float otherRadius) {
  return ::CheckCollisionCircles(center, this->radius, otherCenter,
                                 otherRadius);
}

CollisionResult
CircleColliderComponent::CheckCollisionWithEntities(Vector2 *predictedCenter) {
  IScene *currentScene = SceneManager::GetInstance()->GetCurrentScene();
  CollisionResult result;
  result.hasCollided = false;

  if (currentScene == nullptr)
    return result;

  Vector2 center =
      (predictedCenter == nullptr) ? this->GetWorldCenter() : *predictedCenter;
  std::vector<IEntity *> entities = currentScene->GetEntities();

  for (IEntity *entity : entities) {
    if (entity == this->entity)
      continue;

    // 1. Tenta checar contra Hitboxes Retangulares (O padrão dos
    // inimigos/blocos)
    ColliderComponent *rectCollider = entity->GetComponent<ColliderComponent>();
    if (rectCollider != nullptr && rectCollider->IsEnabled()) {
      if (this->CheckCollisionRec(center, rectCollider->GetWorldBounds())) {
        result.type = CollisionType::ENTITY_COLLISION;
        result.otherEntity = entity;
        result.selfEntity = this->entity;
        result.hasCollided = true;
        return result; // Para a física padrão, retorna o primeiro que bater
      }
    }

    // 2. Tenta checar contra outros Círculos
    CircleColliderComponent *circleCollider =
        entity->GetComponent<CircleColliderComponent>();
    if (circleCollider != nullptr && circleCollider->IsEnabled()) {
      if (this->CheckCollisionCircle(center, circleCollider->GetWorldCenter(),
                                     circleCollider->GetRadius())) {
        result.type = CollisionType::ENTITY_COLLISION;
        result.otherEntity = entity;
        result.selfEntity = this->entity;
        result.hasCollided = true;
        return result;
      }
    }
  }

  return result;
}

CollisionResult
CircleColliderComponent::CheckLevelCollision(Vector2 *predictedCenter) {
  IScene *currentScene = SceneManager::GetInstance()->GetCurrentScene();
  LevelGenerator *levelGen =
      currentScene ? currentScene->GetLevelGenerator() : nullptr;

  CollisionResult result;
  result.hasCollided = false;

  if (levelGen == nullptr)
    return result;

  Vector2 center =
      (predictedCenter == nullptr) ? this->GetWorldCenter() : *predictedCenter;

  // Cria um quadrado imaginário ao redor do círculo para buscar os blocos no
  // LevelGenerator
  Rectangle searchBounds = {center.x - this->radius, center.y - this->radius,
                            this->radius * 2.0f, this->radius * 2.0f};

  std::vector<Rectangle> tiles = levelGen->GetAdjacentSolidTiles(searchBounds);

  for (Rectangle &tile : tiles) {
    if (this->CheckCollisionRec(center, tile)) {
      result.type = CollisionType::ENVIRONMENT_COLLISION;
      result.otherTile = tile;
      result.selfEntity = this->entity;
      result.hasCollided = true;
      return result;
    }
  }

  return result;
}

CollisionResult
CircleColliderComponent::EvaluateCollisions(Vector2 *predictedCenter) {
  CollisionResult result;
  result.hasCollided = false;

  if (!this->IsEnabled())
    return result;

  result = this->CheckCollisionWithEntities(predictedCenter);
  if (result.hasCollided)
    return result;

  result = this->CheckLevelCollision(predictedCenter);
  if (result.hasCollided)
    return result;

  return result;
}

std::vector<IEntity *> CircleColliderComponent::GetEntitiesInRadius() {
  IScene *currentScene = SceneManager::GetInstance()->GetCurrentScene();
  std::vector<IEntity *> hitEntities;

  if (currentScene == nullptr || !this->IsEnabled())
    return hitEntities;

  Vector2 center = this->GetWorldCenter();

  for (IEntity *entity : currentScene->GetEntities()) {
    if (entity == this->entity)
      continue;

    ColliderComponent *rectCollider = entity->GetComponent<ColliderComponent>();
    if (rectCollider != nullptr && rectCollider->IsEnabled()) {
      if (this->CheckCollisionRec(center, rectCollider->GetWorldBounds())) {
        hitEntities.push_back(entity);
        continue;
      }
    }

    CircleColliderComponent *circleCollider =
        entity->GetComponent<CircleColliderComponent>();
    if (circleCollider != nullptr && circleCollider->IsEnabled()) {
      if (this->CheckCollisionCircle(center, circleCollider->GetWorldCenter(),
                                     circleCollider->GetRadius())) {
        hitEntities.push_back(entity);
      }
    }
  }

  return hitEntities;
}

void CircleColliderComponent::Draw() {
  if (!this->IsEnabled())
    return;

  Vector2 center = this->GetWorldCenter();
  if (this->showDebugCollisions) {
    // Usa verde se for um Trigger/Área de Ataque, vermelho se for parede
    Color colColor = this->isSolid ? RED : GREEN;
    ::DrawCircleLines(center.x, center.y, this->radius, colColor);
  }
}

void CircleColliderComponent::Update() {
  if (!this->IsEnabled())
    return;

  if (IsKeyPressed(KEY_F1))
    this->showDebugCollisions = !this->showDebugCollisions;
}

void CircleColliderComponent::Debug() {
  IComponent::Debug();
  ImGui::Checkbox("Is Solid", &this->isSolid);
  ImGui::InputFloat2("Offset", &this->offset.x);
  ImGui::InputFloat("Radius", &this->radius);
}