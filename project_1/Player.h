#ifndef PROJECT_2_PLAYER
#define PROJECT_2_PLAYER

#include "Arena.h"

#include <iostream>
using namespace std;

class Arena;

class Player {
 public:
  // Constructor
  Player(Arena *ap, int r, int c);

  // Accessors
  int row() const;
  int col() const;
  bool isDead() const;

  // Mutators
  string dropPoisonPellet();
  string move(int dir);
  void setDead();

 private:
  Arena *m_arena;
  int m_row;
  int m_col;
  bool m_dead;
};

#endif  // PROJECT_2_PLAYER
