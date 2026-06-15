#include "game/scenes/main.h"
#include "game/ecs/player.h"
#include "game/level/stage_manager.h"
#include "game/level/state_manager.h"
#include "game/level/timer.h"
#include "game/triggers/win_trigger.h"
#include "game/shop/shop.h"
#include "game/ui/main_hud.h"
#include "thirdParty/raylibRmlUi.h"
#include "engine/core/asset_manager.h"
#include <iostream>
#include <raylib.h>

MainScene::MainScene() : IScene("Main Scene") {
  this->levelGenerator =
      new LevelGenerator(AssetManager::GetResourcePath("src/game/level/level1.ldtk").c_str());
  this->levelGenerator->Build(0);

  Player *player = new Player();
  player->SetPosition({251.0f, 102.0f});
  this->AddEntity(player);

  Timer *timer = new Timer(30.0f, TimeDirection::COUNTDOWN, 2, 100);
  this->AddEntity(timer);
  this->AddEntity(new WinTrigger());
  this->AddEntity(new StateManager());

  this->AddEntity(new StageManager(this, player, timer));

  this->GetCamera().SetTarget(player);
  this->GetCamera().SetZoom(4.0f);
  this->GetCamera().SetBounds(1, 1, this->levelGenerator->GetLevelWidth(),
                              this->levelGenerator->GetLevelHeight());

  Rml::ElementDocument *hudDoc =
      RaylibRmlUi::LoadDocument(AssetManager::GetResourcePath("src/game/ui/main_hud/index.rml").c_str());
  this->AddEntity(new MainHud(hudDoc));

  Rml::ElementDocument *shopDoc =
      RaylibRmlUi::LoadDocument(AssetManager::GetResourcePath("src/game/ui/shop/index.rml").c_str());
  this->AddEntity(new Shop(shopDoc, timer));

  // Pause listeners
  EventSystem::GetInstance()->AddListener(
      EventType::ON_SHOP_OPENED, [this](void *data) { this->paused = true;
      });
  EventSystem::GetInstance()->AddListener(
      EventType::ON_SHOP_CLOSED, [this](void *data) { this->paused = false;
      });

  std::cout << "MainScene loaded successfully!" << std::endl;
  timer->Start();
}

MainScene::~MainScene() { delete this->levelGenerator; }

void MainScene::Update() {
  if (IsKeyPressed(KEY_F1)) {
    this->levelGenerator->SetShowDebugCollisions(
        !this->levelGenerator->GetShowDebugCollisions());
  }

  if (::IsWindowResized()) {
    this->GetCamera().SetBounds(1, 1, this->levelGenerator->GetLevelWidth(),
                                this->levelGenerator->GetLevelHeight());
    this->GetCamera().SetZoom(5.0f);
  }

  for (IEntity *entity : this->entities) {
    // Shop needs to update even when paused (to handle clicks)
    if (this->paused && entity->GetName() != "Shop")
      continue;
    entity->Update();
  }
  this->FlushPendingChanges();
  this->GetCamera().Update();
}

void MainScene::Draw() {

  GetCamera().Begin();
  IScene::Draw();
  this->levelGenerator->Draw();
  for (IEntity *entity : this->entities) {
    entity->Draw();
  }
  GetCamera().End();
}
