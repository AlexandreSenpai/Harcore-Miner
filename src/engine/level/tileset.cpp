#include "engine/level/tileset.h"
#include <raylib.h>

Tileset::Tileset(const char *path) { this->texture = ::LoadTexture(path); }

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
