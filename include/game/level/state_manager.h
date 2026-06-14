#include "engine/ecs/entity.h"

class StateManager : public IEntity {
private:
  int money = 0;
  int currentStageLevel = 1;

public:
  StateManager();
  ~StateManager();

  void Update() override;
  void Draw() override;

  void AddMoney(int amount);
  void RemoveMoney(int amount);
  int GetMoney() const;

  void NextStage();
  int GetCurrentStageLevel() const;

  void OnReceiveMoney(void *data);
  void OnPurchase(void *data);
};