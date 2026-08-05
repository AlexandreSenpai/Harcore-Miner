#include "engine/managers/scene_manager.h"
#include <raylib.h>

SceneManager *SceneManager::instance = nullptr;

SceneManager::SceneManager() { this->currentScene = nullptr; }

SceneManager::~SceneManager() {
  if (this->currentScene != nullptr) {
    this->Unload();
  }
  this->sceneFactories.clear();
}

SceneManager *SceneManager::GetInstance() {
  if (instance == nullptr) {
    instance = new SceneManager();
  }
  return instance;
}

void SceneManager::Unload() {
  if (this->currentScene != nullptr) {
    this->currentScene.reset();
  }
}

void SceneManager::ForceLoad(const char *sceneName) {
  if (this->currentScene != nullptr) {
    this->Unload();
  }

  auto it = this->sceneFactories.find(sceneName);

  if (it != this->sceneFactories.end()) {
    std::unique_ptr<IScene> scene = it->second->Create(); // instantiating scene
    this->currentScene = std::move(scene);
  } else {
    this->currentScene.reset();
  }
}

void SceneManager::Load(const char *sceneName) {
  if (this->currentScene == nullptr) {
    this->ForceLoad(sceneName);
  } else {
    this->pendingSceneName = sceneName;
  }
}

void SceneManager::Add(const char *sceneName, SceneFactory *sceneFactory) {
  this->sceneFactories[sceneName] = sceneFactory;
}

void SceneManager::Remove(const char *sceneName) {
  this->sceneFactories.erase(sceneName);
}

IScene *SceneManager::GetCurrentScene() { return this->currentScene.get(); }

void SceneManager::ChangeScene() {
  if (!this->pendingSceneName.empty()) {
    std::string nextScene = this->pendingSceneName;
    this->pendingSceneName = "";
    this->ForceLoad(nextScene.c_str());
  }
}
