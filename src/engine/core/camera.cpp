#include "engine/core/camera.h"
#include "engine/ecs/entity.h"
#include <cmath>
#include <raylib.h>

GameCamera::GameCamera(float zoom, float rotation, float smoothSpeed)
    : smoothSpeed(smoothSpeed) {
  camera = {{0}};
  camera.zoom = zoom;
  camera.rotation = rotation;
  camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void GameCamera::SetTarget(IEntity *entity) {
  target = entity;

  // Snap to target immediately on first assignment
  if (target) {
    Vector2 *pos = target->GetPosition();
    camera.target = *pos;
  }
}

void GameCamera::Update() {
  // Keep the offset in sync with the current screen size so the camera
  // stays centered after a window resize.
  camera.offset = {std::floor(GetScreenWidth() / 2.0f),
                   std::floor(GetScreenHeight() / 2.0f)};

  if (!target) {
    return;
  }

  Vector2 *targetPos = target->GetPosition();

  camera.target = *targetPos;
  ClampToLevelBounds();
}

void GameCamera::SetBounds(float x, float y, float width, float height) {
  bounds = {x, y, width, height};
  hasBounds = true;
}

void GameCamera::ClampToLevelBounds() {
  if (!hasBounds) {
    return;
  }

  // Calculate the visible area in world coordinates
  float halfWidth = camera.offset.x / camera.zoom;
  float halfHeight = camera.offset.y / camera.zoom;

  // Clamp so the camera viewport never goes outside the bounds
  float minX = bounds.x + halfWidth;
  float minY = bounds.y + halfHeight;
  float maxX = bounds.x + bounds.width - halfWidth;
  float maxY = bounds.y + bounds.height - halfHeight;

  // If the level is smaller than the viewport, center it
  if (minX > maxX) {
    camera.target.x = bounds.x + bounds.width / 2.0f;
  } else {
    if (camera.target.x < minX)
      camera.target.x = minX;
    if (camera.target.x > maxX)
      camera.target.x = maxX;
  }

  if (minY > maxY) {
    camera.target.y = bounds.y + bounds.height / 2.0f;
  } else {
    if (camera.target.y < minY)
      camera.target.y = minY;
    if (camera.target.y > maxY)
      camera.target.y = maxY;
  }
}

void GameCamera::Begin() {
  Camera2D renderCam = camera;
  renderCam.target.x = std::floor(renderCam.target.x);
  renderCam.target.y = std::floor(renderCam.target.y);
  BeginMode2D(renderCam);
}

void GameCamera::End() { EndMode2D(); }
