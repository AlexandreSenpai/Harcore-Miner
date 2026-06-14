#include "engine/ecs/entity.h"
#include "engine/ecs/texture.h"

class HealthComponent;
class TextureComponent;

struct DestroyedBlockEvent {
  int reward;
};

class Block : public IEntity {
private:
  HealthComponent *health;
  TextureComponent *texture;
  AnimationResult destroyAnimationResult;
  int reward = 0;
  bool rewardDispatched = false;

public:
  Block(float maxHealth = 100.0f, int reward = 0);
  ~Block();

  void Update() override;
  void Draw() override;
};