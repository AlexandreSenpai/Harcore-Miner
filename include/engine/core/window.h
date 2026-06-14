#pragma once
#include "engine/core/scene.h"
#include "engine/managers/scene_manager.h"

class Window {
private:
  int width;
  int height;
  int fps;
  const char *title;

  SceneManager *sceneManager;

public:
  Window(int width, int height, int fps, const char *title);
  ~Window();

  void CreateWindow();
  void SetScene(IScene *scene);
  void Draw();
  void Update();
};