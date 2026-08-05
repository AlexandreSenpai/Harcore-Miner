#include "game/scenes/menu.h"
#include "engine/core/camera.h"
#include "engine/core/primitives/square.h"
#include "engine/ecs/texture.h"
#include <iostream>
#include <raylib.h>

MenuScene::MenuScene() : IScene("Menu Scene") {
  Square *square = new Square(528, 288);
  TextureComponent *tex = new TextureComponent(square);
  square->AddComponent(tex);

  tex->AddAnimation("idle",
                    {
                        .texturePath = "src/game/assets/menu/menu-Sheet.png",
                        .frameRow = 0,
                        .totalFrames = 25,
                        .frameWidth = 528,
                        .frameHeight = 288,
                        .framesPerSecond = 16,
                    });
  tex->SetCurrentAnimation("idle");
  tex->PlayAnimation(true);

  this->AddEntity(square);

  this->GetCamera().SetTarget(square, CameraAnchor::CENTERED, true,
                              CameraScaleMode::FILL);

  std::cout << "MenuScene loaded successfully!" << std::endl;
}

MenuScene::~MenuScene() {}

void MenuScene::Update() {
  for (IEntity *entity : this->entities) {
    entity->Update();
  }

  if (::IsKeyPressed(KEY_ENTER)) {
    std::cout << "Load Main Scene" << std::endl;
    SceneManager::GetInstance()->Load("Main");
  }

  this->GetCamera().Update();
}

void MenuScene::Draw() {
  GetCamera().Begin();
  IScene::Draw();
  for (IEntity *entity : this->entities) {
    entity->Draw();
  }
  GetCamera().End();
}
