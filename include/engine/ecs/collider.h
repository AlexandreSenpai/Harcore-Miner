#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/component_metadata.h"
#include "engine/ecs/entity.h"
#include <raylib.h>

enum CollisionType { ENVIRONMENT_COLLISION, ENTITY_COLLISION };

struct CollisionResult {
  CollisionType type;
  Rectangle overlap;
  Rectangle *otherTile;
  IEntity *otherEntity;
  IEntity *selfEntity;
  bool hasCollided;
};

class ColliderComponent : public IComponent {
private:
  Rectangle boundingBox;
  bool isSolid;
  bool showDebugCollisions = false;

public:
  static const int ID = ComponentType::COLLIDER;

  ColliderComponent(IEntity *entity, float width, float height,
                    float offsetX = 0.0f, float offsetY = 0.0f);
  ~ColliderComponent() override = default;

  Rectangle GetWorldBounds() const;

  bool IsSolid() const { return isSolid; }
  void SetSolid(bool solid) { this->isSolid = solid; }

  bool CheckCollision(Rectangle selfBounds, Rectangle other,
                      Rectangle &overlap) const;

  CollisionResult
  CheckLevelCollision(Rectangle *entityColliderBounds = nullptr) const;
  CollisionResult
  CheckCollisionWithEntities(Rectangle *entityColliderBounds = nullptr) const;

  CollisionResult EvaluateCollisions(Rectangle *entityColliderBounds = nullptr);

  void Update() override;
  void Draw() override;
  void Debug() override;

  int GetId() const override { return ID; }
};