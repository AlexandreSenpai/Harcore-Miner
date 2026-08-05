#include "engine/ecs/entity.h"

class Square : public IEntity {
private:
public:
  Square(int width = 16, int height = 16);
  ~Square();

  void Draw() override;
  void Update() override;
};