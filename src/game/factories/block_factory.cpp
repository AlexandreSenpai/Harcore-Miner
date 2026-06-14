#include "game/factories/block_factory.h"
#include "engine/ecs/collider.h"
#include "engine/ecs/texture.h"
#include "game/ecs/block.h"

IEntity *BlockFactory::CreateBlock(const BlockData &data) {
  Block *block = new Block(data.maxHealth, data.reward);
  block->SetName(data.name.c_str());

  TextureComponent *texture = block->GetComponent<TextureComponent>();
  if (texture != nullptr) {
    texture->AddAnimation("breaking", {.texturePath = data.texturePath.c_str(),
                                       .frameRow = data.textureRow,
                                       .totalFrames = data.totalFrames,
                                       .frameSize = 16,
                                       .framesPerSecond = 1,
                                       .frameTint = data.frameTint});
    texture->AddAnimation("destroying",
                          {.texturePath = data.texturePath.c_str(),
                           .frameRow = 0,
                           .totalFrames = 6,
                           .frameSize = 16,
                           .framesPerSecond = 6,
                           .frameTint = data.frameTint});
    texture->SetCurrentAnimation("breaking");
  }

  ColliderComponent *collider = block->GetComponent<ColliderComponent>();
  if (collider != nullptr) {
    collider->SetEnabled(data.isSolid);
  }

  return block;
}
