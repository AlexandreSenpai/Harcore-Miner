#include "engine/core/primitives/square.h"
#include "engine/ecs/entity.h"

Square::Square(int width, int height) : IEntity("Square") {
  this->transform.x = width;
  this->transform.y = height;
}

Square::~Square() {}

void Square::Draw() { ::IEntity::Draw(); }

void Square::Update() { ::IEntity::Update(); }