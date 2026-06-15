#include "engine/level/level_generator.h"
#include "engine/core/asset_manager.h"
#include "thirdParty/json.hpp"
#include <fstream>
#include <iostream>
#include <raylib.h>

LevelGenerator::LevelGenerator(const char *levelPath) {
  this->levelPath = levelPath;
  this->LoadWorldFile();
}

LevelGenerator::~LevelGenerator() {}

void LevelGenerator::LoadWorldFile() {
  std::ifstream file(AssetManager::GetResourcePath(this->levelPath));
  if (!file.is_open()) {
    std::cerr << "Error opening level file: " << this->levelPath << std::endl;
    return;
  }

  this->world = nlohmann::json::parse(file);

  nlohmann::json tilesets = this->world["defs"]["tilesets"];
  for (nlohmann::json tileset : tilesets) {
    this->tilesets.push_back(
        Tileset(tileset["relPath"].get<std::string>().c_str()));

    if (tileset.contains("customData")) {
      for (nlohmann::json customData : tileset["customData"]) {
        int tileId = customData["tileId"];
        std::string dataString = customData["data"].get<std::string>();

        try {
          nlohmann::json dataJson = nlohmann::json::parse(dataString);
          if (dataJson.contains("has_collision") &&
              dataJson["has_collision"] == true) {
            this->solidTileIds.insert(
                tileId); // Salva o ID deste tile como "sólido"
          }
        } catch (nlohmann::json::parse_error &e) {
          // Ignora erros de parse caso haja metadados mal formatados
        }
      }
    }
  }
}

void LevelGenerator::Build(int levelIndex) {
  nlohmann::json levels = this->world["levels"];

  if (levelIndex >= levels.size()) {
    std::cerr << "Error: Level index out of bounds" << std::endl;
    return;
  }

  nlohmann::json level = levels[levelIndex];

  this->levelWidth = level["pxWid"];
  this->levelHeight = level["pxHei"];

  this->gridWidth = this->levelWidth / this->gridSize;
  this->gridHeight = this->levelHeight / this->gridSize;

  this->collisionGrid.assign(this->gridHeight,
                             std::vector<bool>(this->gridWidth, false));

  nlohmann::json layers = level["layerInstances"];

  for (int layerIdx = layers.size() - 1; layerIdx >= 0; layerIdx--) {

    nlohmann::json layer = layers[layerIdx];

    if (layer["__type"] == "Tiles") {
      nlohmann::json gridTiles = layer["gridTiles"];

      for (nlohmann::json gridTile : gridTiles) {
        int tileId = gridTile["t"];
        float x = gridTile["px"][0];
        float y = gridTile["px"][1];
        float tileX = gridTile["src"][0];
        float tileY = gridTile["src"][1];
        Rectangle rect = {tileX, tileY, (float)gridSize, (float)gridSize};
        Vector2 pos = {x, y};

        TileDrawInfo info;
        info.tileId = tileId;
        info.layerName = layer["__identifier"];
        info.tilesetIdx = 0; // all layers share the same tileset
        info.srcRect = rect;
        info.position = pos;

        if (this->solidTileIds.find(tileId) != this->solidTileIds.end()) {
          int gridX = static_cast<int>(x / gridSize);
          int gridY = static_cast<int>(y / gridSize);

          if (gridY < this->gridHeight && gridX < this->gridWidth) {
            this->collisionGrid[gridY][gridX] = true; // Marca como ocupado!
          }
        }

        this->drawList.push_back(info);
      }
    }
  }
}

void LevelGenerator::Draw() {
  for (const TileDrawInfo &info : this->drawList) {
    ::DrawTextureRec(this->tilesets[info.tilesetIdx].GetTexture(), info.srcRect,
                     info.position, WHITE);

    // Draw debug indicator for solid tiles
    if (this->showDebugCollisions &&
        this->solidTileIds.find(info.tileId) != this->solidTileIds.end()) {
      ::DrawRectangleLines(static_cast<int>(info.position.x),
                           static_cast<int>(info.position.y), gridSize,
                           gridSize, RED);
    }
  }
}

bool LevelGenerator::CheckCollisionWithGrid(Rectangle bounds) const {
  // Calcula em quais células do grid os 4 cantos da hitbox estão
  int startX = static_cast<int>(bounds.x / this->gridSize);
  int startY = static_cast<int>(bounds.y / this->gridSize);
  int endX =
      static_cast<int>((bounds.x + bounds.width - 0.01f) / this->gridSize);
  int endY =
      static_cast<int>((bounds.y + bounds.height - 0.01f) / this->gridSize);

  // Varre todas as células do grid que a hitbox sobrepõe
  for (int y = startY; y <= endY; y++) {
    for (int x = startX; x <= endX; x++) {
      if (IsCellOccupied(x, y)) {
        return true; // Bateu numa parede do LDtk!
      }
    }
  }
  return false; // Caminho livre no cenário
}

bool LevelGenerator::IsCellOccupied(int x, int y) const {
  // Se tentar checar algo fora dos limites do mapa, devolve true (como se fosse
  // uma parede invisível intransponível)
  if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight) {
    return true;
  }
  // Caso contrário, retorna o que está salvo no grid
  return collisionGrid[y][x];
}

bool LevelGenerator::IsTileSolid(int tileId) const {
  return this->solidTileIds.find(tileId) != this->solidTileIds.end();
}

std::vector<Rectangle>
LevelGenerator::GetAdjacentSolidTiles(Rectangle bounds) const {
  std::vector<Rectangle> solidTiles;

  int startX = static_cast<int>(bounds.x / this->gridSize);
  int startY = static_cast<int>(bounds.y / this->gridSize);
  int endX =
      static_cast<int>((bounds.x + bounds.width - 0.01f) / this->gridSize);
  int endY =
      static_cast<int>((bounds.y + bounds.height - 0.01f) / this->gridSize);

  // Expand checks by 1 tile to ensure we catch nearby solids for resolution
  for (int y = startY - 1; y <= endY + 1; y++) {
    for (int x = startX - 1; x <= endX + 1; x++) {
      if (IsCellOccupied(x, y)) {
        solidTiles.push_back({(float)x * gridSize, (float)y * gridSize,
                              (float)gridSize, (float)gridSize});
      }
    }
  }

  return solidTiles;
}