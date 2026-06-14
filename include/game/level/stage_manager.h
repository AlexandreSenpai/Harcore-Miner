#pragma once
#include "engine/ecs/entity.h"
#include "thirdParty/json.hpp"
#include <fstream>

class IScene;
class IEntity;
class Timer;

class StageManager : public IEntity {
public:
  StageManager(IScene *scene, IEntity *player, Timer *timer);
  ~StageManager();

  void Update();
  void Draw();
  void OnTimerFinish(void *data);
  void OnWinTrigger(void *data);
  void OnShopClosed(void *data);
  void CreateStage(int stageLevel = 1);

private:
  int currentStageLevel = 1;
  bool wonLevel = false;
  bool timesUp = false;
  bool pendingStageReset = false;

  IEntity *player;
  IScene *scene;
  Timer *timer;

  nlohmann::json blocksJsonFile;
};