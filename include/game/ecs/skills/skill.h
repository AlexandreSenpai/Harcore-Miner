#pragma once

#include <memory>
#include <raylib.h>
#include <string>

class Player;

class Skill {
protected:
  Player *player = nullptr;

public:
  Skill(std::string name, std::string description, int cost, int maxLevel);
  virtual ~Skill();

  virtual void SetPlayerReference(Player *player) { this->player = player; };

  virtual void Update();
  virtual void Draw();
  virtual bool LevelUp();
  virtual std::unique_ptr<Skill> Clone() const;

  std::string name;
  std::string description;

  int cost = 0;
  int level = 1;
  int max_level = 1;

  bool is_active = true;
  bool applied = false;
};