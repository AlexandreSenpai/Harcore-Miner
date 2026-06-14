#pragma once

#include <raylib.h>

class IEntity;

struct CameraBounds {
  float x;
  float y;
  float width;
  float height;
};

class GameCamera {
private:
  Camera2D camera;
  IEntity *target = nullptr;
  float smoothSpeed;
  CameraBounds bounds;
  bool hasBounds = false;

  void ClampToLevelBounds();

public:
  GameCamera(float zoom = 1.0f, float rotation = 0.0f,
             float smoothSpeed = 0.1f);

  void Update();

  void Begin();
  void End();

  void SetTarget(IEntity *entity);
  IEntity *GetTarget() const { return target; }

  void SetBounds(float x, float y, float width, float height);
  void ClearBounds() { hasBounds = false; }
  bool HasBounds() const { return hasBounds; }

  void SetZoom(float zoom) { camera.zoom = zoom; }
  void SetRotation(float rotation) { camera.rotation = rotation; }
  void SetOffset(Vector2 offset) { camera.offset = offset; }
  void SetSmoothSpeed(float speed) { smoothSpeed = speed; }

  float GetZoom() const { return camera.zoom; }
  float GetRotation() const { return camera.rotation; }
  Vector2 GetOffset() const { return camera.offset; }
  float GetSmoothSpeed() const { return smoothSpeed; }

  Camera2D &GetRaylibCamera() { return camera; }
  const Camera2D &GetRaylibCamera() const { return camera; }
};
