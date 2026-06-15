#include "engine/level/tileset.h"
#include "engine/core/asset_manager.h"
#include <raylib.h>

Tileset::Tileset(const char *path) {
  this->texture = ::LoadTexture(AssetManager::GetResourcePath(path).c_str());
}

Tileset::~Tileset() {
  if (this->ownsTexture) {
    ::UnloadTexture(this->texture);
  }
}

Tileset::Tileset(Tileset &&other) noexcept
    : texture(other.texture), ownsTexture(other.ownsTexture) {
  other.ownsTexture = false;
}

Tileset &Tileset::operator=(Tileset &&other) noexcept {
  if (this != &other) {
    if (this->ownsTexture) {
      ::UnloadTexture(this->texture);
    }
    this->texture = other.texture;
    this->ownsTexture = other.ownsTexture;
    other.ownsTexture = false;
  }
  return *this;
}

Texture2D Tileset::GetTexture() const { return this->texture; }
