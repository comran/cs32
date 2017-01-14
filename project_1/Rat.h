#ifndef PROJECT_2_RAT
#define PROJECT_2_RAT

class Arena;

class Rat {
 public:
  // Constructor
  Rat(Arena *ap, int r, int c);

  // Accessors
  int row() const;
  int col() const;
  bool isDead() const;

  // Mutators
  void move();

 private:
  Arena *m_arena;
  int m_row;
  int m_col;
  int m_health;
  int m_idleTurnsRemaining;
};

bool attemptMove(const Arena &a, int dir, int &r, int &c);

#endif  // PROJECT_2_RAT
