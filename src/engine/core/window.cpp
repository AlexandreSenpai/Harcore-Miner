#include "engine/core/window.h"
#include "raylib.h"
#include "thirdParty/raylibRmlUi.h"
#include "thirdParty/rlImGui.h"
#include <imgui.h>

Window::Window(int width, int height, int fps, const char *title) {
  this->width = width;
  this->height = height;
  this->title = title;
  this->fps = fps;

  this->sceneManager = SceneManager::GetInstance();
}

Window::~Window() {
  this->sceneManager->Unload();
  ::rlImGuiShutdown();
  ::CloseWindow();
}

void Window::CreateWindow() {
  ::SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  ::InitWindow(width, height, title);
  ::rlImGuiSetup(true);

  if (fps > 0) {
    ::SetTargetFPS(fps);
  }
}

void Window::Update() {
  IScene *currentScene = this->sceneManager->GetCurrentScene();

  if (currentScene != nullptr) {
    currentScene->Update();
  }
}

void Window::Draw() {
  ::BeginDrawing();
  ::ClearBackground(RAYWHITE);

  ::rlImGuiBegin();

  IScene *currentScene = this->sceneManager->GetCurrentScene();

  if (currentScene != nullptr) {
    currentScene->Draw();
  }

  ::rlImGuiEnd();

  RaylibRmlUi::Draw();

  ::EndDrawing();
}