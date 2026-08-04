#include "engine/core/game.h"

int main() {
  Game game = Game(1280, 720, 0, "Cave Miner");
  game.Run();

  return 0;
}