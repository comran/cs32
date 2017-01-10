#ifndef PROJECT_2_ARENA
#define PROJECT_2_ARENA

#include "globals.h"
#include "Rat.h"
#include "Player.h"
#include "History.h"

#include <iostream>
using namespace std;

class Rat;
class Player;
class History;

class Arena {
 public:
  // Constructor/destructor
  Arena(int nRows, int nCols);
  ~Arena();

  // Accessors
  int rows() const;
  int cols() const;
  Player *player() const;
  int ratCount() const;
  int getCellStatus(int r, int c) const;
  int numberOfRatsAt(int r, int c) const;
  void display(string msg) const;

  // Mutators
  void setCellStatus(int r, int c, int status);
  bool addRat(int r, int c);
  bool addPlayer(int r, int c);
  void moveRats();
  History& history();

 private:
  int m_grid[MAXROWS][MAXCOLS];
  int m_rows;
  int m_cols;
  Player *m_player;
  Rat *m_rats[MAXRATS];
  int m_nRats;
  int m_turns;

  // Helper functions
  void checkPos(int r, int c) const;
  bool isPosInBounds(int r, int c) const;
  History m_history;
};

#endif  // PROJECT_2_ARENA
