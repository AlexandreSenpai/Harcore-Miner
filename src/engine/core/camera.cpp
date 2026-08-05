#include "engine/core/camera.h"
#include "engine/ecs/entity.h"
#include <cmath>
#include <iostream>
#include <raylib.h>

GameCamera::GameCamera(float zoom, float rotation, float smoothSpeed)
    : smoothSpeed(smoothSpeed) {
  camera = {{0}};
  camera.zoom = zoom;

  std::cout << "Initial Zoom" << zoom << std::endl;

  camera.rotation = rotation;
  camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void GameCamera::ScaleToFit() {
  IEntity *target = this->target;

  if (!target)
    return;

  Vector2 *size = target->GetTransform();
  int width = size->x;
  int height = size->y;

  if (width <= 0 || height <= 0)
    return;

  float targetZoomX = (float)GetScreenWidth() / width;
  float targetZoomY = (float)GetScreenHeight() / height;

  if (scaleMode == CameraScaleMode::FILL) {
    camera.zoom = std::max(targetZoomX, targetZoomY);
  } else {
    camera.zoom = std::min(targetZoomX, targetZoomY);
  }
}

void GameCamera::SetAnchor(bool fitToScreen) {
  if (!this->target)
    return;

  this->fitToScreen = fitToScreen;
  Vector2 *pos = this->target->GetPosition();
  Vector2 *size = this->target->GetTransform();

  switch (this->anchor) {
  case CENTERED:
    camera.target = {pos->x + size->x / 2.0f, pos->y + size->y / 2.0f};
    break;
  case TOP_LEFT:
    camera.target = *pos;
    break;
  case TOP_RIGHT:
    camera.target = {pos->x + size->x, pos->y};
    break;
  case BOTTOM_LEFT:
    camera.target = {pos->x, pos->y + size->y};
    break;
  case BOTTOM_RIGHT:
    camera.target = {pos->x + size->x, pos->y + size->y};
    break;
  }

  if (fitToScreen) {
    this->ScaleToFit();
  }
}

void GameCamera::SetTarget(IEntity *entity, CameraAnchor anchor,
                           bool fitToScreen, CameraScaleMode mode) {
  this->target = entity;
  this->anchor = anchor;
  this->scaleMode = mode;

  this->SetAnchor(fitToScreen);
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
  Vector2 *targetSize = target->GetTransform();

  this->SetAnchor(this->fitToScreen);
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
  renderCam.target.x =
      std::round(renderCam.target.x * camera.zoom) / camera.zoom;
  renderCam.target.y =
      std::round(renderCam.target.y * camera.zoom) / camera.zoom;
  BeginMode2D(renderCam);
}

void GameCamera::End() { EndMode2D(); }
