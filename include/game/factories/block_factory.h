#pragma once

#include "engine/ecs/entity.h"
#include <string>

struct BlockData {
  std::string name;
  std::string texturePath;
  int textureRow;
  int totalFrames;
  bool isSolid;
  Color frameTint;

  // Atributos Base (Lidos do JSON)
  float baseProbability;
  float probabilityShift;
  float baseDurability;
  float durabilityScale;
  int baseReward;
  float rewardScale;

  // Valores Calculados (Gerados pela Engine no momento de instanciar)
  float currentProbability;
  float maxHealth;
  int reward;
};

class BlockFactory {
public:
  static IEntity *CreateBlock(const BlockData &data);
};
