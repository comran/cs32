#include "History.h"

History::History(int nRows, int nCols) {
  m_rows = nRows;
  m_cols = nCols;

  const int num_spots = nRows * nCols;
  history = new int[num_spots];
  for(int i = 0;i < num_spots;i++) {
    history[i] = 0;
  }
}

bool History::record(int r, int c) {
  if (r <= 0  ||  c <= 0  ||  r > m_rows  ||  c > m_cols) return false;

  history[(r - 1) * m_cols + c - 1]++;

  return true;
}

void History::display() const {
  clearScreen();
  for (int i = 0; i < m_rows * m_cols; i++) {
    if(i % m_cols == 0 && i != 0) cout << endl;

    if(history[i] == 0) {
      cout << '.';
    } else {
      cout << (char)(64 + history[i]);
    }
  }

  cout << endl;
}
