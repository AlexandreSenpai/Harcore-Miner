#pragma once
#include <raylib.h>

class Tileset {
private:
  Texture2D texture;
  bool ownsTexture = true;

public:
  Tileset(const char *path);
  ~Tileset();

  // Prevent copies (would double-free the texture)
  Tileset(const Tileset &) = delete;
  Tileset &operator=(const Tileset &) = delete;

  // Allow moves
  Tileset(Tileset &&other) noexcept;
  Tileset &operator=(Tileset &&other) noexcept;

  Texture2D GetTexture() const;
};