#pragma once

#include "engine/core/scene.h"
#include "engine/managers/scene_manager.h"

class MenuScene : public IScene {

public:
  MenuScene();
  ~MenuScene() override;

  void Update() override;
  void Draw() override;
};

class MenuSceneFactory : public SceneFactory {
public:
  std::unique_ptr<IScene> Create() override {
    return std::make_unique<MenuScene>();
  }
};
