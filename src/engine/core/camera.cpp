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
    camera.target.x = static_cast<int>(bounds.x + bounds.width / 2.0f);
  } else {
    if (camera.target.x < minX)
      camera.target.x = static_cast<int>(minX);
    if (camera.target.x > maxX)
      camera.target.x = static_cast<int>(maxX);
  }

  if (minY > maxY) {
    camera.target.y = static_cast<int>(bounds.y + bounds.height / 2.0f);
  } else {
    if (camera.target.y < minY)
      camera.target.y = static_cast<int>(minY);
    if (camera.target.y > maxY)
      camera.target.y = static_cast<int>(maxY);
  }
}

void GameCamera::Begin() {
  Camera2D renderCam = camera;
  renderCam.target.x = std::round(renderCam.target.x * camera.zoom) / camera.zoom;
  renderCam.target.y = std::round(renderCam.target.y * camera.zoom) / camera.zoom;
  BeginMode2D(renderCam);
}

void GameCamera::End() { EndMode2D(); }
