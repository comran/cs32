#ifndef PROJECT_2_GAME
#define PROJECT_2_GAME

#include "Arena.h"

class Game {
 public:
  // Constructor/destructor
  Game(int rows, int cols, int nRats);
  ~Game();

  // Mutators
  void play();

 private:
  Arena *m_arena;

  // Helper functions
  string takePlayerTurn();
};

bool recommendMove(const Arena &a, int r, int c, int &bestDir);
int computeDanger(const Arena &a, int r, int c);

#endif  // PROJECT_2_GAME
