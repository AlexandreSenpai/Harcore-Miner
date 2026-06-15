#include "engine/core/game.h"
#include "engine/core/window.h"
#include "engine/core/asset_manager.h"
#include "game/scenes/main.h"
#include "raylib.h"
#include "thirdParty/raylibRmlUi.h"
#include <iostream>

Game::Game(int windowWidth, int windowHeight, int fps, const char *title) {
  this->window = new Window(windowWidth, windowHeight, fps, title);
}

Game::~Game() { delete this->window; }

void Game::Run() {
  this->window->CreateWindow();

  SceneManager *sceneManager = SceneManager::GetInstance();

  ::InitAudioDevice();
  RaylibRmlUi::Initialize(GetScreenWidth(), GetScreenHeight());
  RaylibRmlUi::LoadFont(AssetManager::GetResourcePath("src/game/ui/Roboto-Regular.ttf").c_str());

  sceneManager->Add("Main", new MainSceneFactory());
  sceneManager->Load("Main");

  RaylibRmlUi::EnableDebugger();

  while (!WindowShouldClose()) {
    sceneManager->ChangeScene();

    RaylibRmlUi::Update();
    this->window->Update();

    this->window->Draw();
  }

  sceneManager->Unload();
  RaylibRmlUi::DeInitialize();
  ::CloseAudioDevice();
}