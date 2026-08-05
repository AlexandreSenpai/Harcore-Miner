#pragma once

#include "engine/core/scene.h"
#include <map>
#include <string>

class SceneFactory {
public:
  virtual std::unique_ptr<IScene> Create() = 0;
};

class SceneManager {
private:
  static SceneManager *instance;
  std::unique_ptr<IScene> currentScene;
  std::string pendingSceneName = "";
  std::map<std::string, SceneFactory *> sceneFactories;

  void ForceLoad(const char *sceneName);

public:
  SceneManager();
  ~SceneManager();

  static SceneManager *GetInstance();

  void Load(const char *sceneName);
  void Unload();
  void Add(const char *sceneName, SceneFactory *sceneFactory);
  void Remove(const char *sceneName);

  IScene *GetCurrentScene();

  void ChangeScene();
};
