#include "engine/ecs/entity.h"

class WinTrigger : public IEntity {
public:
  WinTrigger();
  ~WinTrigger();

  void Update() override;
  void Draw() override;
  void Debug() override;
};