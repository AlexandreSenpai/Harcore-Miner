#pragma once
#include "engine/core/window.h"

class Game {

private:
  int windowWidth = 1280;
  int windowHeight = 720;
  int fps = 60;
  const char *title = "Game";
  Window *window;

public:
  Game(int windowWidth, int windowHeight, int fps, const char *title);
  ~Game();

  void Run();
};