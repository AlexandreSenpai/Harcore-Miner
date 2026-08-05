#pragma once

#include <raylib.h>

class IEntity;

struct CameraBounds {
  float x;
  float y;
  float width;
  float height;
};

enum CameraAnchor {
  CENTERED,
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT,
};

enum class CameraScaleMode {
  FIT,  // Fits entire target entity inside screen (may introduce letterboxing)
  FILL, // Fills entire screen with target entity (no letterboxing, crops overflow)
};

class GameCamera {
private:
  Camera2D camera;
  IEntity *target = nullptr;
  CameraAnchor anchor = CENTERED;
  CameraScaleMode scaleMode = CameraScaleMode::FIT;
  bool fitToScreen = false;
  float smoothSpeed;
  CameraBounds bounds;
  bool hasBounds = false;

  void ClampToLevelBounds();
  void ScaleToFit();

public:
  GameCamera(float zoom = 0.0f, float rotation = 0.0f,
             float smoothSpeed = 0.1f);

  void Update();

  void Begin();
  void End();

  void SetTarget(IEntity *entity, CameraAnchor anchor = CENTERED,
                 bool fitToScreen = false,
                 CameraScaleMode mode = CameraScaleMode::FIT);
  void SetAnchor(bool fitToScreen = false);
  IEntity *GetTarget() const { return target; }

  void SetScaleMode(CameraScaleMode mode) { scaleMode = mode; }
  CameraScaleMode GetScaleMode() const { return scaleMode; }

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
