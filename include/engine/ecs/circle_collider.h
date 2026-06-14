#pragma once
#include "engine/ecs/collider.h"
#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/entity.h"
#include <raylib.h>
#include <vector>

class CircleColliderComponent : public IComponent {
private:
  float radius;
  Vector2 offset;
  bool isSolid;
  bool showDebugCollisions = false;

public:
  static const int ID = ComponentType::CIRCLE_COLLIDER;

  CircleColliderComponent(IEntity *entity, float radius, float offsetX = 0.0f,
                          float offsetY = 0.0f);

  Vector2 GetWorldCenter();
  float GetRadius() { return radius; }

  bool IsSolid() { return isSolid; }

  // Métodos de checagem
  bool CheckCollisionRec(Vector2 center, Rectangle other);
  bool CheckCollisionCircle(Vector2 center, Vector2 otherCenter,
                            float otherRadius);

  // O padrão da engine
  CollisionResult
  CheckCollisionWithEntities(Vector2 *predictedCenter = nullptr);
  CollisionResult CheckLevelCollision(Vector2 *predictedCenter = nullptr);
  CollisionResult EvaluateCollisions(Vector2 *predictedCenter = nullptr);

  // NOVO: Método específico para sistemas de combate
  std::vector<IEntity *> GetEntitiesInRadius();

  void Update() override;
  void Draw() override;
  void Debug() override;

  int GetId() const override { return ID; }
};