#ifndef PROJECT_1_HISTORY
#define PROJECT_1_HISTORY

#include "globals.h"

class History {
 public:
  History(int nRows, int nCols);
  ~History();
  bool record(int r, int c);
  void display() const;

 private:
  int m_rows;
  int m_cols;
  int *history;
};

#endif  // PROJECT_1_HISTORY
