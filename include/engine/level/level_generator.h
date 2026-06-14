#pragma once
#include "engine/level/tileset.h"
#include "thirdParty/json.hpp"
#include <raylib.h>
#include <string>
#include <unordered_set>
#include <vector>

struct TileDrawInfo {
  int tileId;
  std::string layerName;
  int tilesetIdx;
  Rectangle srcRect;
  Vector2 position;
};

class LevelGenerator {
private:
  const char *levelPath;
  std::vector<Tileset> tilesets;
  std::vector<TileDrawInfo> drawList;
  nlohmann::json world;
  int levelWidth = 0;
  int levelHeight = 0;

  std::unordered_set<int> solidTileIds; // Guarda os IDs das paredes
  std::vector<std::vector<bool>>
      collisionGrid; // O mapa de colisões em si (true = parede)

  int gridSize = 16;
  int gridWidth = 0;
  int gridHeight = 0;

  bool showDebugCollisions = false;

  void LoadWorldFile();

public:
  LevelGenerator(const char *levelPath);
  ~LevelGenerator();

  void Build(int levelIndex);
  void Draw();

  void SetShowDebugCollisions(bool show) { showDebugCollisions = show; }
  bool GetShowDebugCollisions() const { return showDebugCollisions; }

  int GetLevelWidth() const { return levelWidth; }
  int GetLevelHeight() const { return levelHeight; }
  bool IsCellOccupied(int x, int y) const;
  bool IsTileSolid(int tileId) const;
  bool CheckCollisionWithGrid(Rectangle bounds) const;

  std::vector<Rectangle> GetAdjacentSolidTiles(Rectangle bounds) const;

  int GetGridSize() const { return gridSize; }
  int GetGridWidth() const { return gridWidth; }
  int GetGridHeight() const { return gridHeight; }
};