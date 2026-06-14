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
    delete this->currentScene;
    this->currentScene = nullptr;
  }
}

void SceneManager::Load(const char *sceneName) {
  if (this->currentScene != nullptr) {
    this->Unload();
  }

  auto it = this->sceneFactories.find(sceneName);

  if (it != this->sceneFactories.end()) {
    IScene *scene = it->second->Create(); // instantiating scene
    this->currentScene = scene;
  } else {
    this->currentScene = nullptr;
  }
}

void SceneManager::Add(const char *sceneName, SceneFactory *sceneFactory) {
  this->sceneFactories[sceneName] = sceneFactory;
}

void SceneManager::Remove(const char *sceneName) {
  this->sceneFactories.erase(sceneName);
}

IScene *SceneManager::GetCurrentScene() { return this->currentScene; }

void SceneManager::ChangeScene() {
  if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
    this->Load("Battle Scene");
  }

  if (IsKeyPressed(KEY_LEFT_BRACKET)) {
    this->Load("Auto Move Validation Scene");
  }
}
