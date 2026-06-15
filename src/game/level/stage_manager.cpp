#include "game/level/stage_manager.h"
#include "engine/core/scene.h"
#include "engine/core/asset_manager.h"
#include "game/ecs/block.h"
#include "game/factories/block_factory.h"
#include "game/level/timer.h"
#include "game/ui/main_hud.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <thirdParty/json.hpp>
#include <vector>

StageManager::StageManager(IScene *scene, IEntity *player, Timer *timer)
    : IEntity("StageManager") {
  this->player = player;
  this->scene = scene;
  this->timer = timer;

  std::ifstream file(AssetManager::GetResourcePath("src/game/level/blocks.json"));

  if (!file.is_open()) {
    std::cerr << "Error opening blocks json file" << std::endl;
    return;
  }

  this->blocksJsonFile = nlohmann::json::parse(file);

  this->CreateStage(this->currentStageLevel);
  scene->FlushPendingChanges();

  EventSystem::GetInstance()->AddListener(
      EventType::ON_TIMER_FINISH,
      [this](void *data) { this->OnTimerFinish(data); });
  EventSystem::GetInstance()->AddListener(
      EventType::ON_WIN_TRIGGER,
      [this](void *data) { this->OnWinTrigger(data); });
  EventSystem::GetInstance()->AddListener(
      EventType::ON_SHOP_CLOSED,
      [this](void *data) { this->OnShopClosed(data); });
};

StageManager::~StageManager() {};

void StageManager::Update() {
  if (this->pendingStageReset) {
    this->pendingStageReset = false;
    this->wonLevel = false;
    this->timesUp = false;
  }
}

void StageManager::Draw() {};

void StageManager::OnTimerFinish(void *data) {
  TimerData *timerData = (TimerData *)data;

  if (this->wonLevel) {
    return;
  }

  this->timesUp = true;

  std::cout << "Timer finished!" << std::endl;
  std::cout << "Current Time: " << timerData->currentTime << std::endl;
  std::cout << "Max Duration: " << timerData->maxDuration << std::endl;
  std::cout << "Direction: " << timerData->direction << std::endl;
  std::cout << "Is Finished: " << timerData->isFinished << std::endl;
}

void StageManager::OnWinTrigger(void *data) {
  if (this->timesUp || this->wonLevel) {
    return;
  }

  std::cout << "Win trigger activated!" << std::endl;

  this->wonLevel = true;
  this->player->SetPosition({251.0f, 102.0f});
  this->currentStageLevel++;

  StageChangedEvent stageEvent;
  stageEvent.stageLevel = this->currentStageLevel;
  EventSystem::GetInstance()->Dispatch(EventType::ON_STAGE_CHANGED,
                                       &stageEvent);

  // Don't create stage yet — wait for shop to close
}

void StageManager::OnShopClosed(void *data) {
  if (!this->wonLevel)
    return;

  this->CreateStage(this->currentStageLevel);
  this->pendingStageReset = true;
}

void StageManager::CreateStage(int stageLevel) {

  scene->RequestDestroyEntitiesByType<Block>();

  Vector2 anchor = {272.0f, 288.0f};

  std::vector<BlockData> activeBlocks;
  float totalStageProbability = 0.0f;

  // 1. CARREGAMENTO E CÁLCULO DE ESCALONAMENTO MATEMÁTICO
  for (const auto &blockJson : this->blocksJsonFile["blocks"]) {
    BlockData data;
    data.name = blockJson["name"];
    data.texturePath = blockJson["texturePath"];
    data.textureRow = blockJson["textureRow"];
    data.totalFrames = blockJson["totalFrames"];
    data.isSolid = blockJson["isSolid"];

    data.baseProbability = blockJson["baseProbability"];
    data.probabilityShift = blockJson["probabilityShift"];
    data.baseDurability = blockJson["baseDurability"];
    data.durabilityScale = blockJson["durabilityScale"];
    data.baseReward = blockJson["baseReward"];
    data.rewardScale = blockJson["rewardScale"];

    auto tintArray = blockJson["frameTint"];
    data.frameTint = {static_cast<unsigned char>(tintArray[0]),
                      static_cast<unsigned char>(tintArray[1]),
                      static_cast<unsigned char>(tintArray[2]),
                      static_cast<unsigned char>(tintArray[3])};

    // Calcula a probabilidade de surgir baseada no estágio (Estágio 1 = base)
    float currentProb =
        data.baseProbability + (data.probabilityShift * (stageLevel - 1));

    // Se a probabilidade cair abaixo de 0, o bloco para de existir neste nível
    if (currentProb > 0.0f) {

      // Calcula a Vida e a Recompensa usando juros compostos: valor * (escala ^
      // niveis)
      data.maxHealth =
          data.baseDurability * std::pow(data.durabilityScale, stageLevel - 1);
      data.reward = static_cast<int>(
          data.baseReward * std::pow(data.rewardScale, stageLevel - 1));

      data.currentProbability = currentProb;
      totalStageProbability += currentProb;

      activeBlocks.push_back(data);
    }
  }

  // 2. PREPARAÇÃO DO GRID
  int stageCols = 10;
  int stageRows = 30;
  std::vector<std::vector<std::string>> mapPlan(
      stageRows, std::vector<std::string>(stageCols, ""));

  // --- PASSO A: Distribuir os blocos usando Peso Probabilístico (Weighted
  // Random) ---
  for (int y = 0; y < stageRows; y++) {
    for (int x = 0; x < stageCols; x++) {

      // Sorteia um número de 0 até o total das probabilidades
      float randomWeight = static_cast<float>(GetRandomValue(0, 10000)) /
                           10000.0f * totalStageProbability;
      float cumulative = 0.0f;
      std::string selectedName = activeBlocks[0].name;

      for (const BlockData &b : activeBlocks) {
        cumulative += b.currentProbability;
        if (randomWeight <= cumulative) {
          selectedName = b.name;
          break;
        }
      }
      mapPlan[y][x] = selectedName;
    }
  }

  // --- PASSO B: Injetar as Receitas de Combo por cima da geração ---
  std::vector<std::string> comboRecipe = {"Green Block", "Green Block",
                                          "Pink Block", "Blue Block"};
  int combosToInject = 2;

  for (int c = 0; c < combosToInject; c++) {
    for (const std::string &blockName : comboRecipe) {
      int attempts = 0;
      bool placed = false;

      while (!placed && attempts < 50) {
        int cX = GetRandomValue(1, stageCols - 2);
        int cY = GetRandomValue(5, stageRows - 2);

        if (mapPlan[cY][cX] ==
            "Stone Block") { // Esconde no meio das pedras fracas
          mapPlan[cY][cX] = blockName;
          placed = true;
        }
        attempts++;
      }
    }
  }

  // --- PASSO C: Instanciar as Entidades ---
  for (int y = 0; y < stageRows; y++) {
    for (int x = 0; x < stageCols; x++) {

      std::string targetName = mapPlan[y][x];
      BlockData dataToUse = activeBlocks[0]; // Fallback de segurança

      // Localiza o bloco gerado e pega suas estatísticas
      for (const BlockData &b : activeBlocks) {
        if (b.name == targetName) {
          dataToUse = b;
          break;
        }
      }

      IEntity *block = BlockFactory::CreateBlock(dataToUse);
      block->SetPosition({anchor.x + x * 16.0f, anchor.y + y * 16.0f});
      scene->RequestAddEntity(block);
    }
  }
};