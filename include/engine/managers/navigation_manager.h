#pragma once
#include "engine/pathfinding/a_star.h"
#include "engine/pathfinding/grid.h"
#include <vector>

class NavigationManager {
private:
  static NavigationManager *instance;

  Grid *activeGrid;
  AStar astar;

  NavigationManager();

public:
  static NavigationManager *GetInstance();

  void SetGrid(Grid *grid);
  Grid *GetGrid();

  void GetPath(Node *start, Node *end, std::vector<Node *> &cachedPath,
               int &pathVersion, Node *&lastTarget);

  Node *GetClosestWalkableNode(Node *targetNode, Node *currentNode);
  bool IsCloseEnough(Node *start, Node *end, int range);
  int GetDistance(Node *a, Node *b);
  std::tuple<Node *, int> GetClosestNode(Node *origin,
                                         std::vector<Node *> nodes);
};