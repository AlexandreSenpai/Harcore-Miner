#include "engine/core/asset_manager.h"
#include <iostream>

AssetManager *AssetManager::instance = nullptr;

AssetManager::AssetManager() {}

AssetManager::~AssetManager() {}

AssetManager *AssetManager::GetInstance() {
  if (instance == nullptr) {
    instance = new AssetManager();
  }
  return instance;
}

Texture2D AssetManager::LoadTexture(const char *path) {
  if (this->textures.find(path) != this->textures.end()) {
    return this->textures[path];
  }
  Texture2D texture = ::LoadTexture(path);
  ::SetTextureFilter(texture, TEXTURE_FILTER_POINT);
  this->textures[path] = texture;

  return texture;
}

void AssetManager::UnloadTexture(const char *path) {
  if (this->textures.find(path) == this->textures.end()) {
    return;
  }
  ::UnloadTexture(this->textures[path]);
  this->textures.erase(path);
}

Texture2D *AssetManager::GetTexture(const char *path) {
  if (this->textures.find(path) == this->textures.end()) {
    return nullptr;
  }
  return &this->textures[path];
}

Shader AssetManager::LoadShader(const char *path) {
  if (this->shaders.find(path) != this->shaders.end()) {
    return this->shaders[path];
  }
  Shader shader = ::LoadShader(0, path);
  this->shaders[path] = shader;

  std::cout << "Shader loaded: " << path << std::endl;

  return shader;
}

void AssetManager::UnloadShader(const char *path) {
  if (this->shaders.find(path) == this->shaders.end()) {
    return;
  }
  ::UnloadShader(this->shaders[path]);
  this->shaders.erase(path);
}

Shader *AssetManager::GetShader(const char *path) {
  if (this->shaders.find(path) == this->shaders.end()) {
    return nullptr;
  }
  return &this->shaders[path];
}