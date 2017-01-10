#ifndef PROJECT_2_HISTORY
#define PROJECT_2_HISTORY

#include "globals.h"

#include <iostream>
using namespace std;

class History {
 public:
  History(int nRows, int nCols);
  bool record(int r, int c);
  void display() const;
 private:
  int m_rows;
  int m_cols;
  int* history;
};

#endif  // PROJECT_2_HISTORY
