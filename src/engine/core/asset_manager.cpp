#include "engine/core/asset_manager.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

AssetManager *AssetManager::instance = nullptr;

AssetManager::AssetManager() {}

AssetManager::~AssetManager() {}

AssetManager *AssetManager::GetInstance() {
  if (instance == nullptr) {
    instance = new AssetManager();
  }
  return instance;
}

std::string AssetManager::GetResourcePath(const std::string &path) {
  std::string normalized = path;
  std::replace(normalized.begin(), normalized.end(), '\\', '/');

  size_t srcPos = normalized.find("src/");
  if (srcPos != std::string::npos) {
    normalized = normalized.substr(srcPos);
  }

  std::filesystem::path p(normalized);
  if (p.is_absolute()) {
    return normalized;
  }

  if (std::filesystem::exists(p)) {
    return normalized;
  }

  // Check relative to the application directory (where the executable lives)
  std::filesystem::path appDir(::GetApplicationDirectory());
  
  std::filesystem::path attemptApp1 = appDir / p;
  if (std::filesystem::exists(attemptApp1)) {
    return attemptApp1.string();
  }

  std::filesystem::path attemptApp2 = appDir / ".." / p;
  if (std::filesystem::exists(attemptApp2)) {
    return attemptApp2.string();
  }

  std::filesystem::path attemptApp3 = appDir / ".." / ".." / p;
  if (std::filesystem::exists(attemptApp3)) {
    return attemptApp3.string();
  }

  // Fallback to checking relative to current working directory
  std::filesystem::path attempt2 = std::filesystem::current_path() / ".." / p;
  if (std::filesystem::exists(attempt2)) {
    return attempt2.string();
  }

  std::filesystem::path attempt3 = std::filesystem::current_path() / ".." / ".." / p;
  if (std::filesystem::exists(attempt3)) {
    return attempt3.string();
  }

  return normalized;
}

Texture2D AssetManager::LoadTexture(const char *path) {
  if (this->textures.find(path) != this->textures.end()) {
    return this->textures[path];
  }
  std::string resolved = GetResourcePath(path);
  Texture2D texture = ::LoadTexture(resolved.c_str());
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
  std::string resolved = GetResourcePath(path);
  Shader shader = ::LoadShader(0, resolved.c_str());
  this->shaders[path] = shader;

  std::cout << "Shader loaded: " << resolved << std::endl;

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