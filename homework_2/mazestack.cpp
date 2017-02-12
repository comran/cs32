#include <iostream>
#include <string>
#include <stack>
using namespace std;

class Coord {
 public:
  Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
  int r() const { return m_r; }
  int c() const { return m_c; }

 private:
  int m_r;
  int m_c;
};

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er,
                int ec) {
  stack<Coord> coordinates;

  coordinates.push(Coord(sr, sc));
  maze[sr][sc] = '%';

  while (!coordinates.empty()) {
    int row = coordinates.top().r();
    int col = coordinates.top().c();
    coordinates.pop();

    if (row == er && col == ec) return true;  // Found ending coordinate.

    if (row > 0 /* there is a row above */ &&
        maze[row - 1][col] != 'X' /* can move north */ &&
        maze[row - 1][col] != '%' /* have not already visited that spot */) {
      coordinates.push(Coord(row - 1, col));
      maze[row - 1][col] = '%';
    }

    if (col < nCols - 1 /* there is a column to the right */ &&
        maze[row][col + 1] != 'X' /* can move east */ &&
        maze[row][col + 1] != '%' /* have not already visited that spot */) {
      coordinates.push(Coord(row, col + 1));
      maze[row][col + 1] = '%';
    }

    if (row < nRows - 1 /* there is a row below */ &&
        maze[row + 1][col] != 'X' /* can move south */ &&
        maze[row + 1][col] != '%' /* have not already visited that spot */) {
      coordinates.push(Coord(row + 1, col));
      maze[row + 1][col] = '%';
    }

    if (col > 0 /* there is a column to the left */ &&
        maze[row][col - 1] != 'X' /* can move west */ &&
        maze[row][col - 1] != '%' /* have not already visited that spot */) {
      coordinates.push(Coord(row, col - 1));
      maze[row][col - 1] = '%';
    }
  }

  return false;
}

int main() {
  string maze[10] = {"XXXXXXXXXX",  //////////////////////////////
                     "X........X",  //////////////////////////////
                     "XX.X.XXXXX",  //////////////////////////////
                     "X..X.X...X",  //////////////////////////////
                     "X..X...X.X",  //////////////////////////////
                     "XXXX.XXX.X",  //////////////////////////////
                     "X.X....XXX",  //////////////////////////////
                     "X..XX.XX.X",  //////////////////////////////
                     "X...X....X",  //////////////////////////////
                     "XXXXXXXXXX"};

  if (pathExists(maze, 10, 10, 6, 4, 1, 1))
    cout << "Solvable!" << endl;
  else
    cout << "Out of luck!" << endl;
}
