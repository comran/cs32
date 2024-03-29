#ifndef PROJECT_1_GAME
#define PROJECT_1_GAME

class Arena;

#include <string>
#include <iostream>
using namespace std;

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

#endif  // PROJECT_1_GAME
