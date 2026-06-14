#pragma once

#include "engine/core/scene.h"
#include "engine/level/level_generator.h"
#include "engine/managers/scene_manager.h"

class MainScene : public IScene {
private:
  LevelGenerator *levelGenerator;
  bool paused = false;

public:
  MainScene();
  ~MainScene() override;

  LevelGenerator *GetLevelGenerator() const override { return levelGenerator; }

  void Update() override;
  void Draw() override;
};

class MainSceneFactory : public SceneFactory {
public:
  IScene *Create() override { return new MainScene(); }
};
