#include "engine/core/game.h"
#include "engine/core/window.h"
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
  RaylibRmlUi::LoadFont("C:\\Users\\alexa\\scripts\\games\\Cave "
                        "Miner\\src\\game\\ui\\Roboto-Regular.ttf");

  sceneManager->Add("Main", new MainSceneFactory());
  sceneManager->Load("Main");

  // RaylibRmlUi::EnableDebugger();

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